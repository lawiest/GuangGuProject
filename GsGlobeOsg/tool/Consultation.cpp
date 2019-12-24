#include "Consultation.h"
#include <process.h> \
//#include "osgEarthQt\viewerwidget"
#include "osgEarth\XmlUtils"
#include <osg/ImageStream>
#include <tool/CodeTransformHelp.h>
 

#pragma region CslStep
CslStep::CslStep()
{
	m_bVisible = true;
}
CslStep::CslStep(string sName)
{
	m_sName = sName;
	m_bVisible = true;
}
CslStep::~CslStep()
{
}
string CslStep::Name()
{
	return m_sName;
}
void CslStep::Visible(bool bVisible)
{
	m_bVisible = bVisible;
}
bool CslStep::Visible()
{
	return m_bVisible;
}
#pragma endregion

#pragma region CslCommonStep
CslCommonStep::CslCommonStep(unsigned short eStepType, string sName, MapNode* pMapNode, osg::Group *pRoot):CslStep(sName)
{
	m_eType = eStepType;
	m_ptrNode = 0;
	m_pRoot = pRoot;
	m_pMapNode = pMapNode;
	m_sStepClass = "commonstep";
	 m_nX = -1;
	 m_nY = -1;
}
CslCommonStep::~CslCommonStep()
{
	ClearState();

	OnAction.Clear();
	OnDestructor(st_Param(m_eType, m_vtData));
	OnDestructor.Clear();
}
bool CslCommonStep::Action()
{
	if (m_ptrNode && m_pMapNode)
		m_pMapNode->addChild(m_ptrNode);
	if (m_ptrOverlay && m_pMapNode)
		m_pMapNode->addChild(m_ptrOverlay);
		/*mRoot->addChild(overlay);*/
	if (m_pRoot && m_ptrImageNode.get())
		m_pRoot->addChild(m_ptrImageNode);
	OnAction(st_Param(m_eType, m_vtData));
	return true;
}
void CslCommonStep::SetData(string sKey, string sValue)
{
	m_vtData.push_back(st_StepData(sKey, sValue));
}
void CslCommonStep::Save(void *pParentNode)
{
	XmlElement *pEle = static_cast<XmlElement *>(pParentNode);
	if (!pEle)
		return;
	
	pEle->addSubElement("stepclass", "");
	XmlElement *pSubEle = pEle->getSubElement("stepclass");
	pSubEle->addSubElement(m_sStepClass, m_sName);
	
	pEle->addSubElement("steptype", Stringify()<<m_eType);
	
	pEle->addSubElement("visible",m_bVisible?"true":"false");
	
	pEle->addSubElement("keyvalue", "");
	pSubEle = pEle->getSubElement("keyvalue");
	for (int i=0; i<m_vtData.size(); i++)
	{
		pSubEle->addSubElement(m_vtData[i].sKey, m_vtData[i].sValue);
	}

	if (m_pMapNode && m_ptrNode)
	{
		pEle->addSubElement("billboard", m_strContext);
		pSubEle = pEle->getSubElement("billboard");
		string sText = Stringify()<<m_Point.x()<<","<<m_Point.y()<<","<<m_Point.z();
		pSubEle->addSubElement("point", sText);
	}
	if (m_pMapNode && m_ptrOverlay)
	{
		pEle->addSubElement("overlay", m_sImageFile);
		pSubEle = pEle->getSubElement("overlay");
		string sText = Stringify()<<m_Bounds.xMin()<<","<<m_Bounds.yMin()<<","<<m_Bounds.xMax()<<","<<m_Bounds.yMax();
		pSubEle->addSubElement("bounds", sText);
	}
	if (m_pRoot && m_ptrImageNode)
	{
		pEle->addSubElement("front", m_sImageFile);
		pSubEle = pEle->getSubElement("front");
		string sText = Stringify()<<m_nX<<","<<m_nY;
		pSubEle->addSubElement("bounds", sText);
	}
}
void CslCommonStep::Load(void *pParentNode)
{
	XmlElement *pChild = (XmlElement *)pParentNode;

	XmlElement *pEleVislble = pChild->getSubElement("visible");
	if (pEleVislble->getText() == "false")
		m_bVisible = false;

	XmlElement *pEleKeyValue = pChild->getSubElement("keyvalue");
	for (int j=0; j<pEleKeyValue->getChildren().size(); j++)
	{
		XmlElement *pEleValue = (XmlElement *) ( (pEleKeyValue->getChildren())[j] ).get();
		SetData(pEleValue->getName(), pEleValue->getText());
	}

	XmlElement *pEleBillBoard = pChild->getSubElement("billboard");
	if (pEleBillBoard)
	{
		string sContext = pEleBillBoard->getText();
		XmlElement *pEle= pEleBillBoard->getSubElement("point");
		std::vector<double> vtCoords;
		std::stringstream ss(pEle->getText());
		std::string sCoord;
		while (std::getline(ss, sCoord, ','))
		{
			vtCoords.push_back(atof(sCoord.c_str()));
		}
		CreateBillboard(m_pMapNode, sContext, GeoPoint(m_pMapNode->getMapSRS(), vtCoords[0], vtCoords[1], vtCoords[2]));
	}

	XmlElement *pEleOverlay = pChild->getSubElement("overlay");
	if (pEleOverlay)
	{
		string sFilePath = pEleOverlay->getText();
		XmlElement *pEle= pEleOverlay->getSubElement("bounds");
		std::vector<double> vtCoords;
		std::stringstream ss(pEle->getText());
		std::string sCoord;
		while (std::getline(ss, sCoord, ','))
		{
			vtCoords.push_back(atof(sCoord.c_str()));
		}
		OverlayImage(sFilePath.c_str(), Bounds(vtCoords[0], vtCoords[1], vtCoords[2],vtCoords[3]), m_pMapNode);
	}

	XmlElement *pEleFront = pChild->getSubElement("front");
	if (pEleFront)
	{
		string sFilePath = pEleFront->getText();
		XmlElement *pEle= pEleFront->getSubElement("bounds");
		std::vector<int> vtCoords;
		std::stringstream ss(pEle->getText());
		std::string sCoord;
		while (std::getline(ss, sCoord, ','))
		{
			vtCoords.push_back(atoi(sCoord.c_str()));
		}
		FrontImage(sFilePath.c_str(), vtCoords[0], vtCoords[1]);
	}
}
void CslCommonStep::CreateBillboard(MapNode* pMapNode, string &sContext, GeoPoint &PT)
{
	m_pMapNode = pMapNode;
	m_strContext = sContext;
	m_Point = PT;

	//GeoPoint geoPt(pMapNode->getMapSRS(), 106.62691176489632, 26.638605162369700, 544.93962166551501);
	osgEarth::Symbology::Style SymStyle;
	SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->size()=18.0f;
	SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=Color::White;
	SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color()=Color::Red;
	SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "../resource/Fonts/simhei.ttf";
	SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	string strContext = CodeHelp::String_To_UTF8(m_strContext);
	m_ptrNode = new LabelNode(m_Point, strContext, SymStyle);
	m_ptrNode->setDynamic( true );
	m_ptrNode->setCullingActive(true);
}
void CslCommonStep::OverlayImage(const char *strFilePath, Bounds &oRect, MapNode* pMapNode)
{
	m_pMapNode = pMapNode;
	osg::Image* image = osgDB::readImageFile(strFilePath);
	osg::ImageStream* is = dynamic_cast<osg::ImageStream*>(image);
	if (is)
		is->play();
	m_ptrOverlay = new ImageOverlay(m_pMapNode);
	m_ptrOverlay->setImage(image);
	m_ptrOverlay->setBounds(oRect);
	
	m_sImageFile = strFilePath;
	m_Bounds = oRect;
}
void CslCommonStep::FrontImage(const char *strFilePath, int x, int y)
{
	m_ptrImageNode = new GeoGlobe::ImageNode(strFilePath);
	m_nX = x;
	m_nY = y;
	//m_ptrImageNode->setViewport(x, y, WINOWS_WIDTH * 0.25, WINOWS_HEIGHT * 0.25);
	m_ptrImageNode->setViewport(x, y, 1 * 0.25, 1 * 0.25);

	m_sImageFile = strFilePath;
}
void CslCommonStep::ClearState()
{
	if (m_pMapNode && m_ptrNode)
		m_pMapNode->removeChild(m_ptrNode);
	if (m_pMapNode && m_ptrOverlay)
		m_pMapNode->removeChild(m_ptrOverlay);
	if (m_pRoot && m_ptrImageNode.get())
		m_pRoot->removeChild(m_ptrImageNode);
}
#pragma endregion

