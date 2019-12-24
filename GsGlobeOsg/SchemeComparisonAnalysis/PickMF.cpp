#include "SchemeComparisonAnalysis/PickMF.h"
#include "SchemeComparisonAnalysis/FindVertexVisitor.h"
#include "SchemeComparisonAnalysis/FindNodeVisitor.h"

PickMFHandler::PickMFHandler( osg::ref_ptr<CModelF> *m_doModelNode,osg::ref_ptr<CModelF::CModelFWallGeode> *m_doCMFWall,osg::ref_ptr<CModelF::CModelFFloorGeometry> *m_doCMFFloor,COEDraggerContainers* m_OEDraggerContain,bool * m_pickModel,bool *m_pickWallSwitch,bool *m_pickMFWall,bool *m_pickFloorSwitch,bool *m_pickMFFloor,bool * m_translateModel,bool * m_rotateModel,bool *m_scaleModel,bool * m_extrusionModel,bool* m_gridviewCModelFswitch,std::vector<osg::ref_ptr<osg::Image>> m_GeoImage,osgEarth::MapNode* m_MapNode,osg::ref_ptr<osg::Group> m_allcmfroot):
_mx(0.0f), _my(0.0f)
{
	//高亮显示颜色
	CModelColor=osg::Vec4d(1.0f,0.925f,0.545f,1.0f);
	CModelWallColor=osg::Vec4d(1.0f,0.0f,1.0f,0.0f);
	CModelFloorColor=osg::Vec4d(0.0f,1.0f,1.0f,0.0f);

	_pCOEDraggerCintain=m_OEDraggerContain;

	tempObjTexNum=0;

	_mdirection=osg::Vec3d(0.0f, 0.0f, 1.0f);//默认拉伸方向向上，在经纬度适用。

	_stateTemp=new osg::StateSet();

	_doModelNode=m_doModelNode;//指针地址相同
	_pickModelSwitch=m_pickModel;

	_doCMFWall=m_doCMFWall;//指针地址相同
	_pickMFWall=m_pickMFWall;
	_pickMFWallSwitch=m_pickWallSwitch;

	_doCMFFloor=m_doCMFFloor;//指针地址相同
	_pickMFFloor=m_pickMFFloor;
	_pickMFFloorSwitch=m_pickFloorSwitch;

	_translateswitch=m_translateModel;
	_shownewtranslateMartrix=false;
	_rotateswitch=m_rotateModel;
	_shownewrotateMartrix=false;
	_scaleswitch=m_scaleModel;
	_shownewscaleMartrix=false;

	_extrusionswitch=m_extrusionModel;
	_gridviewCModelFswitch=m_gridviewCModelFswitch;

	_deletemodelswitch=false;
	_hidemodelswitch=false;

	_GeoImage=m_GeoImage;

	//setMapAndCMFRoot
	_pMapNode=m_MapNode;
	_pMapSRS=_pMapNode->getMapSRS();
	_pGeoSRS=_pMapNode->getMapSRS()->getGeographicSRS();

	_pAllCMfroot  = m_allcmfroot;

	//层面拾取辅助按键
	setCMFWallModKeyMask(osgGA::GUIEventAdapter::MODKEY_RIGHT_ALT);
	setCMFFloorModKeyMask(osgGA::GUIEventAdapter::MODKEY_RIGHT_CTRL);
	setCMFWallKeyEvent(osgGA::GUIEventAdapter::KEY_M);
	setCMFFloorKeyEvent(osgGA::GUIEventAdapter::KEY_N);

	_PermittedByCMFWallModKeyMask=false;
	_PermittedByCMFFloorModKeyMask=false;
	_PermittedpickCMFWallByKeyEvent=false;
	_PermittedpickCMFFloorByKeyEvent=false;


}

