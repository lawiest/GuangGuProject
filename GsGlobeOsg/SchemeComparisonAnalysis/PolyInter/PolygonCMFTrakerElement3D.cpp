#include <SchemeComparisonAnalysis/PolyInter/PolygonCMFTrakerElement3D.h>
#include <model/LodModel.h> 
#include <osgEarthAnnotation/AnnotationData>
#define ANNOTATION_PATH_WIDTH 12.0f



PolygonCMFTrakerElement3D::PolygonCMFTrakerElement3D(osgEarth::MapNode* mapNode, GeoGlobe::IMainWindow* mainWindow, osgEarth::Annotation::FeatureNode* polygon,bool bDrawDrager):
_mapNode(mapNode), _draggers(0L), _pathColor(Color(Color::White, 0.0)), _fillColor(Color(Color::Navy, 0)), _editing(polygon ? true : false),
_drawLead(true), _mouseDown(-1), _lastPoint(-500.0, 0.0, 0.0), _bFinished(false)
{
	_bDrawDrager = bDrawDrager;
	 
	m_MainWindow = mainWindow;
	setNumChildrenRequiringEventTraversal( 1 );

	_polygon = new osgEarth::Symbology::Polygon(); 
	//If editing an existing polygon
	if (polygon)
	{
		osgEarth::Annotation::AnnotationData* data = NULL;

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
			ls->stroke()->color() = osgEarth::Symbology::Color::White;
			ls->stroke()->width() = 2.0f;
			ls->stroke()->stipple() = 0x0F0F;
			ls->tessellation() = 20;
			_lineStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
			_lineStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;
		}
	}

}

void PolygonCMFTrakerElement3D::traverse(osg::NodeVisitor& nv)
{
	if (nv.getVisitorType() == osg::NodeVisitor::EVENT_VISITOR)
	{
		osgGA::EventVisitor* ev = static_cast<osgGA::EventVisitor*>(&nv);
		for(osgGA::EventQueue::Events::iterator itr = ev->getEvents().begin();
			itr != ev->getEvents().end();
			++itr)
		{
			osgGA::GUIEventAdapter* ea = dynamic_cast<osgGA::GUIEventAdapter*>(itr->get());
			if ( ea && handle(*ea, *(ev->getActionAdapter())))
				ea->setHandled(true);
		}
	}
	osg::Group::traverse(nv);
}

bool PolygonCMFTrakerElement3D::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
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

		_addpointsCompletedCallback->completed(viewer,this,_mapNode.get());
		return true;
	}

	return false;
}

PolygonCMFTrakerElement3D::~PolygonCMFTrakerElement3D()
{

}

//添加多边形点
/*
void PolygonCMFTrakerElement3D::mapMouseClick(const osgEarth::GeoPoint& point, int button)
{
	if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		addPoint(point);
	}
}*/
void PolygonCMFTrakerElement3D::mapMouseClick(const osgEarth::GeoPoint& geoPoint, const osg::Vec3d& worldPoint,int button)
{
	if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		addPoint(geoPoint,worldPoint);
	}
}

//多边形实时绘制
void PolygonCMFTrakerElement3D::mapMouseMove(const osgEarth::GeoPoint& point)
{
	_polygon->back().set(point.vec3d());
	refreshFeatureNode(true);
}

void PolygonCMFTrakerElement3D::addPoint(const osgEarth::GeoPoint& point)
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
		polyStyle.getOrCreate<LineSymbol>()->stroke()->width() = ANNOTATION_PATH_WIDTH;
		polyStyle.getOrCreate<LineSymbol>()->tessellation() = 20;
		polyStyle.getOrCreate<PolygonSymbol>()->fill()->color() = _fillColor;
		polyStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;

		_polyFeature = new osgEarth::Features::Feature(_polygon, _mapNode->getMapSRS(), polyStyle);

		if (!_polyNode.valid())
		{
			_polyNode = new osgEarth::Annotation::FeatureNode(_mapNode.get(), _polyFeature);
			addChild(_polyNode);
		}
	}

	refreshFeatureNode();
	createPointDragger(_polygon->size() - 1, point);
}

void PolygonCMFTrakerElement3D::movePoint(int index, const osgEarth::GeoPoint& position)
{
	(*_polygon.get())[index] = position.vec3d();
	refreshFeatureNode();
}

void PolygonCMFTrakerElement3D::refreshFeatureNode(bool geometryOnly)
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
				_polyFeature->style()->getOrCreate<LineSymbol>()->stroke()->width() = ANNOTATION_PATH_WIDTH;
			}


			_polyFeature->style()->getOrCreate<PolygonSymbol>()->fill()->color() = _fillColor;
			_polyFeature->style()->getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN /*AltitudeSymbol::CLAMP_ABSOLUTE*/;
			_polyFeature->style()->getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
		}

		_polyNode->setFeature(_polyFeature);
	}
}

