#pragma once
#include "ModelPick.h"
#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>

#include <osgViewer/CompositeViewer>

#include <osgGA/TerrainManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <osg/Material>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/Camera>
#include <osg/io_utils>
#include <osg/ShapeDrawable>
#include <osg/TexGen>
#include <osg/BlendColor>
#include <osgText/Text>
#include <sstream>



#include <osgWidget/Util>
#include <osgWidget/WindowManager>
#include <osgWidget/Table>
#include <osgWidget/ViewerEventHandlers>

#include <osgEarthAnnotation/FeatureNode>
/*extern osg::Group* g_root;

void addWidget(osg::Group* root_group,osgViewer::View * root_view ,float oX,float oY)
{
	osgWidget::Table* table = new osgWidget::Table("table", 3, 3);

	// Here we create our "cells" manually, though it will often be convenient to
	// do so algorithmically. Also, notice how we set the text name of each widget to
	// correspond with it's "index" in the table. This is merely a convenience, which
	// we use later...
	table->addWidget(new osgWidget::Widget("0, 0", 100.0f, 25.0f), 0, 0);
	table->addWidget(new osgWidget::Widget("0, 1", 100.0f, 25.0f), 0, 1);
	//table->addWidget(new osgWidget::Widget("0, 2", 100.0f, 75.0f), 0, 2);
	osgWidget::Widget* W0 = new osgWidget::Widget("0, 2", 100.0f, 75.0f);
	W0->setImage(osgDB::readImageFile("D:/OSG/OpenSceneGraph-Data/Images/frame1.jpg"), true);
	//table->addWidget(W0, 0, 2);

	/*table->addWidget(new osgWidget::Widget("1, 0", 200.0f, 45.0f), 1, 0);
	table->addWidget(new osgWidget::Widget("1, 1", 200.0f, 45.0f), 1, 1);
	table->addWidget(new osgWidget::Widget("1, 2", 200.0f, 45.0f), 1, 2);

	table->addWidget(new osgWidget::Widget("2, 0", 300.0f, 65.0f), 2, 0);
	table->addWidget(new osgWidget::Widget("2, 1", 300.0f, 65.0f), 2, 1);

	osgWidget::Label* label = new osgWidget::Label("2, 2", "");
	label->setFont("fonts/simhei.ttf");
	label->setFontSize(13);
	label->setFontColor(1.0f, 1.0f, 1.0f, 1.0f);
	label->setLabel( "...reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla\n");
	table->addWidget(label, 2, 2);

	table->getBackground()->setColor(0.5f, 0.0f, 0.5f, 0.0f);
	table->attachMoveCallback();

	// Use a hackish method of setting the spacing for all Widgets.
	for(osgWidget::Table::Iterator i = table->begin(); i != table->end(); i++)
		i->get()->setPadding(1.0f)
		;

	// Now we fetch the very first 0, 0 Widget in the table using an awkward method.
	// This is merely one way to fetch a Widget from a Window, there are many others.
	// The osgWidget::Window::getByName interface will be very handy in scripting languages
	// where users will want to retrieve handles to existing Windows using a useful
	// textual name, such as "MainGUIParent" or something.
	table->getByName("0, 0")->setAlignHorizontal(osgWidget::Widget::HA_LEFT);
	table->getByName("0, 0")->setAlignVertical(osgWidget::Widget::VA_BOTTOM);
	table->getByName("0, 0")->setPadLeft(50.0f);
	table->getByName("0, 0")->setPadTop(3.0f);

	// Change the colors a bit to differentiate this row from the others.
	//table->getByName("2, 0")->setColor(1.0f, 0.0f, 0.0f, 1.0f, osgWidget::Widget::LOWER_LEFT);
	//table->getByName("2, 1")->setColor(1.0f, 0.0f, 0.0f, 0.5f);
	//table->getByName("2, 2")->setColor(1.0f, 0.0f, 0.5f, 0.2f);
	//table->setOrigin(300,300);
	osg::ref_ptr<osgWidget::WindowManager> wm = 
		new osgWidget::WindowManager(root_view, 1024.f, 800.f, 0xf0000000);

	

	//创建一个正射投影的相机
	osg::Camera *camera = wm->createParentOrthoCamera();
	//添加tabs， 代码见下面
	//osgWidget::Box*   box   = new osgWidget::Box("vbox", osgWidget::Box::VERTICAL);
	osg::ref_ptr<osgWidget::Canvas> pCanvas = new osgWidget::Canvas("contents");
	//pCanvas->addWidget(W0, 0.0f, 0.0f);

	osgWidget::Label* label = new osgWidget::Label("label", "");
	label->setFont("fonts/simhei.ttf");
	label->setFontSize(24);
	label->setFontColor(1.0f, 0.0f, 0.0f, 1.0f);
	label->setLabel( "pick test ttttttttttttt\n");
	label->setColor(0.0f, 1.0f, 0.0f, 0.5f);
	label->setSize(100,24);
	pCanvas->addWidget(label, 0.0f, 0.0f);

	//box->addWidget(W0);
	//W0->addSize(512,512);
	//contents->setOrigin(300,300);
	pCanvas->getBackground()->addColor(0.5f,0.5f,0.0f,0.5f);
	pCanvas->setPosition(50.0f,50.0f,0);

	wm->addChild(pCanvas);
	wm->resizeAllWindows();
	g_root->addChild(camera);
}




/** create quad at specified position. */
/*osg::Drawable* createSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height, osg::Image* image=NULL)
{
	// set up the Geometry.
	osg::Geometry* geom = new osg::Geometry;

	osg::Vec3Array* coords = new osg::Vec3Array(4);
	(*coords)[0] = corner;
	(*coords)[1] = corner+width;
	(*coords)[2] = corner+width+height;
	(*coords)[3] = corner+height;


	geom->setVertexArray(coords);

	osg::Vec3Array* norms = new osg::Vec3Array(1);
	(*norms)[0] = width^height;
	(*norms)[0].normalize();

	geom->setNormalArray(norms, osg::Array::BIND_OVERALL);

	osg::Vec2Array* tcoords = new osg::Vec2Array(4);
	(*tcoords)[0].set(0.0f,0.0f);
	(*tcoords)[1].set(1.0f,0.0f);
	(*tcoords)[2].set(1.0f,1.0f);
	(*tcoords)[3].set(0.0f,1.0f);
	geom->setTexCoordArray(0,tcoords);

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

	if (image)
	{
		osg::StateSet* stateset = new osg::StateSet;
		osg::Texture2D* texture = new osg::Texture2D;
		texture->setImage(image);
		stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
		geom->setStateSet(stateset);
	}

	return geom;
}

void billboardtest(GeoGlobe::Model::Model* model)
{

	// add billboard test
	osg::Billboard* center = new osg::Billboard();
	center->setMode(osg::Billboard::POINT_ROT_EYE);
	center->addDrawable(
		createSquare(osg::Vec3(-0.50000f,0.0f,-0.50000f),osg::Vec3(100000.0f,0.0f,0.0f),osg::Vec3(0.0f,0.0f,100000.0f),osgDB::readImageFile("D:/OSG/OpenSceneGraph-Data/Images/reflect.rgb")),
		osg::Vec3(300000000.0f,0.0f,0.0f));


	//
	osg::Matrixd vertialGroundMatrix;
	model->getMap()->getProfile()->getSRS()->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(135.0f),osg::DegreesToRadians(-35.0f),1500.0f,vertialGroundMatrix);

	//对于osgb格式来说，只要设置它的旋转矩阵即可在虚拟地球上绘制
	osg::MatrixTransform* mt=new osg::MatrixTransform;
	mt->setMatrix(vertialGroundMatrix);
	mt->addChild(center);
	model->addChild(mt);
}*/
namespace GeoGlobe
{
	namespace Model
	{

