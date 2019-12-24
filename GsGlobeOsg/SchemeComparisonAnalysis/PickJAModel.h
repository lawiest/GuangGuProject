#ifndef _PICKJAMODEL_H_    //如果没有定义这个宏
#define _PICKJAMODEL_H_     //定义这个宏

#include "SchemeComparisonAnalysis/CoreOSG.h"
#include "model/Model.h"
#include "model/ModelTile.h"
#include "osg/BlendColor"
#include "osg/BlendFunc"

//对象选取事件处理器
class PickJAHandler : public osgGA::GUIEventHandler 
{
public:

	PickJAHandler(osg::ref_ptr<GeoGlobe::Model::LodModel>* m_doJALodModelNode, osg::ref_ptr<GeoGlobe::Model::Model>* m_doJAModelNode,COEDraggerContainers* m_OEDraggerContain,bool * m_pickJAModelSwitch,bool * m_translateJAModelSwitch,bool * m_rotateJAModelSwitch,bool *m_scaleJAModelSwitch):
	  _mx(0.0f),
		  _my(0.0f)
	  {
		  //
		  _stateTemp=new osg::StateSet();

		  _pdoJALodModelNode=m_doJALodModelNode;
		  _pdoJAModelNode=m_doJAModelNode;//指针地址相同

		  _pCOEDraggerCintain=m_OEDraggerContain;

		  _ppickJAModelSwitch=m_pickJAModelSwitch;
		  _ptranslateSwitch=m_translateJAModelSwitch;
		  _pscaleSwitch=m_scaleJAModelSwitch;
		  _protateSwitch=m_rotateJAModelSwitch;
	  }
	  
	  ~PickJAHandler()
	  {
		  //
	  }
	  //事件处理函数
	  bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
	  {
		  osg::ref_ptr<osgViewer::View> view = dynamic_cast<osgViewer::View*>(&aa);
		  if (!view) return false;

		  switch(ea.getEventType())
		  {
			  //鼠标双击
		  case(osgGA::GUIEventAdapter::DOUBLECLICK):
			  {
				  if(ea.getButton()==osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)//0，1，2 右左中
				  {
					  //更新鼠标位置
					  _mx = ea.getX();
					  _my = ea.getY();
					  pick(view.get(), ea);

				  }
				  break;
			  }
		  case(osgGA::GUIEventAdapter::RELEASE):
			  {
				  if (_mx==ea.getX() && _my==ea.getY())
				  {
					  
				  }
				  break;
			  }
		  case(osgGA::GUIEventAdapter::KEYDOWN):
			  {
				  if (ea.getKey()=='M'||ea.getKey()=='m')
				  {

				  }
				  break;
			  }
		  default:
			  break;
		  }
		  return false;
	  }