void PolygonCMFTrakerElement3D::createPointDragger(int index, const osgEarth::GeoPoint& point)
{
	osgEarth::Annotation::SphereDragger* sd = new osgEarth::Annotation::SphereDragger(_mapNode.get());
	sd->setSize(4.0f);
	sd->setColor(Color::Magenta);
	sd->setPickColor(Color::Green);
	sd->setPosition(point);
	PointDraggerCallback* callback = new PointDraggerCallback(index, this);
	sd->addPositionChangedCallback(callback);

	if (!_draggers)
	{
		_draggers = new osg::Group();
		addChild(_draggers);
	}

	_draggers->addChild(sd);
}

void PolygonCMFTrakerElement3D::clearDisplay()
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

void PolygonCMFTrakerElement3D::resetValues()
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

void PolygonCMFTrakerElement3D::updateDisplay()
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
			_featureNode = new osgEarth::Annotation::FeatureNode( _mapNode.get(), feature );
			_featureNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			addChild( _featureNode.get() );
		}
		else
		{
			_featureNode->setFeature(feature);
		}
	}
}

void PolygonCMFTrakerElement3D::setMapNode( MapNode* mapNode )
{
	_mapNode = _mapNode;
}
MapNode* PolygonCMFTrakerElement3D::getMapNode() 
{
	return _mapNode.get(); 
}

//获取多边形内的模型
void PolygonCMFTrakerElement3D::AddCMFPointsCompletedCallback::completed(osgViewer::Viewer* viewer, PolygonCMFTrakerElement3D* Ele,osg::Group* group)
{
	//清除上一次的结果
	_mapModel.clear();

	PolygonCMFSegmentIntersector* picker = new PolygonCMFSegmentIntersector(Ele->_vecGeoPoint);
	IntersectionVisitorCMF iv(picker);
	viewer->getCamera()->accept(iv);

	if (!picker->containsIntersections())
		return;

	PolygonCMFSegmentIntersector::Intersections intersections = picker->getIntersections();
	for(PolygonCMFSegmentIntersector::Intersections::iterator hitr = intersections.begin();
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

			osg::ref_ptr<CModelF> modelnode =  dynamic_cast<CModelF*>(node.get());
			if (!modelnode)
				continue;

			ULONGLONG uKey = *modelnode->_mukey;
			//Ele->HightModel(modelnode,uKey);
			_mapModel.insert(std::map<ULONGLONG,osg::ref_ptr<CModelF>>::value_type(uKey,modelnode));
		}
	}

	Ele->m_MainWindow->setCMFRelocationData(_mapModel,group,Ele,m_mapState);
	//DemolitionWindow * ptrDemoliWindow = new DemolitionWindow(_mapModel,group,Ele);
	//ptrDemoliWindow->show();
}

void PolygonCMFTrakerElement3D::addPoint(const osgEarth::GeoPoint& point, const osg::Vec3d& worldPoint)
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
		polyStyle.getOrCreate<LineSymbol>()->stroke()->color() = _pathColor;
		polyStyle.getOrCreate<LineSymbol>()->stroke()->width() = 2.0f/*ANNOTATION_PATH_WIDTH*/;
		polyStyle.getOrCreate<LineSymbol>()->tessellation() = 20;
		polyStyle.getOrCreate<PolygonSymbol>()->fill()->color() = _fillColor;
		polyStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;

		_polyFeature = new osgEarth::Features::Feature(_polygon, _mapNode->getMapSRS(), polyStyle);

		if (!_polyNode.valid())
		{
			_polyNode = new osgEarth::Annotation::FeatureNode(_mapNode.get(), _polyFeature);
			addChild(_polyNode);
		}
	}

	refreshFeatureNode();
	if (_bDrawDrager)
		createPointDragger(_polygon->size() - 1, point);
}

double PolygonCMFTrakerElement3D::CalcArea()
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



void PolygonCMFTrakerElement3D::HightModel(CModelF* modelnode,int64 uKey)
{
	//测试·
	osg::StateSet* state = modelnode->getOrCreateStateSet();
	if (!state)
		return;

	//记录原来的状态
	osg::ref_ptr<osg::StateSet> stateTemp = dynamic_cast<osg::StateSet*> (state->clone(osg::CopyOp::DEEP_COPY_ALL));
	_addpointsCompletedCallback->m_mapState.insert(std::map<ULONGLONG,osg::ref_ptr<osg::StateSet>>::value_type(uKey,stateTemp));

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
