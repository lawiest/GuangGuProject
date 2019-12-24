#include "QtFramework/Tracker/Tracker.h"
#include "qt\geospace2d.h"
#include <sys/timeb.h> 

GLOBE_NS

long long getSystemTime() {
	timeb t;
	ftime(&t);
	return t.time * 1000 + t.millitm;
}

void FormaterLen(double len, std::string &txt)
{
	if (len < 1000)
	{
		char buf[30];
		sprintf(buf, "%.1f米", len);
		txt = buf;
	}
	else {
		char buf[30];
		sprintf(buf, "%.1f公里", len/1000.0);
		txt = buf;
	}
}
void FormaterArea(double len, std::string &txt)
{
	if (len < 1000000)
	{
		char buf[30];
		sprintf(buf, "%.1f平方米", len);
		txt = buf;
	}
	else {
		char buf[30];
		sprintf(buf, "%.1f平方公里", len / 1000000.0);
		txt = buf;
	}
}

GsZoomPanTracker::GsZoomPanTracker(GeoStar::Kernel::QT::GsGeoSpace2D* pMap):GeoStar::Kernel::GsSymbolTracker(pMap->Map())
{
	m_ptrGeoSpace2D = pMap;
	m_isMoved = false;
}

GsZoomPanTracker::~GsZoomPanTracker()
{
}

void GsZoomPanTracker::OnMouseDown(GsButton eButton, int nKeyboard, const Utility::GsPT & pt)
{
	if (eButton == GsButton::eLeftButton)
	{
		m_isMoved = true;
		GsRawPoint p = Map()->ScreenDisplay()->DisplayTransformation()->ToMap(pt);
		Map()->ScreenDisplay()->PanStart(p.X, p.Y);
	}
}

void GsZoomPanTracker::OnMouseUp(GsButton eButton, int nKeyboard, const Utility::GsPT & pt)
{
	if (!m_isMoved)
		return;
	GsBox box = Map()->ScreenDisplay()->PanStop();
	Map()->ViewExtent(box);
	m_isMoved = false;
	UpdateMap();
}

void GsZoomPanTracker::OnMouseMove(GsButton eButton, int nKeyboard, const Utility::GsPT & pt)
{
	if (!m_isMoved)
		return;
	if (!Map()->ScreenDisplay()->HasStartPan())
		return;

	GsRawPoint p = Map()->ScreenDisplay()->DisplayTransformation()->ToMap(pt);
	Map()->ScreenDisplay()->PanMoveTo(p.X, p.Y);
}

void GsZoomPanTracker::OnMouseWheel(GsButton eButton, int nDelta, int nKeyboard, const Utility::GsPT & pt)
{
	GsBox box = Map()->ViewExtent();
	double dblScale = nDelta < 0 ? 1.2 : 1 / 1.2;

	GsRawPoint p = Map()->ScreenDisplay()->DisplayTransformation()->ToMap(pt);
	Map()->ViewExtent(box.Scale(p, dblScale));
	UpdateMap();
}


void GsZoomPanTracker::UpdateMap()
{
	m_ptrGeoSpace2D->Update();
}

GeoStar::Kernel::QT::GsGeoSpace2D * GsZoomPanTracker::GeoSpace2D() const
{
	return m_ptrGeoSpace2D;
}





GsPolylineTracker::GsPolylineTracker(GeoStar::Kernel::QT::GsGeoSpace2D * pMap):GsZoomPanTracker(pMap)
{
	m_MaxCount = -1;
	m_MinCount = -1;
	m_isNeedAdd = false;
	m_MouseTime = 0;
	m_DownX = 0;
	m_DownY = 0;
	m_isFinished = false;
	m_IsMouseDown = false;
}

GsPolylineTracker::~GsPolylineTracker()
{

}

void GsPolylineTracker::Reset()
{
	m_Points.clear();
	m_isNeedAdd = (m_MaxCount == 1);
	m_isFinished = false;
	UpdateMap();
}

int GsPolylineTracker::MaxPointCount() const
{
	return m_MaxCount;
}

void GsPolylineTracker::MaxPointCount(int maxpoint)
{
	m_MaxCount = maxpoint;
}

int GsPolylineTracker::MinPointCount() const
{
	return m_MinCount;
}

void GsPolylineTracker::MinPointCount(int minPoint)
{
	m_MinCount = minPoint;
}

void GsPolylineTracker::OnMouseDown(GsButton eButton, int nKeyboard, const Utility::GsPT & pt)
{
	GsZoomPanTracker::OnMouseDown(eButton, nKeyboard, pt);
	if (m_isFinished)
		return;
	m_IsMouseDown = true;
	if (eButton == GsButton::eLeftButton || eButton == GsButton::eRightButton)
	{
		m_DownX = pt.X;
		m_DownY = pt.Y;
		m_MouseTime = getSystemTime();
		
	}
}