#pragma region CslUIStep
CslUIStep::CslUIStep(unsigned short eStepType, string sName, osgViewer::Viewer *pView/*QWidget *pWidget*/):CslCommonStep(eStepType, sName, 0,0)
{
	m_MsgBox.SetTargetViewer(pView);
	m_sStepClass = "uistep";
}
CslUIStep::~CslUIStep()
{
}
bool CslUIStep::Action()
{
	CslCommonStep::Action();
	m_MsgBox.PostMsg();
	return false;
}
void CslUIStep::Save(void *pParentNode)
{
	XmlElement *pEle = static_cast<XmlElement *>(pParentNode);
	if (!pEle)
		return;
	
	/*pEle->addSubElement("stepclass", "");
	XmlElement *pSubEle = pEle->getSubElement("stepclass");
	pSubEle->addSubElement("uistep", m_sName);

	pEle->addSubElement("steptype", Stringify()<<m_eType);
	pEle->addSubElement("visible",m_bVisible?"true":"false");

	pEle->addSubElement("keyvalue", "");
	pSubEle = pEle->getSubElement("keyvalue");
	for (int i=0; i<m_vtData.size(); i++)
	{
		pSubEle->addSubElement(m_vtData[i].sKey, m_vtData[i].sValue);
	}*/
	CslCommonStep::Save(pParentNode);
	pEle->addSubElement("mouseevent", "");
	XmlElement *pSubEle = pEle->getSubElement("mouseevent");
	string sKey;
	for (int i=0; i<m_MsgBox.m_vtVirtualMsg.size(); i++)
	{
		st_Message &oMsg = m_MsgBox.m_vtVirtualMsg[i];
		switch (oMsg.m_MessageType)
		{
		case st_Message::emMsgType_Press:
			{
				vector<string> vtPoints;
				sKey = "mousepress";
				char buff[50] = {};
				sprintf_s(buff, "%f", oMsg.m_vtPoints[0].x());
				vtPoints.push_back(string(buff));
				memset(buff, 0, 50);
				sprintf_s(buff, "%f", oMsg.m_vtPoints[0].y());
				vtPoints.push_back(string(buff));
				memset(buff, 0, 50);
				sprintf_s(buff, "%f", oMsg.m_vtPoints[0].z());
				vtPoints.push_back(string(buff));
				string sValue = Stringify()<<vtPoints[0]<<","<<vtPoints[1]<<","<<vtPoints[2];
				pSubEle->addSubElement(sKey, sValue);
			}
			break;
		case st_Message::emMsgType_Release:
			{
				vector<string> vtPoints;
				sKey = "mouserelease";
				char buff[50] = {};
				sprintf_s(buff, "%f", oMsg.m_vtPoints[0].x());
				vtPoints.push_back(string(buff));
				memset(buff, 0, 50);
				sprintf_s(buff, "%f", oMsg.m_vtPoints[0].y());
				vtPoints.push_back(string(buff));
				memset(buff, 0, 50);
				sprintf_s(buff, "%f", oMsg.m_vtPoints[0].z());
				vtPoints.push_back(string(buff));
				string sValue = Stringify()<<vtPoints[0]<<","<<vtPoints[1]<<","<<vtPoints[2];
				pSubEle->addSubElement(sKey, sValue);
			}
			break;
		case st_Message::emMsgType_Move:
			{
				vector<string> vtPoints;
				sKey = "mousemove";
				char buff[50] = {};
				sprintf_s(buff, "%f", oMsg.m_vtPoints[0].x());
				vtPoints.push_back(string(buff));
				memset(buff, 0, 50);
				sprintf_s(buff, "%f", oMsg.m_vtPoints[0].y());
				vtPoints.push_back(string(buff));
				memset(buff, 0, 50);
				sprintf_s(buff, "%f", oMsg.m_vtPoints[0].z());
				vtPoints.push_back(string(buff));
				memset(buff, 0, 50);
				sprintf_s(buff, "%f", oMsg.m_vtPoints[1].x());
				vtPoints.push_back(string(buff));
				memset(buff, 0, 50);
				sprintf_s(buff, "%f", oMsg.m_vtPoints[1].y());
				vtPoints.push_back(string(buff));
				memset(buff, 0, 50);
				sprintf_s(buff, "%f", oMsg.m_vtPoints[1].z());
				vtPoints.push_back(string(buff));
				string sValue = Stringify()<<vtPoints[0]<<","<<vtPoints[1]<<","<<vtPoints[2]<<","<<vtPoints[3]<<","<<vtPoints[4]<<","<<vtPoints[5];
				pSubEle->addSubElement(sKey, sValue);
			}
			break;
		}
	}
	/*for (int i=0; i<m_MsgBox.m_vtMsg.size(); i++)
	{
		QMouseEvent *pEvent = m_MsgBox.m_vtMsg[i];
		string sKey;
		switch (pEvent->type())
		{
		case 2:
			sKey = "mousepress";
			break;
		case 3:
			sKey = "mouserelease";
			break;
		case 5:
			sKey = "mousemove";
			break;
		}
		if (sKey.empty())
			continue;
		string sValue = Stringify()<<pEvent->pos().x()<<","<<pEvent->pos().y();
		pSubEle->addSubElement(sKey, sValue);
	}*/
	
}
void CslUIStep::Load(void *pParentNode)
{
	CslCommonStep::Load(pParentNode);
	XmlElement *pChild = (XmlElement *)pParentNode;
	XmlElement *pEleEvent = pChild->getSubElement("mouseevent");
	for (int j=0; j<pEleEvent->getChildren().size(); j++)
	{
		XmlElement *pMouseEvent = (XmlElement *) ( (pEleEvent->getChildren())[j] ).get();
		string sEventName = pMouseEvent->getName();
		string sEventValue = pMouseEvent->getText();

		std::vector<double> vtCoords;
		std::stringstream ss(sEventValue);
		std::string sCoord;
		while (std::getline(ss, sCoord, ','))
		{
			vtCoords.push_back(atof(sCoord.c_str()));
		}

		/*if (sEventName == "mousemove")
			m_MsgBox.m_vtMsg.push_back(new QMouseEvent(QEvent::MouseMove, QPoint(vtCoords[0], vtCoords[1]), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
		if (sEventName == "mousepress")
			m_MsgBox.m_vtMsg.push_back(new QMouseEvent(QEvent::MouseButtonPress, QPoint(vtCoords[0], vtCoords[1]), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
		if (sEventName == "mouserelease")
			m_MsgBox.m_vtMsg.push_back(new QMouseEvent(QEvent::MouseButtonRelease, QPoint(vtCoords[0], vtCoords[1]), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));*/
		if (sEventName == "mousemove")
			m_MsgBox.m_vtVirtualMsg.push_back(st_Message(osg::Vec3(vtCoords[0], vtCoords[1], vtCoords[2]), osg::Vec3(vtCoords[3], vtCoords[4], vtCoords[5]), st_Message::emMsgType_Move));
		if (sEventName == "mousepress")
			m_MsgBox.m_vtVirtualMsg.push_back(st_Message(osg::Vec3(vtCoords[0], vtCoords[1], vtCoords[2]), st_Message::emMsgType_Press));
		if (sEventName == "mouserelease")
			m_MsgBox.m_vtVirtualMsg.push_back(st_Message(osg::Vec3(vtCoords[0], vtCoords[1], vtCoords[2]), st_Message::emMsgType_Release));
		
	}
}
#pragma endregion

