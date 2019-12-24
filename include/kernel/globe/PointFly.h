#pragma once
#include <utility.h>
#include <GsReference.h>

namespace GeoGlobe
{
	namespace Tool
	{
		class PointRotationHandler;
	}
}

namespace osgViewer
{
	class Viewer;
}

GLOBE_NS

class EarthManipulator;


class GS_API PointFly :public GeoStar::Utility::GsRefObject
{
public:
	PointFly();

	void beginRotation(double m_angle);

	void stopRotation();

	virtual ~PointFly();

private:
	GsReferencePtr m_ptrPointFlyHandle;
};


GS_SMARTER_PTR(PointFly);

GLOBE_ENDNS
