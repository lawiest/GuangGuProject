//选中房屋模型
#ifndef _PICKMF_H_    //如果没有定义这个宏
#define _PICKMF_H_     //定义这个宏

#include "CoreOSG.h"
#include "osg/BlendColor"
#include "osg/BlendFunc"
#include "SchemeComparisonAnalysis/ModelFc.h"
//#include "FindVertexVisitor.h"


#include "SchemeComparisonAnalysis/CoEarth.h"

#include "SchemeComparisonAnalysis/rwTxt.h"
#include "SchemeComparisonAnalysis/ObjMtlReadOrWrite.h"
#include "SchemeComparisonAnalysis/CDraggerContainers.h"

//对象选取事件处理器
class PickMFHandler : public osgGA::GUIEventHandler 
{
public:

	PickMFHandler( osg::ref_ptr<CModelF> *m_doModelNode,osg::ref_ptr<CModelF::CModelFWallGeode> *m_doCMFWall,osg::ref_ptr<CModelF::CModelFFloorGeometry> *m_doCMFFloor,COEDraggerContainers* m_OEDraggerContain,bool * m_pickModel,bool *m_pickWallSwitch,bool *m_pickMFWall,bool *m_pickFloorSwitch,bool *m_pickMFFloor,bool * m_translateModel,bool * m_rotateModel,bool *m_scaleModel,bool * m_extrusionModel,bool* m_gridviewCModelFswitch,std::vector<osg::ref_ptr<osg::Image>> m_GeoImage,osgEarth::MapNode* m_MapNode,osg::ref_ptr<osg::Group> m_allcmfroot);

	  ~PickMFHandler()
	  {
		  //
		  _MapModel.clear();
		  _MapState.clear();
		  _GeoImage.clear();
	  }


	  //事件处理函数
	  bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
	  //对象选取事件处理器
	  //原(需要精确找到节点)
	  void pick(osg::ref_ptr<osgViewer::View> view, const osgGA::GUIEventAdapter& ea);

	  void ex(osg::ref_ptr<CModelF> modelnode,ULONGLONG uKey,const osgEarth::SpatialReference* mMapSRS,const osgEarth::SpatialReference* mGeoSRS);

	  //CModelFWallGeode对象选取事件处理器
	  void pickCMFWallGeode(osg::ref_ptr<osgViewer::View> view, const osgGA::GUIEventAdapter& ea);

	  //CModelFFloorGeometry对象选取事件处理器
	  void pickCMFFloorGeom(osg::ref_ptr<osgViewer::View> view, const osgGA::GUIEventAdapter& ea);

	  void CreateLabel(GeoPoint geoPoint,std::string modelhight);

	  void RemoveLabel();

	  //面层选取长按键
	  void setCMFWallModKeyMask(unsigned int mask) { _pickCMFWallModKeyMask = mask; }
	  unsigned int getCMFWallModKeyMask() const { return _pickCMFWallModKeyMask; }

	  void setCMFFloorModKeyMask(unsigned int mask) { _pickCMFFloorModKeyMask = mask; }
	  unsigned int getCMFFloorModKeyMask() const { return _pickCMFFloorModKeyMask; }

	  void setCMFWallKeyEvent(int key) { _pickCMFWallKeyEvent = key; }
	  int getCMFWallKeyEvent() const { return _pickCMFWallKeyEvent; }

	  void setCMFFloorKeyEvent(int key) { _pickCMFFloorKeyEvent = key; }
	  int getCMFFloorKeyEvent() const { return _pickCMFFloorKeyEvent; }

	  //设置地理节点与母节点
	  void setMapAndCMFRoot(osg::ref_ptr<CModelF> m_doModelNode);

	  //设置选中房屋、面、层的颜色
	  void setHightLightCMFColor(osg::Vec4 cmfcolor)
	  {
		  CModelColor=cmfcolor;
	  }
	  void setHightLightGeodeColor(osg::Vec4 wallcolor)
	  {
		  CModelWallColor=wallcolor;
	  }
	  void setHightLightGeomColor(osg::Vec4 floorcolor)
	  {
		  CModelFloorColor=floorcolor;
	  }

