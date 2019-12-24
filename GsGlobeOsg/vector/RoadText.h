#pragma once
#include <string>
#include <osg/Vec3d>
#include <osg/Vec3>
#include <osg/MatrixTransform>
#include <SceneBox.h>
#include <osgEarth/SpatialReference>
#include <osgEarth/MapNode>
#include <osgEarth/GeoTransform>

namespace osgText
{
	class Text;
}

namespace GeoGlobe 
{
	enum PoseState
	{
		positive,
		opposite
	};

	enum DistState
	{
		Zero,
		One,
		Tow
	};

    class RoadText
    {
    public:

		//RoadText(osg::Vec3d p0, osg::Vec3d p1, std::string strRoad, int nIndex, double dbSize, osg::Vec4 color, std::string strFont);
		RoadText(double& fLineLength, osg::Vec3d p0, osg::Vec3d p1, const std::vector<osg::ref_ptr<osg::Geode>> &vTextGeode);
			
		void OnChange();

        virtual ~RoadText(){ }
		osg::Node* LocatorAndRotate(osg::Vec3d vPositionText, double angle, osg::Geode* textGeode, bool bCheck = false);

    private:
		osg::Vec3d m_vecStart;
		osg::Vec3d m_vecEnd;
		float m_Angle;

		GLOBE_NAME::SceneBox m_scBox;
		PoseState m_PoseState;
		DistState m_DistState;

		osg::Vec3d m_vecPosition;
		float m_dbCharacterSize;
		float m_dbCurrentCharacterSize;
		osg::ref_ptr<osgText::Text> m_ptrText;
		osg::ref_ptr<osg::MatrixTransform> m_Trans;

	public:
		bool Convert(const osgEarth::GeoPoint& pIn, osgEarth::GeoPoint& pOut, double dHeight = 0.0);
		bool Convert(const osgEarth::GeoPoint& pStart, const osgEarth::GeoPoint& pEnd, std::vector<osgEarth::GeoPoint> &vOut, double dHeight = 0.0);
	protected:
		bool PickSurface(osg::Vec3d start, osg::Vec3d end, osg::Vec3d& result);
		bool PickSurface(const osgEarth::GeoPoint& pStart, const osgEarth::GeoPoint& pEnd, osgEarth::GeoPoint& pResult);
		std::vector<osgEarth::GeoTransform*> m_vPtrLocators;
		bool m_bCheck;

    };
}
