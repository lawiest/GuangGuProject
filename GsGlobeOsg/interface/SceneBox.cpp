#include "SceneBox.h"
#include "osgEarth/Units"
#include <osgViewer/Viewer>
#include "tool/GGEarthManipulator.h"
#include "OsgCoreSingleton.h"

GLOBE_NS

SceneBox::SceneBox()
{
}

bool SceneBox::IsPositive()
{
	double Heading = OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint().heading().value().as(osgEarth::Units::DEGREES);
	double PrePitch = OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint().pitch().value().as(osgEarth::Units::DEGREES);
	return fabs(Heading - 0) <= 0.2 && fabs(PrePitch - (-90.0)) <= 0.2;
}

void SceneBox::Reset()
{
	OsgCoreSingleton::Instance()->GGEarthManipulator()->setFixedAngle(false);
	OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->setMinMaxDistance(1.0, DBL_MAX);
}

void SceneBox::setFixedAngle(const bool& bFixedAngle)
{
	OsgCoreSingleton::Instance()->GGEarthManipulator()->setFixedAngle(bFixedAngle);
}

bool SceneBox::getFixedAngle()
{
	return OsgCoreSingleton::Instance()->GGEarthManipulator()->getFixedAngle();
}

void SceneBox::setHomePosition(const KERNEL_NAME::GsRawPoint3D& point)
{
	const SpatialReference* ptrSrs = OsgCoreSingleton::Instance()->MapNode()->getMapSRS();
	Viewpoint vp;
	vp.focalPoint() = GeoPoint(ptrSrs, point.X, point.Y, 0, ALTMODE_ABSOLUTE);
	vp.heading()->set(0.0, Units::DEGREES);
	vp.pitch()->set(-90, Units::DEGREES);
	vp.range()->set(point.Z, Units::METERS);
	vp.positionOffset()->set(0, 0, 0);
	OsgCoreSingleton::Instance()->GGEarthManipulator()->setHomeViewpoint(vp);
}

KERNEL_NAME::GsRawPoint3D& SceneBox::getHomePosition()
{
	const Viewpoint viewpoint = OsgCoreSingleton::Instance()->GGEarthManipulator()->getHomeViewpoint();
	const osgEarth::GeoPoint point = viewpoint.focalPoint().get();
	return KERNEL_NAME::GsRawPoint3D(point.x(), point.y(), viewpoint.getRange());
}

void SceneBox::RestoreAngle(double dbTime)
{
	GGEarthManipulator* ptrManip = OsgCoreSingleton::Instance()->GGEarthManipulator();

	if (ptrManip)
	{
		osgEarth::Viewpoint here = ptrManip->getViewpoint();
		here.setHeading(m_dPreHeading);
		here.setPitch(m_dPrePitch);
		ptrManip->setViewpoint(here, dbTime);
	}
}

void SceneBox::PositiveAngle(double dbTime)
{
	GGEarthManipulator* ptrManip = OsgCoreSingleton::Instance()->GGEarthManipulator();

	if (NULL != ptrManip &&!IsPositive())
	{
		osgEarth::Viewpoint here = ptrManip->getViewpoint();
		m_dPreHeading = here.heading().value().as(osgEarth::Units::DEGREES);
		m_dPrePitch = here.pitch().value().as(osgEarth::Units::DEGREES);
		here.setHeading(0);
		here.setPitch(-90);
		ptrManip->setViewpoint(here, dbTime);
	}
}

void SceneBox::RangeFlyTo(double dXMin, double dXMax, double dYMin, double dYMax)
{
	double dAltitude = RangeToAltitude(dXMin, dXMax, dYMin, dYMax);
	InterpolateFlyTo((dXMax+ dXMin)/2,(dYMax+dYMin)/2,dAltitude,0,-90, dAltitude,5);
}

void SceneBox::RangeGoTo(double dXMin, double dXMax, double dYMin, double dYMax)
{
	double dAltitude = RangeToAltitude(dXMin, dXMax, dYMin, dYMax);
	InterpolateFlyTo((dXMax + dXMin) / 2, (dYMax + dYMin) / 2, dAltitude, 0, -90, dAltitude, 0);
}

//dheight滑轮缩放范围
void SceneBox::PointFlyTo(double dLongitude, double dLatitude, double dHeight)
{
	InterpolateFlyTo(dLongitude, dLatitude, 15, 0, -90, dHeight, 5);
}

void SceneBox::PointGoTo(double dLongitude, double dLatitude, double dHeight)
{
	InterpolateFlyTo(dLongitude, dLatitude, 15, 0, -90, dHeight, 0);
}

osg::Vec3d ToDKE(osg::Vec3d&  pt, double dblRadius)
{
	osg::Vec3d  ptRet;
	//实际的高程加上地球的半径
	pt.z() += dblRadius;
	//X和Y弧度。
	pt.y() *= osg::PI / 180.0;
	pt.x() *= osg::PI / 180.0;

	//Z坐标
	ptRet.z() = pt.z() * sin(pt.y());

	//由维度值计算出坐标点所在Z平面上的长度。
	double dblZLen = pt.z() * cos(pt.y());

	//计算出x和y
	ptRet.x() = dblZLen * cos(pt.x());
	ptRet.y() = dblZLen * sin(pt.x());
	return ptRet;
}

