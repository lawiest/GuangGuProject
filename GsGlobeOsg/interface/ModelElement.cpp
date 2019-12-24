#include "ModelElement.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <osgDB/ReadFile>

#include <osgEarth/GeoTransform>
#include <osgEarth/MapNode>
#include <Analysis/MatrixTransformVisible.h>

/*临时代码*/
//#include <osg/BlendColor>
//#include <osg/BlendFunc>
//#include <BillBoardElement.h>
//#include <osgViewer/Viewer>
//#include <osgEarthSymbology/Color>
//#include <osgEarthUtil/Controls>
//#include "tool/CodeTransformHelp.h"
//
//using namespace osgEarth::Util::Controls;
//
//static std::map < std::string, osg::ref_ptr<osg::Node> > g_Model1;
//static std::map < std::string, osg::ref_ptr<osg::Node> > g_Model2;
//static std::map < std::string, osg::ref_ptr<osg::Node> > g_Model3;
//static std::map < std::string, osg::ref_ptr<osg::Node> > g_Model4;
//static std::map < std::string, osg::ref_ptr<osg::Node> > g_Model5;
//
//class PickModelHandler : public osgGA::GUIEventHandler
//{
//public:
//
//	PickModelHandler()
//	{
//		m_ptrPreNode = NULL;
//		m_ptrPreState = NULL;
//
//		osg::ref_ptr<ControlCanvas> ptrControlCanvas = ControlCanvas::getOrCreate(GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer());
//
//		grid = new Grid();
//		grid->setBackColor(osgEarth::Symbology::Color(osgEarth::Symbology::Color::Black, 0.5f));
//		grid->setVisible(false);
//
//		bottom = new VBox();
//		//bottom->setBackColor(1.0f, 1.0f, 1.0f, 0.5);
//		bottom->setBackColor(0.0f, 0.0f, 0.0f, 0.5);
//		bottom->setMargin(0);
//		bottom->setChildSpacing(5);
//		bottom->setVertAlign(Control::ALIGN_TOP);
//		bottom->setHorizAlign(Control::ALIGN_LEFT);
//		bottom->setVisible(false);
//
//		LabelControl* ptrCodeLabel;
//		LabelControl* ptrLocateLabel;
//		LabelControl* ptrQuanShuLabel;
//		LabelControl* ptrShiYongQuanTypeLabel;
//		LabelControl* ptrTudiYongTuLabel;
//
//		ptrCodeLabel = CreateLabel();
//		ptrCodeLabel->setText(CodeHelp::String_To_UTF8("宗地代码："));
//
//		ptrLocateLabel = CreateLabel();
//		ptrLocateLabel->setText(CodeHelp::String_To_UTF8("土地座落："));
//
//		ptrQuanShuLabel = CreateLabel();
//		ptrQuanShuLabel->setText(CodeHelp::String_To_UTF8("权属性质："));
//
//		ptrShiYongQuanTypeLabel = CreateLabel();
//		ptrShiYongQuanTypeLabel->setText(CodeHelp::String_To_UTF8("使用权类型："));
//
//		ptrTudiYongTuLabel = CreateLabel();
//		ptrTudiYongTuLabel->setText(CodeHelp::String_To_UTF8("土地用途："));
//
//		LabelControl* ptrLandInfoLabel = CreateLabel();
//		//ptrLandInfoLabel->setAlign(Controls::Control::ALIGN_CENTER, Controls::Control::ALIGN_CENTER);
//		ptrLandInfoLabel->setText(CodeHelp::String_To_UTF8("用地信息"));
//		ptrLandInfoLabel->setBackColor(osg::Vec4f(1.0f, 1.0f, 0.0f, 0.5f));
//
//		grid->addControl(ptrLandInfoLabel);
//		grid->setControl(0, 1, ptrCodeLabel);
//		grid->setControl(0, 2, ptrLocateLabel);
//		grid->setControl(0, 3, ptrQuanShuLabel);
//		grid->setControl(0, 4, ptrShiYongQuanTypeLabel);
//		grid->setControl(0, 5, ptrTudiYongTuLabel);
//		//grid->setControl(0, 5, ptrLandInfoLabel);
//		
//		LabelControl*  ptrCodeContex = CreateLabel();
//		ptrCodeContex->setText("420501003001GB00001");
//
//		LabelControl*  ptrLocateContex = CreateLabel();
//		ptrLocateContex->setText(CodeHelp::String_To_UTF8("宜昌市常刘路103号"));
//
//		LabelControl*  ptrQuanShuContex = CreateLabel();
//		ptrQuanShuContex->setText(CodeHelp::String_To_UTF8("国有土地使用权"));
//
//		LabelControl*  ptrShiYongQuanTypeContext = CreateLabel();
//		ptrShiYongQuanTypeContext->setText(CodeHelp::String_To_UTF8("出让"));
//
//		LabelControl*  ptrLandInfoContex = CreateLabel();
//		ptrLandInfoContex->setText(CodeHelp::String_To_UTF8("住宅"));
//
//		grid->setControl(5, 1, ptrCodeContex);
//		grid->setControl(5, 2, ptrLocateContex);
//		grid->setControl(5, 3, ptrQuanShuContex);
//		grid->setControl(5, 4, ptrShiYongQuanTypeContext);
//		grid->setControl(5, 5, ptrLandInfoContex);
//
//		LabelControl* ptrBuildingName;
//		LabelControl* ptrNumber;
//		LabelControl* ptrBuildYear;
//		LabelControl* ptrBuildStruct;
//
//		ptrBuildingName = CreateLabel();
//		ptrBuildingName->setText(CodeHelp::String_To_UTF8("建筑名称："));
//		ptrNumber = CreateLabel();
//		ptrNumber->setText(CodeHelp::String_To_UTF8("编号："));
//		ptrBuildYear = CreateLabel();
//		ptrBuildYear->setText(CodeHelp::String_To_UTF8("建筑年代："));
//		ptrBuildStruct = CreateLabel();
//		ptrBuildStruct->setText(CodeHelp::String_To_UTF8("结构："));
//
//		LabelControl* ptrBuildinfo = CreateLabel();
//		ptrBuildinfo->setText(CodeHelp::String_To_UTF8("房屋信息"));
//		grid->addControl(ptrBuildinfo);
//		ptrBuildinfo->setBackColor(osg::Vec4f(1.0f, 1.0f, 0.0f, 0.5f));
//
//		grid->setControl(0, 7, ptrBuildingName);
//		grid->setControl(0, 8, ptrNumber);
//		grid->setControl(0, 9, ptrBuildYear);
//		grid->setControl(0, 10, ptrBuildStruct);
//
//		LabelControl* ptrBuildingNameCon = CreateLabel();
//		ptrBuildingNameCon->setText(CodeHelp::String_To_UTF8("康居园37栋"));
//		LabelControl*  ptrNumberCon = CreateLabel();
//		ptrNumberCon->setText("1－37");
//		LabelControl*  ptrBuildYearCon = CreateLabel();
//		ptrBuildYearCon->setText(CodeHelp::String_To_UTF8("2016年6月1日"));
//		LabelControl*  ptrBuildStructCon = CreateLabel();
//		ptrBuildStructCon->setText(CodeHelp::String_To_UTF8("砖混"));
//
//		grid->setControl(5, 7, ptrBuildingNameCon);
//		grid->setControl(5, 8, ptrNumberCon);
//		grid->setControl(5, 9, ptrBuildYearCon);
//		grid->setControl(5, 10, ptrBuildStructCon);
//
//
//		LabelControl* ptrOwnerName;
//		LabelControl* ptrSex;
//		LabelControl* ptrNation;
//		LabelControl* ptrIDType;
//		LabelControl* ptrIDNum;
//		LabelControl* ptrAddre;
//
//		ptrOwnerName = CreateLabel();
//		ptrOwnerName->setText(CodeHelp::String_To_UTF8("姓名："));
//		ptrSex = CreateLabel();
//		ptrSex->setText(CodeHelp::String_To_UTF8("性别："));
//		ptrNation = CreateLabel();
//		ptrNation->setText(CodeHelp::String_To_UTF8("民族："));
//		ptrIDType = CreateLabel();
//		ptrIDType->setText(CodeHelp::String_To_UTF8("证件类型："));
//		ptrIDNum = CreateLabel();
//		ptrIDNum->setText(CodeHelp::String_To_UTF8("证件号："));
//		ptrAddre = CreateLabel();
//		ptrAddre->setText(CodeHelp::String_To_UTF8("户籍地址："));
//
//		LabelControl* ptrOwnerInfo = CreateLabel();
//		ptrOwnerInfo->setText(CodeHelp::String_To_UTF8("住户信息"));
//		grid->addControl(ptrOwnerInfo);
//		ptrOwnerInfo->setBackColor(osg::Vec4f(1.0f, 1.0f, 0.0f, 0.5f));
//
//		grid->setControl(0, 12, ptrOwnerName);
//		grid->setControl(0, 13, ptrSex);
//		grid->setControl(0, 14, ptrNation);
//		grid->setControl(0, 15, ptrIDType);
//		grid->setControl(0, 16, ptrIDNum);
//		grid->setControl(0, 17, ptrAddre);
//
//
//		ptrptrOwnerNameCon = CreateLabel();
//		ptrptrOwnerNameCon->setText(CodeHelp::String_To_UTF8("贺道全"));
//		ptrSexCon = CreateLabel();
//		ptrSexCon->setText(CodeHelp::String_To_UTF8("男"));
//		LabelControl*  ptrNationCon = CreateLabel();
//		ptrNationCon->setText(CodeHelp::String_To_UTF8("汉"));
//		LabelControl*  ptrIDTypeCon = CreateLabel();
//		ptrIDTypeCon->setText(CodeHelp::String_To_UTF8("身份证"));
//		ptrIDNumCon = CreateLabel();
//		ptrIDNumCon->setText("420501198008082856");
//		ptrAddreCon = CreateLabel();
//		ptrAddreCon->setText(CodeHelp::String_To_UTF8("宜昌市常刘路103号康居园37栋"));
//
//		grid->setControl(5, 12, ptrptrOwnerNameCon);
//		grid->setControl(5, 13, ptrSexCon);
//		grid->setControl(5, 14, ptrNationCon);
//		grid->setControl(5, 15, ptrIDTypeCon);
//		grid->setControl(5, 16, ptrIDNumCon);
//		grid->setControl(5, 17, ptrAddreCon);
//
//		ptrControlCanvas->addControl(/*bottom*/grid);
//	}
//
//	LabelControl* CreateLabel()
//	{
//		LabelControl* ptrLabel = new LabelControl();
//
//		osgText::Font* font = osgText::readFontFile("../resource/Fonts/simhei.ttf");
//		ptrLabel->setFont(font);
//		ptrLabel->setFontSize(15.0);
//		ptrLabel->setEncoding(osgText::String::ENCODING_UTF8);
//		ptrLabel->setText(CodeHelp::String_To_UTF8("栋数:"));
//		ptrLabel->setMargin(0);
//		//ptrLabel->setForeColor(osg::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
//
//		return ptrLabel;
//	}
//
//	void Hight(osg::StateSet* ptrStateSet)
//	{
//		//关闭灯光
//		ptrStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
//		//打开混合融合模式
//		ptrStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
//		ptrStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
//		ptrStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//
//		osg::BlendColor* bc = new osg::BlendColor();
//		osg::BlendFunc*bf = new osg::BlendFunc();
//		ptrStateSet->setAttributeAndModes(bf, osg::StateAttribute::ON);
//		ptrStateSet->setAttributeAndModes(bc, osg::StateAttribute::ON);
//		bf->setSource(osg::BlendFunc::ONE_MINUS_CONSTANT_COLOR);
//		bf->setDestination(osg::BlendFunc::CONSTANT_ALPHA);
//		bc->setConstantColor(osg::Vec4(1.0f, 1.0f, 0.0f, 0.8));
//	}
//
//	void HightFloor(osg::StateSet* ptrStateSet)
//	{
//		//关闭灯光
//		ptrStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
//		//打开混合融合模式
//		ptrStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
//		ptrStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
//		ptrStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//
//		osg::BlendColor* bc = new osg::BlendColor();
//		osg::BlendFunc*bf = new osg::BlendFunc();
//		ptrStateSet->setAttributeAndModes(bf, osg::StateAttribute::ON);
//		ptrStateSet->setAttributeAndModes(bc, osg::StateAttribute::ON);
//		bf->setSource(osg::BlendFunc::ONE_MINUS_CONSTANT_COLOR);
//		bf->setDestination(osg::BlendFunc::CONSTANT_ALPHA);
//		bc->setConstantColor(osg::Vec4(1.0f, 0.0f, 1.0f, 0.8));
//	}
//
//	void Clear(osg::StateSet* ptrStateSet)
//	{
//		//关闭灯光
//		ptrStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
//		//打开混合融合模式
//		ptrStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
//		ptrStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
//		ptrStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//
//		osg::BlendColor* bc = new osg::BlendColor();
//		osg::BlendFunc*bf = new osg::BlendFunc();
//		ptrStateSet->setAttributeAndModes(bf, osg::StateAttribute::ON);
//		ptrStateSet->setAttributeAndModes(bc, osg::StateAttribute::ON);
//		bf->setSource(osg::BlendFunc::CONSTANT_ALPHA);
//		bf->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
//		bc->setConstantColor(osg::Vec4(1.0f, 1.0f, 0.0f, 0.0));
//	}
//
//	void HighLightAll()
//	{
//		std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it = g_Model1.begin();
//		for (; it != g_Model1.end(); it++)
//		{
//			Hight(it->second->getOrCreateStateSet());
//		}
//
//		std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it2 = g_Model2.begin();
//		for (; it2 != g_Model2.end(); it2++)
//		{
//			Hight(it2->second->getOrCreateStateSet());
//		}
//
//		std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it3 = g_Model3.begin();
//		for (; it3 != g_Model3.end(); it3++)
//		{
//			Hight(it3->second->getOrCreateStateSet());
//		}
//
//		std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it4 = g_Model4.begin();
//		for (; it4 != g_Model4.end(); it4++)
//		{
//			Hight(it4->second->getOrCreateStateSet());
//		}
//
//		std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it5 = g_Model5.begin();
//		for (; it5 != g_Model5.end(); it5++)
//		{
//			Hight(it5->second->getOrCreateStateSet());
//		}
//	}
//
//	void HighLightFloor(int nIndex)
//	{
//		if (nIndex == 1)
//		{
//			std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it = g_Model1.begin();
//			for (; it != g_Model1.end(); it++)
//			{
//				HightFloor(it->second->getOrCreateStateSet());
//			}
//		}
//		else if (nIndex == 2)
//		{
//			std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it2 = g_Model2.begin();
//			for (; it2 != g_Model2.end(); it2++)
//			{
//				HightFloor(it2->second->getOrCreateStateSet());
//			}
//		}
//		else if (nIndex == 3)
//		{
//			std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it3 = g_Model3.begin();
//			for (; it3 != g_Model3.end(); it3++)
//			{
//				HightFloor(it3->second->getOrCreateStateSet());
//			}
//		}
//		else if (nIndex == 4)
//		{
//			std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it4 = g_Model4.begin();
//			for (; it4 != g_Model4.end(); it4++)
//			{
//				HightFloor(it4->second->getOrCreateStateSet());
//			}
//		}
//		else if (nIndex == 5)
//		{
//			std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it5 = g_Model5.begin();
//			for (; it5 != g_Model5.end(); it5++)
//			{
//				HightFloor(it5->second->getOrCreateStateSet());
//			}
//		}
//	}
//
//	int FindFloor(std::string strName)
//	{
//		int nIndex = atoi(strName.substr(strName.find("_") + 1).c_str());
//
//		if (nIndex == 90 || nIndex == 89 || (nIndex >= 67 && nIndex <= 80))
//		{
//			ptrptrOwnerNameCon->setText(CodeHelp::String_To_UTF8("贺道全"));
//			ptrSexCon->setText(CodeHelp::String_To_UTF8("男"));
//			ptrIDNumCon->setText("420501198008082856");
//			ptrAddreCon->setText(CodeHelp::String_To_UTF8("北京市望京街道与埠通西大街路口2栋12单元1号"));
//
//			return 1;
//		}
//		else if (nIndex == 85 || nIndex == 88 || (nIndex >= 51 && nIndex <= 64))
//		{
//			ptrptrOwnerNameCon->setText(CodeHelp::String_To_UTF8("徐雷"));
//			ptrSexCon->setText(CodeHelp::String_To_UTF8("男"));
//			ptrIDNumCon->setText("420701198001282856");
//			ptrAddreCon->setText(CodeHelp::String_To_UTF8("上海市闵行区萃庄水清南路江南苑3栋14单元11号"));
//			return 2;
//		}
//		else if (nIndex == 84 || nIndex == 87 || (nIndex >= 35 && nIndex <= 48))
//		{
//			ptrptrOwnerNameCon->setText(CodeHelp::String_To_UTF8("肖大宝"));
//			ptrSexCon->setText(CodeHelp::String_To_UTF8("男"));
//			ptrIDNumCon->setText("42070119900102856");
//			ptrAddreCon->setText(CodeHelp::String_To_UTF8("武汉市卡梅尔小镇社区梁山头社区3栋2单元213号"));
//			return 3;
//		}
//		else if (nIndex == 83 || nIndex == 86 || (nIndex >= 19 && nIndex <= 32))
//		{
//			ptrptrOwnerNameCon->setText(CodeHelp::String_To_UTF8("李新园"));
//			ptrSexCon->setText(CodeHelp::String_To_UTF8("女"));
//			ptrIDNumCon->setText("42070119870518856");
//			ptrAddreCon->setText(CodeHelp::String_To_UTF8("武汉市东湖高新区关南园路理想城1栋1单元520号"));
//			return 4;
//		}
//		else if (nIndex == 82 || nIndex == 81 || (nIndex >= 3 && nIndex <= 16))
//		{
//			ptrptrOwnerNameCon->setText(CodeHelp::String_To_UTF8("朱子正"));
//			ptrSexCon->setText(CodeHelp::String_To_UTF8("女"));
//			ptrIDNumCon->setText("42070119861225856");
//			ptrAddreCon->setText(CodeHelp::String_To_UTF8("武汉市东湖高新区关山大道当国际花园5栋2单元"));
//			return 5;
//		}
//	}
//
//	void ClearAll()
//	{
//		std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it = g_Model1.begin();
//		for (; it != g_Model1.end(); it++)
//		{
//			Clear(it->second->getOrCreateStateSet());
//		}
//
//		std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it2 = g_Model2.begin();
//		for (; it2 != g_Model2.end(); it2++)
//		{
//			Clear(it2->second->getOrCreateStateSet());
//		}
//
//		std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it3 = g_Model3.begin();
//		for (; it3 != g_Model3.end(); it3++)
//		{
//			Clear(it3->second->getOrCreateStateSet());
//		}
//
//		std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it4 = g_Model4.begin();
//		for (; it4 != g_Model4.end(); it4++)
//		{
//			Clear(it4->second->getOrCreateStateSet());
//		}
//
//		std::map < std::string, osg::ref_ptr<osg::Node> >::iterator it5 = g_Model5.begin();
//		for (; it5 != g_Model5.end(); it5++)
//		{
//			Clear(it5->second->getOrCreateStateSet());
//		}
//	}
//
//	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
//	{
//		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
//
//		if (ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && ea.getEventType() == osgGA::GUIEventAdapter::PUSH)
//		{
//			osgUtil::LineSegmentIntersector::Intersections intersections;
//			if (!view->computeIntersections(ea, intersections))
//				return false;
//
//			if (m_ptrPreNode && m_ptrPreState)
//				m_ptrPreNode->setStateSet(m_ptrPreState);
//
//			for (osgUtil::LineSegmentIntersector::Intersections::iterator it = intersections.begin(); it != intersections.end(); ++it)
//			{
//				osg::NodePath nodePath = it->nodePath;
//
//				for (int i = 0; i < nodePath.size(); i++)
//				{
//					osg::ref_ptr<osg::Node> ptrNode = nodePath[i];
//					if (ptrNode == NULL)
//						continue;
//
//					if (ptrNode->getName().find("FW") != std::string::npos)
//					{
//						m_ptrPreNode = ptrNode;
//
//						printf((ptrNode->getName() + "\n").c_str());
//
//						osg::StateSet* state = ptrNode->getOrCreateStateSet();
//						if (!state)
//							continue;
//
//						//记录原来的状态
//						m_ptrPreState = dynamic_cast<osg::StateSet*> (state->clone(osg::CopyOp::DEEP_COPY_ALL));
//
//						//关闭灯光
//						state->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
//						//打开混合融合模式
//						state->setMode(GL_BLEND, osg::StateAttribute::ON);
//						state->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
//						state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//
//						osg::BlendColor* bc = new osg::BlendColor();
//						osg::BlendFunc*bf = new osg::BlendFunc();
//						state->setAttributeAndModes(bf, osg::StateAttribute::ON);
//						state->setAttributeAndModes(bc, osg::StateAttribute::ON);
//						bf->setSource(osg::BlendFunc::CONSTANT_COLOR);
//						bf->setDestination(osg::BlendFunc::ZERO);
//						bc->setConstantColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0));
//
//						//osg::Vec3d vecPoint = it->getWorldIntersectPoint();
//						//osgEarth::GeoPoint mapPoint;
//						//mapPoint.fromWorld(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), vecPoint);
//
//						//if (NULL == m_ptrBill)
//						//{
//						//	m_ptrBill = new GLOBE_NAME::BillBoardElement(mapPoint.x(), mapPoint.y(), mapPoint.z(), "", "./house.png");
//						//	osg::ref_ptr<osg::Node> ptrNode = m_ptrBill->Handle()->CastTo<osg::ref_ptr<osg::Node> >();
//						//	GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->addChild(ptrNode);
//						//}
//
//						//m_ptrBill->setPosition(mapPoint.x(), mapPoint.y(), mapPoint.z());
//
//						break;
//					}
//				}
//				break;
//			}
//		}
//		else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
//		{
//			osgUtil::LineSegmentIntersector::Intersections intersections;
//			if (!view->computeIntersections(ea, intersections))
//				return false;
//
//			if (m_ptrPreNode && m_ptrPreState)
//				m_ptrPreNode->setStateSet(m_ptrPreState);
//
//			ClearAll();
//			grid->setVisible(false);
//			for (osgUtil::LineSegmentIntersector::Intersections::iterator it = intersections.begin(); it != intersections.end(); ++it)
//			{
//				osg::NodePath nodePath = it->nodePath;
//
//				for (int i = 0; i < nodePath.size(); i++)
//				{
//					osg::ref_ptr<osg::Node> ptrNode = nodePath[i];
//					if (ptrNode == NULL)
//						continue;
//
//					if (ptrNode->getName().find("FW") != std::string::npos)
//					{
//						m_ptrPreNode = ptrNode;
//
//						HighLightAll();
//
//						int nIndex  = FindFloor(ptrNode->getName());
//						HighLightFloor(nIndex);
//						grid->setVisible(true);
//
//						osg::StateSet* state = ptrNode->getOrCreateStateSet();
//						if (!state)
//							continue;
//
//						//记录原来的状态
//						m_ptrPreState = dynamic_cast<osg::StateSet*> (state->clone(osg::CopyOp::DEEP_COPY_ALL));
//
//						//关闭灯光
//						state->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
//						//打开混合融合模式
//						state->setMode(GL_BLEND, osg::StateAttribute::ON);
//						state->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
//						state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//
//						osg::BlendColor* bc = new osg::BlendColor();
//						osg::BlendFunc*bf = new osg::BlendFunc();
//						state->setAttributeAndModes(bf, osg::StateAttribute::ON);
//						state->setAttributeAndModes(bc, osg::StateAttribute::ON);
//						bf->setSource(osg::BlendFunc::ONE_MINUS_CONSTANT_COLOR);
//						bf->setDestination(osg::BlendFunc::CONSTANT_ALPHA);
//						bc->setConstantColor(osg::Vec4(0.0f, 1.0f, 1.0f, 0.8));
//
//						break;
//					}
//				}
//				break;
//			}
//		}
//		return false;
//	}
//
//	GLOBE_NAME::BillBoardElementPtr m_ptrBill;
//	osg::ref_ptr<osg::Node> m_ptrPreNode;
//	osg::ref_ptr<osg::StateSet> m_ptrPreState;
//
//	LabelControl*  ptrptrOwnerNameCon ;
//	LabelControl*  ptrSexCon;
//	LabelControl*  ptrIDNumCon;
//	LabelControl*  ptrAddreCon;
//	Grid* grid;
//	VBox* bottom;
//};
/*临时代码*/


