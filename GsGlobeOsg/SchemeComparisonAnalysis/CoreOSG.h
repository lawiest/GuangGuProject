//OSG核心库文件
#ifndef _COREOSG_H_    //如果没有定义这个宏
#define _COREOSG_H_     //定义这个宏
#if 1

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>//事件操作
#include <osgViewer/api/win32/GraphicsWindowWin32>

#include <osg/Geode>
#include <osg/Group>
#include <osg/Node>//结点
#include <osg/Notify>
#include <osg/Switch>//结点开关
#include <osg/MatrixTransform>//移动/旋转/缩放 矩阵
#include <osg/Geometry>//绘制几何图形
#include <osg/LineWidth>//控制线宽
#include <osg/ShapeDrawable> //绘制内置几何体
#include <osg/Shape>//内置集合类型
#include <osg/CoordinateSystemNode> 
#include <osg/Switch> 
#include <osg/Billboard>//广告牌
#include <osg/Texture2D>//纹理
#include <osg/TexGen>
#include <osg/Texture1D>
#include <osg/TexEnv>
#include <osg/TextureRectangle>
#include <osg/TexMat>
#include <osg/Image>//加载图片
#include <osg/PositionAttitudeTransform>//位置转换
#include <osg/AutoTransform>//位置自动对齐
#include <osg/LOD>//细节层次
#include <osg/CoordinateSystemNode>//坐标结点
#include <osg/AnimationPath>//动态路径设置
#include <osg/ClipNode>//裁剪
#include <osg/PolygonMode>//多边形模式设置
#include <osg/Camera>
#include <osg/NodeVisitor>//结点访问
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/ShapeDrawable>
#include <osg/Material>//材质
#include <osg/NodeCallback>
#include <osg/Depth>
#include <osg/CullFace>
#include <osg/TextureCubeMap>
#include <osg/Light>//光对象 
#include <osg/LightSource>//光源
#include <osg/BoundingSphere>
//#include <osg/BoundingBox>
#include <osg/TexGenNode>
#include <osg/Fog>
#include <osg/Math>
#include <osg/io_utils>
#include <osg/ComputeBoundsVisitor>
#include <osg/NodeVisitor>

#include <osgParticle/PrecipitationEffect>//粒子系统

#include <osgFX/Cartoon>//卡通渲染
#include <osgFX/BumpMapping>//凹凸贴图
#include <osgFX/AnisotropicLighting>//异性光照
#include <osgFX/Scribe>//物体模型加边

#include <osgSim/OverlayNode>
#include <osgSim/DOFTransform>

#include <osgDB/ReadFile>//读取
#include <osgDB/WriteFile>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/FileUtils>

#include <osgUtil/Tessellator>//分格化
#include <osgUtil/LineSegmentIntersector>//
#include <osgUtil/Optimizer> //优化窗口头文件
#include <osgUtil/SmoothingVisitor> 

#include <osgText/Text> //文字

#include <osgGA/StateSetManipulator>//状态
#include <osgGA/TrackballManipulator> 
#include <osgGA/FlightManipulator> 
#include <osgGA/DriveManipulator> 
#include <osgGA/KeySwitchMatrixManipulator> 
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator> 
#include <osgGA/GUIEventHandler>

#include <osgAnimation/AnimationManagerBase>
#include <osgAnimation/Bone>
#include <osgAnimation/BasicAnimationManager>

#include <osgWidget/ViewerEventHandlers>


#include <osgShadow/ShadowedScene>  
//#include <osgShadow/ShadowVolume>  
#include <osgShadow/ShadowTexture>  
#include <osgShadow/ShadowMap>  
#include <osgShadow/SoftShadowMap>  
#include <osgShadow/ParallelSplitShadowMap>  
#include <osgShadow/MinimalShadowMap>  
#include <osgShadow/StandardShadowMap>  
#include <osgShadow/SoftShadowMap>  
#include <osgShadow/ViewDependentShadowTechnique>  

//操作器
/*
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TabBoxTrackballDragger>
#include <osgManipulator/TabPlaneDragger>
#include <osgManipulator/TabPlaneTrackballDragger>
#include <osgManipulator/Scale1DDragger>
#include <osgManipulator/Scale2DDragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/Translate1DDragger>
#include <osgManipulator/Translate2DDragger>
#include <osgManipulator/TranslateAxisDragger>
#include <osgManipulator/TranslatePlaneDragger>
#include <osgManipulator/RotateCylinderDragger>
#include <osgManipulator/AntiSquish>*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#endif

using namespace std;

//定义加载模型变量
#if 0
osg::ref_ptr<osg::Node>cow=osgDB::readNodeFile("cow.osg");
osg::ref_ptr<osg::Node>glider=osgDB::readNodeFile("glider.osg");
osg::ref_ptr<osg::Node>Cessna=osgDB::readNodeFile("Cessna.osg");
osg::ref_ptr<osg::Node> truck = osgDB::readNodeFile("dumptruck.osg");
osg::ref_ptr<osg::Node> spaceship = osgDB::readNodeFile("spaceship.osg");
osg::ref_ptr<osg::Node> lz = osgDB::readNodeFile("lz.osg");
osg::ref_ptr<osg::Node> logo = osgDB::readNodeFile("logo.ive");
osg::ref_ptr<osg::Node> axe=osgDB::readNodeFile("axes.osgt") ;
osg::ref_ptr<osg::Node> brdm=osgDB::readNodeFile("brdm.ive");

#endif

enum DragType
{
	TranslateAxis,
	Rotateball,
	ExtScaleAxis,
	ScaleAxis,
	ScalePlane
};

enum BackType
{
	ManipulatorCModel,
	HideCModel,
	DeleteCModel,
	ExtrusionCModel,
	AlterCModel,
	ShowAllHideCModel,
	ChangeGeomTexCModel,
	ChangeGeodeTexCModel,
	ScaleRatio,
	NONECModel
};

enum BackJAType
{
	ManipulatorJAModel,
	HideJAModel,
	DeleteJAModel,
	ShowAllHideJAModel,
	ScaleRatioJA,
	NONEJAModel
};

//吉奥几何对象类型设定的枚举量
enum geoGEOMETRYTYPE1
{
	GEO_GEOMETRY_UNKNOWN = 0,
	GEO_GEOMETRY_POINT = 1,
	GEO_GEOMETRY_PATH = 2,
	GEO_GEOMETRY_POLYGON = 3,
	GEO_GEOMETRY_COLLECTION = 4,
	GEO_GEOMETRY_MULTIPOINT = 5,
	GEO_GEOMETRY_POLYLINE = 6,
	GEO_GEOMETRY_ANNOTATION = 7,
	GEO_GEOMETRY_ENVELOPE = 8,
	GEO_GEOMETRY_LINE = 9,
	GEO_GEOMETRY_CIRCLEARC = 10,
	GEO_GEOMETRY_RING = 11
};

struct fcscmf_blob_head//blob?????
{
	long		oid;
	long		oid2;
	geoGEOMETRYTYPE1	type;
	long		coordnum;
	long		dim;
	long		infonum;
} ;

#endif