		PickHandler::PickHandler(osg::Group* root) 
		{
			m_root = root;
			m_bTransparent = false;
			m_fTransValue = 1.0f;
			m_ptrConWnd = nullptr;
			m_ptrMainWindow = nullptr;
			m_bFinished = false;
		}

		PickHandler::~PickHandler() 
		{
			_mapModel.clear();
			_MapState.clear();
		}


		bool PickHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
		{
			switch(ea.getEventType())
			{
			//拾取改为释放时响应
			case(osgGA::GUIEventAdapter::RELEASE):
				{
					osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
					if (view)
						pick(view,ea);
					return false;
				}
			case(osgGA::GUIEventAdapter::KEYDOWN):
				{
					if (ea.getKey()=='c')
					{
						osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
						osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter(ea);
						event->setX((ea.getXmin()+ea.getXmax())*0.5);
						event->setY((ea.getYmin()+ea.getYmax())*0.5);
						if (view) pick(view,*event);
					}
					return false;
				}
			default:
				return false;
			}
		}

		//清除已有模型状态
		void PickHandler::clearModelState()
		{
			//移出原来选中的对象
			for(std::map<ULONGLONG,osg::ref_ptr<osg::StateSet>>::iterator itState = _MapState.begin();itState != _MapState.end();++itState)
			{
				std::map<ULONGLONG,osg::ref_ptr<GeoGlobe::Model::LodModel>>::iterator itModel = _mapModel.find(itState->first);
				if (itModel == _mapModel.end())
					continue;
				//还原原有的渲染状态
				itModel->second->setStateSet(itState->second);
			}
			_mapModel.clear();
			_MapState.clear();
		}

