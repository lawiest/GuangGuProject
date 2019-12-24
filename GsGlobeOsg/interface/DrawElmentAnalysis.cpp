#include "globe\DrawElmentAnalysis.h"
#include "Analysis\GeoMapTools.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
GLOBE_NS


DrawElmentAnalysis::DrawElmentAnalysis()
{
	osg::ref_ptr<GeoGlobe::DrawElementTool> ptrDistanceAnalysis3D = new GeoGlobe::DrawElementTool(OsgCoreSingleton::Instance()->MapNode());
	m_ptrAnalysisHandle = new ReferenceImp<GeoGlobe::DrawElementTool>(ptrDistanceAnalysis3D);

	m_AnalysisType = eDrawElementTool;
}

DrawElmentAnalysis::~DrawElmentAnalysis()
{

}
void DrawElmentAnalysis::SetEnable(bool enable)
{
	osg::ref_ptr<GeoGlobe::DrawElementTool> ptrDrawElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawElementTool>>();
	ptrDrawElementTool->SetEnable(enable);
}

void DrawElmentAnalysis::SetMonitorMouseMove(bool enable)
{
	osg::ref_ptr<GeoGlobe::DrawElementTool> ptrDrawElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawElementTool>>();
	ptrDrawElementTool->SetMonitorMouseMove(enable);
}

bool DrawElmentAnalysis::Enable()
{
	osg::ref_ptr<GeoGlobe::DrawElementTool> ptrDrawElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawElementTool>>();
	return ptrDrawElementTool->Enable();
}

int DrawElmentAnalysis::MinPointCount() 
{
	osg::ref_ptr<GeoGlobe::DrawElementTool> ptrDrawElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawElementTool>>();
	return ptrDrawElementTool->MinPointCount();
}
void DrawElmentAnalysis::MinPointCount(int min)
{
	osg::ref_ptr<GeoGlobe::DrawElementTool> ptrDrawElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawElementTool>>();
	ptrDrawElementTool->MinPointCount(min);
}
int DrawElmentAnalysis::MaxPointCount()
{
	osg::ref_ptr<GeoGlobe::DrawElementTool> ptrDrawElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawElementTool>>();
	return ptrDrawElementTool->MaxPointCount();
}
void DrawElmentAnalysis::MaxPointCount(int max)
{
	osg::ref_ptr<GeoGlobe::DrawElementTool> ptrDrawElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawElementTool>>();
	ptrDrawElementTool->MaxPointCount(max);
}


UTILITY_NAME::GsDelegate<void(const std::vector<KERNEL_NAME::GsRawPoint3D>&)>& DrawElmentAnalysis::OnDrawFinishEvent()
{
	// TODO: 在此处插入 return 语句
	osg::ref_ptr<GeoGlobe::DrawElementTool> ptrDrawElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawElementTool>>();
	return ptrDrawElementTool->OnDrawFinishEvent;
}

UTILITY_NAME::GsDelegate<void(const KERNEL_NAME::GsRawPoint3D&)>& DrawElmentAnalysis::OnMouseDoubleClickEvent()
{
	osg::ref_ptr<GeoGlobe::DrawElementTool> ptr = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawElementTool>>();
	return ptr->OnDoubleClickEvent;
}

DrawRectElmentAnalysis::DrawRectElmentAnalysis() 
{
	osg::ref_ptr<GeoGlobe::DrawElementTool> ptrDistanceAnalysis3D = new GeoGlobe::DrawRectElementTool(OsgCoreSingleton::Instance()->MapNode());
	m_ptrAnalysisHandle = new ReferenceImp<GeoGlobe::DrawElementTool>(ptrDistanceAnalysis3D);
	m_AnalysisType = eDrawElementTool;
}

DrawRectElmentAnalysis::~DrawRectElmentAnalysis()
{
}
DrawElementType DrawRectElmentAnalysis::GetDrawType()
{
	osg::ref_ptr<GeoGlobe::DrawRectElementTool> ptrDrawElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawRectElementTool>>();
	return DrawElementType(ptrDrawElementTool->GetDrawModel());
}
void DrawRectElmentAnalysis::SetDrawType(DrawElementType type)
{
	osg::ref_ptr<GeoGlobe::DrawRectElementTool> ptrDrawElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawRectElementTool>>();
	ptrDrawElementTool->SetDrawModel(type);
}
bool DrawRectElmentAnalysis::isKeyPress(int key)
{
	osg::ref_ptr<GeoGlobe::DrawRectElementTool> ptrDrawElementTool = Handle()->CastTo<osg::ref_ptr<GeoGlobe::DrawRectElementTool>>();
	return ptrDrawElementTool->isPressKey(key);
}
GLOBE_ENDNS


