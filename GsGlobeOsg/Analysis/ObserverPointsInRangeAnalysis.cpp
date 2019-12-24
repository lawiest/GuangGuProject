
#include <Analysis/ObserverPointsInRangeAnalysis.h>
#include <sstream>
#include <string>
#include <tool/CodeTransformHelp.h>
#include <PlaceName/GeoBillboard.h> 


GeoGlobe::ObserverPointsInRangeAnalysis::ObserverPointsInRangeAnalysis(MapNode* mapNode, GeoGlobe::IConsultationWindow* consultationWnd)
{
	m_bFinished = false;
	setMapNode(mapNode);

	m_consultationWnd = consultationWnd;
	_startLineTraker  = new LineTraker3DElement(getMapNode());  
	_startLineTraker->setDefaultDragMode(LineTraker3DElement::DRAGMODE_VERTICAL);
	addChild(_startLineTraker);
	_nStep = 0;
}

GeoGlobe::ObserverPointsInRangeAnalysis::~ObserverPointsInRangeAnalysis()
{
}

MapNode* GeoGlobe::ObserverPointsInRangeAnalysis::getMapNode() 
{ 
	return _mapNode.get();
}

void GeoGlobe::ObserverPointsInRangeAnalysis::setMapNode( MapNode* mapNode )
{
	MapNode* oldMapNode = getMapNode();
	if ( oldMapNode != mapNode )
	{
		_mapNode = mapNode;
	}
}

void GeoGlobe::ObserverPointsInRangeAnalysis::traverse(osg::NodeVisitor& nv)
{
	if (nv.getVisitorType() == osg::NodeVisitor::EVENT_VISITOR)
	{
		osgGA::EventVisitor* ev = static_cast<osgGA::EventVisitor*>(&nv);
		for(osgGA::EventQueue::Events::iterator itr = ev->getEvents().begin() ; itr != ev->getEvents().end() ; ++itr)
		{
			osgGA::GUIEventAdapter* ea = dynamic_cast<osgGA::GUIEventAdapter*>(itr->get());
			if ( ea && handle(*ea, *(ev->getActionAdapter())))
				ea->setHandled(true);
		}
	}
	osg::Group::traverse(nv);
}

bool GeoGlobe::ObserverPointsInRangeAnalysis::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (ea.getHandled()) 
		return false;

	if (_nStep>=2)
		return false;

	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view) 
		return false;

	if (!_mapNode.valid()) 
		return false;

	if (_nStep == 0 && _startLineTraker->IsFinished())
	{
		GeoPoint geoPt = _startLineTraker->getPosition();
		_rlos = new RadialLineOfSightNode(_mapNode);

		_rlos->setCenter(geoPt);
		_rlos->setNumSpokes(150);
		_rlos->setFill( true );
		_rlos->setGoodColor( osg::Vec4(0,1,0,0.3) );
		_rlos->setBadColor( osg::Vec4(1,0,0,0.3) );
		_rlos->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		//_rlos->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false));

		addChild(_rlos);

		_nStep++;//0->1

		//构建billboard
		CreateBillboard(geoPt);

		geoPt.z() = 0.0f;
		geoPt.toWorld(_startWorld);
	}

	if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE && _nStep == 1 && _rlos.valid())
	{
		double lon, lat, h;
		if (getLocationAt(view, ea.getX(), ea.getY(), lon, lat,h))
		{
			GeoPoint geoEndPt = GeoPoint(getMapNode()->getMapSRS(),lon,lat,h,ALTMODE_ABSOLUTE);
			//设置半径
			geoEndPt.z() = 0.0f;
			osg::Vec3d endpt;
			geoEndPt.toWorld(endpt);
			osg::Vec3 distance=_startWorld-endpt;
			_rlos->setRadius(distance.length());
			
		}
	}

	if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && _nStep == 1 && _rlos.valid())
	{
		_nStep ++;//1->2

		if (NULL != m_consultationWnd)
			m_consultationWnd->ObserverPoints(_startLineTraker->getScreenStart(),_startLineTraker->getScreenEnd());

		m_bFinished = true;
	}

	return false;
}

bool GeoGlobe::ObserverPointsInRangeAnalysis::getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat, double &h)
{
	osgUtil::LineSegmentIntersector::Intersections results;            
	if ( getMapNode() &&  view->computeIntersections( x, y, results, 0x1 ) )
	{
		// find the first hit under the mouse:
		osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
		osg::Vec3d point = first.getWorldIntersectPoint();

		double lat_rad, lon_rad, height;       
		getMapNode()->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight( 
			point.x(), point.y(), point.z(), lat_rad, lon_rad, height );

		lat = osg::RadiansToDegrees( lat_rad );
		lon = osg::RadiansToDegrees( lon_rad );
		h = height;
		return true;
	}
	return false;
}

void GeoGlobe::ObserverPointsInRangeAnalysis::CreateBillboard(GeoPoint geoPt)
{
	//构建billboard
	std::ostringstream s1;
	s1 << "经度："<<geoPt.x()<<"\n纬度："<<geoPt.y()<<"\n高程："<<geoPt.z()<<"\n";
	std::string na = s1.str();
	na = CodeHelp::String_To_UTF8(na);
	osgEarth::Symbology::Style chapin;
	chapin.getOrCreate<osgEarth::IconSymbol>()->declutter() = false;
	chapin.getOrCreate<osgEarth::TextSymbol>()->declutter() = false;
	chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->size()=18.0f;
	//chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=Color::White;
	chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color()=Color::White;
	chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
	chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> pn = new GeoGlobe::PlaceName::GeoBillboard(geoPt,na,chapin,true);
	pn->setCullingActive(false);
	addChild(pn);
}

bool GeoGlobe::ObserverPointsInRangeAnalysis::IsFinished()
{
	return m_bFinished;
}
