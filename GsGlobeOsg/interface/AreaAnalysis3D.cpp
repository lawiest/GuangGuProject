#include "AreaAnalysis3D.h"
#include "innerimp.h"
#include "Analysis/Area3DTrakerElement.h"
#include "OsgCoreSingleton.h"

GLOBE_NS

AreaAnalysis3D::AreaAnalysis3D()
{
	osg::ref_ptr<Area3DTrakerElement> ptrAreaAnalysis3D = new Area3DTrakerElement(OsgCoreSingleton::Instance()->MapNode());
	m_ptrAnalysisHandle = new ReferenceImp<Area3DTrakerElement>(ptrAreaAnalysis3D);
	m_AnalysisType = eAreaAnalysis3D;
}

UTILITY_NAME::GsDelegate<void(double)>& AreaAnalysis3D::OnFinish()
{
	return Handle()->CastTo<osg::ref_ptr<Area3DTrakerElement> >()->OnFinish;
}

AreaAnalysis3D::~AreaAnalysis3D()
{

}

GLOBE_ENDNS