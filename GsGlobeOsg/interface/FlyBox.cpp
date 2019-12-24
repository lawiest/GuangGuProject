#include "FlyBox.h"
#include "tool/GGEarthManipulator.h"
#include "tool/AnimationPathHandler.h"
#include "tool/PointRotationHandler.h"
#include "tool/FreeFlyHandler.h"
#include <osgViewer/Viewer>
#include "OsgCoreSingleton.h"
#include "innerimp.h"

GLOBE_NS

FlyBox::FlyBox()
{
	osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> ptrAnimationPathHandler = new GeoGlobe::Tool::AnimationPathHandler;
		m_ptrFlyHandle = new ReferenceImp<GeoGlobe::Tool::AnimationPathHandler>(ptrAnimationPathHandler);

	osg::ref_ptr<GeoGlobe::Tool::PointRotationHandler> ptrPointRotationHandler = new GeoGlobe::Tool::PointRotationHandler;
	m_ptrPointFlyHandle = new ReferenceImp<GeoGlobe::Tool::PointRotationHandler>(ptrPointRotationHandler);
	
	osg::ref_ptr<GeoGlobe::Tool::FreeFlyHandler> ptrFreeFlyHandler = new GeoGlobe::Tool::FreeFlyHandler;
	m_ptrFreeFly = new ReferenceImp<GeoGlobe::Tool::FreeFlyHandler>(ptrFreeFlyHandler);
	_freeFly = true;
}


void FlyBox::addFlyPoint()
{
	if (OsgCoreSingleton::Instance()->GGEarthManipulator() == NULL)
	{		
		if (!OsgCoreSingleton::Instance()->OsgViewer())
			return;	
	}

	m_ptrFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> >()->recordControlPoint(1, 5);
	
}

void FlyBox::beginFly(std::string fileName)
{
	osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> ptrAnimPathHandle = m_ptrFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> >();

	if (fileName != "")   //¶ÁÈ¡ÎÄ¼þ
	{
		ptrAnimPathHandle->_ifSwitchOn = false;
		ptrAnimPathHandle->_animationPathFile = fileName;
		ptrAnimPathHandle->readAnimationPath();
	}	
	OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrAnimPathHandle);
	ptrAnimPathHandle->_ifSwitchOn = true;
	ptrAnimPathHandle->play(fileName);
	
}

void FlyBox::pauseFly()
{
	m_ptrFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> >()->pause();
}

void FlyBox::restartFly()
{
	m_ptrFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> >()->restartFly();
}

void FlyBox::backFly(bool bIsBack)
{
	m_ptrFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> >()->backFly(bIsBack);
}

void FlyBox::stopFly()
{	
	m_ptrFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> >()->stop();
// 	if (m_ptrAnimationPathHandler == NULL)
// 		return;
// 
// 	m_ptrAnimationPathHandler->_ifSwitchOn = false;
// 	m_ptrAnimationPathHandler->exit();
}

void FlyBox::saveFlyPath(std::string fileName)
{
 	 if (m_ptrFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> >()->_recordTimepointList.size() == 0)
		 return;
 	 
 	 	//if (osgDB::fileExists("../resource/FlyPath.path"))
 	 	//	DeleteFile(_T("../resource/FlyPath.path"));
 		 	
 	 if (fileName == "")
 	 		return;
 	 
	 m_ptrFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> >()->writeAnimationPath(fileName);
 	 	//m_ptrAnimationPathHandler->writeAnimationPath("../resource/FlyPath.path");
 	 
 		//m_ptrAnimationPathHandler->clearRecordPoint();
}

void FlyBox::reSetFlyPath()
{
	m_ptrFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> >()->reset();
}

void FlyBox::fastSpeed()
{
	m_ptrFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> >()->fastSpeed();
}

void FlyBox::reduceSpeed()
{
	m_ptrFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::AnimationPathHandler> >()->reduceSpeed();
}

void FlyBox::fastFreeFlySpeed()
{
	m_ptrFreeFly->CastTo<osg::ref_ptr<GeoGlobe::Tool::FreeFlyHandler> >()->fastSpeed();
}

void FlyBox::reduceFreeFlySpeed()
{
	m_ptrFreeFly->CastTo<osg::ref_ptr<GeoGlobe::Tool::FreeFlyHandler> >()->reduceSpeed();
}

void FlyBox::fastRotationSpeed()
{
	m_ptrPointFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::PointRotationHandler> >()->fastSpeed();
}

void FlyBox::reduceRotaionSpeed()
{
	m_ptrPointFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::PointRotationHandler> >()->reduceSpeed();
}

void FlyBox::beginRotation(double m_angle)
{
	if (OsgCoreSingleton::Instance()->GGEarthManipulator() == NULL)
	{
		if (!OsgCoreSingleton::Instance()->OsgViewer())
			return;
	}

	osg::ref_ptr<GeoGlobe::Tool::PointRotationHandler> ptrPointFlyHandle = m_ptrPointFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::PointRotationHandler> >();

	OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrPointFlyHandle);
	m_ptrPointFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::PointRotationHandler> >()->play(m_angle);

}

void FlyBox::stopRotation()
{
	m_ptrPointFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::PointRotationHandler> >()->stop();
}

void FlyBox::beginFreeFly()
{
	if (OsgCoreSingleton::Instance()->GGEarthManipulator() == NULL)
	{
		if (!OsgCoreSingleton::Instance()->OsgViewer())
			return;
	}

	osg::ref_ptr<GeoGlobe::Tool::FreeFlyHandler> ptrFreeFlyHandler = m_ptrFreeFly->CastTo<osg::ref_ptr<GeoGlobe::Tool::FreeFlyHandler> >();

	OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrFreeFlyHandler);
	m_ptrFreeFly->CastTo<osg::ref_ptr<GeoGlobe::Tool::FreeFlyHandler> >()->play();
	_freeFly = true;
}

void FlyBox::stopFreeFly()
{
	m_ptrFreeFly->CastTo<osg::ref_ptr<GeoGlobe::Tool::FreeFlyHandler> >()->stop();
	_freeFly = false;
}

FlyBox::~FlyBox()
{

}

GLOBE_ENDNS