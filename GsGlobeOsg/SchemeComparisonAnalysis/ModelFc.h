#ifndef _MODELFC_H_    //如果没有定义这个宏
#define _MODELFC_H_     //定义这个宏

#include "SchemeComparisonAnalysis/CoreOSG.h"

#include "SchemeComparisonAnalysis/CoEarth.h"

#include "SchemeComparisonAnalysis/translateAxisCMDragger.h"
#include "SchemeComparisonAnalysis/TrackballCMDragger.h"
#include "SchemeComparisonAnalysis/scaleAxisCMDDragger.h"
#include "SchemeComparisonAnalysis/ExtScaleAxisCMDragger.h"

#include "SchemeComparisonAnalysis/DrawLine.h"
#include "SchemeComparisonAnalysis/CMFText.h"
//#include "SchemeComparisonAnalysis/FindVertexVisitor.h"


using namespace CMFManipulator;




//房屋类
class CModelF :public osg::MatrixTransform
{
public:
	CModelF(ULONGLONG *m_ukey,osg::ref_ptr<osg::Vec3dArray> m_modelLLVertices,osgEarth::MapNode* m_MapNode,osg::ref_ptr<osg::Group> m_allCMFroot,std::vector<int> m_getTexNums,std::vector<osg::ref_ptr<osg::Image>> m_GeoImage,float *m_floorHight=new float,int *m_floorNums=new int,osg::Vec3d m_direction=osg::Vec3d(0.0f, 0.0f, 1.0f),osg::ref_ptr<osg::Vec3dArray> m_newmodelLLVertices=new osg::Vec3dArray);

	CModelF(ULONGLONG *m_ukey,osgEarth::MapNode* m_MapNode);

~CModelF()
	{

	}

//楼层类
class CModelFFloorGeometry:public osg::Geometry
{
public:
	CModelFFloorGeometry(ULONGLONG m_mukeyFloorGeom=-1,ULONGLONG m_mukeyWallGeom=-1,ULONGLONG m_mukeyCMFGeom=-1,int m_faceTexNum=0);
	~CModelFFloorGeometry()
	{

	}

	void setCModelFloorTexture(osg::ref_ptr<osg::Texture2D> mtexture);
	void setCModelFloorColor(osg::ref_ptr<osg::Vec4dArray> mcolor);
public:
	//楼层标识符
	ULONGLONG _mukeyFloorGeom;
	//楼层所在面
	ULONGLONG _mukeyWallGeom;
	//楼层所在房屋
	ULONGLONG _mukeyCMFGeom;

	//纹理贴图
	int _faceTexNumGeom;//面纹理号
};

//面
class CModelFWallGeode: public osg::Geode
{
public:
	CModelFWallGeode(ULONGLONG m_mukeyWallGeode=-1,ULONGLONG m_mukeyCMFGeom=-1,int m_WallTexNumGeode=0) ;
	~CModelFWallGeode()
	{

	}

	void setCModelFaceTexture(osg::ref_ptr<osg::Texture2D> mtexture);
	void setCModelFaceColor(osg::ref_ptr<osg::Vec4dArray> mcolor);
public:
	//面标识符
	ULONGLONG _mukeyWallGeode;
	//面所在房屋
	ULONGLONG _mukeyCMFGeode;

