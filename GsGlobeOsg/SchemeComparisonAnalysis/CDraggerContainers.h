#ifndef _CDRAGGERCONTAINERS_H_    //如果没有定义这个宏
#define _CDRAGGERCONTAINERS_H_     //定义这个宏

#include "SchemeComparisonAnalysis/CoreOSG.h"

#include "SchemeComparisonAnalysis/CoEarth.h"

#include "SchemeComparisonAnalysis/ModelFc.h"

#include "SchemeComparisonAnalysis/translateAxisCMDragger.h"
#include "SchemeComparisonAnalysis/TrackballCMDragger.h"
#include "SchemeComparisonAnalysis/scaleAxisCMDDragger.h"
#include "SchemeComparisonAnalysis/ExtScaleAxisCMDragger.h"

//#include "SchemeComparisonAnalysis/FindVertexVisitor.h"

#include "model/Model.h"
#include "model/LodModel.h"
#include "model/ModelTile.h"


using namespace CMFManipulator;
using namespace std;


class COEDraggerContainers
{
public:
	COEDraggerContainers(osgEarth::MapNode* m_MapNode,osg::ref_ptr<osg::Group> m_root);

	~COEDraggerContainers()
	{

	}


	void addDraggerToScene(osg::ref_ptr<osg::MatrixTransform> m_sceneMatTransf,DragType m_dragname/*, bool fixedSizeInScreen*/);

	void DisableDragger(osg::ref_ptr<osg::MatrixTransform> m_draggerToMatTransf) ;

	osg::Matrix getPreDraggerMatrix(){  return *_predragTmpMatrix;  }

	void ResetDraggerContainers(osg::Matrix mTmpMatrix);

	osg::ref_ptr<CMFManipulator::Dragger> getDragger() { return _Dragger.get(); }
	const osg::ref_ptr<CMFManipulator::Dragger> getDragger() const { return _Dragger.get(); }

private:

	void computeDragdir(osg::ref_ptr<osg::MatrixTransform> m_sceneMatTransf,osg::ref_ptr<osg::Vec3dArray> xyzcModelWorldVertices);

public:
	//经纬度转世界坐标
	osgEarth::MapNode* _mMapNode;
	const osgEarth::SpatialReference* _mMapSRS;
	const osgEarth::SpatialReference* _mGeoSRS;

	//拖拽器
	osg::ref_ptr<CMFManipulator::Dragger> _Dragger;
	osg::ref_ptr<osg::Group> _root;

	//拉伸
	osg::Vec3d _direction;
	float *_mHlength;//高度
	osg::ref_ptr<osg::Vec3dArray> _newLlVertices;//包含底面和顶面的点，点数是原来顶点的两倍
	osg::ref_ptr<osg::Vec3dArray> _prenewLlVertices;

	osg::Matrix* _predragTmpMatrix;
	int _predragTempTimes;

	vector<osg::Matrix>* _dragCMVectMatrix;
	int * _dragCMTimes;

	vector<osg::Matrix>* _predragCMVectMatrix;
	int * _predragCMTimes;

	osg::ref_ptr<osg::MatrixTransform> _preTransfModel;

protected:


private:

};


#endif
