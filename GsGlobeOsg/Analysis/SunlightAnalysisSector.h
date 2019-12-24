#ifndef SUNLIGHTANALYSISSECTOR_H
#define SUNLIGHTANALYSISSECTOR_H 1

#include <osgEarthUtil/RadialLineOfSight>
#include <osgEarth/MapNode>
#include <osgEarth/MapNodeObserver>
#include <osgEarth/Terrain>
#include <osgEarth/GeoData>
#include <Analysis/PointTrakerElement.h>
#include <Analysis/LineTraker3DElement.h>
#include <osgEarthUtil/LineOfSight>
#include <osgEarthUtil/LinearLineOfSight>
#include <vector>
#include "FormHook.h"


using namespace osgEarth::Util;
 

namespace GeoGlobe
{
	class SunlightAnalysisSector : public osg::Group, public MapNodeObserver
    {
    public:
        SunlightAnalysisSector(MapNode* mapNode, GeoGlobe::IConsultationWindow* consultationWnd = NULL);
        virtual ~SunlightAnalysisSector();

		virtual void setMapNode( MapNode* mapNode );
		virtual MapNode* getMapNode();

		virtual void traverse(osg::NodeVisitor& nv);

		bool IsFinished();

		//获取该点的8小时日照时间
		float GetSunlightTime();

		//分析按钮的响应
		void Analysis();

	protected:
		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
		bool getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat, double &h);

		//初始化
		void InitSector();
		//更新分析点，即可更新全天日照分析
		void UpdateSector(GeoPoint geoPt);

    private:
		osg::ref_ptr< MapNode > _mapNode;
		//线的集合
		vector<osg::ref_ptr< LinearLineOfSightNode >> m_rlos;
		osg::ref_ptr< GeoGlobe::LineTraker3DElement> _startLineTraker;
		int _nStep;
		osg::Vec3d _startWorld;
		osgViewer::View* m_view;

		GeoGlobe::IConsultationWindow* m_consultationWnd;

		bool m_bFinished;
		//分析步长（几分钟分析一次）
		int m_step;
		//分析结果，每天多少小时日照
		float m_time;
		//绘制的线的长度，禹州楼房不高，默认设置120米
		int m_LineLength;
    };

} // namespace GeoGlobe

#endif