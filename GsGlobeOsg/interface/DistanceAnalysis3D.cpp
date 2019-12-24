#include "DistanceAnalysis3D.h"
#include "Analysis/Distance3DTrakerElement.h"

#include "innerimp.h"
#include <AnalysisCallBack.h>

GLOBE_NS

DistanceAnalysis3D::DistanceAnalysis3D()
{
	osg::ref_ptr<Distance3DTrakerElement> ptrDistanceAnalysis3D = new Distance3DTrakerElement();
	m_ptrAnalysisHandle = new ReferenceImp<Distance3DTrakerElement>(ptrDistanceAnalysis3D);

	m_AnalysisType = eDistanceAnalysis3D;
}

UTILITY_NAME::GsDelegate<void(double)>& DistanceAnalysis3D::OnFinish()
{
	return Handle()->CastTo<osg::ref_ptr<Distance3DTrakerElement> >()->OnFinish;
}

DistanceAnalysis3D::~DistanceAnalysis3D()
{

}

GLOBE_ENDNS

