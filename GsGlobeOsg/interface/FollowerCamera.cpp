#include "FollowerCamera.h"
#include "tool/GGEarthManipulator.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <osgEarth/GeoMath>
#include <osgEarthAnnotation/GeoPositionNode>
#include <osgViewer/Viewer>
#include <osgEarthAnnotation/AnnotationUtils>

GLOBE_NS

osg::ref_ptr<osgEarth::Annotation::GeoPositionNode>      g_geoPos = NULL;
osg::ref_ptr<osg::Node>  g_ptrModel;

FollowerCamera::FollowerCamera(Modal eModel)
{
	m_eModal = eModel;
	Init();
}

void FollowerCamera::setModal(Modal eModel)
{
	m_eModal = eModel;
	Init();
}

void FollowerCamera::Init()
{
	if (g_geoPos == NULL && m_eModal == follow)
	{
		g_geoPos = new Annotation::GeoPositionNode();
		osgEarth::Viewpoint vp = GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();

		g_ptrModel = osgEarth::Annotation::AnnotationUtils::createHemisphere(5.0, osg::Vec4(1, .7, .4, 0.0));
		g_geoPos->getPositionAttitudeTransform()->addChild(g_ptrModel);
		GLOBE_NAME::OsgCoreSingleton::Instance()->Root()->addChild(g_geoPos);
		vp.setNode(g_ptrModel);
		GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(vp, 0);
	}
}

void FollowerCamera::ReEstablish()
{
	osgEarth::Viewpoint vp = GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();
	vp.setNode(g_ptrModel);
	GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(vp,3);
}

void FollowerCamera::Bind(Element* ptrElement)
{
	m_ptrElement = ptrElement;
	ptrElement->OnPositionChange.Add(this, &FollowerCamera::OnPositionChange);
	double x, y, z;
	ptrElement->getPosition(x, y, z);

	osgEarth::Viewpoint vp = GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();
	vp.focalPoint() = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(),x, y, z);
	vp.setRange(z+100);

	osg::ref_ptr < osg::Node > ptrNode;
	vp.getNode(ptrNode);
	if (ptrNode == NULL)
	{
		vp.setNode(g_ptrModel);
	}

	GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(vp,3);

	time_t now = time(NULL);
	m_preTime = now;
}

void FollowerCamera::Unbind(Element* ptrElement)
{
	m_ptrElement = NULL;
	ptrElement->OnPositionChange.Remove(this, &FollowerCamera::OnPositionChange);

	if (m_eModal == follow)
	{
		osgEarth::Viewpoint vp = GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();
		vp.focalPoint() = g_geoPos->getPosition();
		vp.setNode(NULL);
		GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(vp, 0);
	}

}

void FollowerCamera::OnPositionChange(KERNEL_NAME::GsRawPoint3D point)
{
	if (m_eModal == interp)
	{	
		time_t now = time(NULL);

		if (now - m_preTime > 4)
		{
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->setArcViewpointTransitions(false);
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->setAutoViewpointDurationEnabled(false);
			osgEarth::Viewpoint vp /*= GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint()*/;
			vp.focalPoint() = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), point.X, point.Y, point.Z);
			vp.setRange(point.Z + 100);

			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(vp, 4);
			m_preTime = now;
		}
	}
	else
	{	
		g_geoPos->setPosition(osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(),point.X, point.Y, point.Z));
	}
}

FollowerCamera::~FollowerCamera()
{

}

GLOBE_ENDNS