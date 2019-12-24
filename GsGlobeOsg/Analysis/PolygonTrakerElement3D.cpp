#include <Analysis/PolygonTrakerElement3D.h>
#include <model/LodModel.h>
#include <PlaceName/GeoBillboard.h>
#include <osgEarth/GLUtils>
#define ANNOTATION_PATH_WIDTH 12.0f

 
#include <osg/ImageStream>
#include <osgEarth/GeoMath>
#include "../SunLigntAnalysis.h"

//class ImageBuilder
//{
//public:
//	ImageBuilder(osgViewer::Viewer* view,int r, int g, int b, osgEarth::Symbology::Polygon& polygon, 
//					osgEarth::Symbology::Polygon& geopolygon,
//		osgEarth::MapNode* pMapNode,osg::Group* modelGroup,  CSunLightAnalysis *pAnalysis, int nDis, int nTime)
//	{
//		/*算法说明*/
//		//涉及到屏幕范围到地理范围的投影和不规则判断
//		//通过地理经纬度判断，得到屏幕上图形大小
//		//将屏幕不规则范围投影到按地理位置经纬度方向摆放的图片上去
//		//最终通过图片上的不规则范围，做一次裁剪，贴到地面
//		/*end*/
//		// 
//		//m_view = view;
//		//m_Polygon = polygon;
//		//m_GeoPolygon = geopolygon;
//		////经纬度polygon
//		//m_nInterval = nDis;
//		//m_pMapNode = pMapNode;
//
//		//Vec3d v00 = LocalToWindow(view,m_pMapNode,Vec3d(geopolygon.getBounds().xMin(), geopolygon.getBounds().yMin(), geopolygon.getBounds().zMin()));
//		//Vec3d v10 = LocalToWindow(view,m_pMapNode,Vec3d(geopolygon.getBounds().xMax(), geopolygon.getBounds().yMin(), geopolygon.getBounds().zMin()));
//		//Vec3d v01 = LocalToWindow(view,m_pMapNode,Vec3d(geopolygon.getBounds().xMin(), geopolygon.getBounds().yMax(), geopolygon.getBounds().zMin()));
//		//Vec3d v11 = LocalToWindow(view,m_pMapNode,Vec3d(geopolygon.getBounds().xMax(), geopolygon.getBounds().yMax(), geopolygon.getBounds().zMin()));
//		//m_nHeight = Distance(v00,v01);
//		//m_nWidth = Distance(v00,v10);
//
//		////水平比率
//		//double x_r = (geopolygon.getBounds().xMax() - geopolygon.getBounds().xMin())/m_nWidth;
//		////垂直比率
//		//double y_r = (geopolygon.getBounds().yMax() - geopolygon.getBounds().yMin())/m_nHeight;
//
//		//m_ImagePolygon.clear();
//		//for(int index = 0; index < geopolygon.size(); index++)
//		//{
//		//	Vec3d vgeoI = geopolygon.at(index);
//		//	Vec3d vI = Vec3d((vgeoI.x() - geopolygon.getBounds().xMin())/x_r,(vgeoI.y() - geopolygon.getBounds().yMin())/y_r,0);
//		//	m_ImagePolygon.push_back(vI);
//		//}
//		//
//		////m_nHeight = m_Polygon.getBounds().height();
//		////m_nWidth = m_Polygon.getBounds().width();
//		//m_pImage = new QImage(m_nWidth,m_nHeight,QImage::Format_ARGB32_Premultiplied);
//		//m_pImage->fill(QColor(r, g, b));
//		//m_Painter.begin(m_pImage);
//
//		//
//		////水平距离
//		//double dblHorDis = GeoMath::distance(Vec3d(geopolygon.getBounds().xMin(), geopolygon.getBounds().yMax(), 0), 
//		//									Vec3d(geopolygon.getBounds().xMax(), geopolygon.getBounds().yMax(), 0),
//		//									m_pMapNode->getMapSRS());
//		////垂直距离
//		//double dblVerDis = GeoMath::distance(Vec3d(geopolygon.getBounds().xMin(), geopolygon.getBounds().yMax(), 0), 
//		//									Vec3d(geopolygon.getBounds().xMax(), geopolygon.getBounds().yMin(), 0),
//		//									m_pMapNode->getMapSRS());
//		//
//		//m_nHorCount = dblHorDis / m_nInterval;
//		//m_nVerCount = dblVerDis / m_nInterval;
//
//		////经纬度
//		//double dblHorInterval = geopolygon.getBounds().width() / m_nHorCount;
//		//double dblVerInterval = geopolygon.getBounds().height() / m_nVerCount;
//		////位图
//		//int nHorInterval = m_nWidth / m_nHorCount;
//		//int nVerInterval = m_nHeight / m_nVerCount;
//
//		////初始化（太阳位置/选中的模型组/相交线）,避免在循环中创建  zhangli
//		//pAnalysis->initSunPositionAndModelGroup(nTime,modelGroup);
//
//		//int xx = 0;
//		//int nIndexX = 0;
//		//for (double x=geopolygon.getBounds().xMin(); x<=geopolygon.getBounds().xMax(); x+=dblHorInterval)
//		//{
//		//	int nIndexY = 0;
//		//	if (nIndexX > 0)
//		//		xx += nHorInterval;
//		//	int yy = 0;
//
//		//	for (double y=geopolygon.getBounds().yMax(); y>=geopolygon.getBounds().yMin(); y-=dblVerInterval)
//		//	{
//		//		m_vtPoints.push_back(Vec3d(x, y, 0));
//		//		m_vtScreenPoints.push_back(Vec3d(xx, yy, 0));
//
//		//		if (nIndexY > 0)
//		//			yy += nVerInterval;
//
//		//		//真正绘制的内容
//		//		double Height=0.0;
//		//		m_pMapNode->getTerrain()->getHeight(m_pMapNode->getMapSRS(),x,y,&Height);
//		//		osgEarth::GeoPoint geoP(m_pMapNode->getMapSRS(), x, y, Height+0.9);
//		//		/*osgEarth::GeoPoint geoP(m_pMapNode->getMapSRS(), x, y, 0.0, ALTMODE_ABSOLUTE);*/
//		//		Vec3d vWorld;
//		//		geoP.toWorld(vWorld);
//
//		//		//计算时间，向上取整，界定0-8的范围 zhangli
//		//		int nrTime = pAnalysis->complete(vWorld, nTime);
//		//		int Time_Hour = ceil(nrTime / 60.0f);
//		//		Time_Hour = (Time_Hour > 8)?8:Time_Hour;
//		//		Time_Hour = (Time_Hour < 0)?0:Time_Hour;
//
//		//		m_vtDrawUnit.push_back(st_DrawUint(xx, yy, Time_Hour));
//
//		//		nIndexY++;
//		//	}
//
//		//	nIndexX++;
//		//}
//		
//		//CSunLightAnalysis
//	}
//	~ImageBuilder()
//	{
//		//if (m_pImage)
//			//delete m_pImage;
//	}
//
//	/*世界坐标转换到屏幕坐标*/
//	Vec3d WorldToWindow(osgViewer::Viewer* view, Vec3d world)
//	{
//			osg::Matrix VPW = view->getCamera()->getViewMatrix() *
//                      view->getCamera()->getProjectionMatrix() *
//                      view->getCamera()->getViewport()->computeWindowMatrix();
//			Vec3d window = world * VPW;
//			return window;
//	}
//
//	//距离计算
//	double  Distance(Vec3d v0, Vec3d v1)
//	{
//		return sqrt((v1.x() - v0.x())*(v1.x() - v0.x()) + (v1.y() - v0.y())*(v1.y() - v0.y())+(v1.z() - v0.z())*(v1.z() - v0.z()));
//	}
//
//	/*经纬度坐标转换到屏幕坐标*/
//	Vec3d LocalToWindow(osgViewer::Viewer* view, osgEarth::MapNode* pMapNode,Vec3d local)
//	{
//		//经纬度到屏幕
//		osgEarth::GeoPoint geoP(pMapNode->getMapSRS(), local);
//		Vec3d vWorld;
//		geoP.toWorld(vWorld);
//
//		return WorldToWindow(view,vWorld);
//	}
//	/*屏幕坐标转换到世界坐标*/
//	Vec3d WindowToWorld(osgViewer::Viewer* view, Vec3d window)
//	{
//		Matrix VPW = view->getCamera()->getViewMatrix() *
//                      view->getCamera()->getProjectionMatrix() *
//                      view->getCamera()->getViewport()->computeWindowMatrix();
//		Matrix inverseVPW = Matrix::inverse(VPW);
//		Vec3d world = window * inverseVPW;
//		return world;
//	}
//	/*屏幕坐标转换到经纬度坐标*/
//	Vec3d WindowToLocal(osgViewer::Viewer* view,osgEarth::MapNode* pMapNode, Vec3d window)
//	{
//		Vec3d world = WindowToWorld(view, window);
//
//		//经纬度到屏幕
//		osgEarth::GeoPoint geoP;
//		geoP.fromWorld(pMapNode->getMapSRS(),world);
//
//		return geoP.vec3d();
//	}
//
//	void DrawTest(int x, int y, const char *strText)
//	{
//		/*
//		QRectF rect(x, y, m_nWidth*0.6/m_nHorCount, m_nHeight*0.6/m_nVerCount);
//		m_Painter.setPen(QColor(255, 255, 255));
//		int nFlag = 0;
//		nFlag |= Qt::AlignVCenter;
//		nFlag |= Qt::AlignHCenter;
//		rect = m_Painter.boundingRect(rect, nFlag, QString(strText));
//		m_Painter.drawText(rect, nFlag, QString(strText));
//		*/
//		//测试渲染
//		/*for (int i=0; i<m_vtScreenPoints.size(); i++)
//		{
//			m_Painter.drawEllipse(QPoint(m_vtScreenPoints[i].x(), m_vtScreenPoints[i].y()), 2, 2);
//		}*/
//	}
//	void DrawImage()
//	{
//		/*
//		//设置绘制大小，动态计算，避免拉远挤在一坨
//		float nW = m_nWidth/m_nHorCount;
//		float nH =  m_nHeight/m_nVerCount;
//		QFont font;
//		font.setPixelSize(min(nW,nH));
//		m_Painter.setFont(font);
//
//		for (int i=0; i<m_vtDrawUnit.size(); i++)
//		{
//			st_DrawUint &Uint = m_vtDrawUnit[i];
//			QRectF rect(Uint.x, Uint.y, nW, nH);
//			m_Painter.setPen(Uint.cl);
//			int nFlag = 0;
//			nFlag |= Qt::AlignLeft;
//			nFlag |= Qt::AlignLeft;
//			rect = m_Painter.boundingRect(rect, nFlag, QString(Uint.sTime.c_str()));
//			m_Painter.drawText(rect, nFlag, QString(Uint.sTime.c_str()));
//		}*/
//	}
//	ImageOverlay * AddToMapNode(osgEarth::Bounds& bounds)
//	{
//		/*
//		m_Painter.end();
//
//		
//		
//		//osg屏幕坐标源点在左下，y轴和位图相反
//		int transY = m_Polygon.getBounds().yMax();
//		int transX = m_Polygon.getBounds().xMin();
//		for (int i=0; i<m_nWidth; i++)
//		{
//			for (int j=0; j<m_nHeight; j++)
//			{
//				int nLoginX = i;
//				int nLoginY = m_nHeight - j;
//				if (!m_ImagePolygon.contains2D(i, m_nHeight - j - 1))
//				{
//					m_pImage->setPixel(i,j, QColor(0,0,0,0).value());
//				}
//			}
//		}
//
//		m_pImage->save("D:\\123.png");
//		osg::Image* image = osgDB::readImageFile("D:\\123.png");
//		osg::ImageStream* is = dynamic_cast<osg::ImageStream*>(image);
//		if (is)
//			is->play();
//		osg::ref_ptr<ImageOverlay> ptrOverlay = new ImageOverlay(m_pMapNode);
//		ptrOverlay->setImage(image);
//		ptrOverlay->setBounds(bounds);
//		m_pMapNode->addChild(ptrOverlay);
//		return ptrOverlay.get();*/
//		return NULL;
//	}
//protected:
//	//QImage* m_pImage;
//	//QPainter m_Painter;
//	osgEarth::Symbology::Polygon m_Polygon;
//	osgEarth::Symbology::Polygon m_GeoPolygon;
//	osgEarth::Symbology::Polygon m_ImagePolygon;
//	int m_nHeight;
//	int m_nWidth;
//    int m_nHorCount;
//	int m_nVerCount;
//
//	osgViewer::Viewer* m_view;
//
//	osgEarth::MapNode* m_pMapNode;
//	int m_nInterval;
//
//protected:
//	struct st_DrawUint
//	{
//		int x;
//		int y;
//		std::string sTime;
//		int cl;
//		st_DrawUint(int nx, int ny, int ntime)
//		{
//			x = nx;
//			y = ny;
//			char arr[10] = {};
//			sTime = itoa(ntime, arr, 10);
//
//			if (ntime >= 2)
//				 cl = 0;
//			else
//				cl = 0;
//		}
//	};
//	std::vector<Vec3d> m_vtPoints;
//	std::vector<Vec3d> m_vtScreenPoints;
//	std::vector<st_DrawUint> m_vtDrawUnit;
//};


