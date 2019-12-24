#pragma once
#include <utility.h>
#include <GsReference.h>

namespace GeoGlobe
{
	namespace Tool
	{
		class AnimationPathHandler;
	}
}

namespace osgViewer
{
	class Viewer;
}

GLOBE_NS

class EarthManipulator;


class GS_API FlyBox :public GeoStar::Utility::GsRefObject
{
public:

	FlyBox();

	void addFlyPoint();
	void beginFly(std::string fileName);
	void stopFly();
	void pauseFly();
	void restartFly();
	void backFly(bool bIsBack);
	void saveFlyPath(std::string fileName);
	void reSetFlyPath();

	void beginRotation(double m_angle);
	void stopRotation();
	void fastRotationSpeed();
	void reduceRotaionSpeed();

	void beginFreeFly();
	void stopFreeFly();

	void fastSpeed();
	void reduceSpeed();

	void fastFreeFlySpeed();
	void reduceFreeFlySpeed();

	bool _freeFly;

	virtual ~FlyBox();
		
private:
	GsReferencePtr m_ptrFlyHandle;	
	GsReferencePtr m_ptrPointFlyHandle;
	GsReferencePtr m_ptrFreeFly;
};

GS_SMARTER_PTR(FlyBox);

GLOBE_ENDNS
