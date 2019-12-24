#include "OPInRangeAnalysis.h"
#include "innerimp.h"
#include "Analysis/ObserverPointsInRangeAnalysis.h"
#include "OsgCoreSingleton.h"

GLOBE_NS

OPInRangeAnalysis::OPInRangeAnalysis()
{
	osg::ref_ptr<GeoGlobe::ObserverPointsInRangeAnalysis> ptrOPInRangeAnalysis = new GeoGlobe::ObserverPointsInRangeAnalysis(OsgCoreSingleton::Instance()->MapNode());

	m_ptrAnalysisHandle = new ReferenceImp<GeoGlobe::ObserverPointsInRangeAnalysis>(ptrOPInRangeAnalysis);

	m_AnalysisType = eOPInRangeAnalysis;
}

OPInRangeAnalysis::~OPInRangeAnalysis()
{

}

GLOBE_ENDNS