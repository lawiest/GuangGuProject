#include "PointFly.h"
#include "tool/GGEarthManipulator.h"
#include "tool/PointRotationHandler.h"
#include <osgViewer/Viewer>
#include "OsgCoreSingleton.h"
#include "innerimp.h"

GLOBE_NS

PointFly::PointFly()
{
	osg::ref_ptr<GeoGlobe::Tool::PointRotationHandler> ptrPointRotationHandler = new GeoGlobe::Tool::PointRotationHandler;

	m_ptrPointFlyHandle = new ReferenceImp<GeoGlobe::Tool::PointRotationHandler>(ptrPointRotationHandler);
}

void PointFly::beginRotation(double m_angle)
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

void PointFly::stopRotation()
{
	m_ptrPointFlyHandle->CastTo<osg::ref_ptr<GeoGlobe::Tool::PointRotationHandler> >()->stop();
}


PointFly::~PointFly()
{
}

GLOBE_ENDNS