void PickMFHandler::setMapAndCMFRoot(osg::ref_ptr<CModelF> m_doModelNode)
{
	_pMapNode=m_doModelNode->_mMapNode;
	_pMapSRS=_pMapNode->getMapSRS();
	_pGeoSRS=_pMapNode->getMapSRS()->getGeographicSRS();

	_pAllCMfroot  = m_doModelNode->_mAllCMfroot;
}

bool PickMFHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	osg::ref_ptr<osgViewer::View> view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view) return false;


	//长按键激活
	  //面
	if (_pickCMFWallModKeyMask!=0||_pickCMFWallKeyEvent!=0)
	{
		_PermittedByCMFWallModKeyMask = (_pickCMFWallModKeyMask!=0) ?((ea.getModKeyMask() & _pickCMFWallModKeyMask)!=0) :false;

		if (_pickCMFWallKeyEvent!=0)
		{
			switch (ea.getEventType())
			{
			case osgGA::GUIEventAdapter::KEYDOWN:
				{
					if (ea.getKey()==_pickCMFWallKeyEvent) _PermittedpickCMFWallByKeyEvent = true;
					break;
				}
			case osgGA::GUIEventAdapter::KEYUP:
				{
					if (ea.getKey()==_pickCMFWallKeyEvent) _PermittedpickCMFWallByKeyEvent = false;
					break;
				}
			default:
				break;
			}
		}
	}
	if(_PermittedByCMFWallModKeyMask||_PermittedpickCMFWallByKeyEvent)
	{
		if(ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			if (*_doModelNode&&*_pickMFWallSwitch&&*_pickModelSwitch)
			{
				//更新鼠标位置
				_mx = ea.getX();
				_my = ea.getY();
				 
				pickCMFWallGeode(view.get(), ea);
			}
			else
			{
				printf("请选择房屋或者打开选择墙面开关!\n");
			}
		}
	}

	  //层
	if (_pickCMFFloorModKeyMask!=0||_pickCMFFloorKeyEvent!=0)
	{
		_PermittedByCMFFloorModKeyMask = (_pickCMFFloorModKeyMask!=0) ?((ea.getModKeyMask() & _pickCMFFloorModKeyMask)!=0) :false;

		if (_pickCMFFloorKeyEvent!=0)
		{
			switch (ea.getEventType())
			{
			case osgGA::GUIEventAdapter::KEYDOWN:
				{
					if (ea.getKey()==_pickCMFFloorKeyEvent) _PermittedpickCMFFloorByKeyEvent = true;
					break;
				}
			case osgGA::GUIEventAdapter::KEYUP:
				{
					if (ea.getKey()==_pickCMFFloorKeyEvent) _PermittedpickCMFFloorByKeyEvent = false;
					break;
				}
			default:
				break;
			}
		}
	}
	if(_PermittedByCMFFloorModKeyMask||_PermittedpickCMFFloorByKeyEvent)
	{
		if(ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			if (*_doModelNode&&*_pickMFFloorSwitch&&*_pickModelSwitch)
			{
				//更新鼠标位置
				_mx = ea.getX();
				_my = ea.getY();
				 
				pickCMFFloorGeom(view.get(), ea);
			}
			else
			{
				printf("请选择房屋或者打开选择楼层开关!\n");
			}
		}
	}

