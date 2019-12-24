#include <osgEarth/MapNode>
#include <osg/AutoTransform>
#include <osgViewer/View>
#include <osg/io_utils>
#include <osgGA/EventVisitor>
#include <Analysis/RadarTraker3DElement.h>
#include <tool/CodeTransformHelp.h>

#include <osgSim/SphereSegment>
#include <osgUtil/SmoothingVisitor>
#include <osg/io_utils>


using namespace osgEarth::Annotation;
using namespace osgEarth::Symbology;

GeoGlobe::RadarTraker3DElement::RadarTraker3DElement()
{

}

void GeoGlobe::RadarTraker3DElement::StartAnalysis(MapNode* mapNode, const DragMode& defaultMode)
{
	//LineTraker3DElement(mapNode, defaultMode);
}

GeoGlobe::RadarTraker3DElement::RadarTraker3DElement(MapNode* mapNode, const DragMode& defaultMode) :
	_position(mapNode->getMapSRS(), 0, 0, 0, ALTMODE_RELATIVE),
	_defaultMode(defaultMode)
{
	//光照
	getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	//Traker的状态
	_Finished = false;
	//记录交互操作的步骤，鼠标按下获取第一个点，抬起后开始Traker，在单击结束Traker
	//_nStep = 0;

	//点的初始化
	_SphereElement = new SphereElement(mapNode);
	addChild(_SphereElement);

	setNumChildrenRequiringEventTraversal(1);

	//用于计算垂直投影
	_projector = new osgManipulator::LineProjector;

	//设置MapNode
	setMapNode(mapNode);


}

GeoGlobe::RadarTraker3DElement::~RadarTraker3DElement()
{
	setMapNode(0L);
}

void GeoGlobe::RadarTraker3DElement::setMapNode(MapNode* mapNode)
{
	MapNode* oldMapNode = getMapNode();

	if (oldMapNode != mapNode)
	{
		_mapNode = mapNode;
	}
}

const GeoPoint& GeoGlobe::RadarTraker3DElement::getPosition() const
{
	return _position;
}

void GeoGlobe::RadarTraker3DElement::setPosition(const GeoPoint& position, bool fireEvents)
{
	if (_position != position)
	{
		_position = position;
		_SphereElement->setPosition(position);

		if (fireEvents)
			firePositionChanged();
	}
}

void GeoGlobe::RadarTraker3DElement::firePositionChanged()
{
	for (PositionChangedCallbackList::iterator i = _callbacks.begin(); i != _callbacks.end(); i++)
	{
		i->get()->onPositionChanged(this, _position);
	}
}

void GeoGlobe::RadarTraker3DElement::addPositionChangedCallback(PositionChangedCallback* callback)
{
	_callbacks.push_back(callback);
}

void GeoGlobe::RadarTraker3DElement::removePositionChangedCallback(PositionChangedCallback* callback)
{
	PositionChangedCallbackList::iterator i = std::find(_callbacks.begin(), _callbacks.end(), callback);
	if (i != _callbacks.end())
	{
		_callbacks.erase(i);
	}
}

void GeoGlobe::RadarTraker3DElement::traverse(osg::NodeVisitor& nv)
{
	if (nv.getVisitorType() == osg::NodeVisitor::EVENT_VISITOR)
	{
		osgGA::EventVisitor* ev = static_cast<osgGA::EventVisitor*>(&nv);
		for (osgGA::EventQueue::Events::iterator itr = ev->getEvents().begin();
			itr != ev->getEvents().end();
			++itr)
		{
			osgGA::GUIEventAdapter* ea = dynamic_cast<osgGA::GUIEventAdapter*>(itr->get());
			if (ea && handle(*ea, *(ev->getActionAdapter())))
				ea->setHandled(true);
		}
	}
	osg::Group::traverse(nv);
}

bool GeoGlobe::RadarTraker3DElement::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (ea.getHandled()) return false;

	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view) return false;
	if (!_mapNode.valid()) return false;

	if (_Finished)
		return false;

	if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)// && _nStep == 0)
	{
		double lon, lat, h;
		if (getLocationAt(view, ea.getX(), ea.getY(), lon, lat, h))
		{
			_screenStart = GeoPoint(getMapNode()->getMapSRS(), ea.getX(), ea.getY(), h);
			_nStep++;//0->1
			_start = GeoPoint(getMapNode()->getMapSRS(), lon, lat, h, ALTMODE_ABSOLUTE);
			setPosition(_start, false);
			_start.toWorld(_startWorld);
			
			_pointer.reset();

			osg::Camera *rootCamera = view->getCamera();
			_pointer.setCamera(rootCamera);
			_pointer.setMousePosition(ea.getX(), ea.getY());	
			_Finished = true;

		}		
	}

	if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)//  && _nStep == 1)
	{
		double dbReasult = _end.z() - _start.z();

		if (m_ptrAnalysisCallBack)
			m_ptrAnalysisCallBack->OnFinish(0.0);

		firePositionChanged();
		_Finished = true;
	}
 	return false;
}

const GeoPoint& GeoGlobe::RadarTraker3DElement::getScreenEnd() const
{
	return _screenEnd;
}

// void GeoGlobe::RadarTraker3DElement::AddAnalysisCallBack(GLOBE_NAME::AnalysisCallBack* ptrCallBack)
// {
// 	m_ptrAnalysisCallBack = ptrCallBack;
// }

bool GeoGlobe::RadarTraker3DElement::getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat, double &h)
{
	osgUtil::LineSegmentIntersector::Intersections results;
	if (getMapNode() && view->computeIntersections(x, y, results, 0x1))
	{
		// find the first hit under the mouse:
		osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
		osg::Vec3d point = first.getWorldIntersectPoint();

		double lat_rad, lon_rad, height;
		getMapNode()->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight(
			point.x(), point.y(), point.z(), lat_rad, lon_rad, height);

		lat = osg::RadiansToDegrees(lat_rad);
		lon = osg::RadiansToDegrees(lon_rad);
		h = height;
		return true;
	}
	return false;
}

void GeoGlobe::RadarTraker3DElement::setColor(const osg::Vec4f& color)
{

}

const GeoPoint& GeoGlobe::RadarTraker3DElement::getStart() const
{
	return _start;
}

const osg::Vec3d& GeoGlobe::RadarTraker3DElement::getStartWorld() const
{
	return _startWorld;
}

void GeoGlobe::RadarTraker3DElement::setStart(const GeoPoint& start)
{
	_start = start;
	_start.toWorld(_startWorld);
	_end.toWorld(_endWorld);
}

const GeoPoint& GeoGlobe::RadarTraker3DElement::getEnd() const
{
	return _end;
}

const GeoPoint& GeoGlobe::RadarTraker3DElement::getScreenStart() const
{
	return _screenStart;
}

const osg::Vec3d& GeoGlobe::RadarTraker3DElement::getEndWorld() const
{
	return _endWorld;
}

void GeoGlobe::RadarTraker3DElement::setEnd(const GeoPoint& end)
{
	_end = end;
	_end.toWorld(_endWorld);
}

bool GeoGlobe::RadarTraker3DElement::IsFinished()
{
	return _Finished;
}

