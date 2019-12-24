#include <osgEarthUtil/Controls>
#include <tool/CodeTransformHelp.h>
#include "tool/GGEarthManipulator.h"
#include"Analysis/RotateImageHandler.h"
#include "CompassElement.h"
#include "OsgCoreSingleton.h"

GLOBE_NS

RotateImageHandler::RotateImageHandler(CompassElement* ptrCompassEle)
{
	m_ptrCompassEle = ptrCompassEle;
}

RotateImageHandler::~RotateImageHandler()
{

}

bool RotateImageHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	
	if (NULL == m_ptrCompassEle  || !m_ptrCompassEle->Visible())
		return false;

	osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());

	osg::Matrix matrix = view->getCamera()->getViewMatrix();
	matrix.setTrans(osg::Vec3());

	osg::Vec3 northVec = osg::Z_AXIS * matrix;
	northVec.z() = 0.0f;
	northVec.normalize();

	osg::Vec3 axis = osg::Y_AXIS ^ northVec;
	float angle = atan2(axis.length(), osg::Y_AXIS*northVec);
	axis.normalize();

	double an = angle * 180 / 3.1415926;

	if (axis.z() >= 0)
		an = -an;

	m_ptrCompassEle->setRotation(an);

	return false;
}

GLOBE_ENDNS