// 
// 	if (_pickCMFFloorModKeyMask!=0)
// 	{
// 		_PermittedByCMFFloorModKeyMask = (_PermittedByCMFFloorModKeyMask!=0) ?((ea.getModKeyMask() & _PermittedByCMFFloorModKeyMask)!=0) :false;
// 	}
// 	if(_PermittedByCMFWallModKeyMask)
// 	{
// 		if(ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
// 		{
// 			if (*_doModelNode&&*_pickMFWallSwitch&&*_pickModelSwitch)
// 			{
// 				//更新鼠标位置
// 				_mx = ea.getX();
// 				_my = ea.getY();
// 
// 				pickCMFFloorGeom(view.get(), ea);
// 			}
// 			else
// 			{
// 				printf("请选择房屋或者打开选择墙面开关!\n");
// 			}
// 		}
// 	}
// 
// 	if (_activationKeyEvent!=0)
// 	{
// 		switch (ea.getEventType())
// 		{
// 		case osgGA::GUIEventAdapter::KEYDOWN:
// 			{
// 				if (ea.getKey()==_activationKeyEvent) _activationPermittedByKeyEvent = true;
// 				break;
// 			}
// 		case osgGA::GUIEventAdapter::KEYUP:
// 			{
// 				if (ea.getKey()==_activationKeyEvent) _activationPermittedByKeyEvent = false;
// 				break;
// 			}
// 		default:
// 			break;
// 		}
// 	}
// 	if(_activationPermittedByKeyEvent)
// 	{
// 		if(ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
// 		{
// 			if (*_doModelNode&&*_pickMFWallSwitch&&*_pickModelSwitch)
// 			{
// 				//更新鼠标位置
// 				_mx = ea.getX();
// 				_my = ea.getY();
// 
// 				pickCMFWallGeode(view.get(), ea);
// 			}
// 			else
// 			{
// 				printf("请选择房屋或者打开选择墙面开关!\n");
// 			}
// 		}
// 	}

	switch(ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::DRAG):
		{
			if (*_extrusionswitch)
			{
				osg::ref_ptr<CModelF> modelnode;
				modelnode=*_doModelNode;

				if (modelnode->_Dragger->getDraggerActive())
				{
					osg::Matrix _preShowModelMatrix=*modelnode->_Dragger->_DTClastWorldMatrix;

					//拉伸的长度
					float mlength;

					osg::Vec3d downVecrtice=modelnode->_newLlVertices->at(modelnode->_modelLLVertices->size()-1);
					osg::Vec3d tempdownWorld;
					osgEarth::GeoPoint m_dowm=osgEarth::GeoPoint(_pGeoSRS,downVecrtice.x(),downVecrtice.y(),downVecrtice.z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_pMapSRS);
					m_dowm.toWorld(tempdownWorld);//经纬度转换为世界坐标
					osg::Vec3d tempdownWorld3d=tempdownWorld*_preShowModelMatrix;//新坐标矩阵计算

					osg::Vec3d upVecrtice=modelnode->_newLlVertices->at(modelnode->_newLlVertices->size()-1);
					osg::Vec3d tempupWorld;
					osgEarth::GeoPoint m_up=osgEarth::GeoPoint(_pGeoSRS,upVecrtice.x(),upVecrtice.y(),upVecrtice.z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_pMapSRS);
					m_up.toWorld(tempupWorld);//经纬度转换为世界坐标
					osg::Vec3d tempupWorld3d=tempupWorld*_preShowModelMatrix;//新坐标矩阵计算

					mlength=osg::Vec3(tempupWorld3d.x()-tempdownWorld3d.x(),tempupWorld3d.y()-tempdownWorld3d.y(),tempupWorld3d.z()-tempdownWorld3d.z()).length();
				
					GeoPoint convertPoint;
					convertPoint.fromWorld(_pGeoSRS,tempupWorld3d);

					std::ostringstream s1;
					s1 <<mlength<<"米"<<"\n";
					std::string na = CodeHelp::String_To_UTF8(s1.str());
					m_ptrLabelNode->setPosition(convertPoint);
					m_ptrLabelNode->setText(na);
					m_ptrLabelNode->setStyle(m_SymStyle);

				}
			}
			
		}

		//鼠标按下+左CTRL
	case(osgGA::GUIEventAdapter::DOUBLECLICK):
		{
			if(ea.getButton()==osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON&&!*_extrusionswitch)//0，1，2 右左中
			{
				//更新鼠标位置
				_mx = ea.getX();
				_my = ea.getY();
				pick(view.get(), ea);

			}
			break;
		}
	case (osgGA::GUIEventAdapter::PUSH):
		{
			if (ea.getButton()==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON&&*_gridviewCModelFswitch&&*_doModelNode&&*_pickModelSwitch)
			{
				osg::ref_ptr<CModelF> modelnode;
				modelnode=*_doModelNode;
				if(modelnode->_mAllCMfroot!=NULL&&modelnode->_Dragger!=NULL)
				{
					modelnode->DrawCMFLines();
					modelnode->DrawCMFText("nH");
				}
			}

			break;
		}
	case(osgGA::GUIEventAdapter::RELEASE):
		{
			if (_mx==ea.getX() && _my==ea.getY())
			{
				/*if (_root->getChildIndex(m_ptrLabelNode)!=_root->getNumChildren())
				{
					_root->removeChild(m_ptrLabelNode);
				}*/
				
				
				//执行对象选取
				//pick(view.get(), ea.getX(), ea.getY());
			}
			break;
		}
	case(osgGA::GUIEventAdapter::KEYDOWN):
		{
// 			if (ea.getKey()=='M'||ea.getKey()=='m')
// 			{
// 				if (*_doModelNode&&*_pickMFFloorSwitch&&*_pickModelSwitch)
// 				{
// 					//更新鼠标位置
// 					_mx = ea.getX();
// 					_my = ea.getY();
// 
// 					pickCMFFloorGeom(view.get(), ea);
// 				}
// 				else
// 				{
// 					printf("请选择房屋或者打开选择楼层开关!\n");
// 				}
// 			}

// 			if (ea.getKey()=='n'||ea.getKey()=='N')
// 			{
// 				if (*_doModelNode&&*_pickMFWallSwitch&&*_pickModelSwitch)
// 				{
// 					//更新鼠标位置
// 					_mx = ea.getX();
// 					_my = ea.getY();
// 
// 					pickCMFWallGeode(view.get(), ea);
// 				}
// 				else
// 				{
// 					printf("请选择房屋或者打开选择墙面开关!\n");
// 				}
// 			}
			break;
		}
	default:
		break;
	}
	return false;
}
//对象选取事件处理器
//原(需要精确找到节点)
void PickMFHandler::pick(osg::ref_ptr<osgViewer::View> view, const osgGA::GUIEventAdapter& ea)
{
#if 1
	//创建一个线段交集检测函数
	osgUtil::LineSegmentIntersector::Intersections intersections;
	if (!view->computeIntersections(ea,intersections))
		return;

	//对保存的模型进行设置
	for(std::map<ULONGLONG,osg::ref_ptr<CModelF>>::iterator itmodel = _MapModel.begin();itmodel != _MapModel.end();++itmodel)
	{

		//所有渲染状态清空	  
		itmodel->second->setStateSet(new osg::StateSet);
		itmodel->second->RemoveLinesAText();
		_pCOEDraggerCintain->DisableDragger(itmodel->second);
		itmodel->second->DisableDragger();
	}

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

			if (node->getNumParents()==0)
			{
				continue;
			}

			//沿着路径找到模型节点为止
			osg::ref_ptr<CModelF> modelnode =  dynamic_cast<CModelF*>(node.get());

			if (!modelnode)
			{
				*_pickModelSwitch=false;

				*_doModelNode=modelnode;//如果为空，则外部也为空

				continue;	
			}
			else
			{
				//选中模型
				*_pickModelSwitch=true;//选中模型
				//选中模型后的操作
				*_doModelNode=modelnode;//传进来的地址没有变，只是指向的模型点改变
			}

			modelnode->RemoveLinesAText();

			//高亮显示操作
#if 1
			//获取模型节点状态（以状态地址来标识是否为同一物体）
			osg::StateSet* state = modelnode->getOrCreateStateSet();
			//记录原来的状态
			_stateTemp = modelnode->getOrCreateStateSet();// dynamic_cast<osg::StateSet*> (state->clone(osg::CopyOp::DEEP_COPY_ALL));

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
			state->setAttributeAndModes(bf,osg::StateAttribute::ON||osg::StateAttribute::OVERRIDE);
			state->setAttributeAndModes(bc,osg::StateAttribute::ON||osg::StateAttribute::OVERRIDE);
			//设置源目标参数（靠近屏幕的是源），现在就是选中的东东；CONSTANT_COLOR指使用固定色彩
			bf->setSource(osg::BlendFunc::CONSTANT_COLOR);
			//设置目标（远离屏幕的是目标），ZERO表示目标不参与混色
			bf->setDestination(osg::BlendFunc::ZERO);
			//固定色彩是多少#FFEC8B
			bc->setConstantColor(CModelColor);


			if (*_translateswitch)
			{
				_pCOEDraggerCintain->DisableDragger(modelnode);
				_pCOEDraggerCintain->addDraggerToScene(modelnode,TranslateAxis);
			} 
			else if(*_rotateswitch)
			{
				_pCOEDraggerCintain->DisableDragger(modelnode);
				_pCOEDraggerCintain->addDraggerToScene(modelnode,Rotateball);
			}
			else if (*_scaleswitch)
			{
				_pCOEDraggerCintain->DisableDragger(modelnode);
				_pCOEDraggerCintain->addDraggerToScene(modelnode,ScaleAxis);
			}
			else if (*_extrusionswitch)
			{
				_pCOEDraggerCintain->DisableDragger(modelnode);
				modelnode->DisableDragger();
				modelnode->addExtrusionDragger(ExtScaleAxis);
			}
			else
			{
				modelnode->DisableDragger();
				_pCOEDraggerCintain->DisableDragger(modelnode);
			}

			//面 几何体高亮取消，如果选中过 //防止预先选中
			if (*_doCMFWall!=NULL)
			{
				osg::ref_ptr<CModelF::CModelFWallGeode> geodenode=*_doCMFWall;
				//楼层 几何体所在面被选中，全部变色与面一样
				for(int i=0;i<geodenode->getNumDrawables();i++)
				{
					osg::ref_ptr<CModelF::CModelFFloorGeometry> geometrygeodenode=dynamic_cast<CModelF::CModelFFloorGeometry*>(geodenode->getDrawable(i)->asGeometry());
					//BlendColor:定义一种混合颜色
					osg::BlendColor* bc =new osg::BlendColor();

					//固定色彩是多少#FFEC8B
					bc->setConstantColor(CModelColor);//1.0f,0.925f,0.545,1.0
					geometrygeodenode->getOrCreateStateSet()->setAttributeAndModes(bc,osg::StateAttribute::ON);//允许继承父节点的渲染
				}
				*_doCMFWall=NULL;
				_pukeym=-1;
			}

			//楼层 几何体高亮取消，如果选中过 //防止预先选中
			if (*_doCMFFloor!=NULL)
			{

				//BlendColor:定义一种混合颜色
				osg::BlendColor* bc =new osg::BlendColor();

				//固定色彩是多少#FFEC8B
				bc->setConstantColor(CModelColor);//1.0f,0.925f,0.545,1.0
				osg::ref_ptr<CModelF::CModelFFloorGeometry>  geometrynode=*_doCMFFloor;
				geometrynode->getOrCreateStateSet()->setAttributeAndModes(bc,osg::StateAttribute::ON);//允许继承父节点的渲染

				*_doCMFFloor=NULL;
				_pukeyf=-1;
			}
#endif
			_MapModel.clear();
			//相应的模型对应相应的状态，若选中模型之前没被选中，则保存到map
			_MapModel.insert(std::map<ULONGLONG,osg::ref_ptr<CModelF>>::value_type(*modelnode->_mukey,modelnode));

			break;
		}
		break;
	}

