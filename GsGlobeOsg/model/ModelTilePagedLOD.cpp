
#include <Model/ModelTilePagedLOD.h>

#include <osg/Version>
#include <osgEarth/Registry>
#include <cassert>
#include <osg/PagedLOD>
#include <osg/CullStack>
#include <osg/Notify>
#include <iostream>

#include <algorithm>

using namespace GeoGlobe::Model;

using namespace osg;

#define LC "[TilePagedLOD] "

//#define OE_TEST OE_INFO
#define OE_TEST OE_NULL



void 
	ModelTilePagedLOD::useAutoClamp()
{
	_autoClampCallback=new AutoClampCallback(this);
	_mapNode->getTerrain()->addTerrainCallback(_autoClampCallback);
}

ModelTilePagedLOD::ModelTilePagedLOD(osgEarth::MapNode* mapNode) :
osg::PagedLOD()

{
	_mapNode=mapNode;
	_hasRealData=false;
	_normalTraverse=true;
	_spliteFactor=1.0f;

}

ModelTilePagedLOD::~ModelTilePagedLOD()
{

	if(_autoClampCallback!=NULL)
		_mapNode->getTerrain()->removeTerrainCallback(_autoClampCallback);
	
}

bool ModelTilePagedLOD::splite(osg::NodeVisitor& nv,double timeStamp,double frameNumber,double range)
{

	//瓦片完全包含在视域内不分裂
	//有一部分在视域外则分裂
	//视域范围小于模型瓦片大小也分裂
	//分裂成果则运行分裂的瓦片，否则运行父瓦片
	if (_children.size()>1)
	{
		if (_children[1]->asGroup()->getNumChildren()>0)
		{
			_perRangeDataList[1]._timeStamp=timeStamp;
			_perRangeDataList[1]._frameNumber=frameNumber;
			//运行分裂的瓦片
			_children[1]->accept(nv);
			return true;
		}
		else
		{
			//分裂的瓦片未加载，尽管满足分裂条件，但任然运行父瓦片
			return false;
		}
		
	}
	else
	{
		//float priority = 0.6;
		if(nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR)
		{
			float priority = (getInitialBound().radius()*6-range)/(getInitialBound().radius()*6-0);
			nv.getDatabaseRequestHandler()->requestNodeFile(_perRangeDataList[1]._filename,nv.getNodePath(),priority,nv.getFrameStamp(), _perRangeDataList[1]._databaseRequest, _databaseOptions.get());
		}
		_perRangeDataList[0]._timeStamp=timeStamp;
		_perRangeDataList[0]._frameNumber=frameNumber;
		//运行父瓦片
		_children[0]->accept(nv);
		return true;
	}
	
}


void
	ModelTilePagedLOD::setTileKey( TileKey& tikeKey )
{
//	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_tileKeyMutex);
	_tileKey=TileKey(tikeKey);
}

