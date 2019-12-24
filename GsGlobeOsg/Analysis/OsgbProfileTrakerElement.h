#pragma once
#include <osgEarthAnnotation/FeatureNode> 
#include "osgGA/GUIEventHandler"
#include "osgEarth/MapNode"
#include <osgViewer/View>
#include <event.h>
#include "placename/GeoBillboard.h"


class OsgbProfileTrakerElement : public osgGA::GUIEventHandler
{
public:
	OsgbProfileTrakerElement(osg::Node* ptrNode, bool bAutoCalc = true, double dbStep = 100.0);
	virtual ~OsgbProfileTrakerElement();

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	bool pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osg::Vec3d& world);

	osg::Node* createLine(osg::Vec3d lbegin, osg::Vec3d lend, osg::Vec3d offset);

	std::vector<osg::Vec3d> getPickPoints();

	void Calculate(double dbStep);
	osg::Vec3d PickOsgb(osg::Vec3d point);

	osg::Node* DisplayGroup();

	//绘制完成
	UTILITY_NAME::GsDelegate<void()> OnDrawFinish;
	//计算完成
	UTILITY_NAME::GsDelegate<void(const std::vector<double>&, const std::vector<double>&)> OnCalcFinish;

private:
	osgEarth::MapNode* m_mapNode;
	bool m_bIsFirst;
	bool m_bIsFinished;


	std::vector<double> m_vecDistance;
	std::vector<double> m_vecElevation;

	//拾取的点及距离
	std::vector<osg::Vec3d> m_PickPoints;
	osg::Vec3d m_world;//鼠标拾取的点

	bool m_isDrag;

	osgEarth::Symbology::Style m_SymStyle;

	//橡皮筋
	osg::ref_ptr<osg::Node> m_ptrRubberBand;
	osg::ref_ptr<osg::Node> m_closeRubberBand;

	osg::ref_ptr<osg::Group> m_ptrDisplayGroup;

	osg::ref_ptr<osg::Node> m_osgbLayer;
	osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> m_ptrBillboard;

	bool m_bIsAutoCalc;
	double m_dbStep;
};


