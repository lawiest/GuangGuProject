#pragma once

#include <osgGA/GUIEventHandler>
#include <osgEarth/MapNode>
#include <osgViewer/View>
#include <osg/Vec3d>
#include <placename/PlaceNode.h>
#include <event.h>

class Distance3DTrakerElement : public osgGA::GUIEventHandler
{
public:
	Distance3DTrakerElement();//, GeoGlobe::IConsultationWindow* consultationWnd);
	virtual ~Distance3DTrakerElement();

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	bool pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osg::Vec3d& world);
	
	//设置label
	GeoGlobe::Annotation::PlaceNode* CreateLabel(osgEarth::MapNode* m_mapNode, osgEarth::GeoPoint mapPoint, std::string na);

	//添加线
	osg::Node* createLine(osg::Vec3d lbegin, osg::Vec3d lend,osg::Vec3d offset);
	
	double getDistance3D();
	std::vector<osg::Vec3d> getPickPoints();

	osg::Node* DisplayGroup();

	UTILITY_NAME::GsDelegate<void(double)> OnFinish;

private:
	osg::ref_ptr<osgEarth::MapNode> m_mapNode;
	bool m_bIsFirst;
	bool m_bIsFinished;

	//拾取的点及距离
	std::vector<osg::Vec3d> m_PickPoints;
	double m_Distance3D;

	bool m_isDrag;

	//鼠标拾取的点
	osg::Vec3d world;

	//橡皮筋
	osg::ref_ptr<osg::Node> RubberBand;

	//距离的Label
	osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> m_ptrLabelNode;
	osgEarth::Symbology::Style m_SymStyle;

	//旋转矩阵
	osg::ref_ptr<osg::MatrixTransform> mt; 

	osg::ref_ptr<osg::Group> displayGroup;
};