#pragma region CslCompositeStep
CslCompositeStep::CslCompositeStep(string sName):CslStep(sName)
{
}
CslCompositeStep::~CslCompositeStep()
{
	this->clear();
}
bool CslCompositeStep::Action()
{
	unsigned int nSize = this->size();
	for (int i=0; i<nSize; i++)
	{
		(*this)[i]->Action();
	}
	return true;
}
void CslCompositeStep::Save(void *pParentNode)
{
	unsigned int nSize = this->size();
	for (int i=0; i<nSize; i++)
	{
		(*this)[i]->Save(pParentNode);
	}
}
void CslCompositeStep::Load(void *pParentNode)
{
	
}
#pragma endregion

#pragma region ConsultationFlow
ConsultationFlow::ConsultationFlow()
{
	m_nIndex = 0;	
}
ConsultationFlow::ConsultationFlow(const char *strConfigFile)
{
	m_nIndex = 0;
}
ConsultationFlow::~ConsultationFlow(void)
{
}
//数据环境
string ConsultationFlow::DataEnv()
{
	return m_sDataEnv;
}
void ConsultationFlow::DataEnv(const char *strPath)
{
	m_sDataEnv = strPath;
}
//名字
string ConsultationFlow::Name()
{
	return m_sName;
}
void ConsultationFlow::Name(const char *strPath)
{
	m_sName = strPath;
}
//保存
void ConsultationFlow::Save(const char *strPath)
{
	std::ofstream out;
	out.open(strPath);
	osgEarth::XmlDocument xmlDoc;

	xmlDoc.addSubElement("consultationflow", m_sName);
	XmlElement *pEle = xmlDoc.getSubElement("consultationflow");
	pEle->addSubElement("dataenv", m_sDataEnv);
	for (int i=0; i<this->size(); i++)
	{
		pEle->addSubElement(Stringify()<<"step_"<<i, "");
		(*this)[i]->Save(pEle->getSubElement(Stringify()<<"step_"<<i));
	}

	xmlDoc.store(out);
}
void ConsultationFlow::Load(const char *strPath, osgViewer::Viewer *pView, MapNode* pMapNode, osg::Group *pRoot)
{
	const std::string sPath(strPath);
	XmlDocument *pxmlDoc = XmlDocument::load(sPath, 0L);
	if (!pxmlDoc)
		return;

	XmlElement *pEleDoc = pxmlDoc->getSubElement("Document");
	XmlElement *pEle = pEleDoc->getSubElement("consultationflow");
	if (!pEle)
		return;
	m_sName = pEle->getText();
	m_sDataEnv = pEle->getSubElement("dataenv")->getText();

	XmlNodeList listNodes = pEle->getChildren();
	string name;
	for (int i=2; i<listNodes.size(); i++)
	{
		XmlElement *pChild = dynamic_cast<XmlElement *>(listNodes[i].get());
		XmlElement *pEleClass = pChild->getSubElement("stepclass");
		XmlElement *pEle = (XmlElement *) ( (pEleClass->getChildren())[0] ).get();
		string sClass = pEle->getName();
		string sName = pEle->getText();

		XmlElement *pEleType = pChild->getSubElement("steptype");
		string sType = pEleType->getText();
		
		CslStep *pStep = 0;
		if (sClass == "commonstep")
		{
			pStep = new CslCommonStep(atoi(sType.c_str()), sName, pMapNode, pRoot);
		}
		else if (sClass == "uistep")
		{
			pStep = new CslUIStep(atoi(sType.c_str()), sName, pView);
		}
		else
			continue;
		pStep->Load(pChild);
		this->push_back(pStep);
	}
	
}
bool ConsultationFlow::ManualPlay(int nIndex)
{
	if (nIndex<0 || nIndex>=this->size())
		return false;

	OnClearAllState();
	if ((*this)[nIndex]->Visible())
		(*this)[nIndex]->Action();
	return true;
}
int ConsultationFlow::ManualPlay()
{
	while (!(*this)[m_nIndex]->Visible())
	{
		if (m_nIndex==size()-1)
			return m_nIndex;
		m_nIndex++;
	}

	int nReturn = m_nIndex;

	for (int i=0; i<size(); i++)
	{
		if (m_nIndex != i)
			((*this)[i])->ClearState();
	}
	/*if (m_nIndex == 0)
		((*this)[size()-1])->ClearState();
	if (m_nIndex > 0)
		((*this)[m_nIndex-1])->ClearState();*/

	ManualPlay(m_nIndex);
	
	if (m_nIndex >= this->size()-1)
		m_nIndex = 0;
	else
		m_nIndex++;

	return nReturn;
}