GeoGlobe::PolygonTrakerElement3D::PolygonTrakerElement3D(osgEarth::MapNode* mapNode,/*GeoGlobe::IMainWindow* mainWindow,*/
	osgEarth::Annotation::FeatureNode* polygon,bool bDrawDrager):
_mapNode(mapNode), 
_draggers(0L), 
_pathColor(Color(Color::Yellow, 1.0)),
_fillColor(Color(Color::Navy, 0.4)),
_editing(polygon ? true : false),
_drawLead(true),
_mouseDown(-1), 
_lastPoint(-500.0, 0.0, 0.0), 
_bFinished(false)
{
	_bDrawDrager = bDrawDrager;
	//m_MainWindow = mainWindow;

	setNumChildrenRequiringEventTraversal( 1 );

	_polygon = new osgEarth::Symbology::Polygon();

	//If editing an existing polygon
	if (polygon)
	{
		osgEarth::Annotation::AnnotationData* data = new osgEarth::Annotation::AnnotationData;

		//Get path name
		_inName = data ? data->getName() : "";
		//Get path description
		_inDescription = data ? data->getDescription() : "";

		_polyNode = polygon;

		const osgEarth::Features::Feature* feat = polygon->getFeature();
		if (feat)
		{
			_inFeature = const_cast<osgEarth::Features::Feature*>(feat);

			//Get path color
			const osgEarth::Symbology::LineSymbol* lineSymbol = feat->style()->get<LineSymbol>();
			if (lineSymbol)
			{
				if (lineSymbol->stroke()->width() != 0.0)
				{
					_pathColor = lineSymbol->stroke()->color();
				}
			}

			//Get fill color
			const osgEarth::Symbology::PolygonSymbol* polySymbol = feat->style()->get<PolygonSymbol>();
			if (polySymbol)
			{
				_fillColor = polySymbol->fill()->color();
			}

			//Get path points
			const osgEarth::Symbology::Polygon* polyGeom = dynamic_cast<const osgEarth::Symbology::Polygon*>(feat->getGeometry());
			if (polyGeom)
			{
				for (osgEarth::Symbology::LineString::const_iterator it = polyGeom->begin(); it != polyGeom->end(); ++it)
					addPoint(osgEarth::GeoPoint(_mapNode->getMapSRS(), (*it).x(), (*it).y(), (*it).z(), ALTMODE_RELATIVE));
			}
		}
	}
	else
	{
		//Add an end point to follow the mouse cursor
		_polygon->push_back(osg::Vec3d(0.0, 0.0, 0.0));

		if (_drawLead)
		{
			//Define a style for the lead line
			osgEarth::Symbology::LineSymbol* ls = _lineStyle.getOrCreateSymbol<osgEarth::Symbology::LineSymbol>();
			ls->stroke()->color() = _pathColor;
			ls->stroke()->width() = 2.0f;
			ls->stroke()->stipple() = 0x0F0F;
			ls->tessellation() = 20;
			_lineStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
			_lineStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;// osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;
		}
	}

}