#endif

}

//CModelFWallGeode对象选取事件处理器
void PickMFHandler::pickCMFWallGeode(osg::ref_ptr<osgViewer::View> view, const osgGA::GUIEventAdapter& ea)
{
	//创建一个线段交集检测函数
	osgUtil::LineSegmentIntersector::Intersections intersections;
	if (!view->computeIntersections(ea,intersections))
		return;

	//对保存的模型进行设置
	for(std::map<ULONGLONG,osg::ref_ptr<CModelF::CModelFWallGeode>>::iterator itgeom = _MapCMFGeode.begin();itgeom != _MapCMFGeode.end();++itgeom)
	{
		//楼层 几何体所在面被选中，全部变色与面一样
		for(int i=0;i<itgeom->second->getNumDrawables();i++)
		{
			osg::ref_ptr<CModelF::CModelFFloorGeometry> geometrygeodenode=dynamic_cast<CModelF::CModelFFloorGeometry*>(itgeom->second->getDrawable(i)->asGeometry());
			//BlendColor:定义一种混合颜色
			osg::BlendColor* bc =new osg::BlendColor();

			//固定色彩是多少#FFEC8B
			bc->setConstantColor(CModelColor);//1.0f,0.925f,0.545,1.0
			geometrygeodenode->getOrCreateStateSet()->setAttributeAndModes(bc,osg::StateAttribute::ON);//允许继承父节点的渲染
		}
	}

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
			osg::ref_ptr<CModelF::CModelFWallGeode> geodenode=dynamic_cast<CModelF::CModelFWallGeode*>(node.get());

			if (!geodenode)
			{
				*_pickMFWall=false; 

				*_doCMFWall=geodenode;//如果为空，则外部也为空

				continue;
			}
			else
			{
				*_pickMFWall=true;
				//选中面 几何体后的操作 
				*_doCMFWall=geodenode;//传进来的地址没有变，只是指向的模型点改变
			}

			//判断是不是同一个房屋
			osg::ref_ptr<CModelF> m_doCModelNode=*_doModelNode;
			if (geodenode->_mukeyCMFGeode!=*m_doCModelNode->_mukey)//防止选错
			{
				return;
			}

			//高亮显示操作
#if 1
			//楼层 几何体所在面被选中，全部变色
			for(int i=0;i<geodenode->getNumDrawables();i++)
			{
				osg::ref_ptr<CModelF::CModelFFloorGeometry> geometrygeodenode=dynamic_cast<CModelF::CModelFFloorGeometry*>(geodenode->getDrawable(i)->asGeometry());
				//BlendColor:定义一种混合颜色
				osg::BlendColor* bc =new osg::BlendColor();

				//固定色彩是多少#FFEC8B
				bc->setConstantColor(CModelWallColor);//1.0f,0.925f,0.545,1.0
				geometrygeodenode->getOrCreateStateSet()->setAttributeAndModes(bc,osg::StateAttribute::ON);//允许继承父节点的渲染
			}
#endif
			_MapCMFGeode.clear();
			//相应的模型对应相应的状态，若选中模型之前没被选中，则保存到map
			_MapCMFGeode.insert(std::map<ULONGLONG,osg::ref_ptr<CModelF::CModelFWallGeode>>::value_type(geodenode->_mukeyWallGeode,geodenode));
			break;
		}
		break;
	}
}