		void PickHandler::clearModelState(unsigned long long nID)
		{
			std::map<ULONGLONG,osg::ref_ptr<osg::StateSet>>::iterator itState = _MapState.find(nID);
			std::map<ULONGLONG,osg::ref_ptr<GeoGlobe::Model::LodModel>>::iterator itModel = _mapModel.find(itState->first);
			if (itState==_MapState.end() || itModel==_mapModel.end())
				return;

			itModel->second->setStateSet(itState->second);
			_mapModel.erase(itModel);
			_MapState.erase(itState);
		}

		void PickHandler::setMainWindow(GeoGlobe::IMainWindow * ptrMainWindow)
		{
			m_ptrMainWindow = ptrMainWindow;
		}

		void PickHandler::setConWnd(GeoGlobe::IConsultationWindow * ptrConWnd)
		{
			m_ptrConWnd = ptrConWnd;
		}

		void PickHandler::Transparent(float fValue)
		{
			if (fValue >= 1.0f)
				m_bTransparent = false;
			else
				m_bTransparent = true;
			m_fTransValue = fValue;
		}

		void PickHandler::setFinished(bool bFinish)
		{
			m_bFinished = bFinish;
		}

		void PickHandler::virtualpick(GeoGlobe::Model::LodModel *pModelNode)
		{
			osg::StateSet* state = pModelNode->getOrCreateStateSet();
			if (!state)
				return;

			m_ptrMainWindow->setModelNode(pModelNode);

			//记录原来的状态
			osg::ref_ptr<osg::StateSet> stateTemp = dynamic_cast<osg::StateSet*> (state->clone(osg::CopyOp::DEEP_COPY_ALL));

			//关闭灯光
			state->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
			//打开混合融合模式
			state->setMode(GL_BLEND,osg::StateAttribute::ON);
			state->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
			state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

			if (m_bTransparent)
			{
				//使用BlendFunc
				//可以参考http://www.dreamingwish.com/frontui/article/default/glblendfunc.html
				//和openGL接口几乎一致
				//BlendColor:定义一种混合颜色
				osg::BlendColor* bc =new osg::BlendColor(osg::Vec4(1.0f,1.0f,1.0f,m_fTransValue));
				//BlendFunc：定义一种混合方式
				osg::BlendFunc*bf = new osg::BlendFunc();
				//属性设置，开关打开
				state->setAttributeAndModes(bf,osg::StateAttribute::ON);
				state->setAttributeAndModes(bc,osg::StateAttribute::ON);
				//设置源目标参数（靠近屏幕的是源），现在就是选中的东东；CONSTANT_COLOR指使用固定色彩
				bf->setSource(/*osg::BlendFunc::CONSTANT_COLOR*/osg::BlendFunc::CONSTANT_ALPHA);
				//设置目标（远离屏幕的是目标），ZERO表示目标不参与混色
				bf->setDestination(/*osg::BlendFunc::ZERO*/osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
				//固定色彩是多少#FFEC8B
				//bc->setConstantColor(osg::Vec4(1.0f,0.925f,0.545,1.0));
				bc->setConstantColor(osg::Vec4(1.0f,1.0f,0.0f,m_fTransValue));
			}
			else
			{
				//使用BlendFunc
				//可以参考http://www.dreamingwish.com/frontui/article/default/glblendfunc.html
				//和openGL接口几乎一致
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
				//bc->setConstantColor(osg::Vec4(1.0f,0.925f,0.545,1.0));
				bc->setConstantColor(osg::Vec4(1.0f,1.0f,0.0f,1.0));
			}
					
					
			_mapModel.insert(std::map<ULONGLONG,osg::ref_ptr<GeoGlobe::Model::LodModel>>::value_type(pModelNode->_options->_feature->m_nFID,pModelNode));
			_MapState.insert(std::map<ULONGLONG,osg::ref_ptr<osg::StateSet>>::value_type(pModelNode->_options->_feature->m_nFID,stateTemp));
		}

		void PickHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
		{
			if (m_bFinished)
				return;

			osgUtil::LineSegmentIntersector::Intersections intersections;
			if (!view->computeIntersections(ea,intersections))
				return;
			
			//清除已有模型状态
			clearModelState();

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

					osg::ref_ptr<GeoGlobe::Model::LodModel> modelnode =  dynamic_cast<GeoGlobe::Model::LodModel*>(node.get());
					if (!modelnode)
						continue;

					// Style our labels:
					Style labelStyle;
					labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_CENTER;
					labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::Yellow;
					//parent->addChild(new LabelNode(mapNode, GeoPoint(geoSRS, -175, -35), "Antimeridian polygon", labelStyle));

					//ULONGLONG uKey = modelnode->_options->_geoKey;
					ULONGLONG uKey = modelnode->_options->_feature->m_nFID;
					//首先查找map中是否存在model,如果找到则已经使用特殊绘制
					std::map<ULONGLONG,osg::ref_ptr<GeoGlobe::Model::LodModel>>::iterator it = _mapModel.find(uKey);
					if (it != _mapModel.end())
						continue;

					//测试
					osg::StateSet* state = modelnode->getOrCreateStateSet();
					if (!state)
						continue;

					if (m_ptrMainWindow != NULL)
						m_ptrMainWindow->setModelNode(modelnode);

					if (m_ptrConWnd!=NULL && !m_ptrConWnd->IsPickUpFinished())
						m_ptrConWnd->AddModelHightLightStep(uKey);

					//记录原来的状态
					osg::ref_ptr<osg::StateSet> stateTemp = dynamic_cast<osg::StateSet*> (state->clone(osg::CopyOp::DEEP_COPY_ALL));

					//关闭灯光
					state->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
					//打开混合融合模式
					state->setMode(GL_BLEND,osg::StateAttribute::ON);
					state->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
					state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

					if (m_bTransparent)
					{
						//使用BlendFunc
						//可以参考http://www.dreamingwish.com/frontui/article/default/glblendfunc.html
						//和openGL接口几乎一致
						//BlendColor:定义一种混合颜色
						osg::BlendColor* bc =new osg::BlendColor(osg::Vec4(1.0f,1.0f,1.0f,m_fTransValue));
						//BlendFunc：定义一种混合方式
						osg::BlendFunc*bf = new osg::BlendFunc();
						//属性设置，开关打开
						state->setAttributeAndModes(bf,osg::StateAttribute::ON);
						state->setAttributeAndModes(bc,osg::StateAttribute::ON);
						//设置源目标参数（靠近屏幕的是源），现在就是选中的东东；CONSTANT_COLOR指使用固定色彩
						bf->setSource(/*osg::BlendFunc::CONSTANT_COLOR*/osg::BlendFunc::CONSTANT_ALPHA);
						//设置目标（远离屏幕的是目标），ZERO表示目标不参与混色
						bf->setDestination(/*osg::BlendFunc::ZERO*/osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
						//固定色彩是多少#FFEC8B
						//bc->setConstantColor(osg::Vec4(1.0f,0.925f,0.545,1.0));
						bc->setConstantColor(osg::Vec4(1.0f,1.0f,0.0f,m_fTransValue));
					}
					else
					{
						//使用BlendFunc
						//可以参考http://www.dreamingwish.com/frontui/article/default/glblendfunc.html
						//和openGL接口几乎一致
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
						//bc->setConstantColor(osg::Vec4(1.0f,0.925f,0.545,1.0));
						bc->setConstantColor(osg::Vec4(1.0f,1.0f,0.0f,1.0));
					}
					
					
					_mapModel.insert(std::map<ULONGLONG,osg::ref_ptr<GeoGlobe::Model::LodModel>>::value_type(uKey,modelnode));
					_MapState.insert(std::map<ULONGLONG,osg::ref_ptr<osg::StateSet>>::value_type(uKey,stateTemp));

					break;
				}

				break;
			}
		}
	}
}
