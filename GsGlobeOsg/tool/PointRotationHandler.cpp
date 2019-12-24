#include "Tool/PointRotationHandler.h"
#include <osgViewer/Viewer>
#include "OsgCoreSingleton.h"
#include "tool/GGEarthManipulator.h"

using namespace GeoGlobe::Tool;

PointRotationHandler::PointRotationHandler()
{

}

void PointRotationHandler::play(double flyAngle)
{
	m_angle = flyAngle;
	GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();
	_fly = true;
}

void PointRotationHandler::stop()
{
	GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->cancelViewpointTransition();
	_fly = false;
}

void PointRotationHandler::reduceSpeed()
{
	if (m_angle > 5 )
	{
		m_angle -= 20;
	}	
}

void PointRotationHandler::fastSpeed()
{
	m_angle += 40;
}



bool PointRotationHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osg::ref_ptr<GGEarthManipulator> ptrManp = GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator();

	osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());

	if (!ptrManp.valid())
		ptrManp = dynamic_cast<GGEarthManipulator*>(view->getCameraManipulator());

	if (!ptrManp.valid())
		return false;

	osgEarth::Viewpoint vp;
	

	vp = ptrManp->getViewpoint();
	
	//double pitch = osg::DegreesToRadians(vp.getPitch());
	double heading = osg::DegreesToRadians(vp.getHeading());

	if (ea.getEventType() == osgGA::GUIEventAdapter::FRAME&&_fly)
	{
		if (!ptrManp->isSettingViewpoint())
		{
//  			while (pitch <= 360)
//  			{				
// 				ptrManp->rotateByZ(pitch);
// 				pitch += m_angle;
// 			}			 
			heading = vp.getHeading();
			heading -= osg::DegreesToRadians(m_angle);
			vp.setHeading(heading);
			ptrManp->setViewpoint(vp);
		}
	}
	return false;
}