void GeoGlobe::PolygonTrakerElement3D::traverse(osg::NodeVisitor& nv)
{
	if (nv.getVisitorType() == osg::NodeVisitor::EVENT_VISITOR)
	{
		osgGA::EventVisitor* ev = static_cast<osgGA::EventVisitor*>(&nv);
		for (osgGA::EventQueue::Events::iterator itr = ev->getEvents().begin(); itr != ev->getEvents().end(); ++itr)
		{
			osgGA::GUIEventAdapter* ea = dynamic_cast<osgGA::GUIEventAdapter*>(itr->get());
			if ( ea && handle(*ea, *(ev->getActionAdapter())))
				ea->setHandled(true);
		}
	}
	osg::Group::traverse(nv);
}

bool GeoGlobe::PolygonTrakerElement3D::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (_bFinished)
		return false;

	osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());

	if ( ea.getEventType() == osgGA::GUIEventAdapter::PUSH  && _mouseDown == -1)
	{
		_mouseDown = ea.getButton();
		_xDown = ea.getX();
		_yDown = ea.getY();
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE && _lastPoint.x() != -500.0)
	{
		osg::Vec3d world;
		if (_mapNode->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), world))
		{
			osgEarth::GeoPoint mapPoint;
			mapPoint.fromWorld( _mapNode->getMapSRS(), world );

			mapMouseMove(mapPoint);

			_currentPoint = mapPoint.vec3d();

			if (_polygon->size()<3)
				updateDisplay();
		}
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)
	{
		if (ea.getButton() == _mouseDown)
		{
			if (_xDown == ea.getX() && _yDown == ea.getY())
			{
				osg::Vec3d world;
				if (_mapNode->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), world))
				{
					osgEarth::GeoPoint mapPoint;
					mapPoint.fromWorld( _mapNode->getMapSRS(), world );

					/**/
					_xDown = ea.getX();
					_yDown = ea.getY();

					//osg::Matrix VPW = view->getCamera()->getViewMatrix() *
					//			view->getCamera()->getProjectionMatrix() *
					//			view->getCamera()->getViewport()->computeWindowMatrix();
					//Vec3d window = world * VPW;

					_currentPoint = mapPoint.vec3d();
					_lastPoint = mapPoint.vec3d();

					mapMouseClick(mapPoint,world,_mouseDown);
					osg::Vec3d pointScreen(ea.getX(), ea.getY(), 0);
					_vecScreenPoint.push_back(pointScreen);
					updateDisplay();
				}
			}

			_mouseDown = -1;
		}
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK)
	{
		_bFinished = true;

		osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		if (!viewer) 
			return true;

		osg::Vec3d pointScreen(ea.getX(), ea.getY(), 0);
		_vecScreenPoint.push_back(pointScreen);

		if (_addpointsCompletedCallback.valid())
			_addpointsCompletedCallback->completed(viewer,this,_mapNode.get());

		if (_areaAnalysisCompletedCallback.valid())
			_areaAnalysisCompletedCallback->completed(viewer,this,_mapNode.get());

		//if (_reginAnalysisCompletedCallback.valid())
		//	_reginAnalysisCompletedCallback->completed(viewer,this,_mapNode.get());

		return true;
	}

	return false;
}

