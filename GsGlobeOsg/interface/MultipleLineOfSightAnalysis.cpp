#include "MultipleLineOfSightAnalysis.h"
#include "innerimp.h"
#include "OsgCoreSingleton.h"
#include "Analysis/MultipleLinearLineOfSightAnalysis.h"

GLOBE_NS

MultipleLineOfSightAnalysis::MultipleLineOfSightAnalysis()
{
	osg::ref_ptr<GeoGlobe::MultipleLinearLineOfSightAnalysis> ptrMultipleLineOfSightAnalysis = new GeoGlobe::MultipleLinearLineOfSightAnalysis(OsgCoreSingleton::Instance()->MapNode());

	m_ptrAnalysisHandle = new ReferenceImp<GeoGlobe::MultipleLinearLineOfSightAnalysis>(ptrMultipleLineOfSightAnalysis);

	m_AnalysisType = eMultipleLineOfSightAnalysis;
}

MultipleLineOfSightAnalysis::~MultipleLineOfSightAnalysis()
{

}

GLOBE_ENDNS