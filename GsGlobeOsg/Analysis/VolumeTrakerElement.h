#pragma once
#include <osgEarthAnnotation/FeatureNode> 
#include "osgGA/GUIEventHandler"
#include "osgEarth/MapNode"
#include <osgViewer/View>
#include <event.h>
#include "placename/GeoBillboard.h"

struct OsgbCalcParameter
{
	OsgbCalcParameter()
	{
		m_ptrOsgbNode = NULL;
		m_ptrGeo = NULL;
		m_dbElevation = 0.0;
	}

	osg::Node* m_ptrOsgbNode;
	osg::Geometry* m_ptrGeo;
	double m_dbElevation;
};

class OsgbCalculateThread : public OpenThreads::Thread
{
public:

	OsgbCalculateThread();

	virtual ~OsgbCalculateThread();

	virtual void run();

	void setParameter(const OsgbCalcParameter& parameter);

public:
	UTILITY_NAME::GsDelegate<void (double, double, double, double)> onDone;

private:
	OsgbCalcParameter m_parameter;
};


class VolumeTrakerElement : public osgGA::GUIEventHandler
{
public:
	VolumeTrakerElement(osg::Node* ptrNode, bool bAutoCalc = true, double dbElevation = 0.0);
	virtual ~VolumeTrakerElement();

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	bool pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osg::Vec3d& world);

	osg::Node* createLine(osg::Vec3d lbegin, osg::Vec3d lend, osg::Vec3d offset);

	double getArea3D();
	std::vector<osg::Vec3d> getPickPoints();

	void Calculate(double dbElevation);

	void OnCalcDone(double aveHeightLine, double aveHeightFace, double cut, double fill);

	osg::Node* DisplayGroup();

	//绘制完成
	UTILITY_NAME::GsDelegate<void()> OnDrawFinish;
	//计算完成
	UTILITY_NAME::GsDelegate<void(double)> OnCalcFinish;

private:
	osgEarth::MapNode* m_mapNode;
	bool m_bIsFirst;
	bool m_bIsFinished;

	//拾取的点及距离
	std::vector<osg::Vec3d> m_PickPoints;
	double m_Area3D;
	osg::Vec3d world;//鼠标拾取的点

	bool m_isDrag;

	osgEarth::Symbology::Style m_SymStyle;

	//橡皮筋
	osg::ref_ptr<osg::Node> m_ptrRubberBand;
	osg::ref_ptr<osg::Node> m_closeRubberBand;

	osg::ref_ptr<osg::Group> m_ptrDisplayGroup;

	osg::ref_ptr<osg::Node> m_osgbLayer;
	osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> m_ptrBillboard;
	OsgbCalculateThread* m_ptrCalcThread;
	double m_dbElevation;
	bool m_bIsAutoCalc;
};