//CModelFFloorGeometry对象选取事件处理器
void PickMFHandler::pickCMFFloorGeom(osg::ref_ptr<osgViewer::View> view, const osgGA::GUIEventAdapter& ea)
{
	//创建一个线段交集检测函数
	osgUtil::LineSegmentIntersector::Intersections intersections;
	if (!view->computeIntersections(ea,intersections))
		return;

	//对保存的模型进行设置
	for(std::map<ULONGLONG,osg::ref_ptr<CModelF::CModelFFloorGeometry>>::iterator itgeom = _MapCMFGeom.begin();itgeom != _MapCMFGeom.end();++itgeom)
	{
		//BlendColor:定义一种混合颜色
		osg::BlendColor* bc =new osg::BlendColor();

		//固定色彩是多少#FFEC8B
		bc->setConstantColor(CModelColor);//1.0f,0.925f,0.545,1.0
		//所有渲染状态恢复
		itgeom->second->getOrCreateStateSet()->setAttributeAndModes(bc,osg::StateAttribute::ON);

	} 

	for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
		hitr != intersections.end();
		++hitr)
	{ 

		osg::ref_ptr<CModelF::CModelFFloorGeometry> geometrynode=dynamic_cast<CModelF::CModelFFloorGeometry*>(hitr->drawable->asGeometry());


		if (!geometrynode)
		{
			*_pickMFFloor=false;

			*_doCMFFloor=geometrynode;//如果为空，则外部也为空

			continue;
		}
		else
		{
			*_pickMFFloor=true;
			//选中面 几何体后的操作 
			*_doCMFFloor=geometrynode;//传进来的地址没有变，只是指向的模型点改变
		}

		//判断是不是同一个房屋
		osg::ref_ptr<CModelF> m_doCModelNode=*_doModelNode;
		if (geometrynode->_mukeyCMFGeom!=*m_doCModelNode->_mukey)//防止选错
		{
			return;
		}

		//高亮显示操作
#if 1
		//BlendColor:定义一种混合颜色
		osg::BlendColor* bc =new osg::BlendColor();

		//固定色彩是多少#FFEC8B
		bc->setConstantColor(CModelFloorColor);//1.0f,0.925f,0.545,1.0
		geometrynode->getOrCreateStateSet()->setAttributeAndModes(bc,osg::StateAttribute::ON);//允许继承父节点的渲染

#endif
		_MapCMFGeom.clear();
		//相应的模型对应相应的状态，若选中模型之前没被选中，则保存到map
		_MapCMFGeom.insert(std::map<ULONGLONG,osg::ref_ptr<CModelF::CModelFFloorGeometry>>::value_type(geometrynode->_mukeyFloorGeom,geometrynode));

		break;
	}
}

