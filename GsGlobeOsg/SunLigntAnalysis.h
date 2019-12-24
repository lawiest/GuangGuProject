#ifndef GEOGLOBE_SUNLIGHTANALYSIS_H
#define GEOGLOBE_SUNLIGHTANALYSIS_H 1

#include <osgViewer/Viewer>
#include <model/LodModel.h>
#include "SchemeComparisonAnalysis/ModelFc.h"
#include <math.h>

using namespace std;

class CSunLightAnalysis
{
public:
	CSunLightAnalysis(void);
	CSunLightAnalysis(osgViewer::Viewer* viewer,const osgEarth::DateTime& qDate);
	~CSunLightAnalysis(void);

	//初始化（太阳位置/选中的模型组/相交线）
	void initSunPositionAndModelGroup(int nTime, osg::Group* modelGroup);
	
	//沿线分析
	int LineAnalysis(/*osgViewer::Viewer* viewer,*/GeoGlobe::Model::LodModel* lodmodel,int* nSampletime,char* filepath);

	//满窗分析
	int CSunLightAnalysis::WindowAnalysis(GeoGlobe::Model::LodModel* lodmodel,int* windowditance,int* nSampletime,char* filepath);

	int CSunLightAnalysis::WindowAnalysisCMF(CModelF* CMFmodel,int* windowditance,int* nSampletime,char* filepath);

	int CSunLightAnalysis::calculate(osgEarth::GeoPoint& CenterLatlon,int* windowditance,int* Sampletime,char* filepath);

	//单点计算
	int complete(osg::Vec3d &vPoint, int nSampletime=5);

	//遮挡分析
	void OcclusionQuery(GeoGlobe::Model::LodModel* lodmodel,int* nSampletime);



	//int complete(osgViewer::Viewer* viewer,osg::Vec3d &vPoint,QDate &qDate, int nSampletime=5);

protected:
    std::vector<osg::Vec3d> m_vSunPosition;
	osg::ref_ptr<osgUtil::LineSegmentIntersector> m_ls;
    osg::ref_ptr<osgUtil::IntersectionVisitor> m_iv;
	//参与计算的模型组
    osg::ref_ptr<osg::Node> m_regionNodeRoot; 

public:
	//GeoGlobe::Model::LodModel* _lodmodel;
	//QDate* _date;
	int aveMinute;//平均日照时间
	std::vector<unsigned long long> OcclusionId;//存遮挡物的ID

private:
	osgViewer::Viewer* m_viewer;
	osgEarth::DateTime m_Date;

};




#endif