GeoGlobe::PolygonTrakerElement3D::~PolygonTrakerElement3D()
{

}

void GeoGlobe::PolygonTrakerElement3D::mapMouseClick(const osgEarth::GeoPoint& geoPoint, const osg::Vec3d& worldPoint,int button)
{
	if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		addPoint(geoPoint,worldPoint);
	}
}

void GeoGlobe::PolygonTrakerElement3D::mapMouseMove(const osgEarth::GeoPoint& point)
{
	_polygon->back().set(point.vec3d());
	refreshFeatureNode(true);
}

void GeoGlobe::PolygonTrakerElement3D::addPoint(const osgEarth::GeoPoint& point)
{
	_vecGeoPoint.push_back(osg::Vec3d(point.x(),point.y(),point.z()));

	if (_editing)
		_polygon->push_back(point.vec3d());
	else
		_polygon->insert(_polygon->end() - 1, point.vec3d());

	if (!_polyFeature.valid() && _polygon->size() > 2)
	{
		osgEarth::Symbology::Style polyStyle;
		polyStyle.getOrCreate<LineSymbol>()->stroke()->color() = _pathColor;
		polyStyle.getOrCreate<LineSymbol>()->stroke()->width() = 2.0f/*ANNOTATION_PATH_WIDTH*/;
		polyStyle.getOrCreate<LineSymbol>()->tessellation() = 20;
		polyStyle.getOrCreate<PolygonSymbol>()->fill()->color() = _fillColor;
		polyStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
		polyStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;// AltitudeSymbol::TECHNIQUE_DRAPE;
		_polyFeature = new osgEarth::Features::Feature(_polygon, _mapNode->getMapSRS(), polyStyle);

		if (!_polyNode.valid())
		{
			_polyNode = new osgEarth::Annotation::FeatureNode( _polyFeature);

			_polyNode->setMapNode(_mapNode.get());
			//_featureNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			GLUtils::setLighting(_polyNode->getOrCreateStateSet(), osg::StateAttribute::OFF);
			addChild(_polyNode);
		}
	}

	refreshFeatureNode();
	if (_bDrawDrager)
		createPointDragger(_polygon->size() - 1, point);
}

