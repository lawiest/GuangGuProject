#include "VolumeAnalysis.h"
#include "innerimp.h"
#include "Analysis/VolumeTrakerElement.h"
#include "OsgCoreSingleton.h"
#include <osgEarth/GeoTransform>

GLOBE_NS

VolumeAnalysis::VolumeAnalysis(Layer* ptrLayer, bool bAutoCalc, double dbElevation)
{
	osg::ref_ptr<osgEarth::GeoTransform> ptrNode = ptrLayer->Handle()->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >();

	osg::ref_ptr<VolumeTrakerElement> ptrVolumeAnalysis = new VolumeTrakerElement(ptrNode,dbElevation, bAutoCalc);
	m_ptrAnalysisHandle = new ReferenceImp<VolumeTrakerElement>(ptrVolumeAnalysis);
	m_AnalysisType = eVolumeAnalysis;
}

void VolumeAnalysis::Calculate(double dbElevation)
{
	if (m_ptrAnalysisHandle == NULL)
		return;

	osg::ref_ptr<VolumeTrakerElement> ptrVolumeAnalysis = Handle()->CastTo<osg::ref_ptr<VolumeTrakerElement> >();
	if (ptrVolumeAnalysis)
	{
		ptrVolumeAnalysis->Calculate(dbElevation);
	}
}

UTILITY_NAME::GsDelegate<void()>& VolumeAnalysis::OnDrawFinish()
{
	return Handle()->CastTo<osg::ref_ptr<VolumeTrakerElement> >()->OnDrawFinish;
}

UTILITY_NAME::GsDelegate<void(double)>& VolumeAnalysis::OnCalcFinish()
{
	return Handle()->CastTo<osg::ref_ptr<VolumeTrakerElement> >()->OnCalcFinish;
}

VolumeAnalysis::~VolumeAnalysis()
{

}

GLOBE_ENDNS