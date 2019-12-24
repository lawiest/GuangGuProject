#include <SchemeComparisonAnalysis/PolyInter/IntersectionVisitorCMF.h>

#include <osg/PagedLOD>
#include <osg/Transform>
#include <osg/Projection>
#include <osg/Camera>
#include <osg/Geode>
#include <osg/Billboard>
#include <osg/Geometry>
#include <osg/Notify>
#include <osg/io_utils>

#include <SchemeComparisonAnalysis/PolyInter/PolygonCMFSegmentIntersector.h>
#include "SchemeComparisonAnalysis/ModelFc.h"

#include <Model/Model.h>
#include <model/LodModel.h>


IntersectionVisitorCMF::IntersectionVisitorCMF(Intersector* intersector, ReadCallback* readCallback)
{
	// override the default node visitor mode.
	setTraversalMode(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN);

	setIntersector(intersector);

	setReadCallback(readCallback);
}

void IntersectionVisitorCMF::setIntersector(Intersector* intersector)
{
	// keep reference around just in case intersector is already in the _intersectorStack, otherwise the clear could delete it.
	osg::ref_ptr<Intersector> temp = intersector;

	_intersectorStack.clear();

	if (intersector) _intersectorStack.push_back(intersector);
}

void IntersectionVisitorCMF::reset()
{
	if (!_intersectorStack.empty())
	{
		osg::ref_ptr<Intersector> intersector = _intersectorStack.front();
		intersector->reset();

		_intersectorStack.clear();
		_intersectorStack.push_back(intersector);
	}
}


/*
void IntersectionVisitorCMF::apply(osg::Node& node)
{
	// OSG_NOTICE<<"apply(Node&)"<<std::endl;

	if (!enter(node)) return;

	// OSG_NOTICE<<"inside apply(Node&)"<<std::endl;

	traverse(node);

	leave();
}
*/

void IntersectionVisitorCMF::apply(osg::MatrixTransform& group)
{
	if (!enter(group)) return;

	CModelF* model = dynamic_cast<CModelF*>(&group);//因为CMF是直接放在根节点下面的，所以只需要在group中寻找
	if (model != NULL)
	{
		intersect(model);
	}

	traverse(group);

	leave();
}

/*
void IntersectionVisitorCMF::apply(osg::MatrixTransform& node)
{
	if (!enter(node)) return;

	//GeoGlobe::Model::Model* model = dynamic_cast<GeoGlobe::Model::Model*>(&node);
	//if (model != NULL)
	//{
	//	int  a = 0;
	//	intersect(&node);
	//}

	apply(static_cast<Transform&>(node));
}

void IntersectionVisitorCMF::apply(osg::Geode& geode)
{
	// OSG_NOTICE<<"apply(Geode&)"<<std::endl;

	if (!enter(geode)) return;

	// OSG_NOTICE<<"inside apply(Geode&)"<<std::endl;

	for(unsigned int i=0; i<geode.getNumDrawables(); ++i)
	{
		//intersect( geode.getDrawable(i) );
	}

	leave();
}

void IntersectionVisitorCMF::apply(osg::LOD& lod)
{
	if (!enter(lod)) return;

	traverse(lod);

	leave();
}*/

/*
void IntersectionVisitorCMF::apply(CModelF& plod)
{
	if (!enter(plod)) return;

	CModelF* model = dynamic_cast<CModelF*>(&plod);
	if (model != NULL)
	{
		intersect(&plod);
	}

	if (plod.getNumFileNames()>0)
	{
#if 1
		// Identify the range value for the highest res child
		float targetRangeValue;
		if( plod.getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT )
			targetRangeValue = 1e6; // Init high to find min value
		else
			targetRangeValue = 0; // Init low to find max value

		const osg::LOD::RangeList rl = plod.getRangeList();
		osg::LOD::RangeList::const_iterator rit;
		for( rit = rl.begin();
			rit != rl.end();
			rit++ )
		{
			if( plod.getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT )
			{
				if( rit->first < targetRangeValue )
					targetRangeValue = rit->first;
			}
			else
			{
				if( rit->first > targetRangeValue )
					targetRangeValue = rit->first;
			}
		}

		// Perform an intersection test only on children that display
		// at the maximum resolution.
		unsigned int childIndex;
		for( rit = rl.begin(), childIndex = 0;
			rit != rl.end();
			rit++, childIndex++ )
		{
			if( rit->first != targetRangeValue )
				// This is not one of the highest res children
				continue;

			osg::ref_ptr<osg::Node> child( NULL );
			if( plod.getNumChildren() > childIndex )
				child = plod.getChild( childIndex );

			if( (!child.valid()) && (_readCallback.valid()) )
			{
				// Child is NULL; attempt to load it, if we have a readCallback...
				unsigned int validIndex( childIndex );
				if (plod.getNumFileNames() <= childIndex)
					validIndex = plod.getNumFileNames()-1;

				child = _readCallback->readNodeFile( plod.getDatabasePath() + plod.getFileName( validIndex ) );
			}

			if ( !child.valid() && plod.getNumChildren()>0)
			{
				// Child is still NULL, so just use the one at the end of the list.
				child = plod.getChild( plod.getNumChildren()-1 );
			}

			if (child.valid())
			{
				child->accept(*this);
			}
		}
#else
		// older code than above block, that assumes that the PagedLOD is ordered correctly
		// i.e. low res children first, no duplicate ranges.

		osg::ref_ptr<osg::Node> highestResChild;

		if (plod.getNumFileNames() != plod.getNumChildren() && _readCallback.valid())
		{
			highestResChild = _readCallback->readNodeFile( plod.getDatabasePath() + plod.getFileName(plod.getNumFileNames()-1) );
		}

		if ( !highestResChild.valid() && plod.getNumChildren()>0)
		{
			highestResChild = plod.getChild( plod.getNumChildren()-1 );
		}

		if (highestResChild.valid())
		{
			highestResChild->accept(*this);
		}
#endif
	}

	leave();
}
*/