void GsPolylineTracker::OnMouseUp(GsButton eButton, int nKeyboard, const Utility::GsPT & pt)
{
	GsZoomPanTracker::OnMouseUp(eButton, nKeyboard, pt);
	m_IsMouseDown = false;
	if (m_isFinished)
		return;
	long upTime = getSystemTime();
	float dx = pt.X - m_DownX;
	float dy = pt.Y - m_DownY;
	bool isSingleClick = (upTime - m_MouseTime < 500) && (dx*dx + dy*dy < 300);
	if (eButton == GsButton::eLeftButton && isSingleClick)
	{
		MouseLeftClick(eButton, nKeyboard, pt);
	}
	else if (eButton == GsButton::eRightButton && isSingleClick)
	{
		MouseRightClick(eButton, nKeyboard, pt);
	}
}

void GsPolylineTracker::OnMouseMove(GsButton eButton, int nKeyboard, const Utility::GsPT & pt)
{
	GsZoomPanTracker::OnMouseMove(eButton, nKeyboard, pt);
	if (m_isFinished || m_IsMouseDown)
		return;
	GsRawPoint p = Map()->ScreenDisplay()->DisplayTransformation()->ToMap(pt);
	if (m_isNeedAdd)
	{
		if (m_Points.size() != MaxPointCount())
			m_Points.push_back(p);
		m_isNeedAdd = false;
		return;
	}
	if (m_Points.size() == 0)
		return;
	m_Points[m_Points.size() - 1] = p;
	UpdateMap();
}

void GsPolylineTracker::OnTrackerDraw(GsDisplay * pDisp)
{
	if (m_Points.size() == 0)
		return;
	OnDraw(pDisp, m_Points);
}

void GsPolylineTracker::FinishTracker()
{
	m_isFinished = true;
	OnFinishTracker(m_Points);
}

void GsPolylineTracker::OnFinishTracker(const std::vector<GsRawPoint>& points)
{

}

bool GsPolylineTracker::MouseLeftClick(GsButton eButton, int nKeyboard, const Utility::GsPT & pt)
{
	//
	if (m_Points.size() == MaxPointCount())
	{
		FinishTracker();
		return true;
	}
	if (m_Points.size() == 0)
	{
		GsRawPoint p = Map()->ScreenDisplay()->DisplayTransformation()->ToMap(pt);
		m_Points.push_back(p);
	}
	m_isNeedAdd = true;
	return false;
}

bool GsPolylineTracker::MouseRightClick(GsButton eButton, int nKeyboard, const Utility::GsPT & pt)
{
	return false;
}

bool GsPolylineTracker::OnDraw(GsDisplay * pDisp, const std::vector<GsRawPoint>& points)
{
	return false;
}

// 测量Tracker 编辑过程
GsMeasureTracker::GsMeasureTracker(GeoStar::Kernel::QT::GsGeoSpace2D * pMap):GsPolylineTracker(pMap)
{
	m_MeasureType = GsMeasureType::Length;
	MinPointCount(2);
	m_ptrLineSymbol = new GeoStar::Kernel::GsSimpleLineSymbol(GsColor::Red, 0.5);
	m_ptrFillSymbol = new GeoStar::Kernel::GsSimpleFillSymbol(GsColor::FromARGB(0,0,255,128));
	//
	m_ptrGsGeoLine = new GsGeodesic(pMap->Map()->ScreenDisplay()->DisplayTransformation()->SpatialReference());
	m_ptrGsGeodesicPoly = new GsGeodesicPolygon(pMap->Map()->ScreenDisplay()->DisplayTransformation()->SpatialReference());
	//
	m_ptrTextBrush = NULL;
	m_ptrTextFormat = NULL;
}

GsMeasureTracker::~GsMeasureTracker()
{
	delete m_ptrGsGeoLine;
	delete m_ptrGsGeodesicPoly;
}

GsMeasureType GsMeasureTracker::MeasureType() const
{
	return m_MeasureType;
}

void GsMeasureTracker::MeasureType(GsMeasureType type)
{
	m_MeasureType = type;
}

bool GsMeasureTracker::MouseRightClick(GsButton eButton, int nKeyboard, const Utility::GsPT & pt)
{
	FinishTracker();
	return false;
}

bool GsMeasureTracker::OnDraw(GsDisplay * pDisp, const std::vector<GsRawPoint>& points)
{
	if (m_MeasureType == GsMeasureType::Length)
	{
		OnDrawLength(pDisp, points);
	}
	else if (m_MeasureType == GsMeasureType::Area)
	{
		OnDrawArea(pDisp, points);
	}
	return false;
}