void GeoGlobe::PolygonTrakerElement3D::addPoint(const osgEarth::GeoPoint& point, const osg::Vec3d& worldPoint)
{
	_vecGeoPoint.push_back(osg::Vec3d(point.x(),point.y(),point.z()));
	_vecWorldPoint.push_back(worldPoint);
	  
	if (_editing)
 		_polygon->push_back(point.vec3d());
	else
		_polygon->insert(_polygon->end() - 1, point.vec3d());

	if (!_polyFeature.valid() && _polygon->size() > 2)
	{
		osgEarth::Symbology::Style polyStyle;
		//stroke---线程参数？？
		polyStyle.getOrCreate<LineSymbol>()->stroke()->color() = _pathColor;
		polyStyle.getOrCreate<LineSymbol>()->stroke()->width() = 2.0f/*ANNOTATION_PATH_WIDTH*/;
		//将线段分段
		polyStyle.getOrCreate<LineSymbol>()->tessellation() = 40;
		
		polyStyle.getOrCreate<PolygonSymbol>()->fill()->color() = _fillColor;
		polyStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
		polyStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;// AltitudeSymbol::TECHNIQUE_DRAPE;
		
		_polyFeature = new osgEarth::Features::Feature(_polygon, _mapNode->getMapSRS(), polyStyle);

		if (!_polyNode.valid())
		{
			_polyNode = new osgEarth::Annotation::FeatureNode(_polyFeature);
			_polyNode->setMapNode(_mapNode.get());
			//_featureNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			GLUtils::setLighting(_featureNode->getOrCreateStateSet(), osg::StateAttribute::OFF);
			addChild(_polyNode);
		}
	}

	refreshFeatureNode();
	if (_bDrawDrager)
		createPointDragger(_polygon->size() - 1, point);
}

void GeoGlobe::PolygonTrakerElement3D::movePoint(int index, const osgEarth::GeoPoint& position)
{
	(*_polygon.get())[index] = position.vec3d();
	refreshFeatureNode();
}

void GeoGlobe::PolygonTrakerElement3D::refreshFeatureNode(bool geometryOnly)
{
	if (_polyNode.valid() && _polyFeature.valid())  
	{
		if (!geometryOnly)
		{
			if (_pathColor.a() == 0.0)
			{
				_polyFeature->style()->getOrCreate<LineSymbol>()->stroke()->color() = _fillColor;
				_polyFeature->style()->getOrCreate<LineSymbol>()->stroke()->width() = 0.0;
			}
			else
			{
				_polyFeature->style()->getOrCreate<LineSymbol>()->stroke()->color() = _pathColor;
				_polyFeature->style()->getOrCreate<LineSymbol>()->stroke()->width() = 2.0f/*ANNOTATION_PATH_WIDTH*/;
				_polyFeature->style()->getOrCreate<LineSymbol>()->stroke()->stipple() = 0x0F0F;
				_polyFeature->style()->getOrCreate<LineSymbol>()->tessellation() = 20;
			}

			
			AltitudeSymbol* as = _polyFeature->style()->getOrCreate<AltitudeSymbol>();
			as->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
			as->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;//AltitudeSymbol::TECHNIQUE_DRAPE;
			_polyFeature->style()->getOrCreate<PolygonSymbol>()->fill()->color() = _fillColor;
			_polyFeature->style()->getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN /*AltitudeSymbol::CLAMP_ABSOLUTE*/;
		    _polyFeature->style()->getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;//AltitudeSymbol::TECHNIQUE_DRAPE;
			_polyFeature->style()->getOrCreate<RenderSymbol>()->depthOffset()->enabled() = true;
			_polyFeature->style()->getOrCreate<RenderSymbol>()->depthOffset()->minBias() = 1000;

		}

		_polyNode->setFeature(_polyFeature);
	}
}

void GeoGlobe::PolygonTrakerElement3D::createPointDragger(int index, const osgEarth::GeoPoint& point)
{
// 	osgEarth::Annotation::SphereDragger* sd = new osgEarth::Annotation::SphereDragger(_mapNode.get());
// 	sd->setSize(4.0f);
// 	sd->setColor(Color::Magenta);
// 	sd->setPickColor(Color::Green);
// 	sd->setPosition(point);
// 	PointDraggerCallback* callback = new PointDraggerCallback(index, this);
// 	sd->addPositionChangedCallback(callback);
// 
// 	if (!_draggers)
// 	{
// 		_draggers = new osg::Group();
// 		addChild(_draggers);
// 	}
// 
// 	_draggers->addChild(sd);
}