	//纹理贴图
	int _faceTexNumGeode;//面纹理号
};

//创建二维纹理状态对象
osg::ref_ptr<osg::StateSet> createTexture2DState(osg::ref_ptr<osg::Image> image);

void x_min_max(osg::ref_ptr<osg::Vec3dArray> m_modelLLVertices,double* x_min,double* x_max);

void y_min_max(osg::ref_ptr<osg::Vec3dArray> m_modelLLVertices,double* y_min,double* y_max);

/****修改纹理、颜色****/
#if 1 
//纹理
void setCModelWallTexture(osg::ref_ptr<osg::Texture2D> mtexture);
//颜色
void setCModelWallColor(osg::ref_ptr<osg::Vec4dArray> mcolor);

#endif

//****楼层****//
#if 1

//绘制一层
osg::ref_ptr<osg::Geometry> CreateFloorQuad(osg::ref_ptr<osg::Vec3Array> m_Vertices,ULONGLONG m_geodeNum,int m_TexNum);

//绘制一个面
osg::ref_ptr<osg::Geode> CreateFloorSideQuad(osg::ref_ptr<osg::Vec3Array> m_Vertices);

//新经纬度坐标，每层楼的顶点
void saveFloorNewLlVertices(osg::Vec3d direction=osg::Vec3d(0.0f, 0.0f, 1.0f));

//边界内插点
void InsertLinePoint(osg::Vec3d direction=osg::Vec3d(0.0f, 0.0f, 1.0f));

//内插值确定
void saveInsertLlVertices(osg::ref_ptr<osg::Vec3dArray> m_tInsetLineLlVertices,osg::Vec3d direction=osg::Vec3d(0.0f, 0.0f, 1.0f));

int get2PointInserttime(osg::Vec3d p1,osg::Vec3d p2);

void setInsertValue(double insertValue){_InsertValue=insertValue;}

void DrawCMFLines();

void DrawCMFText(const char* mstring);

void RemoveLinesAText();
//创建模型,绘制一个房屋
void CreateFloorextrusion();

#endif
//****！****//

//创建预显示模型
osg::ref_ptr<osg::Geode> CreateTransparentModel(osg::ref_ptr<osg::Vec3dArray> newllVertices);

//拖拽器
void addExtrusionDragger(DragType chooseDragType);

void DisableDragger();

void computeDragdir(osg::ref_ptr<osg::MatrixTransform> m_sceneMatTransf,osg::ref_ptr<osg::Vec3dArray> xyzcModelWorldVertices,osg::ref_ptr<osg::Vec3dArray> xyzcModelLlVertices);

void setFloorHeight();

float getFloorHeight();

void setFloorNums();

int getFloorNums();

public:
	//坐标
	double *_Xmax;
	double *_Xmin;
	double *_Ymax;
	double *_Ymin;

	osg::ref_ptr<osg::Vec3dArray> _modelLLVertices;
	osg::ref_ptr<osg::Vec3dArray> _premodelLLVertices;//模型原点保存
	//房屋标志符
	ULONGLONG *_mukey;
	//每个房屋的每个面标志符
	ULONGLONG _geokeym;
	//每个面的楼层标志符
	ULONGLONG _geokeyf;

	//经纬度转世界坐标
	osgEarth::MapNode* _mMapNode;
	const osgEarth::SpatialReference* _mMapSRS;
	const osgEarth::SpatialReference* _mGeoSRS;

	//拖拽器
	osg::ref_ptr<CMFManipulator::Dragger> _Dragger;
	osg::ref_ptr<osg::Group> _mAllCMfroot;

	//拉伸
	osg::Vec3d _direction;
	float _mHlength;//高度
	osg::ref_ptr<osg::Vec3dArray> _newLlVertices;//包含底面、顶面中间楼层的点，点数是原来顶点的楼层数+1倍
	osg::ref_ptr<osg::Vec3dArray> _prenewLlVertices;

	//格网
	double _InsertValue;//
	osg::ref_ptr<osg::Vec3dArray> _InsetLineLlVertices;//包含底面、顶面中间楼层的点，点数是原来顶点的楼层数+1倍
	osg::ref_ptr<osg::Vec3dArray> _preInsetLineLlVertices;
	int*_InsetLineTime;//线上内插次数
	osg::ref_ptr<osg::Vec3dArray> _InsetLineDirect;//线内插方向
	osg::ref_ptr<CDrawLine> _CMFLines;//网格线
	//字
	osg::ref_ptr<CCMFText> _CMFText;
	osg::ref_ptr<osg::Group> _LineATextroot;

	//纹理贴图
	//纹理包
	std::vector<osg::ref_ptr<osg::Image>> _GeoImage;
	//接收纹理编号
	std::vector<int> _getTexNums;

	//楼层
	float _mfloorHeight;//每层高度
	int _mfloorNums;//楼层数


};

#endif
