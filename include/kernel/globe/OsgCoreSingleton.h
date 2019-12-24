#pragma once

#include <utility.h>
#include <osg/Vec3d>

namespace osg
{
	class Vec3d;
}
namespace osg
{
	class Group;
}

namespace osgEarth
{
	class MapNode;
}

namespace osgViewer
{
	class Viewer;
}

GLOBE_NS

class LonAndLatElement;
class EarthManipulator;
class ElementBox;
class LayerBox;

class GS_API OsgCoreSingleton : public GeoStar::Utility::GsRefObject
{
public:

	static OsgCoreSingleton* Instance();

	/*拾取BillBoard等屏幕元素需要使用osgearth自带的RTT技术拾取必须把BillBoard加到MapNode子群组中，
	注意是子群组，不能直接作为孩子加到MapNode下面，目前原因不明*/
	osg::Group* RTTPickGroup();
	osg::Group* Root();
	//osg::Group* Vector();

	osgEarth::MapNode* MapNode();
	LonAndLatElement* LonAndLat();

	//将正确的经纬度坐标算到相对于倾斜偏移坐标
	void CorrectPosByOblique(osg::Vec3d& vecPos);

	//将相对于倾斜偏移坐标算到正确的经纬度坐标
	void DeCorrectPosByOblique(osg::Vec3d& vecPos);

	void setSrsInfor(const std::string& strSrs, const osg::Vec3d& vecOrigin);
	void getSrsInfor(std::string& strSrs, osg::Vec3d& vecOrigin);

	ElementBox* getElementBox();
	LayerBox * getLayerBox();
	osgViewer::Viewer* OsgViewer();
	EarthManipulator* GGEarthManipulator();

	void RegisteMapNode(osgEarth::MapNode* ptrMapNode);
	void RegisteLonAndLatElement(LonAndLatElement* ptrLonAndLat);

	void RegisteElement(ElementBox * ptrElementBox);
	void RegisteLayerBox(LayerBox * ptrLayerBox);

protected:
	OsgCoreSingleton();
	virtual ~OsgCoreSingleton();

	std::string m_strSrs;
	osg::Vec3d m_vecOrigin;

	osg::Group* m_ptrRTTPickGroup;
	osg::Group* m_ptrRoot;
	//osg::Group* m_Vector;
	osgEarth::MapNode* m_ptrMapNode;
	osgViewer::Viewer* m_ptrViewer;
	EarthManipulator* m_ptrManip;
	LonAndLatElement* m_ptrLonAndLat;
	ElementBox * m_ptrElementBox;
	LayerBox * m_ptrLayerBox;
	//std::vector<osg::ref_ptr<osgEarth::Features::Feature> > m_vecFeatures;
};

GS_SMARTER_PTR(OsgCoreSingleton);

GLOBE_ENDNS