void ConsultationFlow::ClearAllState()
{
	for (int i=0; i<size(); i++)
	{
		((*this)[i])->ClearState();
	}
	OnClearAllState();
}
#pragma endregion


#pragma region UIMsgBox
UIMsgBox::UIMsgBox()
{
	//m_pWidget = 0;
}
UIMsgBox::~UIMsgBox()
{
	//for (int i=0; i<m_vtMsg.size(); i++)
	{
		//delete m_vtMsg[i];
	}
	//m_vtMsg.clear();
}
void UIMsgBox::SetTargetViewer(osgViewer::Viewer *pView)
{
//	osgQt::GraphicsWindowQt* pGW = (osgQt::GraphicsWindowQt*)pView->getCamera()->getGraphicsContext();
	//m_pWidget = pGW->getGLWidget();
	m_pView = pView;
}
//QWidget *UIMsgBox::GetTargetWidget()
//{
//	return m_pWidget;
//}
void UIMsgBox::AddMouseMoveMessage(int nStartX, int nStartY, int nEndX, int nEndY, bool bNeedInterpolation/* = true*/)
{
	osg::ref_ptr<osgUtil::SceneView> pView = new osgUtil::SceneView();
	pView->setCamera(m_pView->getCamera());
	osg::Vec3 vScreen1(float(nStartX), float(nStartY), 0.0f);
	osg::Vec3 vScreen2(float(nEndX), float(nEndY), 0.0f);
	osg::Vec3 vWorld1;
	osg::Vec3 vWorld2;
	pView->projectWindowIntoObject(vScreen1, vWorld1);
	pView->projectWindowIntoObject(vScreen2, vWorld2);
	m_vtVirtualMsg.push_back(st_Message(vWorld1, vWorld2, st_Message::emMsgType_Move));
	
	/*if (!bNeedInterpolation)
	{
		m_vtMsg.push_back(new QMouseEvent(QEvent::MouseMove, QPoint(nStartX, nStartY), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
		m_vtMsg.push_back(new QMouseEvent(QEvent::MouseMove, QPoint(nEndX, nEndY), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
		return;
	}

	int nSpacingX = 0;
	int nSpacingY = 0;
	if (nStartX < nEndX)
		nSpacingX = 3;
	else
		nSpacingX = -3;

	if (nStartY < nEndY)
		nSpacingY = 3;
	else
		nSpacingY = -3;

	int nX = nStartX;
	int nY = nStartY;
	m_vtMsg.push_back(new QMouseEvent(QEvent::MouseMove, QPoint(nX, nY), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
	for (int i = 0; i<5; i++)
	{
		if (nSpacingX > 0)
		{
			if (nX >= nEndX)
				nX = nEndX;
			else
				nX += nSpacingX;
		}
		else
		{
			if (nX <= nEndX)
				nX = nEndX;
			else
				nX += nSpacingX;
		}

		if (nSpacingY > 0)
		{
			if (nY >= nEndY)
				nY = nEndY;
			else
				nY += nSpacingY;
		}
		else
		{
			if (nY <= nEndY)
				nY = nEndY;
			else
				nY += nSpacingY;
		}

		m_vtMsg.push_back(new QMouseEvent(QEvent::MouseMove, QPoint(nX, nY), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
	}*/
	
}
void UIMsgBox::AddMouseClickMessage(int nX, int nY)
{
	/*QMouseEvent *pPressMsg = new QMouseEvent(QEvent::MouseButtonPress, QPoint(nX, nY), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QMouseEvent *pReleaseMsg = new QMouseEvent(QEvent::MouseButtonRelease, QPoint(nX, nY), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	m_vtMsg.push_back(pPressMsg);
	m_vtMsg.push_back(pReleaseMsg);*/

	osg::ref_ptr<osgUtil::SceneView> pView = new osgUtil::SceneView();
	pView->setCamera(m_pView->getCamera());
	osg::Vec3 vScreen(float(nX), float(nY), 0.0f);
	osg::Vec3 vWorld;
	pView->projectWindowIntoObject(vScreen, vWorld);

	m_vtVirtualMsg.push_back(st_Message(vWorld, st_Message::emMsgType_Press));
	m_vtVirtualMsg.push_back(st_Message(vWorld, st_Message::emMsgType_Release));
	
}
/*
unsigned int WINAPI ThreadPostMessage (void* pParam)
{
	UIMsgBox *pBox = (UIMsgBox *)pParam;
	if (!pBox->GetTargetWidget())
		return 0;
	pBox->m_vtMsg.clear();
	//insert
	
	osg::ref_ptr<osgUtil::SceneView> pView = new osgUtil::SceneView();
	pView->setCamera(pBox->m_pView->getCamera());
	for (size_t i=0; i<pBox->m_vtVirtualMsg.size(); i++)
	{
		st_Message &oMsg = pBox->m_vtVirtualMsg[i];
		if (oMsg.m_MessageType == st_Message::emMsgType_Press)
		{
			osg::Vec3 vScreen;
			pView->projectObjectIntoWindow(pBox->m_vtVirtualMsg[i].m_vtPoints[0], vScreen);
			QMouseEvent *pPressMsg = new QMouseEvent(QEvent::MouseButtonPress, QPoint(vScreen.x(), vScreen.y()), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
			pBox->m_vtMsg.push_back(pPressMsg);
		}
		if (oMsg.m_MessageType == st_Message::emMsgType_Release)
		{
			osg::Vec3 vScreen;
			pView->projectObjectIntoWindow(pBox->m_vtVirtualMsg[i].m_vtPoints[0], vScreen);
			QMouseEvent *pReleaseMsg = new QMouseEvent(QEvent::MouseButtonRelease, QPoint(vScreen.x(), vScreen.y()), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
			pBox->m_vtMsg.push_back(pReleaseMsg);
		}
		if (oMsg.m_MessageType == st_Message::emMsgType_Move)
		{
			osg::Vec3 vScreen1, vScreen2;
			pView->projectObjectIntoWindow(pBox->m_vtVirtualMsg[i].m_vtPoints[0], vScreen1);
			pView->projectObjectIntoWindow(pBox->m_vtVirtualMsg[i].m_vtPoints[1], vScreen2);
			pBox->m_vtMsg.push_back(new QMouseEvent(QEvent::MouseMove, QPoint(vScreen1.x(), vScreen1.y()), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
			pBox->m_vtMsg.push_back(new QMouseEvent(QEvent::MouseMove, QPoint(vScreen2.x(), vScreen2.y()), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
		}
	}

	//消息post后会被释放，这里需要拷贝一次
	for (int i=0; i<pBox->m_vtMsg.size(); i++)
	{
		QMouseEvent *pMsg = pBox->m_vtMsg[i];
		QMouseEvent *pPostMsg = new QMouseEvent(*pMsg);
		QApplication::postEvent(pBox->GetTargetWidget(), pPostMsg);
	}

	return 0;
}*/
void UIMsgBox::PostMsg()
{
	//unsigned nThreadID = -1;
	//_beginthreadex(0, 0, ThreadPostMessage, this, 0/*CREATE_SUSPENDED*/, &nThreadID);
}
#pragma endregion