GLOBE_NS

ModelElement::ModelElement(std::string strUrl, double x, double y, double z)
{
 	//临时代码
 	/*static bool bInit = false;
	if (!bInit)
 	{	
 		bInit = true;
 		OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(new PickModelHandler());
 	}*/
 	//临时代码

	osg::ref_ptr<osg::Node> ptrModel = osgDB::readNodeFile(strUrl);
	m_modelUrl = strUrl;
 	//临时代码
 	/*std::string strName = osgDB::getStrippedName(strUrl);
 	int nIndex = atoi(strName.substr(strName.find("_")+1).c_str());
 
 	if (nIndex == 90 || nIndex == 89 || (nIndex>=67 && nIndex<=80))
 		g_Model1[strName] = ptrModel;
 	else if(nIndex == 85 || nIndex == 88 || (nIndex >= 51 && nIndex <= 64))
 		g_Model2[strName] = ptrModel;
 	else if (nIndex == 84 || nIndex == 87 || (nIndex >= 35 && nIndex <= 48))
 		g_Model3[strName] = ptrModel;
 	else if (nIndex == 83 || nIndex == 86 || (nIndex >= 19 && nIndex <= 32))
 		g_Model4[strName] = ptrModel;
 	else if (nIndex == 82 || nIndex == 81 || (nIndex >= 3 && nIndex <= 16))
 		g_Model5[strName] = ptrModel;
 
 	ptrModel->setName(strName);*/
 	//临时代码

	if (NULL == ptrModel)
		return;

	m_ptrModelNode = new ReferenceImp<osg::Node>(ptrModel);

	osg::Vec3d vecPos(x,y,z);
	osgEarth::GeoPoint mapPoint(OsgCoreSingleton::Instance()->MapNode()->getMapSRS(),x,y,z);

	//如果不是经纬度，假设是wgs84的世界坐标，尝试转换
	if (x < -180.0 || x>180 || y < -90 || y>90)
		mapPoint.fromWorld(OsgCoreSingleton::Instance()->MapNode()->getMapSRS(),vecPos);

	////如果转换的坐标还是不对直接返回
	//if (mapPoint.x() < -180.0 || mapPoint.x() > 180 || mapPoint.y() < -90 || mapPoint.y() > 90)
	//	return;

	m_dLongitude = mapPoint.x();
	m_dLatitude = mapPoint.y();
	m_dHeigh = mapPoint.z();

 	/*临时代码*/
  	osg::StateSet* state = ptrModel->getOrCreateStateSet();
	state->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
  	//记录原来的状态
  //	osg::ref_ptr<osg::StateSet> stateTemp = dynamic_cast<osg::StateSet*> (state->clone(osg::CopyOp::DEEP_COPY_ALL));
  	//关闭灯光
	state->setMode(GL_LIGHTING, osg::StateAttribute::OFF| osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);// | osg::StateAttribute::PROTECTED);
  	//打开混合融合模式
  	state->setMode(GL_BLEND, osg::StateAttribute::ON);
  	state->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
  	state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	state->setRenderBinDetails(12, "RenderBin");

  	//osg::BlendColor* bc = new osg::BlendColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0));
  	//osg::BlendFunc*bf = new osg::BlendFunc();
  	//state->setAttributeAndModes(bf, osg::StateAttribute::ON);
  	//state->setAttributeAndModes(bc, osg::StateAttribute::ON);
  	//bf->setSource(/*osg::BlendFunc::CONSTANT_COLOR*/osg::BlendFunc::CONSTANT_ALPHA);
  	//bf->setDestination(/*osg::BlendFunc::ZERO*/osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
  	//bc->setConstantColor(osg::Vec4(1.0f, 1.0f, 0.0f, 0.0));
 	/*临时代码*/




	osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = new osgEarth::GeoTransform();
	m_ptrElementHandle = new ReferenceImp<osgEarth::GeoTransform>(ptrLocator);

	osg::ref_ptr<GeoGlobe::MatrixTransformVisible> ptrMat = new GeoGlobe::MatrixTransformVisible();
	m_ptrMatrix = new ReferenceImp<GeoGlobe::MatrixTransformVisible>(ptrMat);

	ptrMat->addChild(ptrModel);

	ptrLocator->addChild(ptrMat);
	ptrLocator->setPosition(mapPoint);

	m_ElementType = eModelElement;
	m_dbScale = 1;
	m_dbAngle = 0;
	m_rotaAxle = KERNEL_NAME::GsRawPoint3D(0, 0, 1);
}

