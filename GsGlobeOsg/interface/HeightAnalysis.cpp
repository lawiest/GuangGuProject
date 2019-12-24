#include "HeightAnalysis.h"
#include "innerimp.h"
#include "Analysis/LineTraker3DElement.h"
#include "OsgCoreSingleton.h"

GLOBE_NS

HeightAnalysis::HeightAnalysis()
{
	osg::ref_ptr<GeoGlobe::LineTraker3DElement> ptrHeightAnalysis = new GeoGlobe::LineTraker3DElement(OsgCoreSingleton::Instance()->MapNode(), GeoGlobe::LineTraker3DElement::DragMode::DRAGMODE_VERTICAL);
	ptrHeightAnalysis->setCullingActive(false);
	m_ptrAnalysisHandle = new ReferenceImp<GeoGlobe::LineTraker3DElement>(ptrHeightAnalysis);

	m_AnalysisType = eHeighAnalysis;
}

UTILITY_NAME::GsDelegate<void(double)>& HeightAnalysis::OnFinish()
{
	return Handle()->CastTo<osg::ref_ptr<GeoGlobe::LineTraker3DElement> >()->OnFinish;
}

HeightAnalysis::~HeightAnalysis()
{

}

GLOBE_ENDNS