	  void pick(osg::ref_ptr<osgViewer::View> view, const osgGA::GUIEventAdapter& ea)
	  {
		  osgUtil::LineSegmentIntersector::Intersections intersections;
		  if (!view->computeIntersections(ea,intersections))
			  return;

		  //对保存的模型进行设置//移出原来选中的对象
		  for(std::map<UINT64,osg::ref_ptr<GeoGlobe::Model::LodModel>>::iterator itLodmodel =_pmapJALodModel.begin();itLodmodel != _pmapJALodModel.end();++itLodmodel)
		  {
			  //所有渲染状态清空
				  itLodmodel->second->setStateSet(new osg::StateSet);
		  }
		  for(std::map<ULONGLONG,osg::ref_ptr<GeoGlobe::Model::Model>>::iterator itmodel =_pmapJAModel.begin();itmodel != _pmapJAModel.end();++itmodel)
		  {
			  //所有渲染状态清空	
			  _pCOEDraggerCintain->DisableDragger(itmodel->second);
		  }

		  //清空
		  _pmapJALodModel.clear();
		  _pmapJAModel.clear();
		  _pmapJAState.clear();

		  for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
			  hitr != intersections.end();
			  ++hitr)
		  {
			  // the geodes are identified by name.
			  osg::NodePath nodePath = hitr->nodePath;
			  for (int i=0; i<nodePath.size();i++)
			  {
				  osg::ref_ptr<osg::Node> node = nodePath[i];
				  if (node == NULL)
					  continue;

				  //沿着路径找到模型节点为止
				  osg::ref_ptr<GeoGlobe::Model::Model> modelnode =  dynamic_cast<GeoGlobe::Model::Model*>(node.get());
				  if (!modelnode)
				  {
					  *_ppickJAModelSwitch=false;

					  *_pdoJAModelNode=modelnode;//如果为空，则外部也为空
					  *_pdoJALodModelNode=NULL;
					  continue;
				  }

				  *_ppickJAModelSwitch=true;
				  //选中模型后的操作
				  *_pdoJAModelNode=modelnode;//传进来的地址没有变，只是指向的模型点改变
				  *_pdoJALodModelNode=(GeoGlobe::Model::LodModel*)modelnode->getParent(0);
				  osg::ref_ptr<GeoGlobe::Model::LodModel> templodmodel=*_pdoJALodModelNode;

				  UINT64 mnFID=modelnode->_options->_feature->m_nFID;
				  ULONGLONG uKey = modelnode->_options->_geoKey;

				  //获取模型节点状态（以状态地址来标识是否为同一物体）
				  
				  osg::StateSet* state =modelnode->getParent(0)->getOrCreateStateSet();// modelnode->getOrCreateStateSet();
				  

				  //记录原来的状态
				  _stateTemp = dynamic_cast<osg::StateSet*> (state->clone(osg::CopyOp::DEEP_COPY_ALL));

				  //关闭灯光
				  state->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
				  //打开混合融合模式
				  state->setMode(GL_BLEND,osg::StateAttribute::ON);
				  state->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
				  state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				  //使用BlendFunc
				  //BlendColor:定义一种混合颜色
				  osg::BlendColor* bc =new osg::BlendColor();
				  //BlendFunc：定义一种混合方式
				  osg::BlendFunc*bf = new osg::BlendFunc();
				  //属性设置，开关打开
				  state->setAttributeAndModes(bf,osg::StateAttribute::ON);
				  state->setAttributeAndModes(bc,osg::StateAttribute::ON);
				  //设置源目标参数（靠近屏幕的是源），现在就是选中的东东；CONSTANT_COLOR指使用固定色彩
				  bf->setSource(osg::BlendFunc::CONSTANT_COLOR);
				  //设置目标（远离屏幕的是目标），ZERO表示目标不参与混色
				  bf->setDestination(osg::BlendFunc::ZERO);
				  //固定色彩是多少#FFEC8B
				  bc->setConstantColor(osg::Vec4(1.0f,0.925f,0.545,1.0));
				  //bc->setConstantColor(osg::Vec4(1.0f,0.0f,0.0f,1.0));

				  if (*_ptranslateSwitch)
				  {
					  _pCOEDraggerCintain->DisableDragger(modelnode);
					  _pCOEDraggerCintain->addDraggerToScene(modelnode,TranslateAxis);

					  //绑定矩阵
					//  osg::ref_ptr<GeoGlobe::Model::LodModel> templodmodel=*_pdoJALodModelNode;
					  templodmodel->_options->_nowMatrix=_pCOEDraggerCintain->_Dragger->_DTClastWorldMatrix;//最后的矩阵
				  } 
				  else if(*_protateSwitch)
				  {
					  _pCOEDraggerCintain->DisableDragger(modelnode);
					  _pCOEDraggerCintain->addDraggerToScene(modelnode,Rotateball);

					  //绑定矩阵
					//  osg::ref_ptr<GeoGlobe::Model::LodModel> templodmodel=*_pdoJALodModelNode;
					  templodmodel->_options->_nowMatrix=_pCOEDraggerCintain->_Dragger->_DTClastWorldMatrix;//最后的矩阵
				  }
				  else if (*_pscaleSwitch)
				  {
					  _pCOEDraggerCintain->DisableDragger(modelnode);
					  _pCOEDraggerCintain->addDraggerToScene(modelnode,ScaleAxis);

					  //绑定矩阵
					//  osg::ref_ptr<GeoGlobe::Model::LodModel> templodmodel=*_pdoJALodModelNode;
					  templodmodel->_options->_nowMatrix=_pCOEDraggerCintain->_Dragger->_DTClastWorldMatrix;//最后的矩阵
				  }
				  else
				  {
					  _pCOEDraggerCintain->DisableDragger(modelnode);
				  }
  
				  //相应的模型对应相应的状态
				  _pmapJALodModel.insert(std::map<UINT64,osg::ref_ptr<GeoGlobe::Model::LodModel>>::value_type(mnFID,(GeoGlobe::Model::LodModel*)modelnode->getParent(0)));
				  _pmapJAModel.insert(std::map<ULONGLONG,osg::ref_ptr<GeoGlobe::Model::Model>>::value_type(uKey,modelnode));
				  _pmapJAState.insert(std::map<ULONGLONG,osg::ref_ptr<osg::StateSet>>::value_type(uKey,_stateTemp));

				  break;
			  }

			  break;
		  }
	  }

  
public:
	//得到鼠标的位置
	float _mx ;
	float _my;
	//记录模型节点原来的状态
	osg::ref_ptr<osg::StateSet> _stateTemp;
	//记录保存的模型数 （保存上一步操作的数据结构可以参考）
	std::map<ULONGLONG,osg::ref_ptr<GeoGlobe::Model::Model>> _pmapJAModel;
	//记录保存的模型数 （保存上一步操作的数据结构可以参考）
	std::map<UINT64,osg::ref_ptr<GeoGlobe::Model::LodModel>> _pmapJALodModel;
	//记录保存的状态数 （保存上一步操作的数据结构可以参考）
	std::map<ULONGLONG,osg::ref_ptr<osg::StateSet>> _pmapJAState;
	
	//需要处理的模型节点
	 osg::ref_ptr<GeoGlobe::Model::Model>* _pdoJAModelNode;

	 //需要处理的模型节点
	 osg::ref_ptr<GeoGlobe::Model::LodModel>* _pdoJALodModelNode;

	 //模型处理变量
	 bool *_ppickJAModelSwitch;
	 COEDraggerContainers* _pCOEDraggerCintain;
	 //变换开关
	 bool *_ptranslateSwitch;
	 bool *_protateSwitch;
	 bool *_pscaleSwitch;

};



#endif