void GeoGlobe::PolygonTrakerElement3D::clearDisplay()
{
	if (!_editing)
		removeChild(_polyNode);

	removeChild(_draggers);

	_lastPoint.x() = -500.0;
	if (_featureNode.valid())
	{
		removeChild( _featureNode.get() );
		_featureNode = 0L;
	}
}

void GeoGlobe::PolygonTrakerElement3D::resetValues()
{
	if (_inFeature.valid())
	{
		_polyNode->setFeature(_inFeature);
	}
	else
	{
		removeChild(_polyNode);
		_polyNode = 0L;
		_polygon = 0L;
	}
}

void GeoGlobe::PolygonTrakerElement3D::updateDisplay()
{
	if (_drawLead && _lastPoint.x() != -500.0)
	{
		osgEarth::Symbology::LineString* line = new osgEarth::Symbology::LineString();
		line->push_back( _lastPoint );
		line->push_back( _currentPoint );

		osgEarth::Features::Feature* feature = new osgEarth::Features::Feature(line, _mapNode->getMapSRS());
		feature->geoInterp() = osgEarth::GEOINTERP_GREAT_CIRCLE;    
		feature->style() = _lineStyle;

		if (!_featureNode.valid())
		{
			_featureNode = new osgEarth::Annotation::FeatureNode(feature );
			_featureNode->setMapNode(_mapNode.get());
			//_featureNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			GLUtils::setLighting(_featureNode->getOrCreateStateSet(), osg::StateAttribute::OFF);
			addChild( _featureNode.get() );
		}
		else
		{
			_featureNode->setFeature(feature);
		}
	}
}

void GeoGlobe::PolygonTrakerElement3D::setMapNode( MapNode* mapNode )
{
	_mapNode = _mapNode;
}
MapNode* GeoGlobe::PolygonTrakerElement3D::getMapNode() 
{
	return _mapNode.get(); 
}