void PickMFHandler::ex(osg::ref_ptr<CModelF> modelnode,ULONGLONG uKey,const osgEarth::SpatialReference* mMapSRS,const osgEarth::SpatialReference* mGeoSRS)
{
	_replaceCModelF=new CModelF(modelnode->_mukey,modelnode->_modelLLVertices,modelnode->_mMapNode,modelnode->_mAllCMfroot,modelnode->_getTexNums,_GeoImage,&modelnode->_mfloorHeight,&modelnode->_mfloorNums);

	Registry::shaderGenerator().run(_replaceCModelF);//注册

	modelnode->getParent(0)->replaceChild(modelnode,_replaceCModelF);// 替换

	//相应的模型对应相应的状态，若选中模型之前没被选中，则保存到map
	_MapModel.clear();

	_MapModel.insert(std::map<ULONGLONG,osg::ref_ptr<CModelF>>::value_type(uKey,_replaceCModelF));

}

void PickMFHandler::CreateLabel(GeoPoint geoPoint,std::string modelhight)
{
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->size()=18.0f;
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=Color::White;
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color()=Color::Red;
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "../resource/Fonts/simhei.ttf";
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	std::ostringstream s1;
	s1 <<modelhight<<"米"<<"\n";
	std::string na = CodeHelp::String_To_UTF8(s1.str());

	m_ptrLabelNode = new osgEarth::Annotation::LabelNode(_pMapNode,geoPoint,na, m_SymStyle);
	m_ptrLabelNode->setDynamic( true );
	m_ptrLabelNode->setCullingActive(true);
	m_ptrLabelNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	_pAllCMfroot->addChild(m_ptrLabelNode);
}

void PickMFHandler::RemoveLabel()
{
	if (_pAllCMfroot->getChildIndex(m_ptrLabelNode)!=_pAllCMfroot->getNumChildren())
	{
		_pAllCMfroot->removeChild(m_ptrLabelNode);
	}
}
