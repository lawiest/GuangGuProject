#include "LineOfSightAnalysis.h"
#include "Analysis/LinearLineOfSightAnalysis.h"
#include "innerimp.h"
#include "OsgCoreSingleton.h"

GLOBE_NS

LineOfSightAnalysis::LineOfSightAnalysis()
{
	osg::ref_ptr<GeoGlobe::LinearLineOfSightAnalysis> ptrLineOfSightAnalysis = new GeoGlobe::LinearLineOfSightAnalysis(OsgCoreSingleton::Instance()->MapNode());

	m_ptrAnalysisHandle = new ReferenceImp<GeoGlobe::LinearLineOfSightAnalysis>(ptrLineOfSightAnalysis);

	m_AnalysisType = eLineOfSightAnalysis;
}


LineOfSightAnalysis::~LineOfSightAnalysis()
{

}

GLOBE_ENDNS