void GeoGlobe::PolygonTrakerElement3D::ModelQueryCompletedCallback::completed(osgViewer::Viewer* viewer, PolygonTrakerElement3D* Ele,osg::Group* group)
{
	//清除上一次的结果
	_mapModel.clear();
	//group->removeChildren(0,group->getNumChildren()-1);

	GeoGlobe::PolygonSegmentIntersector* picker = new GeoGlobe::PolygonSegmentIntersector(Ele->_vecGeoPoint);
	GeoGlobe::IntersectionVisitor iv(picker);
	viewer->getCamera()->accept(iv);

	if (picker->containsIntersections())
	{
		GeoGlobe::PolygonSegmentIntersector::Intersections intersections = picker->getIntersections();
		for(GeoGlobe::PolygonSegmentIntersector::Intersections::iterator hitr = intersections.begin();
			hitr != intersections.end();
			++hitr)
		{
			// the geodes are identified by name.
			osg::NodePath nodePath = hitr->nodePath;
			for (int i=0; i<nodePath.size();i++)
			{
				osg::ref_ptr<osg::Node> node = nodePath[i];
				if (node == NULL)
					continue;

				osg::ref_ptr<GeoGlobe::Model::LodModel> modelnode =  dynamic_cast<GeoGlobe::Model::LodModel*>(node.get());
				if (!modelnode)
					continue;

				unsigned long long uKey = modelnode->_options->_feature->m_nFID;
				//Ele->HightModel(modelnode,uKey);
				_mapModel.insert(std::map<unsigned long long,osg::ref_ptr<GeoGlobe::Model::LodModel>>::value_type(uKey,modelnode));

				//试试这个group是什么zhangli
				//group->addChild(node);
			}
		}
	}
	Ele->CalcArea();
	Ele->m_MainWindow->setRelocationData(_mapModel,group,Ele,m_mapState);
	//DemolitionWindow * ptrDemoliWindow = new DemolitionWindow(_mapModel,group,Ele);
	//ptrDemoliWindow->show();
}
double GeoGlobe::PolygonTrakerElement3D::CalcArea()
{
	m_areaReault = 0.0;
	//计算面积
	//记录向量叉积之和
	double dblX = 0;
	double dblY = 0;
	double dblZ = 0;
	int nCount = _vecWorldPoint.size();
	osg::Vec3d ptA,ptB;
	osg::Vec3d ptGeo;
	for (int i=0;i<nCount-1;i++)
	{
		ptA = _vecWorldPoint.at(i);
		ptB = _vecWorldPoint.at(i+1);

		dblX += ptA.y() * ptB.z() - ptA.z() * ptB.y();
		dblY += ptA.z() * ptB.x() - ptA.x() * ptB.z();
		dblZ += ptA.x() * ptB.y() - ptA.y() * ptB.x();

		ptGeo = _vecGeoPoint.at(i);
	}

	ptA = _vecWorldPoint.at(nCount-1);
	ptB = _vecWorldPoint.at(0);

	dblX += ptA.y() * ptB.z() - ptA.z() * ptB.y();
	dblY += ptA.z() * ptB.x() - ptA.x() * ptB.z();
	dblZ += ptA.x() * ptB.y() - ptA.y() * ptB.x();

	//面积
	m_areaReault = 0.5 * sqrt(dblX * dblX + dblY * dblY + dblZ * dblZ);
	return m_areaReault;
}
void GeoGlobe::PolygonTrakerElement3D::AreaAnalysisCompletedCallback::completed(osgViewer::Viewer* viewer, PolygonTrakerElement3D* Ele,osgEarth::MapNode* mapnode)
{
	m_dblArea = 0.0;
	//计算面积
	//记录向量叉积之和
	double dblX = 0;
	double dblY = 0;
	double dblZ = 0;
	int nCount = Ele->_vecWorldPoint.size();
	osg::Vec3d ptA,ptB;
	osg::Vec3d ptGeo;
	for (int i=0;i<nCount-1;i++)
	{
		ptA = Ele->_vecWorldPoint.at(i);
		ptB = Ele->_vecWorldPoint.at(i+1);

		dblX += ptA.y() * ptB.z() - ptA.z() * ptB.y();
		dblY += ptA.z() * ptB.x() - ptA.x() * ptB.z();
		dblZ += ptA.x() * ptB.y() - ptA.y() * ptB.x();

		ptGeo = Ele->_vecGeoPoint.at(i);
		m_centerPt.x() += ptGeo.x();
		m_centerPt.y() += ptGeo.y();
		m_centerPt.z() += ptGeo.z();
	}

	ptA = Ele->_vecWorldPoint.at(nCount-1);
	ptB = Ele->_vecWorldPoint.at(0);

	dblX += ptA.y() * ptB.z() - ptA.z() * ptB.y();
	dblY += ptA.z() * ptB.x() - ptA.x() * ptB.z();
	dblZ += ptA.x() * ptB.y() - ptA.y() * ptB.x();

	//面积
	m_dblArea = 0.5 * sqrt(dblX * dblX + dblY * dblY + dblZ * dblZ);

	//中心点
	ptGeo = Ele->_vecGeoPoint.at(nCount-1);
	m_centerPt.x() += ptGeo.x();
	m_centerPt.y() += ptGeo.y();
	m_centerPt.z() += ptGeo.z();

	m_centerPt.x() = m_centerPt.x() / nCount;
	m_centerPt.y() = m_centerPt.y() / nCount;
	m_centerPt.z() = m_centerPt.z() / nCount;



	osgEarth::GeoPoint geoPt = osgEarth::GeoPoint(mapnode->getMapSRS(),m_centerPt);
	//创建广告牌
	std::ostringstream s1;
	if (m_dblArea > 1e6)
	{
		ostringstream osm;  
		// 浮点数转换限制  
		double tmp = m_dblArea/1e6;  
		osm.precision(2);  
		osm.setf(std::ios::fixed); // 将浮点数的位数限定为小数点之后的位数  
		osm << tmp;   

		s1 <<"面积："<<osm.str()<<"平方千米"<<"\n";
	}
	else
	{
		ostringstream osm;  
		// 浮点数转换限制  
		double tmp = m_dblArea;  
		osm.precision(2);  
		osm.setf(std::ios::fixed); // 将浮点数的位数限定为小数点之后的位数  
		osm << tmp;  
		s1 <<"面积："<<osm.str()<<"平方米"<<"\n";
	}

	Ele->OnFinish(m_dblArea);

	std::string na = CodeHelp::String_To_UTF8(s1.str());
	osgEarth::Symbology::Style chapin;
	chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->size()=18.0f;
	//chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=Color::White;
	chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color()=Color::White;
	chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
	chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> pn = new GeoGlobe::PlaceName::GeoBillboard(geoPt,na,chapin,true);
	pn->setCullingActive(false);

	Ele->addChild(pn);
}

void GeoGlobe::PolygonTrakerElement3D::HightModel(GeoGlobe::Model::LodModel* modelnode,int64 uKey)
{
	//测试·
	osg::StateSet* state = modelnode->getOrCreateStateSet();
	if (!state)
		return;

	//记录原来的状态
	osg::ref_ptr<osg::StateSet> stateTemp = dynamic_cast<osg::StateSet*> (state->clone(osg::CopyOp::DEEP_COPY_ALL));
	if (_addpointsCompletedCallback.valid())
        _addpointsCompletedCallback->m_mapState.insert(std::map<unsigned long long,osg::ref_ptr<osg::StateSet>>::value_type(uKey,stateTemp));

	//关闭灯光
	state->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
	//打开混合融合模式
	state->setMode(GL_BLEND,osg::StateAttribute::ON);
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//BlendColor:定义一种混合颜色
	osg::BlendColor* bc =new osg::BlendColor();
	//BlendFunc：定义一种混合方式
	osg::BlendFunc*bf = new osg::BlendFunc();
	//属性设置，开关打开
	state->setAttributeAndModes(bf,osg::StateAttribute::ON);
	state->setAttributeAndModes(bc,osg::StateAttribute::ON);
	//设置源目标参数（靠近屏幕的是源），现在就是选中的东东；CONSTANT_COLOR指使用固定色彩
	bf->setSource(osg::BlendFunc::CONSTANT_COLOR);
	//设置目标（远离屏幕的是目标），ZERO表示目标不参与混色
	bf->setDestination(osg::BlendFunc::ZERO);
	//固定色彩是多少#FFEC8B
	bc->setConstantColor(osg::Vec4(1.0f,1.0f,0.0f,1.0));
}

