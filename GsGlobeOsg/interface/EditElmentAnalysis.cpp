#include "EditElmentAnalysis.h"
#include "Analysis/EditElementTools.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <Element.h>

GLOBE_NS

EditElmentAnalysis::EditElmentAnalysis()
{
	osg::ref_ptr<GeoGlobe::EditElementTool> ptrEditElementTool = new GeoGlobe::EditElementTool(OsgCoreSingleton::Instance()->MapNode());
	m_ptrAnalysisHandle = new ReferenceImp<GeoGlobe::EditElementTool>(ptrEditElementTool);

	m_AnalysisType = eEditElmentAnalysis;
}

EditElmentAnalysis::~EditElmentAnalysis()
{

}

void EditElmentAnalysis::SetEnable(bool enable)
{
	osg::ref_ptr<GeoGlobe::EditElementTool> ptrEditElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::EditElementTool> >();
	ptrEditElementTool->SetEnable(enable);
}

void EditElmentAnalysis::SetOpenMouseMove(bool enable)
{
	osg::ref_ptr<GeoGlobe::EditElementTool> ptrEditElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::EditElementTool> >();
	ptrEditElementTool->SetMonitorMouseMove(enable);
}

bool EditElmentAnalysis::Enable()
{
	osg::ref_ptr<GeoGlobe::EditElementTool> ptrEditElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::EditElementTool> >();
	return ptrEditElementTool->Enable();
}

void EditElmentAnalysis::SetSelectElment(Element * edit, bool isEvent)
{
	osg::ref_ptr<GeoGlobe::EditElementTool> ptrEditElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::EditElementTool> >();
	ptrEditElementTool->SetSelected(edit);
}

UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*, GLOBE_NAME::Element*)>& EditElmentAnalysis::OnSelectEvent()
{
	osg::ref_ptr<GeoGlobe::EditElementTool> ptrEditElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::EditElementTool> >();
	return ptrEditElementTool->OnSelectEvent;
}

UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*)>& EditElmentAnalysis::OnEditElementEvent()
{
	// TODO: 在此处插入 return 语句
	osg::ref_ptr<GeoGlobe::EditElementTool> ptrEditElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::EditElementTool> >();
	return ptrEditElementTool->OnEditElementEvent;
}

UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*)>& EditElmentAnalysis::OnEndEditElementEvent()
{
	// TODO: 在此处插入 return 语句
	osg::ref_ptr<GeoGlobe::EditElementTool> ptrEditElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::EditElementTool> >();
	return ptrEditElementTool->OnEndEditElementEvent;
}

UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*)>& EditElmentAnalysis::OnDeleteElementEvent()
{
	// TODO: 在此处插入 return 语句
	osg::ref_ptr<GeoGlobe::EditElementTool> ptrEditElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::EditElementTool> >();
	return ptrEditElementTool->OnDeleteElementEvent;
}

GLOBE_ENDNS