ModelElement::~ModelElement()
{

}

bool ModelElement::Visible()
{
	return m_bVisible;
}

void ModelElement::setTranslate(double x, double y, double z)
{
	osg::Vec3d inverse = osg::Vec3d(x, y, z);
	m_ptrMatrix->CastTo<osg::ref_ptr<osg::MatrixTransform> >()->setMatrix(osg::Matrix::translate(-inverse));
}

void ModelElement::Visible(bool bVisible)
{
	m_bVisible = bVisible;
	m_ptrMatrix->CastTo<osg::ref_ptr<GeoGlobe::MatrixTransformVisible>>()->setVisible(bVisible);
}

GsRawPoint3D ModelElement::getPosition()
{
	return GsRawPoint3D(m_dLongitude, m_dLatitude, m_dHeigh);
}

void ModelElement::setPosition(double x, double y, double z)
{
	osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >();

	if (NULL == ptrLocator)
		return;

	osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(),osg::Vec3d(x, y, z));
	ptrLocator->setPosition(point);

	m_dLongitude = x;
	m_dLatitude = y;
	m_dHeigh = z;

	OnPositionChange(KERNEL_NAME::GsRawPoint3D(x, y, z));
}

void ModelElement::getPosition(double & x, double & y, double & z)
{
	x = m_dLongitude;
	y = m_dLatitude;
	z = m_dHeigh;
}

