
#include <Analysis/SunlightAnalysisSector.h>
#include <osgEarthUtil/Ephemeris>
#include <osgUtil/IntersectVisitor>
#include <sstream>
#include <string>
#include <tool/CodeTransformHelp.h>
#include <PlaceName/GeoBillboard.h> 


GeoGlobe::SunlightAnalysisSector::SunlightAnalysisSector(MapNode* mapNode, GeoGlobe::IConsultationWindow* consultationWnd)
{

	m_bFinished = false;
	setMapNode(mapNode);

	//m_consultationWnd = consultationWnd;
	_startLineTraker  = new LineTraker3DElement(getMapNode());  
	_startLineTraker->setDefaultDragMode(LineTraker3DElement::DRAGMODE_VERTICAL);
	addChild(_startLineTraker);

	_nStep = 0;
	m_step = 10;
	m_time = 0;
	m_LineLength = 120;
	InitSector();
}

GeoGlobe::SunlightAnalysisSector::~SunlightAnalysisSector()
{
}

MapNode* GeoGlobe::SunlightAnalysisSector::getMapNode() 
{ 
	return _mapNode.get();
}

void GeoGlobe::SunlightAnalysisSector::setMapNode( MapNode* mapNode )
{
	MapNode* oldMapNode = getMapNode();
	if ( oldMapNode != mapNode )
	{
		_mapNode = mapNode;
	}
}

void GeoGlobe::SunlightAnalysisSector::traverse(osg::NodeVisitor& nv)
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

void GeoGlobe::SunlightAnalysisSector::Analysis()
{
	if (NULL == _startLineTraker)
		return;

	GeoPoint geoPt = _startLineTraker->getPosition();
	UpdateSector(geoPt);	
}
bool GeoGlobe::SunlightAnalysisSector::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (ea.getHandled()) 
		return false;
	
	m_view = dynamic_cast<osgViewer::View*>(&aa);
	if (!m_view) 
		return false;

	if (!_mapNode.valid()) 
		return false;

	//不做处理，去响应分析按钮
	//if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	//{
	//		GeoPoint geoPt = _startLineTraker->getPosition();
	//      UpdateSector(geoPt);	
	//}

	return false;
}

void GeoGlobe::SunlightAnalysisSector::InitSector()
{
	if (!m_view) 
		return;

	for(int min=480;min<960;min+=m_step)//m_step分钟间隔
	{
		osg::ref_ptr< LinearLineOfSightNode > _los_i;
		_los_i = new LinearLineOfSightNode(_mapNode,GeoPoint(),GeoPoint());
		addChild(_los_i);
		m_rlos.push_back(_los_i);
	}
}
void GeoGlobe::SunlightAnalysisSector::UpdateSector(GeoPoint geoStartPt)
{
	if(true == m_bFinished)
	{
		return;
	}
	if (!m_view) 
	{
		return;
	}
	if(geoStartPt.x()==0||geoStartPt.y()==0)
	{
		return;
	}
	if(m_rlos.size() == 0)
	{
		InitSector();
	}
	m_bFinished = true;
	//计算上午8点到下午4点的太阳位置，10分钟做一次计算
	int sum_minute=0;
	osg::ref_ptr<osgEarth::Util::Ephemeris> ep = new Ephemeris();
	osg::Vec3d sp;
	geoStartPt.toWorld(sp);
	m_time = 0;
	

	for(int min=480,i=0;min<960;min+=m_step,i++ )//10分钟间隔
	{
		if(i >= m_rlos.size())
		{
			break;
		}
		osg::ref_ptr< LinearLineOfSightNode > _los_i = m_rlos[i];
		_los_i->setStart(geoStartPt);

		int h=min/60;

		//计算太阳位置
		osg::Vec3d sunPosition= ep->getSunPosition(osgEarth::DateTime(2017,4,1, h-8+(min-h*60)/60.0)).geocentric;		

		//归一化到m_LineLength米
		osg::Vec3d d = sunPosition - sp;
		d.length();
		osg::Vec3d sunPosition_near;
		if(d.length() > 0)
		{
			sunPosition_near.x() = m_LineLength*d.x()/d.length();
			sunPosition_near.y() = m_LineLength*d.y()/d.length();
			sunPosition_near.z() = m_LineLength*d.z()/d.length();
			sunPosition_near+=sp;
		}

		GeoPoint latlon1;
		latlon1.fromWorld(getMapNode()->getMapSRS(), sunPosition_near);	
		_los_i->setEnd(latlon1);

		//通过线相交情况，得到累积可见或者不可见时间
		if(_los_i->getHasLOS())
		{
			m_time+=m_step;
		}

	}

	//可见时间分钟换算成小时
	m_time =  (m_time + 0.0f)/ 60;
}

float GeoGlobe::SunlightAnalysisSector::GetSunlightTime()
{
	return m_time;
}

bool GeoGlobe::SunlightAnalysisSector::getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat, double &h)
{
	osgUtil::LineSegmentIntersector::Intersections results;  
	if ( getMapNode() &&  view->computeIntersections( x, y, results) )
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


bool GeoGlobe::SunlightAnalysisSector::IsFinished()
{
	return m_bFinished;
}