#include <osg/ShapeDrawable>
void
ModelTilePagedLOD::traverse(osg::NodeVisitor& nv)
{
//顶层模型瓦片按常规的四叉树分裂
	if(_normalTraverse)
	{
		PagedLOD::traverse(nv);
		return;
	}

	//除了顶层的模型瓦片则采用特殊的四叉树分裂
	//瓦片完全包含在视域内不分裂
	//有一部分在视域外则分裂
	//这么处理的原因是考虑到模型瓦片与一般瓦片不同，一般瓦片级别越高数据量越大，模型瓦片相反
	//下面是具体实现代码，大部分代码都是模仿osg::PageLOD::traverse写的，因此不详细介绍
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
		
//获取当前视点与模型瓦片中心的距离
				required_range = nv.getDistanceToViewPoint(getInitialBound().center()/*getCenter()*/,true);
		

			int lastChildTraversed = -1;
			bool needToLoadChild = false;



            //与osg::PageLOD::traverse的区别主要在这部分
			//if (required_range<=_setting->_visualRange)
			{
				//if((required_range+_initialBound.radius()/*getRadius()*/)>(_setting->_visualRange*1.1))
				//{
				//	if(!splite(nv,timeStamp,frameNumber,required_range))
				//	{
				//		_perRangeDataList[0]._timeStamp=timeStamp;
				//		_perRangeDataList[0]._frameNumber=frameNumber;
				//		_children[0]->accept(nv);
				//		lastChildTraversed = (int)0;
				//	}

				//}
				//判断模型瓦片是否包含可视范围
				//else if(required_range<_initialBound.radius()*_spliteFactor/*getRadius()*/)
				if(required_range<_initialBound.radius()*_spliteFactor/*getRadius()*/)
				{
//分裂瓦片
					if(!splite(nv,timeStamp,frameNumber,required_range))
					{
						//分裂失败则遍历父瓦片
						_perRangeDataList[0]._timeStamp=timeStamp;
						_perRangeDataList[0]._frameNumber=frameNumber;
						_children[0]->accept(nv);
						lastChildTraversed = (int)0;
					}
				}
				else
				{
//模型瓦片完全包含在可视范围内则不需要分裂
					_perRangeDataList[0]._timeStamp=timeStamp;
					_perRangeDataList[0]._frameNumber=frameNumber;
					_children[0]->accept(nv);
					lastChildTraversed = (int)0;
				}
				
	//			osg::CullStack* cullStack = dynamic_cast<osg::CullStack*>(&nv);

	//	/*		if (!cullStack->getCurrentCullingSet().getFrustum().containsAllOf(getInitialBound()))
	//			{
	//				if(!splite(nv,timeStamp,frameNumber,required_range))
	//				{
	//					std::cout<<"end"<<_tikeKey.str()<<std::endl;
	//					_perRangeDataList[0]._timeStamp=timeStamp;
	//					_perRangeDataList[0]._frameNumber=frameNumber;
	//					_children[0]->accept(nv);
	//					lastChildTraversed = (int)0;
	//				}
	//			}*/
	//		
	////		osg::CullStack* cullStack = dynamic_cast<osg::CullStack*>(&nv);
	////		//std::vector<osg::Vec3f> point;
	////		//point.push_back()

	/////*		if ()
	////		{
	////			return;
	////		}*/

	////		//osg::CullStack* cullStack = dynamic_cast<osg::CullStack*>(&nv);



	////		/*if (required_range>2000)
	////		return;*/
	//		if (required_range<_initialBound.radius()*4)
	//		{
	//			if(!splite(nv,timeStamp,frameNumber,required_range))
	//			{
	//				std::cout<<"end"<<_tikeKey.str()<<std::endl;
	//				_perRangeDataList[0]._timeStamp=timeStamp;
	//				_perRangeDataList[0]._frameNumber=frameNumber;
	//				_children[0]->accept(nv);
	//				lastChildTraversed = (int)0;
	//			}


	//		}
	//		else /*if(required_range<_initialBound.radius()*6)*/
	//		{
	//			//std::cout<<"start"<<_tikeKey.str()<<std::endl;
	//			_perRangeDataList[0]._timeStamp=timeStamp;
	//			_perRangeDataList[0]._frameNumber=frameNumber;
	//			_children[0]->accept(nv);
	//			lastChildTraversed = (int)0;
	//		}
			}
		//	if (cullStack->getCurrentCullingSet().getFrustum().containsAllOf(getInitialBound()))
		//	{
		//		//if (required_range>2000)
		//		//{
		//		//	splite(nv,timeStamp,frameNumber);

		//		//}
		//		//else
		//		//{
		//		std::cout<<"contain"<<_tikeKey.str()<<std::endl;
		//			_perRangeDataList[0]._timeStamp=timeStamp;
		//			_perRangeDataList[0]._frameNumber=frameNumber;
		//		    _children[0]->accept(nv);
		//		    lastChildTraversed = (int)0;
		//		/*}*/
		//		


		//	}
		//	else
		//	{
		///*		if (_initialBound.radius()*3<required_range)
		//		{
		//		}*/
		//		
		//		
		//		if (required_range<3000)
		//		{
		//			if (required_range<_initialBound.radius()*2)
		//			{
		//				if(!splite(nv,timeStamp,frameNumber,required_range))
		//				{
		//					std::cout<<"end"<<_tikeKey.str()<<std::endl;
		//					_perRangeDataList[0]._timeStamp=timeStamp;
		//					_perRangeDataList[0]._frameNumber=frameNumber;
		//					_children[0]->accept(nv);
		//				}
		//			}
		//			else
		//			{
		//				std::cout<<"end"<<_tikeKey.str()<<std::endl;
		//				_perRangeDataList[0]._timeStamp=timeStamp;
		//				_perRangeDataList[0]._frameNumber=frameNumber;
		//				_children[0]->accept(nv);

		//			}
		//			
		//		
		//		}

		//			
		//	}
			


			

			//if((required_range>6*getInitialBound().radius()||cullStack->getCurrentCullingSet().getFrustum().containsAllOf(getInitialBound())))
			//{
			//	if (updateTimeStamp)
			//	{
			//		_perRangeDataList[0]._timeStamp=timeStamp;
			//		_perRangeDataList[0]._frameNumber=frameNumber;
			//	}
			//	_children[0]->accept(nv);
			//	lastChildTraversed = (int)0;
			//}
			//else if (_children.size()<2)
			//{
			//	//if (updateTimeStamp)
			//	//{
			//	//	_perRangeDataList[0]._timeStamp=timeStamp;
			//	//	_perRangeDataList[0]._frameNumber=frameNumber;
			//	//}
			//	//_children[0]->accept(nv);
			//	//lastChildTraversed = (int)0;

			//	float priority = 0.3;
			//	nv.getDatabaseRequestHandler()->requestNodeFile(_perRangeDataList[1]._filename,nv.getNodePath(),priority,nv.getFrameStamp(), _perRangeDataList[1]._databaseRequest, _databaseOptions.get());
			//	//needToLoadChild=true;
			//}
			//else
			//{
			//	if (_children[1]->asGroup()->getNumChildren()>0)
			//	{
			//		if (updateTimeStamp)
			//		{
			//			_perRangeDataList[1]._timeStamp=timeStamp;
			//			_perRangeDataList[1]._frameNumber=frameNumber;
			//		}
			//		_children[1]->accept(nv);
			//		lastChildTraversed = (int)1;
			//	}
			//	else
			//	{
			//		if (updateTimeStamp)
			//		{
			//			_perRangeDataList[0]._timeStamp=timeStamp;
			//			_perRangeDataList[0]._frameNumber=frameNumber;
			//		}
			//		_children[0]->accept(nv);
			//		lastChildTraversed = (int)0;

			//	}
			//	
			//
			//}
				
			

			//if(required_range>getInitialBound().radius()*3)
			//{
			//	if (required_range<getInitialBound().radius()*6)
			//	{
			//	
			//	
			//	if (updateTimeStamp)
			//	{
			//		_perRangeDataList[0]._timeStamp=timeStamp;
			//		_perRangeDataList[0]._frameNumber=frameNumber;
			//	}
			//	_children[0]->accept(nv);
			//	lastChildTraversed = (int)0;
			//	}
			//}
			//else if (_children.size()<2)
			//{
			//	//if (updateTimeStamp)
			//	//{
			//	//	_perRangeDataList[0]._timeStamp=timeStamp;
			//	//	_perRangeDataList[0]._frameNumber=frameNumber;
			//	//}
			//	//_children[0]->accept(nv);
			//	//lastChildTraversed = (int)0;

			//	float priority = 0.3;
			//	nv.getDatabaseRequestHandler()->requestNodeFile(_perRangeDataList[1]._filename,nv.getNodePath(),priority,nv.getFrameStamp(), _perRangeDataList[1]._databaseRequest, _databaseOptions.get());
			//	//needToLoadChild=true;
			//}
			//else
			//{
			//	if (updateTimeStamp)
			//	{
			//		_perRangeDataList[1]._timeStamp=timeStamp;
			//		_perRangeDataList[1]._frameNumber=frameNumber;
			//	}
			//	_children[1]->accept(nv);
			//	lastChildTraversed = (int)1;
			//}
			


			//for(unsigned int i=0;i<_rangeList.size();++i)
			//{
		 //   	if (_rangeList[i].first<=required_range && required_range<_rangeList[i].second)
			//	{
			//		if (i<_children.size())
			//		{
			//			if (updateTimeStamp)
			//			{
			//				_perRangeDataList[i]._timeStamp=timeStamp;
			//				_perRangeDataList[i]._frameNumber=frameNumber;
			//			}

			//			_children[i]->accept(nv);
			//			lastChildTraversed = (int)i;
			//		}
			//		else
			//		{
			//			needToLoadChild = true;
			//		}
			//	}
			//}

	/*		if (needToLoadChild)
			{
				unsigned int numChildren = _children.size();*/

				// select the last valid child.
		/*		if (numChildren>0 && ((int)numChildren-1)!=lastChildTraversed)
				{
					if (updateTimeStamp)
					{
						_perRangeDataList[numChildren-1]._timeStamp=timeStamp;
						_perRangeDataList[numChildren-1]._frameNumber=frameNumber;
					}
					_children[numChildren-1]->accept(nv);
				}*/

				// now request the loading of the next unloaded child.
			//	if (!_disableExternalChildrenPaging &&
			//		nv.getDatabaseRequestHandler() &&
			//		numChildren<_perRangeDataList.size())
			//	{
			//		// compute priority from where abouts in the required range the distance falls.
			//		float priority = (_rangeList[numChildren].second-required_range)/(_rangeList[numChildren].second-_rangeList[numChildren].first);

			//		// invert priority for PIXEL_SIZE_ON_SCREEN mode
			//		if(_rangeMode==PIXEL_SIZE_ON_SCREEN)
			//		{
			//			priority = -priority;
			//		}

			//		// modify the priority according to the child's priority offset and scale.
			//		priority = _perRangeDataList[numChildren]._priorityOffset + priority * _perRangeDataList[numChildren]._priorityScale;

			//		if (_databasePath.empty())
			//		{
			//			nv.getDatabaseRequestHandler()->requestNodeFile(_perRangeDataList[numChildren]._filename,nv.getNodePath(),priority,nv.getFrameStamp(), _perRangeDataList[numChildren]._databaseRequest, _databaseOptions.get());
			//		}
			//		else
			//		{
			//			// prepend the databasePath to the child's filename.
			//			nv.getDatabaseRequestHandler()->requestNodeFile(_databasePath+_perRangeDataList[numChildren]._filename,nv.getNodePath(),priority,nv.getFrameStamp(), _perRangeDataList[numChildren]._databaseRequest, _databaseOptions.get());
			//		}
			//	}

			//}


			break;
		}
	default:
		break;
	}
}