double SceneBox::RangeToAltitude(double dxMin, double dxMax, double dyMin, double dyMax)
{
	////每度对应的弧度
	double dbDegree2Radian = osg::PI / 180.0;
	double dEarthRadius = 6371393.0;
	
	//这么计算太粗略
	//return (tan(fabs(dyMax - dyMin)*dbDegree2Radian / 2) * 6371393.0) / tan(30 * dbDegree2Radian / 2.0);
	double dblAltitude = 0;
	double dblH = 0.0;
	//保留下面这一段，等有地形时加进来
	/*ICameraPtr ptrCamera = Camera();
	if (ptrCamera == NULL)
		return dblAltitude;

	//加入地形高成对范围的影响
	ITerrainAccessorPtr ptrTerrainAccessor = NULL;
	ptrCamera->get_TerrainAccessor(&ptrTerrainAccessor);
	if (NULL != ptrTerrainAccessor)
	{
		double dxMiddle = (dxMin + dxMax) / 2;
		double dyMiddle = (dyMin + dyMax) / 2;
		ptrTerrainAccessor->GetPointElevation(dxMiddle, dyMiddle, &dblH);
	}*/

	osg::Vec3d pt1(dxMin, dyMin, dblH);
	osg::Vec3d pt2(dxMin, dyMax, dblH);

	pt1 = ToDKE(pt1, dEarthRadius);
	pt2 = ToDKE(pt2, dEarthRadius);
	dblAltitude = sqrt(pow((pt1.x() - pt2.x()), 2)
	                  +pow((pt1.y() - pt2.y()), 2)
					  +pow((pt1.z() - pt2.z()), 2)) / 2.0f;


	//保证高度能使范围都在视野范围内
	double dbFov = 0.0, aspectRatio = 0.0, zNear = 0.0, zFar = 0.0;

	osgViewer::Viewer* ptrViewer = OsgCoreSingleton::Instance()->OsgViewer();

	if (ptrViewer)
		ptrViewer->getCamera()->getProjectionMatrixAsPerspective(dbFov, aspectRatio, zNear, zFar);

	dblAltitude /= tan(dbFov * dbDegree2Radian / 2);

	return dblAltitude;
}

double SceneBox::HalfAngle(double dbAngle)
{
	double dEarthRadius = 6371393.0;

	osgEarth::Viewpoint vp = OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();

	double dbHeight = vp.getRange();
	//已知两边和一个邻角求第三个角，正弦定理
	double dbSin = ((dbHeight + dEarthRadius)*sin(osg::DegreesToRadians(dbAngle / 2.0))) / dEarthRadius;

	//如果大于1说明已经到全球范围
	if (dbSin >= 1.0)
		dbSin = 1.0;

	//第三角
	double dbThirdA = 180.0 - osg::RadiansToDegrees(asin(dbSin));
	//对应球心半角
	double dbHalAngel = 180.0 - dbAngle / 2.0 - dbThirdA;

	return dbHalAngel;
}

void SceneBox::ViewExtent(double& dxMin, double& dxMax, double& dyMin, double& dyMax)
{
	double dbFov = 0.0, aspectRatio = 0.0,  zNear = 0.0,  zFar = 0.0;
	OsgCoreSingleton::Instance()->OsgViewer()->getCamera()->getProjectionMatrixAsPerspective(dbFov, aspectRatio, zNear, zFar);

	//求得纬度跨度半角
	double dbHalLatifAngel = HalfAngle(dbFov);

	//求出相机水平方向夹角
	double tanHalfHori = tan(osg::DegreesToRadians(dbFov))*aspectRatio;
	double dbHoriAngle = osg::RadiansToDegrees(atan(tanHalfHori));

	//求得经度跨度半角
	double dbHalflonAngel = HalfAngle(dbHoriAngle);

	osgEarth::Viewpoint vp = OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();
	dxMin = vp.focalPoint().value().x() - dbHalflonAngel;
	dxMax = vp.focalPoint().value().x() + dbHalflonAngel;
	dyMax = vp.focalPoint().value().y() + dbHalLatifAngel;
	dyMin = vp.focalPoint().value().y() - dbHalLatifAngel;
}

void SceneBox::InterpolateFlyTo(double dLongitude, double dLatitude, double dHeight, double dHeading, double dPitch, double dRange, double dTime)
{
	GGEarthManipulator* ptrManip = OsgCoreSingleton::Instance()->GGEarthManipulator();

	if (ptrManip)
	{
		osgEarth::Viewpoint here = ptrManip->getViewpoint();
		//相机焦点，地面所以是0
		//这里视角高度我改到了  15  （郝靖宇）  
		osg::Vec3d vecPoint(dLongitude, dLatitude, dHeight);
		here.focalPoint() = osgEarth::GeoPoint(here.focalPoint()->getSRS(), vecPoint, osgEarth::ALTMODE_ABSOLUTE);
		here.setHeading(dHeading);
		here.setPitch(dPitch);
		//和焦点的距离
		here.range() =dRange;
		ptrManip->setViewpoint(here, dTime);
	}
}

