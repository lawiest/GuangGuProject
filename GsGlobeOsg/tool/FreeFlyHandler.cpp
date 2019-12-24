#include "Tool/FreeFlyHandler.h"
#include <osgViewer/Viewer>
#include "OsgCoreSingleton.h"
#include "tool/GGEarthManipulator.h"
#include <osgEarth/GeoMath>
#include <osgEarthAnnotation/AnnotationUtils>

using namespace GeoGlobe::Tool;
using namespace osgEarth::Annotation;

#define D2R (osg::PI/180.0)
#define R2D (180.0/osg::PI)

FreeFlyHandler::FreeFlyHandler()
{
	
}

void FreeFlyHandler::play()
{
	_geo = new Annotation::GeoPositionNode();
	osgEarth::Viewpoint vp =  GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();

	osg::Vec3d eye, center, up;
	GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->getCamera()->getViewMatrixAsLookAt(eye, center, up);	
	
	_focalPoint = vp.focalPoint().get();

	lon0 = _focalPoint.x();
	lat0 = _focalPoint.y();

	osg::Vec3d vec = center - eye; 

	GeoPoint end;

	vec.normalize();
	osg::Vec3d endworld;
	endworld = eye + vec * 50000;
    end.fromWorld(_focalPoint.getSRS(), endworld);	

	height = _focalPoint.z();
	lon1 = end.x();
	lat1 = end.y();

	_model = AnnotationUtils::createHemisphere(5.0, osg::Vec4(1, .7, .4, 1.0));
	
	_geo->getPositionAttitudeTransform()->addChild(_model);
	GLOBE_NAME::OsgCoreSingleton::Instance()->Root()->addChild(_geo);
	vp.setNode(_model);
	GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(vp);

	_fly = true;
	_time = 0.00005;
	t = 0;
}

void FreeFlyHandler::stop()
{
	GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->cancelViewpointTransition();
	_fly = false;
}

void FreeFlyHandler::fastSpeed()
{
	_time = _time * 5;
}

void FreeFlyHandler::reduceSpeed()
{
	_time = _time / 5;
}

void interpolate(double lat1Rad, double lon1Rad,
	double lat2Rad, double lon2Rad,
	double t,
	double& out_latRad, double& out_lonRad)
{
	static osg::EllipsoidModel em; // questionable. make non-static?

	osg::Vec3d v0, v1;

	em.convertLatLongHeightToXYZ(lat1Rad, lon1Rad, 0, v0.x(), v0.y(), v0.z());
	double r0 = v0.length();
	v0.normalize();
	em.convertLatLongHeightToXYZ(lat2Rad, lon2Rad, 0, v1.x(), v1.y(), v1.z());
	double r1 = v1.length();
	v1.normalize();

	osg::Vec3d axis = v0 ^ v1;
	double angle = acos(v0 * v1);
	osg::Quat q(angle * t, axis);

	v0 = (q * v0) * 0.5*(r0 + r1);

	double dummy;
	em.convertXYZToLatLongHeight(v0.x(), v0.y(), v0.z(), out_latRad, out_lonRad, dummy);
}


bool FreeFlyHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osg::ref_ptr<GGEarthManipulator> ptrManp = GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator();

	osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());

	if (!ptrManp.valid())
		ptrManp = dynamic_cast<GGEarthManipulator*>(view->getCameraManipulator());

	if (!ptrManp.valid())
		return false;

	osgEarth::Viewpoint vp = ptrManp->getViewpoint();

	double heading = osg::DegreesToRadians(vp.getHeading());
	double pitch = osg::DegreesToRadians(vp.getPitch()); 	

 	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
	{ 	
		Viewpoint oldvp = ptrManp->getViewpoint();
		double seconds = 0.5;

 		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)  
 		{
			lon1 -= 0.001;
// 			heading = vp.getHeading();
// 			heading -= osg::DegreesToRadians(90.0);
// 			vp.setHeading(heading);
// 			ptrManp->setViewpoint(vp);		
 		}

 		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
 		{	
		    lon1 += 0.001;
//  			heading = vp.getHeading();
//  			heading += osg::DegreesToRadians(90.0);
//  			vp.setHeading(heading);
//  			ptrManp->setViewpoint(vp);			
 		}

 		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
 		{		
			height += 10;
			pitch = vp.getPitch();
			pitch -= osg::DegreesToRadians(45.0);
			vp.setPitch(pitch);
			ptrManp->setViewpoint(vp);
 		}

 		if (ea.getKey() ==osgGA::GUIEventAdapter::KEY_Down)
 		{
			height -= 10;
			pitch = vp.getPitch();
			pitch += osg::DegreesToRadians(45.0);
			vp.setPitch(pitch);
			ptrManp->setViewpoint(vp);

// 			Viewpoint vp;
// 			vp.positionOffset() = oldvp.positionOffset().get() + osg::Vec3f(0, 0, -100);
// 			ptrManp->setViewpoint(vp, seconds);
 	//		pitch = vp.getPitch();
 	//		pitch += osg::DegreesToRadians(45.0);
 	//		vp.setPitch(pitch);
 	//		ptrManp->setViewpoint(vp);
 		}		
		if (ea.getKey() == 'U' || ea.getKey() == 'u')
		{
			_time = _time * 5;
		}
		if (ea.getKey() == 'L' || ea.getKey() == 'l')
		{
			_time = _time / 5;
		}
  	}
 


	if (ea.getEventType() == osgGA::GUIEventAdapter::FRAME &&_fly)
 	{
 		double lat, lon;
 		interpolate(D2R*lat0, D2R*lon0, D2R*lat1, D2R*lon1, t, lat, lon);
		t += _time;

		double focalLon = R2D*lon;
		double focalLat = R2D*lat;

		GeoPoint p(_focalPoint.getSRS(), focalLon, focalLat, height);
 		double bearing = GeoMath::bearing(focalLon, focalLat, lat, lon);

 		_geo->setPosition(p);

	}
	return false;
}