#include "GeoMapTools.h"
#include <osg/MatrixTransform>
#include <osg/Depth>
#include <osg/LineWidth>
#include "globe/OsgCoreSingleton.h"

namespace GeoGlobe
{
	GeoMapTool::GeoMapTool(osgEarth::MapNode* mapNode) :GUIEventHandler(),
		m_isEnable(false),
		m_isOpenMove(false)
	{
		m_mapNode = mapNode;
	}

	GeoMapTool::~GeoMapTool()
	{
	}

	bool GeoMapTool::handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa)
	{
		if (!m_isEnable)
			return false;
		osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
		if (!view)
			return false;
		m_ptrCurView = view;
		if (!m_mapNode.valid())
			return false;
		if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			KERNEL_NAME::GsRawPoint3D point;
			if (getLocationAt(view, ea.getX(), ea.getY(), point))
				MouseDoubleClick(ea, point);
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE&& m_isOpenMove)
		{
			KERNEL_NAME::GsRawPoint3D point;
			if (getLocationAt(view, ea.getX(), ea.getY(), point))
				onMouseMove(ea, point);
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG&&m_isOpenMove)
		{

				KERNEL_NAME::GsRawPoint3D point;
				if (getLocationAt(view, ea.getX(), ea.getY(), point))
					return onMouseDrag(ea, point);
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH)
		{
			KERNEL_NAME::GsRawPoint3D point;
			if (getLocationAt(view, ea.getX(), ea.getY(), point))
				onMousePress(ea, point);
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)
		{
			KERNEL_NAME::GsRawPoint3D point;
			if (getLocationAt(view, ea.getX(), ea.getY(), point))
				onMouseUp(ea, point);
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
		{
			m_CurPressKey.push_back(ea.getKey());
			onKeyDown(ea);
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::KEYUP)
		{
			onKeyUp(ea);
			// 移除键盘存储
			std::vector<int>::iterator it = std::find(m_CurPressKey.begin(), m_CurPressKey.end(), ea.getKey());
			if (it != m_CurPressKey.end())
				m_CurPressKey.erase(it);
		}
		return false;
	}

	bool GeoMapTool::onMousePress(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D& point)
	{
		return false;
	}

	bool GeoMapTool::onMouseMove(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D& point)
	{
		return false;
	}

	bool GeoMapTool::onMouseUp(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D& point)
	{
		return false;
	}

	void GeoMapTool::SetEnable(bool enable)
	{
		m_isEnable = enable;
	}

	void GeoMapTool::SetMonitorMouseMove(bool enable)
	{
		m_isOpenMove = enable;
	}

	bool GeoMapTool::Enable()
	{
		return m_isEnable;
	}

	void GeoMapTool::setMapNode(osgEarth::MapNode * mapNode)
	{
		m_mapNode = mapNode;
	}

	osgEarth::MapNode * GeoMapTool::getMapNode()
	{
		return m_mapNode;
	}

	bool GeoMapTool::LeftMouseClick(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D& point)
	{
		return false;
	}

	bool GeoMapTool::RightMouseClick(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D& point)
	{
		return false;
	}

	bool GeoMapTool::MouseDoubleClick(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D& point)
	{
		return false;
	}

	bool GeoMapTool::getLocationAt(osgViewer::View * view, double x, double y, KERNEL_NAME::GsRawPoint3D & pt)
	{

		osgUtil::LineSegmentIntersector::Intersections intersections;
		view->computeIntersections((float)(x), (float)y, intersections);

		osg::Matrix inverseMV;
		osg::ref_ptr<osg::Camera> cameraMaster = view->getCamera();
		inverseMV.invert(cameraMaster->getViewMatrix());
		osg::Vec3d eye = osg::Vec3d(0.0, 0.0, 0.0) * inverseMV;

		double dLen2Shortest = DBL_MAX;

		bool bFind = false;

		osgUtil::LineSegmentIntersector::Intersections::iterator hitr, hitrShortest;
		for (hitr = intersections.begin(); hitr != intersections.end(); hitr++)
		{
			// the geodes are identified by name.
			osg::NodePath nodePath = hitr->nodePath;
			for (int i = 0; i < nodePath.size(); i++)
			{
				osg::ref_ptr<osg::Node> node = nodePath[i];
				if (node == NULL)
					continue;
				//if (node->getName() == "OsgbLayer")
				{
					osg::Vec3d p = hitr->getWorldIntersectPoint();
					//hitr->nodePath();
					double dLenTmp = (eye.x() - p.x())*(eye.x() - p.x())
						+ (eye.y() - p.y())*(eye.y() - p.y())
						+ (eye.z() - p.z())*(eye.z() - p.z());

					if (dLen2Shortest > dLenTmp)
					{
						dLen2Shortest = dLenTmp;
						hitrShortest = hitr;
						bFind = true;
					}
				}
			}
		}
		if (bFind)
			WorldToGeoPoint(hitrShortest->getWorldIntersectPoint(), pt);
		return bFind;

	}
	bool GeoMapTool::PointToWorld(KERNEL_NAME::GsRawPoint3D & point, osg::Vec3d & outWorld)
	{
		osgEarth::GeoPoint geoP(m_mapNode->getMapSRS(), point.X, point.Y, point.Z);
		geoP.toWorld(outWorld);
		return true;
	}

	bool GeoMapTool::WorldToGeoPoint(osg::Vec3d & world, KERNEL_NAME::GsRawPoint3D & point)
	{
		osgEarth::GeoPoint geoP;
		geoP.fromWorld(m_mapNode->getMapSRS(), world);
		point.X = geoP.x();
		point.Y = geoP.y();
		point.Z = geoP.z();
		return false;
	}

	GLOBE_NAME::ElementBox * GeoMapTool::GetElementBox()
	{
		return GLOBE_NAME::OsgCoreSingleton::Instance()->getElementBox();
	}

	bool GeoMapTool::isPressKey(int key)
	{
		std::vector<int>::iterator it = std::find(m_CurPressKey.begin(), m_CurPressKey.end(), key);
		return it != m_CurPressKey.end();
	}


	/****************************************/
	//*--           NewPolygonLineTool        */
	/****************************************/

	NewPolygonLineTool::NewPolygonLineTool(osgEarth::MapNode * mapNode) :GeoMapTool(mapNode)
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

	NewPolygonLineTool::~NewPolygonLineTool()
	{
	}

	bool NewPolygonLineTool::onMousePress(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D & point)
	{
		if (m_isFinished)
			return false;
		m_IsMouseDown = true;
		m_MouseTime = ea.getTime();
		m_DownX = ea.getX();
		m_DownY = ea.getY();
		return true;
	}

	bool NewPolygonLineTool::onMouseMove(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D & point)
	{
		if (m_isFinished || m_IsMouseDown)
			return false;
		if (m_isNeedAdd)
		{
			if (m_Points.size() != MaxPointCount())
				m_Points.push_back(point);
			m_isNeedAdd = false;
			return true;
		}
		if (m_Points.size() == 0)
			return true;
		m_Points[m_Points.size() - 1] = point;
		Update();
		return true;
	}

	bool NewPolygonLineTool::onMouseUp(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D & point)
	{
		m_IsMouseDown = false;
		if (m_isFinished)
			return false;
		double upTime = ea.getTime();
		float dx = ea.getX() - m_DownX;
		float dy = ea.getY() - m_DownY;
		bool isSingleClick = (upTime - m_MouseTime < 0.8) && (dx*dx + dy*dy < 300);

		if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && isSingleClick)
		{
			return LeftMouseClick(ea, point);
		}
		else if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON&& isSingleClick)
		{
			return RightMouseClick(ea, point);
		}
		return false;
	}

	void NewPolygonLineTool::SetEnable(bool enable)
	{
		GeoMapTool::SetEnable(enable);
		if (!enable)
			Reset();
	}

	bool NewPolygonLineTool::LeftMouseClick(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D & point)
	{
		if (m_Points.size() == MaxPointCount())
		{
			FinishTracker();
			return true;
		}
		if (m_Points.size() == 0)
		{
			m_Points.push_back(point);
		}
		if (m_Points.size() == MaxPointCount())
		{
			FinishTracker();
			return true;
		}
		m_isNeedAdd = true;
		Update();
		return true;
	}

	bool NewPolygonLineTool::RightMouseClick(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D & point)
	{
		return false;
	}

	void NewPolygonLineTool::FinishTracker()
	{
		m_isFinished = true;
		OnFinishTracker(m_Points);
	}

	void NewPolygonLineTool::OnFinishTracker(const std::vector<KERNEL_NAME::GsRawPoint3D>& points)
	{

	}

	void NewPolygonLineTool::Update()
	{
	}

	void NewPolygonLineTool::Reset()
	{
		m_isOpenMove = false;
		m_Points.clear();
		m_isNeedAdd = (m_MaxCount == 1);
		m_isFinished = false;
		Update();
	}

	int NewPolygonLineTool::MinPointCount() const
	{
		return m_MinCount;
	}

	void NewPolygonLineTool::MinPointCount(int min)
	{
		m_MinCount = min;
	}

	int NewPolygonLineTool::MaxPointCount() const
	{
		return m_MaxCount;
	}

	void NewPolygonLineTool::MaxPointCount(int max)
	{
		m_MaxCount = max;
	}

	/***************************/
	DrawElementTool::DrawElementTool(osgEarth::MapNode * mapNode) :
		NewPolygonLineTool(mapNode)
	{
		mt = new osg::MatrixTransform();
		displayGroup = new osg::Group();
		m_mapNode->addChild(displayGroup);
	}

	osg::Node* DrawElementTool::DisplayGroup()
	{
		return displayGroup;
	}

	DrawElementTool::~DrawElementTool()
	{
		getMapNode()->removeChild(displayGroup);
	}

	void DrawElementTool::OnFinishTracker(const std::vector<KERNEL_NAME::GsRawPoint3D>& points)
	{
		OnDrawFinishEvent(points);
		Reset();
	}

	void DrawElementTool::Update()
	{
		if (!m_SimpleLine)
			CreateFeatrue();
		m_SimpleLine->Clear();
		for (int i = 0; i < m_Points.size(); i++)
		{
			m_SimpleLine->AddPoint(osg::Vec3d(m_Points[i].X, m_Points[i].Y, m_Points[i].Z));
		}
		m_SimpleLine->Init();
	}


	bool DrawElementTool::RightMouseClick(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D & point)
	{
		FinishTracker();
		return true;
	}

	bool DrawElementTool::MouseDoubleClick(const osgGA::GUIEventAdapter & ea, const KERNEL_NAME::GsRawPoint3D & point)
	{
		OnDoubleClickEvent(point);
		return true;
	}

	void DrawElementTool::CreateFeatrue()
	{
		m_SimpleLine = new SimpleLineElement();
		m_SimpleLine->SetLineWidth(2);
		m_SimpleLine->SetLineColor(osg::Vec4(1, 1, 0, 1));
		// Define the path feature:
		//_feature = new  osgEarth::Features::Feature(new osgEarth::Symbology::LineString(), getMapNode()->getMapSRS());
		//_feature->geoInterp() = _geoInterpolation;

		//// clamp to the terrain skin as it pages in
		//osgEarth::Symbology::AltitudeSymbol* alt = _feature->style()->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();

		//// offset to mitigate Z fighting
		//osgEarth::Symbology::RenderSymbol* render = _feature->style()->getOrCreate<osgEarth::Symbology::RenderSymbol>();
		//render->depthOffset()->enabled() = true;
		//render->depthOffset()->automatic() = true;

		//// define a style for the line
		//osgEarth::Symbology::LineSymbol* ls = _feature->style()->getOrCreate<osgEarth::Symbology::LineSymbol>();
		//ls->stroke()->color() = osgEarth::Symbology::Color::Yellow;
		//ls->stroke()->width() = 5.0f;
		//ls->stroke()->widthUnits() = osgEarth::Symbology::Units::PIXELS;
		//ls->tessellation() = 150;

		//_featureNode = new osgEarth::Annotation::FeatureNode(getMapNode(), _feature.get());
		//_featureNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

		displayGroup->addChild(m_SimpleLine);
	}

	void DrawElementTool::Clear()
	{
		m_SimpleLine->Clear();
		m_SimpleLine->Init();
		//_feature->getGeometry()->clear();
		//_featureNode->init();
	}
	/************************绘制矩形框的绘制过程***********************/
	//
	void CreateRectPoints(const std::vector<KERNEL_NAME::GsRawPoint3D>& points, std::vector<KERNEL_NAME::GsRawPoint3D>& outPoints)
	{
		double minX = points[0].X;
		double maxX = points[1].X;
		if (maxX < minX)
		{
			maxX = minX;
			minX = points[1].X;
		}
		double minY = points[0].Y;
		double maxY = points[1].Y;
		//
		if (maxY < minY)
		{
			maxY = minY;
			minY = points[1].Y;
		}
		double z = points[0].Z;
		//
		outPoints.push_back(KERNEL_NAME::GsRawPoint3D(minX, minY, z));
		outPoints.push_back(KERNEL_NAME::GsRawPoint3D(minX, maxY, z));
		outPoints.push_back(KERNEL_NAME::GsRawPoint3D(maxX, maxY, z));
		outPoints.push_back(KERNEL_NAME::GsRawPoint3D(maxX, minY, z));
		outPoints.push_back(KERNEL_NAME::GsRawPoint3D(minX, minY, z));
	}
	//
	DrawRectElementTool::DrawRectElementTool(osgEarth::MapNode * mapNode)
		:DrawElementTool(mapNode)
	{
		MaxPointCount(2);//两个点绘制的
	}

	DrawRectElementTool::~DrawRectElementTool()
	{
	}

	void DrawRectElementTool::OnFinishTracker(const std::vector<KERNEL_NAME::GsRawPoint3D>& points)
	{
		if (points.size() < m_MaxCount&&m_MaxCount > -1)
		{
			Reset();
			return;
		}
		if (m_DrawModel == 4)
		{
			std::vector<KERNEL_NAME::GsRawPoint3D> temPoints;
			CreateRectPoints(points, temPoints);
			OnDrawFinishEvent(temPoints);
		}
		else {
			OnDrawFinishEvent(points);
		}
		Reset();
	}

	void DrawRectElementTool::Update()
	{
		if (m_DrawModel != 4)
		{
			DrawElementTool::Update();
			return;
		}
		if (!m_SimpleLine)
			CreateFeatrue();
		m_SimpleLine->Clear();
		// 两个点生成四边形的绘制 
		if (m_Points.size() < m_MaxCount&&m_MaxCount > -1)
		{
			m_SimpleLine->Init();
			return;
		}

		std::vector<KERNEL_NAME::GsRawPoint3D> temPoints;
		CreateRectPoints(m_Points, temPoints);
		for (int i = 0; i < temPoints.size(); i++)
		{
			m_SimpleLine->AddPoint(osg::Vec3d(temPoints[i].X, temPoints[i].Y, temPoints[i].Z));
		}
		m_SimpleLine->Init();
	}

	int DrawRectElementTool::GetDrawModel()
	{
		return m_DrawModel;
	}

	void DrawRectElementTool::SetDrawModel(int model)
	{
		if (m_DrawModel == model)
			return;
		if (model == 1)
			MaxPointCount(1);
		else if (model == 4)
			MaxPointCount(2);
		else
			MaxPointCount(-1);
		m_DrawModel = model;
		Reset();
	}
}



