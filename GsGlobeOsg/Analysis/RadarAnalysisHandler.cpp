#include <Analysis/RadarAnalysisHandler.h>
#include <tool/CodeTransformHelp.h>
#include "OsgCoreSingleton.h"
#include <osg/NodeVisitor>
#include <osg/Geometry>  

using namespace osgEarth::Annotation;

GeoGlobe::RadarAnalysisHandler::RadarAnalysisHandler(MapNode* mapNode, GeoGlobe::IConsultationWindow* consultationWnd)
{
	m_bFinished = false;
	setMapNode(mapNode);	

	m_consultationWnd = consultationWnd;
	_startPointTraker = new RadarTraker3DElement(getMapNode());
	_startPointTraker->setDefaultDragMode(RadarTraker3DElement::DRAGMODE_VERTICAL);
	addChild(_startPointTraker);
	_nStep = 0;
}

GeoGlobe::RadarAnalysisHandler::~RadarAnalysisHandler()
{
}

MapNode* GeoGlobe::RadarAnalysisHandler::getMapNode()
{
	return _mapNode.get();
}

void GeoGlobe::RadarAnalysisHandler::setMapNode(MapNode* mapNode)
{
	MapNode* oldMapNode = getMapNode();
	if (oldMapNode != mapNode)
	{
		_mapNode = mapNode;
	}
}

void GeoGlobe::RadarAnalysisHandler::traverse(osg::NodeVisitor& nv)
{
	if (nv.getVisitorType() == osg::NodeVisitor::EVENT_VISITOR)
	{
		osgGA::EventVisitor* ev = static_cast<osgGA::EventVisitor*>(&nv);
		for (osgGA::EventQueue::Events::iterator itr = ev->getEvents().begin(); itr != ev->getEvents().end(); ++itr)
		{
			osgGA::GUIEventAdapter* ea = dynamic_cast<osgGA::GUIEventAdapter*>(itr->get());
			if (ea && handle(*ea, *(ev->getActionAdapter())))
				ea->setHandled(true);
		}
	}
	osg::Group::traverse(nv);
}


bool GeoGlobe::RadarAnalysisHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (ea.getHandled())
		return false;

	if (_nStep >= 2)
		return false;

	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view)
		return false;

	if (!_mapNode.valid())
		return false;	

	if (_nStep == 0 && _startPointTraker->IsFinished())
	{
 		GeoPoint geoPt = _startPointTraker->getPosition();
		geoPt.toWorld(_radarCenter);

		pGeode = new osg::Geode;
		r_sphere = new RadarSphere(16, 16, _radarCenter, _mapNode);
		pGeode->addDrawable(r_sphere);

		osg::ref_ptr<osg::StateSet> stateset = pGeode->getOrCreateStateSet();
		stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);		stateset->setAttribute(polyMode);
		pGeode->setStateSet(stateset);

		addChild(pGeode);
	
		_nStep++;//0->1

		geoPt.z() = 0.0f;
		geoPt.toWorld(_startWorld);

	}

	if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE && _nStep == 1)
	{
		double lon, lat, h;
		if (getLocationAt(view, ea.getX(), ea.getY(), lon, lat, h))
		{
			GeoPoint geoEndPt = GeoPoint(getMapNode()->getMapSRS(), lon, lat, h, ALTMODE_ABSOLUTE);
			osg::Vec3d endpt;
			geoEndPt.toWorld(endpt);
			osg::Vec3 distance = _startWorld - endpt;
			double p_radius = distance.length();
			r_sphere->setRadius(p_radius);
		}
	}

	if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && _nStep == 1)
	{
		_nStep++;//1->2

		m_bFinished = true;	

		osg::ref_ptr<osg::Geode> ss = compute_line(_mapNode, _radarCenter);
		removeChild(pGeode); 		
		ss->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		addChild(ss.get());
 	}
	return false;
}