//void
//	TilePagedLOD::reclamp( const osgEarth::TileKey& key, osg::Node* tile, const osgEarth::Terrain* terrain )
//{
//	// first verify that the label position intersects the tile:
//	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_tileKeyMutex);
//	if(!_tileKey.valid())
//		return;
//	osgEarth::GeoPoint gp=_tileKey.getExtent().getBoundingGeoCircle().getCenter();
//	if(!gp.isValid())
//		int a=0;
//	if ( key.getExtent().contains( gp.x(), gp.y() ) )
//	{
//		double hamsl;
//		if (_mapNode->getTerrain()->getHeight(tile, gp.getSRS(), gp.x(), gp.y(), &hamsl, 0L))
//		{
//			if (hamsl<0)
//			{
//				int a=0;
//			}
//			
//			gp.z()=hamsl;
//			osg::Vec3d center;
//			gp.toWorld(center);
//			setCenter(center);
//		}
//	//	int a=0;
//		updateTransforms( _mapPosition, tile );
//	}
//}


//bool TilePagedLOD::updateTransforms( GeoPoint& p, osg::Node* patch )
//{
//	//if ( getMapNode() )
//	//{
//	//	//OE_NOTICE << "updateTransforms" << std::endl;
//	//	// make sure the point is absolute to terrain
//	//	GeoPoint absPos(p);
//	//	if ( !makeAbsolute(absPos, patch) )
//	//		return false;
//
//	//	osg::Matrixd local2world;
//	//	if ( !absPos.createLocalToWorld(local2world) )
//	//		return false;
//
//	//	// apply the local tangent plane offset:
//	//	local2world.preMult( osg::Matrix::translate(_localOffset) );
//
//	//	// update the xforms:
//	//	_autoxform->setPosition( local2world.getTrans() );
//	//	_matxform->setMatrix( local2world );
//
//	//	osg::Vec3d world = local2world.getTrans();
//	//	if (_horizonCuller.valid())
//	//	{
//	//		_horizonCuller->_world = world;
//	//	}
//
//	//	if (_occlusionCuller.valid())
//	//	{                                
//	//		_occlusionCuller->setWorld( adjustOcclusionCullingPoint( world ));
//	//	} 
//	//}
//	//else
//	//{
//	//	osg::Vec3d absPos = p.vec3d() + _localOffset;
//	//	_autoxform->setPosition( absPos );
//	//	_matxform->setMatrix( osg::Matrix::translate(absPos) );
//	//}
//
//	double hamsl;
//	//if (_mapNode->getTerrain()->getHeight(patch, p.getSRS(), p.x(), p.y(), &hamsl, 0L))
//	//{
//		//// apply any scale/offset in the symbology:
//		//if ( _altitude.valid() )
//		//{
//		//	if ( _altitude->verticalScale().isSet() )
//		//		hamsl *= _altitude->verticalScale()->eval();
//		//	if ( _altitude->verticalOffset().isSet() )
//		//		hamsl += _altitude->verticalOffset()->eval();
//		//}
//		//p.z() = hamsl;
//		//osg::Vec3d center;
//		//_mapPosition.toWorld(center);
//		//setCenter(center);
//		//if (hamsl<0)
//		//{
//		//	int a=0;
//		//}
//		
//	/*}*/
//
//
////	dirtyBound();
//	return true;
//}