std::string ModelElement::getModelUrl()
{
	return m_modelUrl;
}

void ModelElement::Rotate(GsRawPoint3D gsAxle, double dbAngle)
{
	if (m_dbAngle == dbAngle)
		return;
	osg::ref_ptr<osg::MatrixTransform> ptrMat = m_ptrMatrix->CastTo<osg::ref_ptr<osg::MatrixTransform>>();
	if (!ptrMat)
		return;
	m_dbAngle = dbAngle;
	m_rotaAxle = gsAxle;
	// 先缩放在旋转
	osg::Matrixd matrix = osg::Matrixd::scale(osg::Vec3d(m_dbScale, m_dbScale, m_dbScale));
	matrix.postMult(osg::Matrixd::rotate(osg::DegreesToRadians(m_dbAngle), osg::Vec3d(m_rotaAxle.X, m_rotaAxle.Y, m_rotaAxle.Z)));
	ptrMat->setMatrix(matrix);
	

}

double ModelElement::getdbAngle()
{
	return m_dbAngle;
}

void ModelElement::Scale(double dbScale)
{
	if (m_dbScale == dbScale)
		return;
	osg::ref_ptr<osg::MatrixTransform> ptrMat = m_ptrMatrix->CastTo<osg::ref_ptr<osg::MatrixTransform> >();
	if (!ptrMat)
		return;
	m_dbScale = dbScale;
	osg::Matrixd matrix = osg::Matrixd::scale(osg::Vec3d(m_dbScale, m_dbScale, m_dbScale));
	matrix.postMult(osg::Matrixd::rotate(osg::DegreesToRadians(m_dbAngle), osg::Vec3d(m_rotaAxle.X, m_rotaAxle.Y, m_rotaAxle.Z)));
	ptrMat->setMatrix(matrix);
	//ptrMat->setMatrix(osg::Matrixd::scale(osg::Vec3d(dbScale, dbScale, dbScale)) *ptrMat->getMatrix());
}

double ModelElement::getdbScale()
{
	return m_dbScale;
}

GLOBE_ENDNS