osg::Geode* GeoGlobe::RadarAnalysisHandler::compute_line(osg::Node* node, osg::Vec3d radarCenter)
{
	VertexExtractor(pGeode);
	int size_t = extracted_verts.get()->size();

	osg::Vec4d _goodColor = osg::Vec4(0.0, 1.0, 0.0 ,1.0);
	osg::Vec4d _badColor = osg::Vec4(1.0, 0.0, 0.0, 1.0);

	osg::Geometry* geometry = new osg::Geometry;
	geometry->setUseVertexBufferObjects(true);

	osg::Vec3Array* verts = new osg::Vec3Array();
	verts->reserve(size_t * 5);
	geometry->setVertexArray(verts);

	osg::Vec4Array* colors = new osg::Vec4Array();
	colors->reserve(size_t * 5);

	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	std::vector<osg::Vec3>::iterator iter = extracted_verts.get()->begin();

	osg::ref_ptr<osgUtil::IntersectorGroup> ivGroup = new osgUtil::IntersectorGroup();
	{
		for (int i = 0; i < size_t; i++)
		{
			osg::Vec3d end = osg::Vec3(iter->x(), iter->y(), iter->z());
			osg::ref_ptr<osgUtil::LineSegmentIntersector> dplsi = new osgUtil::LineSegmentIntersector(radarCenter, end);
			ivGroup->addIntersector(dplsi.get());
			iter++;
		}

		osgUtil::IntersectionVisitor iv;
		iv.setIntersector(ivGroup.get());

		node->accept(iv);

		bool firstLOS;

		int m_iNumPartsLatitude_half = 16;
		int m_iNumPartsLongtitude_half = 16;

		int iNumLongtitude = m_iNumPartsLongtitude_half * 2;
		int iNumLatitude = m_iNumPartsLatitude_half * 2;

        //绘制纬线圈通视
		for (int k = 0; k < iNumLatitude; ++k)
		{
			int i;
			for (i = 0; i < iNumLongtitude; ++i)
			{
				if (((iNumLongtitude + 1)*k + i + 1) % (m_iNumPartsLongtitude_half * 2 + 1) != 0)
				{
					osgUtil::LineSegmentIntersector* los1 = dynamic_cast<osgUtil::LineSegmentIntersector*>(ivGroup->getIntersectors()[(iNumLongtitude + 1)*k + i].get());
					osgUtil::LineSegmentIntersector* los2 = dynamic_cast<osgUtil::LineSegmentIntersector*>(ivGroup->getIntersectors()[(iNumLongtitude + 1)*k + i + 1].get());

					osgUtil::LineSegmentIntersector::Intersections& hits1 = los1->getIntersections();
					osgUtil::LineSegmentIntersector::Intersections& hits2 = los2->getIntersections();

					osg::Vec3d end1 = los1->getEnd();
					osg::Vec3d end2 = los2->getEnd();

					osg::Vec3d hit;
					bool hasLOS1 = hits1.empty();
					bool hasLOS2 = hits2.empty();

					if (hasLOS1)
					{
						verts->push_back(end1);
						colors->push_back(_goodColor);
					}
					else
					{
						verts->push_back(end1);
						colors->push_back(_badColor);
					}
					if (hasLOS2)
					{
						verts->push_back(end2);
						colors->push_back(_goodColor);
					}
					else
					{
						verts->push_back(end2);
						colors->push_back(_badColor);
					}
				}
			}
		}

		//绘制经线圈通视
		for (int k = 0; k < iNumLatitude; ++k)
		{
			int i;
			for (i = 0; i < iNumLongtitude; ++i)
			{
				if (((iNumLongtitude + 1)*k + i + 1) % (m_iNumPartsLongtitude_half * 2 + 1) != 0)
				{
					osgUtil::LineSegmentIntersector* los1 = dynamic_cast<osgUtil::LineSegmentIntersector*>(ivGroup->getIntersectors()[(iNumLongtitude + 1)*k + i].get());
					osgUtil::LineSegmentIntersector* los2 = dynamic_cast<osgUtil::LineSegmentIntersector*>(ivGroup->getIntersectors()[(iNumLongtitude + 1)*k + i + (iNumLongtitude + 1)].get());

					osgUtil::LineSegmentIntersector::Intersections& hits1 = los1->getIntersections();
					osgUtil::LineSegmentIntersector::Intersections& hits2 = los2->getIntersections();

					osg::Vec3d end1 = los1->getEnd();
					osg::Vec3d end2 = los2->getEnd();

					osg::Vec3d hit;
					bool hasLOS1 = hits1.empty();
					bool hasLOS2 = hits2.empty();

					if (hasLOS1)
					{
						verts->push_back(end1);
						colors->push_back(_goodColor);
					}
					else
					{
						verts->push_back(end1);
						colors->push_back(_badColor);
					}
			       if (hasLOS2)
				   {
					   verts->push_back(end2);
					   colors->push_back(_goodColor);
				   }
				   else
				   {
					   verts->push_back(end2);
					   colors->push_back(_badColor);
				   }				   
				}
			}
		}
	}		
	geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, verts->size()));

	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(geometry);
	getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	return geode;
}


void GeoGlobe::RadarAnalysisHandler::VertexExtractor(osg::ref_ptr<osg::Geode> geode)
{
	extracted_verts = new osg::Vec3Array;

	for (unsigned int i = 0; i < geode->getNumDrawables(); ++i)
	{
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

		if (!geom)
			continue;
		osg::Vec3Array * verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		if (!verts)
			continue;
		extracted_verts->insert(extracted_verts->end(), verts->begin(), verts->end());
	}
}


bool GeoGlobe::RadarAnalysisHandler::getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat, double &h)
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


bool GeoGlobe::RadarAnalysisHandler::IsFinished()
{
	return m_bFinished;
}


