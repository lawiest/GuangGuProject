#pragma once
#include <utility.h>
#include <geometry.h>

GLOBE_NS

class GS_API SceneBox :public GeoStar::Utility::GsRefObject
{
public:
	SceneBox();

	/*判断当前相机是否摆正*/
	bool IsPositive();

	/*强制让相机姿态摆正，即相对于球面无任何旋转*/
	void PositiveAngle(double dbTime = 2.0);

	/*恢复摆正之前的姿态*/
	void RestoreAngle(double dbTime = 2.0);

	/*设置是否固定视角，不让旋转*/
	void setFixedAngle(const bool& bFixedAngle);

	/*获得是否固定视角*/
	bool getFixedAngle();

	/*设置相机离地面最大最小距离*/
	void setMinMaxDistance(double dbMinDistance = 1.0,double dbMaxDistance = DBL_MAX);

	/*获得相机离地面最大最小距离*/
	void getMinMaxDistance(double& dbMinDistance, double& dbMaxDistance);
	/**设置地形的最小高度值**/
	void setTerrainMinimumDistance(double minDistance);
	/**获取地形的最小高度值*/
	double getTerrainMinimumDistance();

	/*恢复所有三维正常状态*/
	void Reset();

	void setHomePosition(const KERNEL_NAME::GsRawPoint3D& point);
	KERNEL_NAME::GsRawPoint3D& getHomePosition();

	/*范围动画飞行*/
	void RangeFlyTo(double dXMin, double dXMax, double dYMin, double dYMax);

	/*范围直接飞行*/
	void RangeGoTo(double dXMin, double dXMax, double dYMin, double dYMax);

	/*点动画飞行*/
	void PointFlyTo(double dLongitude, double dLatitude, double dHeight);

	/*点直接飞行*/
	void PointGoTo(double dLongitude, double dLatitude, double dHeight);

	/*
	插值飞行
	dLongitude：经度
	dLatitude：纬度
	dHeight：高度
	dHeading：航向角
	dPitch：俯仰角
	dRange：离地面高度
	dTime：飞行时间
	*/
	void InterpolateFlyTo(double dLongitude, double dLatitude, double dHeight,double dHeading ,double dPitch, double dRange,double dTime);

	/*
	获得当前视角位置和姿态
	dLongitude：经度
	dLatitude：纬度
	dHeight：高度
	dHeading：航向角
	dPitch：俯仰角
	dRange：离地面高度
	*/
	void getViewpoint(double& dLongitude, double& dLatitude, double& dHeight, double& dHeading, double& dPitch, double& dRange);

	/*通过范围得到离地面的高度*/
	double RangeToAltitude(double dxMin, double dxMax, double dyMin, double dyMax);

	/*得到全图范围*/
	void ViewExtent(double& dxMin, double& dxMax, double& dyMin, double& dyMax);

	/*通过给一个相机夹角求出半角*/
	double HalfAngle(double dbAngle);

	/*是否开启惯性*/
	void ThrowingEnable(bool bThrowingEnable);

	/*获得惯性状态*/
	bool ThrowingEnable();

	/*经纬度转屏幕坐标*/
	void LonLat2Window(double dbLon, double dbLat, double dbHeight, double& windowX, double& windowY);

	/*屏幕坐标转经纬度*/
	void Window2LonLat(double windowX, double windowY, double &dbLon, double& dbLat, double& dbHeight);

	/*经纬度转世界坐标*/
	void LonLat2World(double dbLon, double dbLat, double dbHeight, double& worldx, double& worldy,double& worldz);

	/*将正确坐标转换到相对倾斜的偏移坐标*/
	void CorrectPosByOblique(KERNEL_NAME::GsRawPoint3D& point);

	/*将相对倾斜的偏移坐标转换到正确坐标*/
	void DeCorrectPosByOblique(KERNEL_NAME::GsRawPoint3D& point);

	/*得到当前相机离地面的高度*/
	double DistanceFromCameraToGround();

	/*得到当前相机相对于正北的旋转量,得到的是角度值*/
	double CameraNorthAngle();

	virtual ~SceneBox();

private:

	double m_dPreHeading;
	double m_dPrePitch;
};

GS_SMARTER_PTR(SceneBox);

GLOBE_ENDNS
