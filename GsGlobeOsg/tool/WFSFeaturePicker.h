#pragma once

#include <osgGA/GUIEventHandler>
#include <Feature.h>
#include <osgEarth/XmlUtils>

namespace osgEarth
{
	namespace Features
	{
		class Feature;
	}
}

namespace GeoGlobe 
{
	struct PickerParamter
	{
		std::string strUrl;
		std::string strTypeName;
		std::string strOriginSrs;
		osg::Matrix transMatrix;
		osg::Vec3d vecOrigin;
	};

	class WFSFeaturePicker : public osgGA::GUIEventHandler
	{
	public:
		WFSFeaturePicker(const PickerParamter& pickParamter);
		virtual ~WFSFeaturePicker();

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

		void Build(osg::Vec3Array* ptrVertex);
		void BuildFeature(osgEarth::XmlElement * ptrFeature);
		void BuildHouse(osg::Vec3Array* ptrVertex);
		void CreateWall(osg::Vec3Array* ptrVec, osg::Vec3Array* ptrVecH, osg::Vec3d vecOrigin);
		void CreatePolygon(osg::Vec3Array* ptrVertices, osg::Vec3d vecOrigin);
		void ClearPreHighLight();
		void splitCoordinates(const std::string& src, osg::Vec3Array* ptrVertex ,GLOBE_NAME::Feature& geoFeature);
		std::string Request(osg::Vec3 point);
		void ParseXML(std::string strData);
		//如果点拾取到两个矢量取最优
		bool getBestFeature(osgEarth::XmlNodeList featureList, osgEarth::XmlElement& featue);
		//处理相关联节点
		void DealWithRelative(int nId);

		UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Feature&)> OnPicked;

	protected:
		double dbElevation;
		double dbHigh;
		//房顶高
		double m_dbFDG;
		//楼高
		double m_dbLG;
		//鼠标拾取到的高
		double m_dbPickHeight;

		bool m_isDrag;

		int nID;
		osg::ref_ptr<osg::Group> ptrBuilding;

		PickerParamter m_pickParamter;
	};
}