//BoundingSphere TilePagedLOD::computeBound() const
//{
//	osg::Vec3d center;
//	_mapPosition.toWorld(center);
//	return osg::BoundingSphere(center, _initialBound.radius());
//}

// The osgDB::DatabasePager will call this automatically to purge expired
// tiles from the scene graph.
//bool
//ModelTilePagedLOD::removeExpiredChildren(double         expiryTime, 
//                                    unsigned       expiryFrame, 
//                                    osg::NodeList& removedChildren)
//{
//	//实现动态删除的部分，如果某一节点很久没用就会被删除，这部分也是模仿PagedLod实现的
//	if (_children.size()>_numChildrenThatCannotBeExpired)
//	{
//		unsigned cindex = _children.size() - 1;
//		if (!_perRangeDataList[cindex]._filename.empty() &&
//			_perRangeDataList[cindex]._timeStamp + _perRangeDataList[cindex]._minExpiryTime < expiryTime &&
//			_perRangeDataList[cindex]._frameNumber + _perRangeDataList[cindex]._minExpiryFrames < expiryFrame)
//		{
//			//如果瓦片长时间没被绘制则将其移除
//			osg::Node* nodeToRemove = _children[cindex].get();
//			removedChildren.push_back(nodeToRemove);
//			return Group::removeChildren(cindex,1);
//		}
//	}
//	return false;
//}

/*以下都是老代码，实现瓦片自动调整高度，因效率不高故不使用*/
void ModelTilePagedLOD::reclamp( const osgEarth::TileKey& key, osg::Node* tile, const osgEarth::Terrain* terrain )
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
			if (hamsl<0)
			{
				int a=0;
			}

			gp.z()=hamsl;
			osg::Vec3d center;
			gp.toWorld(center);
			double radius=getInitialBound().radius();
			setInitialBound(osg::BoundingSphere(center,radius));
			//setInitialBound()
			setCenter(center);
			dirtyBound();
		}
		//	int a=0;
		//updateTransforms( _mapPosition, tile );
	}
}