bool GeoGlobe::PolygonTrakerElement3D::IsFinished()
{
	return _bFinished;
}

void GeoGlobe::PolygonTrakerElement3D::SetFillColor(const osg::Vec4f& fillcolor)
{
	_fillColor =  fillcolor;
}
void GeoGlobe::PolygonTrakerElement3D::SetPathColor(const osg::Vec4f& pathcolor)
{
	_pathColor = pathcolor;
}

//void GeoGlobe::PolygonTrakerElement3D::ReginQueryCompletedCallback::completed(osgViewer::Viewer* viewer, PolygonTrakerElement3D* Ele,osg::Group* group)
//{
//	Ele->_vecScreenPoint.push_back(Ele->_vecScreenPoint[0]);
//	osgEarth::Symbology::Polygon ScerrnPolygon(&(Ele->_vecScreenPoint));
//	osgEarth::Symbology::Polygon GeoPolygon(&(Ele->_vecGeoPoint));
//	m_ScerrnPolygon = ScerrnPolygon;
//	m_GeoPolygon = GeoPolygon;
//	m_pMapNode = (MapNode *)group;
//	m_pviewer = viewer;
//	m_bounds = Ele->_polygon->getBounds();
//
//
//
//	//和GeoGlobe::PolygonTrakerElement3D::AreaAnalysisCompletedCallback::completed重复
//	//要看能否合并到一起。
//	//获取框选区域的模型，合并成一个group，以备相交计算。
//
//	//清除上一次的结果
//	if(m_ptrGroup!=NULL)
//	{
//		m_ptrGroup->removeChildren(0,group->getNumChildren());
//	}
//
//	GeoGlobe::PolygonSegmentIntersector* picker = new GeoGlobe::PolygonSegmentIntersector(Ele->_vecGeoPoint);
//	GeoGlobe::IntersectionVisitor iv(picker);
//	viewer->getCamera()->accept(iv);
//
//	if (picker->containsIntersections())
//	{
//		GeoGlobe::PolygonSegmentIntersector::Intersections intersections = picker->getIntersections();
//		for(GeoGlobe::PolygonSegmentIntersector::Intersections::iterator hitr = intersections.begin();
//			hitr != intersections.end();
//			++hitr)
//		{
//			// the geodes are identified by name.
//			osg::NodePath nodePath = hitr->nodePath;
//			for (int i=0; i<nodePath.size();i++)
//			{
//				osg::ref_ptr<osg::Node> node = nodePath[i];
//				if (node == NULL)
//					continue;
//
//				osg::ref_ptr<GeoGlobe::Model::LodModel> modelnode =  dynamic_cast<GeoGlobe::Model::LodModel*>(node.get());
//				if (!modelnode)
//					continue;
//				/*osg::ref_ptr<GeoGlobe::Model::Model> model0 =  dynamic_cast<GeoGlobe::Model::Model*>(modelnode->getChild(0));
//				if (!model0)
//					continue;
//
//				std::vector<osg::Vec3d> pArray;
//				osg::Geometry* pGeo =  dynamic_cast<osg::Geometry*>(model0->_geometry->getDrawable(0));
//				if(NULL != pGeo)
//				{
//					osg::Vec3Array* verArray = (osg::Vec3Array*)(pGeo->getVertexArray());
//					if(NULL != verArray)
//					{
//						
//						for(osg::Vec3Array::iterator iter = verArray->begin(); iter != verArray->end() ; iter++)
//						{
//							pArray.push_back(osg::Vec3d( iter->x(),iter->y(),iter->z()));
//						}
//					}
//				}*/
//
//				unsigned long long uKey = modelnode->_options->_feature->m_nFID;
//				//Ele->HightModel(modelnode,uKey);
//
//				//得到的结果group
//				m_ptrGroup->addChild(node);
//			}
//		}
//	}
//	//Ele->CalcArea();
//	//DemolitionWindow * ptrDemoliWindow = new DemolitionWindow(_mapModel,group,Ele);
//	//ptrDemoliWindow->show();
//}
////ImageOverlay * GeoGlobe::PolygonTrakerElement3D::ReginQueryCompletedCallback::ReginQuery()
////{
////	CSunLightAnalysis oAnalysis(m_pviewer, m_Date);
////	ImageBuilder o(m_pviewer,0,0,0,m_ScerrnPolygon, m_GeoPolygon, m_pMapNode, m_ptrGroup,&oAnalysis, m_nDis, m_nTime);
////	o.DrawImage();
////
////	return o.AddToMapNode(m_bounds);
////}
//GeoGlobe::PolygonTrakerElement3D::ReginQueryCompletedCallback::ReginQueryCompletedCallback(long long qDate, int nDis, int nTime)
//{
//	m_Date = qDate;
//	m_nDis = nDis;
//	m_nTime = nTime;
//	m_ptrGroup = new osg::Group();
//}

GeoGlobe::PolygonTrakerElement3D::ReginQueryCompletedCallback::ReginQueryCompletedCallback(long long qDate, int nDis, int nTime)
{
}