	  //获取高亮颜色
	  osg::Vec4 getHightLightCMFColor(osg::Vec4 cmfcolor)
	  {
		  return CModelColor;
	  }
	  osg::Vec4 getHightLightGeodeColor(osg::Vec4 wallcolor)
	  {
		 return CModelWallColor;
	  }
	  osg::Vec4 getHightLightGeomColor(osg::Vec4 floorcolor)
	  {
		  return CModelFloorColor;
	  }

public:
	//面层选取长按键
	int   _pickCMFWallModKeyMask;
	bool  _PermittedByCMFWallModKeyMask;
	int   _pickCMFFloorModKeyMask;
	bool  _PermittedByCMFFloorModKeyMask;
	
	int  _pickCMFWallKeyEvent;
	bool _PermittedpickCMFWallByKeyEvent;

	int  _pickCMFFloorKeyEvent;
	bool _PermittedpickCMFFloorByKeyEvent;
	//高亮颜色
	osg::Vec4 CModelColor;
	osg::Vec4 CModelWallColor;
	osg::Vec4 CModelFloorColor;
	//OBJ纹理修改
	int tempObjTexNum;

	//得到鼠标的位置
	float _mx ;
	float _my;
	//记录模型节点原来的状态
	osg::ref_ptr<osg::StateSet> _stateTemp;
	//记录保存的模型数 （保存上一步操作的数据结构可以参考）
	std::map<ULONGLONG,osg::ref_ptr<CModelF>> _MapModel;
	//记录保存的状态数 （保存上一步操作的数据结构可以参考）
	std::map<ULONGLONG,osg::ref_ptr<osg::StateSet>> _MapState;

	//记录保存的模型数 （保存上一步操作的数据结构可以参考）
	std::map<ULONGLONG,osg::ref_ptr<CModelF::CModelFFloorGeometry>> _MapCMFGeom;
	//记录保存的状态数 （保存上一步操作的数据结构可以参考）
	std::map<ULONGLONG,osg::ref_ptr<osg::StateSet>> _MapCMFGeomState;

	//记录保存的模型数 （保存上一步操作的数据结构可以参考）
	std::map<ULONGLONG,osg::ref_ptr<CModelF::CModelFWallGeode>> _MapCMFGeode;
	//记录保存的状态数 （保存上一步操作的数据结构可以参考）
	std::map<ULONGLONG,osg::ref_ptr<osg::StateSet>> _MapCMFGeodeState;


	//需要处理的模型节点
	osg::ref_ptr<CModelF> *_doModelNode;
	//模型处理变量
	bool *_pickModelSwitch;
	//需要处理的面 几何体
	osg::ref_ptr<CModelF::CModelFWallGeode> *_doCMFWall;
	//需要处理的楼层 几何体
	osg::ref_ptr<CModelF::CModelFFloorGeometry> *_doCMFFloor;

	//面 几何体处理变量
	bool *_pickMFWallSwitch;
	bool *_pickMFWall;
	//楼层 几何体处理变量
	bool *_pickMFFloorSwitch;
	bool *_pickMFFloor;

	//模型标识符
	ULONGLONG _ukeym;
	//面标识符
	ULONGLONG _pukeym;
	//楼层标识符
	ULONGLONG _pukeyf;

	COEDraggerContainers* _pCOEDraggerCintain;
	//变换开关
	bool *_translateswitch;
	bool _shownewtranslateMartrix;
	bool *_rotateswitch;
	bool _shownewrotateMartrix;
	bool *_scaleswitch;
	bool _shownewscaleMartrix;
	bool *_extrusionswitch;
	bool *_gridviewCModelFswitch;

	//删除
	bool _deletemodelswitch;
	//隐藏
	bool _hidemodelswitch;
	std::vector<osg::ref_ptr<CModelF>> _hideModelNodeVector;//记录所有隐藏的模型节点
/*
	//拉伸的长度
	float _mlength;*/
	//拉伸方向
	osg::Vec3d _mdirection;

	//拉伸过程中的模型替换变量
	osg::ref_ptr<CModelF> _replaceCModelF;
	//高度或者距离的Label
	osg::ref_ptr<osgEarth::Annotation::LabelNode> m_ptrLabelNode;
	osgEarth::Symbology::Style m_SymStyle;

	//经纬度转世界坐标
	osgEarth::MapNode* _pMapNode;
	const SpatialReference* _pMapSRS; 
	const SpatialReference* _pGeoSRS;

	//拖拽器
	osg::ref_ptr<CMFManipulator::Dragger> _Dragger; 
	osg::ref_ptr<osg::Group> _pAllCMfroot;

	//纹理包
	std::vector<osg::ref_ptr<osg::Image>> _GeoImage;
	//接收纹理编号
	std::vector<int> _pgetTexNums;

};

#endif

