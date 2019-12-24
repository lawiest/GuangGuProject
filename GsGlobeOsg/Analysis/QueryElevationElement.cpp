#include <osgEarthUtil/Controls>
#include <tool/CodeTransformHelp.h>
#include "tool/GGEarthManipulator.h"
#include"Analysis/QueryElevationElement.h"
#include "OsgCoreSingleton.h"
#include "LonAndLatElement.h"
#include <OsgCoreSingleton.h>

GLOBE_NS

QueryElevationElement::QueryElevationElement(LonAndLatElement* ptrLonAndLatEle)
{
	m_ptrLonAndLatEle = ptrLonAndLatEle;
}

QueryElevationElement::~QueryElevationElement()
{
}

void QueryElevationElement::CheckChange(osgEarth::Viewpoint viewpoint)
{
	if (m_preViewPoint.focalPoint()->x() != viewpoint.focalPoint()->x() ||
		m_preViewPoint.focalPoint()->y() != viewpoint.focalPoint()->y() ||
		m_preViewPoint.focalPoint()->z() != viewpoint.focalPoint()->z() ||
		m_preViewPoint.heading() != viewpoint.heading() || 
		m_preViewPoint.pitch() != viewpoint.pitch() || 
		m_preViewPoint.range() != viewpoint.range())
	{
		Change();
	}
}

void QueryElevationElement::update(osgViewer::View* view)
{
	if (!m_ptrLonAndLatEle)
		return;

	osg::ref_ptr<GGEarthManipulator> m_earthpulator = dynamic_cast<GGEarthManipulator*>(view->getCameraManipulator());
	double view_height = m_earthpulator->getDistance();
	osgEarth::Viewpoint viewpp = m_earthpulator->getViewpoint();

	CheckChange(viewpp);

	//MouseMovePoint(GsRawPoint3D(viewpp.focalPoint()->x(), viewpp.focalPoint()->y(), view_height));

	osg::Vec3d mapPos = osg::Vec3d(viewpp.focalPoint()->x(), viewpp.focalPoint()->y(), 0);
	std::ostringstream s1;
	ostringstream osm;
	osm.precision(3);           // 浮点数转换限制  
	osm.setf(std::ios::fixed); // 将浮点数的位数限定为小数点之后的位数  
	osm << view_height;
	s1 << std::fixed << std::setprecision(9)<< "纬度：" << mapPos.y() << "\n" << "经度：" << mapPos.x() << "\n" << "高度(米)：" << osm.str();
	std::string na = CodeHelp::String_To_UTF8(s1.str());
	m_ptrLonAndLatEle->setText(na);

	m_preViewPoint = viewpp;
}

bool QueryElevationElement::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (!m_ptrLonAndLatEle && !m_ptrLonAndLatEle->Visible())
		return false;

	osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
	update(view);

	//if(ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
	//{
	//	osgEarth::GeoPoint mapPoint;
	//	osg::Vec3d world;
	//	osgUtil::LineSegmentIntersector::Intersections hits;
	//	if (view->computeIntersections(ea.getX(), ea.getY(), hits))
	//	{
	//		world = hits.begin()->getWorldIntersectPoint();
	//		mapPoint.fromWorld(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getTerrain()->getSRS(), world);
	//		MouseMovePoint(GsRawPoint3D(mapPoint.x(), mapPoint.y(), mapPoint.z()));
	//	}
	//}

	return false;
}

GLOBE_ENDNS