#include "DistanceAnalysis.h"
#include "innerimp.h"
#include <Analysis/DistanceMeasure.h>
#include "OsgCoreSingleton.h"

#include <osgEarthSymbology/PolygonSymbol>
#include <osgEarthFeatures/Feature>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/LabelNode>
#include <placename/PlaceNode.h>
#include <osgEarthAnnotation/PlaceNode>

using namespace osgEarth::Symbology;
using namespace osgEarth::Annotation;

GLOBE_NS

DistanceAnalysis::DistanceAnalysis()
{
	osg::ref_ptr<GeoGlobe::DistanceMeasureHandler> ptrDistanceAnalysis = new GeoGlobe::DistanceMeasureHandler(OsgCoreSingleton::Instance()->MapNode());
	m_ptrAnalysisHandle = new ReferenceImp<GeoGlobe::DistanceMeasureHandler>(ptrDistanceAnalysis);
				
	m_AnalysisType = eDistanceAnalysis;
}

DistanceAnalysis::~DistanceAnalysis()
{

}

UTILITY_NAME::GsDelegate<void(double)>& DistanceAnalysis::OnFinish()
{
	return Handle()->CastTo<osg::ref_ptr<GeoGlobe::DistanceMeasureHandler> >()->OnFinish;
}

void DistanceAnalysis::Path(bool bPath)
{
	
	Handle()->CastTo<osg::ref_ptr<GeoGlobe::DistanceMeasureHandler> >()->setIsPath(bPath);
}

bool DistanceAnalysis::Path() const
{
	return m_ptrAnalysisHandle->CastTo<osg::ref_ptr<GeoGlobe::DistanceMeasureHandler> >()->getIsPath();
}


GLOBE_ENDNS