void SceneBox::getViewpoint(double& dLongitude, double& dLatitude, double& dHeight, double& dHeading, double& dPitch, double& dRange)
{
	osgEarth::Viewpoint vp = OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();
	dLongitude = vp.focalPoint().get().x();
	dLatitude = vp.focalPoint().get().y();
	dHeight = vp.focalPoint().get().z();
	dHeading = vp.heading()->as(osgEarth::Units::DEGREES);
	dPitch = vp.pitch()->as(osgEarth::Units::DEGREES);
	dRange = vp.range()->as(osgEarth::Units::METERS);
}

void SceneBox::ThrowingEnable(bool bThrowingEnable)
{
	OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->setThrowingEnabled(bThrowingEnable);
}

bool SceneBox::ThrowingEnable()
{
	return OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->getThrowingEnabled();
}

void SceneBox::LonLat2Window(double dbLon, double dbLat, double dbHeight, double& windowX, double& windowY)
{
	osgEarth::GeoPoint geoP(OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), dbLon, dbLat, dbHeight);
	osg::Vec3d world;
	geoP.toWorld(world);

	osgViewer::Viewer* ptrView = OsgCoreSingleton::Instance()->OsgViewer();

	osg::Matrixd VPW = ptrView->getCamera()->getViewMatrix() *
					  ptrView->getCamera()->getProjectionMatrix() *
					  ptrView->getCamera()->getViewport()->computeWindowMatrix();
	osg::Vec3d window = world * VPW;

	windowX = window.x();
	//由于osg屏幕坐标系原点是左下角，要转为常规左上角
	windowY = ptrView->getCamera()->getViewport()->height() - window.y();
}

void SceneBox::Window2LonLat(double windowX, double windowY, double &dbLon, double& dbLat, double& dbHeight)
{
	osgViewer::Viewer* ptrView = OsgCoreSingleton::Instance()->OsgViewer();
	osg::Matrix VPW = ptrView->getCamera()->getViewMatrix() *
					  ptrView->getCamera()->getProjectionMatrix() *
					  ptrView->getCamera()->getViewport()->computeWindowMatrix();

	osg::Matrix inverseVPW = osg::Matrix::inverse(VPW);
	osg::Vec3d world = osg::Vec3d(windowX, windowY, 0.0)* inverseVPW;

	osgEarth::GeoPoint geoP;
	geoP.fromWorld(OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), world);

	dbLon = geoP.x();
	dbLat = geoP.y();
	dbHeight = geoP.z();
}

void SceneBox::LonLat2World(double dbLon, double dbLat, double dbHeight, double & worldx, double & worldy, double & worldz)
{
	osgEarth::GeoPoint geoP(OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), dbLon, dbLat, dbHeight);
	osg::Vec3d world;
	geoP.toWorld(world);
	worldx = world.x();
	worldy = world.y();
	worldz = world.z();
}

void SceneBox::CorrectPosByOblique(KERNEL_NAME::GsRawPoint3D & point)
{
	osg::Vec3d vecPos(point.X, point.Y, point.Z);
	OsgCoreSingleton::Instance()->CorrectPosByOblique(vecPos);
	point.X = vecPos.x();
	point.Y = vecPos.y();
}

/*将相对倾斜的偏移坐标转换到正确坐标*/
void SceneBox::DeCorrectPosByOblique(KERNEL_NAME::GsRawPoint3D& point)
{
	osg::Vec3d vecPos(point.X, point.Y, point.Z);
	OsgCoreSingleton::Instance()->DeCorrectPosByOblique(vecPos);
	point.X = vecPos.x();
	point.Y = vecPos.y();
}

double SceneBox::DistanceFromCameraToGround()
{
	return OsgCoreSingleton::Instance()->GGEarthManipulator()->getDistance();
}

void SceneBox::setMinMaxDistance(double dbMinDistance, double dbMaxDistance)
{
	OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->setMinMaxDistance(dbMinDistance, dbMaxDistance);
}

void SceneBox::getMinMaxDistance(double& dbMinDistance, double& dbMaxDistance)
{
	dbMinDistance = OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->getMinDistance();
	dbMaxDistance = OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->getMaxDistance();
}

void SceneBox::setTerrainMinimumDistance(double minDistance)
{
	OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->setTerrainAvoidanceMinimumDistance(minDistance);
}

double SceneBox::getTerrainMinimumDistance()
{
	return OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->getTerrainAvoidanceMinimumDistance();
}

double SceneBox::CameraNorthAngle()
{
	osgViewer::View* view = OsgCoreSingleton::Instance()->OsgViewer();

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

	return an;
}


SceneBox::~SceneBox()
{

}

GLOBE_ENDNS