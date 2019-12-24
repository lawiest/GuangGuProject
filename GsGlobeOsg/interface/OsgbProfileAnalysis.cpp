#include "OsgbProfileAnalysis.h"
#include "innerimp.h"
#include "Analysis/OsgbProfileTrakerElement.h"
#include "OsgCoreSingleton.h"
#include <osgEarth/GeoTransform>

GLOBE_NS

OsgbProfileAnalysis::OsgbProfileAnalysis(Layer* ptrLayer, bool bAutoCalc, double dbStep )
{
	osg::ref_ptr<osgEarth::GeoTransform> ptrNode = ptrLayer->Handle()->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >();

	osg::ref_ptr<OsgbProfileTrakerElement> ptrOsgbProfileAnalysis = new OsgbProfileTrakerElement(ptrNode, bAutoCalc, dbStep);
	m_ptrAnalysisHandle = new ReferenceImp<OsgbProfileTrakerElement>(ptrOsgbProfileAnalysis);
	m_AnalysisType = eOsgbProfileAnalysis;
}

void OsgbProfileAnalysis::Calculate(double dbStep)
{
	if (m_ptrAnalysisHandle == NULL)
		return;

	osg::ref_ptr<OsgbProfileTrakerElement> ptrOsgbProfileAnalysis = Handle()->CastTo<osg::ref_ptr<OsgbProfileTrakerElement> >();
	if (ptrOsgbProfileAnalysis)
	{
		ptrOsgbProfileAnalysis->Calculate(dbStep);
	}
}

UTILITY_NAME::GsDelegate<void()>& OsgbProfileAnalysis::OnDrawFinish()
{
	return Handle()->CastTo<osg::ref_ptr<OsgbProfileTrakerElement> >()->OnDrawFinish;
}

UTILITY_NAME::GsDelegate<void(const std::vector<double>&, const std::vector<double>&)>& OsgbProfileAnalysis::OnCalcFinish()
{
	return Handle()->CastTo<osg::ref_ptr<OsgbProfileTrakerElement> >()->OnCalcFinish;
}

OsgbProfileAnalysis::~OsgbProfileAnalysis()
{

}

GLOBE_ENDNS