void GsMeasureTracker::OnDrawLength(GsDisplay * pDisp, const std::vector<GsRawPoint>& points)
{
	if (points.size() < 2)return;
	GsPathPtr path = GeoStar::Kernel::GsGeometryFactory::CreatePath(points.data(), points.size());
	m_ptrLineSymbol->StartDrawing(pDisp->Canvas(), pDisp->DisplayTransformation());
	m_ptrLineSymbol->Draw(path);
	m_ptrLineSymbol->EndDrawing();
	double len = 0;
	double coords[4];
	coords[0] = points[0].X;
	coords[1] = points[1].Y;
	std::string txt;
	for (int i = 1, size = points.size(); i < size; i++)
	{
		coords[2] = points[i].X;
		coords[3] = points[i].Y;
		len += m_ptrGsGeoLine->ComputeGeodesic(coords, 4, 2);
	   Utility::GsPTF pt=	Map()->ScreenDisplay()->DisplayTransformation()->FromMap(coords[2], coords[3]);
	   FormaterLen(len, txt);
	    OnDrawText(pDisp, txt, pt);
		coords[0] = coords[2];
		coords[1] = coords[3];
	}
}

void GsMeasureTracker::OnDrawArea(GsDisplay * pDisp, const std::vector<GsRawPoint>& points)
{
	if (points.size() < 2)
		return;
	GsPathPtr path = GeoStar::Kernel::GsGeometryFactory::CreatePath(points.data(), points.size());
	GsSymbolPtr symb = NULL;
	//
	if (points.size() > 2)
		symb = m_ptrFillSymbol.p;
	else
		symb = m_ptrLineSymbol.p;

	symb->StartDrawing(pDisp->Canvas(), pDisp->DisplayTransformation());
	symb->Draw(path);
	symb->EndDrawing();
	if (points.size() < 3)
		return;
	m_ptrGsGeodesicPoly->Reset();
	GsRawPoint minPoint = points[0];
	GsRawPoint maxPoint = points[0];
	for (int i = 0, size = points.size(); i < size; i++)
	{
		m_ptrGsGeodesicPoly->AddPoint(points[i].X, points[i].Y);
		//
		if (minPoint.X > points[i].X)
			minPoint.X = points[i].X;
		else if (maxPoint.X < points[i].X)
			maxPoint.X = points[i].X;
		//
		if (minPoint.Y > points[i].Y)
			minPoint.Y = points[i].Y;
		else if (maxPoint.Y < points[i].Y)
			maxPoint.Y = points[i].Y;
	}
	m_ptrGsGeodesicPoly->AddPoint(points[0].X, points[0].Y);
	double area = 0;
	//计算面积
	m_ptrGsGeodesicPoly->Compute(false, false, &area);
	Utility::GsPTF pt = Map()->ScreenDisplay()->DisplayTransformation()->FromMap((minPoint.X + maxPoint.X)/2, (minPoint.Y + maxPoint.Y)/2);
	std::string txt;
	//绘制面积文本
	FormaterArea(area, txt);
	OnDrawText(pDisp, txt, pt);
}

void GsMeasureTracker::OnDrawText(GsDisplay * pDisp, const std::string & txt, const Utility::GsPTF & pt)
{
	GsCanvasPtr ptrCanvas = pDisp->Canvas();
	if (!m_ptrTextFormat)
	{
		m_ptrTextFormat = ptrCanvas->CreateStringFormat();
		m_ptrTextFormat->Font(Utility::GsUtf8("微软雅黑").Str().c_str());
		m_ptrTextFormat->FontSize(10);
	}
	if (!m_ptrTextBrush)
	{
		m_ptrTextBrush= ptrCanvas->CreateSolidBrush(GsColor::Blue);
	}
	ptrCanvas->DrawString(Utility::GsUtf8( txt).Str().c_str(), txt.length(), m_ptrTextBrush, pt, m_ptrTextFormat);
}

//绘制Tracker 
GsDrawSymbolTracker::GsDrawSymbolTracker(GeoStar::Kernel::QT::GsGeoSpace2D * pMap):GsPolylineTracker(pMap)
{
	Symbol(new GeoStar::Kernel::GsSimpleLineSymbol(GsColor::Red, 0.3));
}

GsDrawSymbolTracker::~GsDrawSymbolTracker()
{

}

bool GsDrawSymbolTracker::OnDraw(GsDisplay * pDisp, const std::vector<GsRawPoint>& points)
{
	GeoStar::Kernel::GsSymbol * symbol = Symbol();
	if(!symbol)
	return false;
	GsPathPtr path= GeoStar::Kernel::GsGeometryFactory::CreatePath(points.data(), points.size());
	symbol->StartDrawing(pDisp->Canvas(), pDisp->DisplayTransformation());
	symbol->Draw(path);
	symbol->EndDrawing();
	return true;
}
bool GsDrawSymbolTracker::MouseRightClick(GsButton eButton, int nKeyboard, const Utility::GsPT & pt)
{
	FinishTracker();
	return false;
}
void GsDrawSymbolTracker::OnFinishTracker(const std::vector<GsRawPoint>& points)
{
	GsPathPtr path = GeoStar::Kernel::GsGeometryFactory::CreatePath(points.data(), points.size());
	//事件调用触发
	OnDrawSymbolFinished(path);
}
GLOBE_ENDNS