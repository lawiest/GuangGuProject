#pragma once
#include <osgEarthAnnotation/FeatureNode> 

#include "osgGA/GUIEventHandler"
#include "osgEarth/MapNode"
#include "osgEarthAnnotation/PlaceNode"
#include "FormHook.h"

class Area3DTrakerElement : public osgGA::GUIEventHandler
{
public:
	Area3DTrakerElement(osgEarth::MapNode* mapNode);
	virtual ~Area3DTrakerElement();

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void AreaMeasurement(osg::Vec3d pickP1, osg::Vec3d pickP2, osg::Vec3d pickP3, double& area);
	bool AreaMeasurement(std::vector<osg::Vec3d> pickPoints, double& area);

	bool pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osg::Vec3d& world);
	
	osgEarth::Annotation::LabelNode* CreateLabel(osgEarth::MapNode* m_mapNode, osgEarth::GeoPoint mapPoint, std::string na);

	osg::Node* createLine(osg::Vec3d lbegin, osg::Vec3d lend, osg::Vec3d offset);
	
	double getArea3D();
	vector<osg::Vec3d> getPickPoints();

	osg::Node* DisplayGroup();


	UTILITY_NAME::GsDelegate<void(double)> OnFinish;

	//void GrahamConvexHull(const vector<osg::Vec2d> m_Points);

private:
	osgEarth::MapNode* m_mapNode;
	osg::ref_ptr<osgEarth::Annotation::PlaceNode> m_PlaceNode;
	bool m_bIsFirst;
	//GeoGlobe::IConsultationWindow* m_consultationWnd;
	bool m_bIsFinished;
	bool m_isDrag;

	//拾取的点及距离
	vector<osg::Vec3d> m_PickPoints;
	double m_Area3D;
	osg::Vec3d world;//鼠标拾取的点

	//距离的Label
	osg::ref_ptr<osgEarth::Annotation::LabelNode> m_ptrLabelNode;
	osgEarth::Symbology::Style m_SymStyle;

	//橡皮筋
	osg::ref_ptr<osg::Node> RubberBand;
	osg::ref_ptr<osg::Node> m_closeRubberBand;

	//旋转矩阵
	osg::ref_ptr<osg::MatrixTransform> mt;

	osg::ref_ptr<osg::Group> displayGroup;


};


