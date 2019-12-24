/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
* Copyright 2008-2014 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include <Base/TilePagedLOD.h>
#include <osg/Version>
#include <osgEarth/Registry>
#include <cassert>
#include <osg/PagedLOD>
#include <osg/CullStack>
#include <osg/Notify>
#include <iostream>
#include <algorithm>
using namespace GeoGlobe;
using namespace Base;

using namespace osg;

void 
	TilePagedLOD::useAutoClamp()
{
	_autoClampCallback=new AutoClampCallback(this);
	_mapNode->getTerrain()->addTerrainCallback(_autoClampCallback);
}


TilePagedLOD::TilePagedLOD(osgEarth::TileKey &  tileKey,osgEarth::MapNode* mapNode) :
osg::PagedLOD()

{
	_tileKey=tileKey;
	_mapNode=mapNode;
    setRadius(tileKey.getExtent().computeBoundingGeoCircle().getRadius());
	osg::Vec3d center;
	osgEarth::GeoPoint gp=tileKey.getExtent().computeBoundingGeoCircle().getCenter();
	gp.toWorld(center);
	setCenter(center);
}

TilePagedLOD::~TilePagedLOD()
{
	if(_autoClampCallback)
		_mapNode->getTerrain()->removeTerrainCallback(_autoClampCallback);
}

/*主要功能代码，几乎和PageLOD一样*/
void TilePagedLOD::traverse(NodeVisitor& nv)
{
	// set the frame number of the traversal so that external nodes can find out how active this
	// node is.
	if (nv.getFrameStamp() &&
		nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR)
	{
		setFrameNumberOfLastTraversal(nv.getFrameStamp()->getFrameNumber());
	}

	double timeStamp = nv.getFrameStamp()?nv.getFrameStamp()->getReferenceTime():0.0;
	unsigned int frameNumber = nv.getFrameStamp()?nv.getFrameStamp()->getFrameNumber():0;
	bool updateTimeStamp = nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR;

	switch(nv.getTraversalMode())
	{
	case(NodeVisitor::TRAVERSE_ALL_CHILDREN):
		std::for_each(_children.begin(),_children.end(),NodeAcceptOp(nv));
		break;
	case(NodeVisitor::TRAVERSE_ACTIVE_CHILDREN):
		{
			float required_range = 0;
			if (_rangeMode==DISTANCE_FROM_EYE_POINT)
			{
				required_range = nv.getDistanceToViewPoint(getCenter(),true);
			}
			else
			{
				osg::CullStack* cullStack = dynamic_cast<osg::CullStack*>(&nv);
				if (cullStack && cullStack->getLODScale()>0.0f)
				{
					required_range = cullStack->clampedPixelSize(getBound()) / cullStack->getLODScale();
				}
				else
				{
					// fallback to selecting the highest res tile by
					// finding out the max range
					for(unsigned int i=0;i<_rangeList.size();++i)
					{
						required_range = osg::maximum(required_range,_rangeList[i].first);
					}
				}
			}

			int lastChildTraversed = -1;
			bool needToLoadChild = false;
			for(unsigned int i=0;i<_rangeList.size();++i)
			{
				if (_rangeList[i].first<=required_range && required_range<_rangeList[i].second)
				{
					if (i<_children.size())
					{
						if(_children[i]->asGroup()->getNumChildren()>0)
						{
							if (updateTimeStamp)
							{
								_perRangeDataList[i]._timeStamp=timeStamp;
								_perRangeDataList[i]._frameNumber=frameNumber;
							}

							_children[i]->accept(nv);
							lastChildTraversed = (int)i;
					   }
						else
						{
							if (updateTimeStamp)
							{
								_perRangeDataList[0]._timeStamp=timeStamp;
								_perRangeDataList[0]._frameNumber=frameNumber;
							}

							_children[0]->accept(nv);
							lastChildTraversed = (int)0;
						}

					}
					else
					{
						needToLoadChild = true;
					}
				}
			}

			if (needToLoadChild)
			{
				unsigned int numChildren = _children.size();

				// select the last valid child.
				if (numChildren>0 && ((int)numChildren-1)!=lastChildTraversed)
				{
					if (updateTimeStamp)
					{
						_perRangeDataList[numChildren-1]._timeStamp=timeStamp;
						_perRangeDataList[numChildren-1]._frameNumber=frameNumber;
					}
					_children[numChildren-1]->accept(nv);
				}

				// now request the loading of the next unloaded child.
				if (!_disableExternalChildrenPaging &&
					nv.getDatabaseRequestHandler() &&
					numChildren<_perRangeDataList.size())
				{
					// compute priority from where abouts in the required range the distance falls.
					float priority = (_rangeList[numChildren].second-required_range)/(_rangeList[numChildren].second-_rangeList[numChildren].first);

					// invert priority for PIXEL_SIZE_ON_SCREEN mode
					if(_rangeMode==PIXEL_SIZE_ON_SCREEN)
					{
						priority = -priority;
					}

					// modify the priority according to the child's priority offset and scale.
					priority = _perRangeDataList[numChildren]._priorityOffset + priority * _perRangeDataList[numChildren]._priorityScale;

					if (_databasePath.empty())
					{
						nv.getDatabaseRequestHandler()->requestNodeFile(_perRangeDataList[numChildren]._filename,nv.getNodePath(),priority,nv.getFrameStamp(), _perRangeDataList[numChildren]._databaseRequest, _databaseOptions.get());
					}
					else
					{
						// prepend the databasePath to the child's filename.
						nv.getDatabaseRequestHandler()->requestNodeFile(_databasePath+_perRangeDataList[numChildren]._filename,nv.getNodePath(),priority,nv.getFrameStamp(), _perRangeDataList[numChildren]._databaseRequest, _databaseOptions.get());
					}
				}

			}

			break;
		}
	default:
		break;
	}
}



void
	TilePagedLOD::reclamp( const osgEarth::TileKey& key, osg::Node* tile, const osgEarth::Terrain* terrain )
{
	// first verify that the label position intersects the tile:
	
	if(!_tileKey.valid())
		return;
	osgEarth::GeoPoint gp=_tileKey.getExtent().computeBoundingGeoCircle().getCenter();
	if(!gp.isValid())
		int a=0;
	if ( key.getExtent().contains( gp.x(), gp.y() ) )
	{
		double hamsl;
		if (_mapNode->getTerrain()->getHeight(tile, gp.getSRS(), gp.x(), gp.y(), &hamsl, 0L))
		{
			//if (hamsl<0)
			//{
			//	int a=0;
			//}
			
			gp.z()=hamsl;
			osg::Vec3d center;
			gp.toWorld(center);
			setCenter(center);
			  dirtyBound();
		}
	//	int a=0;
		//updateTransforms( _mapPosition, tile );
	}
}


