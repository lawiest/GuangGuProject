#include "MarathonAnalysis.h"
#include "Analysis/MarathonTrakerElement.h"

#include "innerimp.h"
#include <AnalysisCallBack.h>


GLOBE_NS

MarathonAnalysis::MarathonAnalysis()
{
	osg::ref_ptr<MarathonTrakerElement> ptrDistanceAnalysis3D = new MarathonTrakerElement();
	m_ptrAnalysisHandle = new ReferenceImp<MarathonTrakerElement>(ptrDistanceAnalysis3D);

	m_AnalysisType = eMarathonAnalysis;
}

MarathonAnalysis::~MarathonAnalysis()
{

}

GLOBE_ENDNS

