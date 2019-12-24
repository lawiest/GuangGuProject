#include "RadarAnalysis.h"
#include "innerimp.h"
#include "Analysis/RadarAnalysisHandler.h"
#include "OsgCoreSingleton.h"

GLOBE_NS

RadarAnalysis::RadarAnalysis()
{
	osg::ref_ptr<GeoGlobe::RadarAnalysisHandler> ptrRadarAnalysis = new GeoGlobe::RadarAnalysisHandler(OsgCoreSingleton::Instance()->MapNode());

	m_ptrAnalysisHandle = new ReferenceImp<GeoGlobe::RadarAnalysisHandler>(ptrRadarAnalysis);

	m_AnalysisType = eRadarAnalysis;
}

RadarAnalysis::~RadarAnalysis()
{

}

GLOBE_ENDNS