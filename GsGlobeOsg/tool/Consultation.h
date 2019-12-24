/*
会商系统

1 Consultation关联一个工作空间，其中所有演示步骤都依赖同一工作空间数据。
*/

#pragma once

#include <vector>
#include <map>
#include "osg/ref_ptr"
#include "osg/Referenced"
//#include <osgEarthQt/ViewerWidget>
//#include "osgEarthAnnotation\PlaceNode"
#include "placename\PlaceNode.h"
#include "osgEarth\mapnode"
#include "osgEarthAnnotation\ImageOverlay"
#include "tool\delegate\event.h"

#include <osgEarthAnnotation/LabelNode>
#include "osgUtil\SceneView"
#include "Analysis\BirdsEyeElement.h"

using namespace osg;
using namespace std;

using namespace osgEarth::Annotation;

class ConsultationFlow;
class QMouseEvent;
class QWidget;

struct st_StepData
{
	st_StepData(string sk, string sv)
	{
		sKey = sk;
		sValue = sv;
	}
	string sKey;
	string sValue;
};

struct st_Param
{
	st_Param(unsigned short p1, vector<st_StepData> &p2)
	{
		eStepType = p1;
		vtParams = p2;
	}
	unsigned short eStepType;
	vector<st_StepData> vtParams;
};

struct st_Message
{
	enum emMsgType
	{
		emMsgType_Press,
		emMsgType_Release,
		emMsgType_Move
	};
	st_Message(osg::Vec3 vPoint, emMsgType eType)
	{
		m_MessageType = eType;
		m_vtPoints.push_back(vPoint);
	}
	st_Message(osg::Vec3 vPoint1, osg::Vec3 vPoint2, emMsgType eType)
	{
		m_MessageType = eType;
		m_vtPoints.push_back(vPoint1);
		m_vtPoints.push_back(vPoint2);
	}

	emMsgType m_MessageType;
	std::vector<osg::Vec3> m_vtPoints;
};

/*
UI消息盒子
*/
class UIMsgBox
{
public:
	UIMsgBox();
	~UIMsgBox();

	void SetTargetViewer(osgViewer::Viewer *pView);
	//QWidget *GetTargetWidget();
	void AddMouseMoveMessage(int nStartX, int nStartY, int nEndX, int nEndY, bool bNeedInterpolation = true);
	void AddMouseClickMessage(int nX, int nY);
	void PostMsg();
	//std::vector<QMouseEvent *> m_vtMsg;
	std::vector<st_Message> m_vtVirtualMsg;
	osgViewer::Viewer *m_pView;
protected:
	//QWidget *m_pWidget;
	
};

/*
流程中一个演示环节
*/
class CslStep : public Referenced
{
	friend class ConsultationFlow;
	friend class CslCompositeStep;
public:
	CslStep();
	CslStep(string sName);
	virtual ~CslStep();
	string Name();
	void Visible(bool bVisible);
	bool Visible();

protected:
	virtual bool Action() = 0;
	virtual void Save(void *pParentNode) = 0;
	virtual void Load(void *pParentNode) = 0;
	virtual void ClearState(){}
	string m_sName;
	bool m_bVisible;
};

/*
普通环节
*/
class CslCommonStep : public CslStep
{
public:
	CslCommonStep(unsigned short eStepType, string sName, MapNode* pMapNode=0, osg::Group *pRoot=0);
	virtual ~CslCommonStep();

	void SetData(string sKey, string sValue);
	
	GsDelegate<void (st_Param)> OnAction;
	GsDelegate<void (st_Param)> OnDestructor;

	void CreateBillboard(MapNode* pMapNode, string &sContext, GeoPoint &PT);
	void OverlayImage(const char *strFilePath, Bounds &oRect, MapNode* pMapNode);
	void FrontImage(const char *strFilePath, int x, int y);

protected:
	void Save(void *pParentNode);
	void Load(void *pParentNode);
	virtual bool Action();
	virtual void ClearState();

protected:
	vector<st_StepData> m_vtData;
	unsigned short m_eType;

	osgEarth::MapNode *m_pMapNode;
	osg::Group* m_pRoot;

	string m_strContext;
	GeoPoint m_Point;
	osg::ref_ptr<osgEarth::Annotation::LabelNode> m_ptrNode;
	osg::ref_ptr<GeoGlobe::ImageNode> m_ptrImageNode;

	osg::ref_ptr<ImageOverlay> m_ptrOverlay;
	Bounds m_Bounds;
	string m_sImageFile;

	string m_sStepClass;
	int m_nX, m_nY;
};

/*
UI关联环节
*/
class CslUIStep : public CslCommonStep
{
public:
	CslUIStep(unsigned short eStepType, string sName, osgViewer::Viewer *pView/*QWidget *pWidget*/);
	virtual ~CslUIStep();
	UIMsgBox m_MsgBox;
protected:
	virtual bool Action();
	void Save(void *pParentNode);
	void Load(void *pParentNode);
};

/*
组合环节
*/
class CslCompositeStep : 
	public CslStep, 
	public vector<osg::ref_ptr<CslStep> >
{
public:
	CslCompositeStep(string sName);
	virtual ~CslCompositeStep();

protected:
	virtual bool Action();
	void Save(void *pParentNode);
	void Load(void *pParentNode);
};

/*
一个会商演示流程
*/
class ConsultationFlow : public vector<osg::ref_ptr<CslStep> >
{
public:
	ConsultationFlow();
	ConsultationFlow(const char *strConfigFile);
	virtual ~ConsultationFlow(void);

	//基础数据环境
	string DataEnv();
	void DataEnv (const char *strPath);

	//名字
	string Name();
	void Name(const char *strPath);

	//保存
	void Save(const char *strPath);

	//加载
	void Load(const char *strPath, osgViewer::Viewer *pView, MapNode* pMapNode, osg::Group *pRoot);

	//手动播放
	bool ManualPlay(int nIndex);
	int ManualPlay();

	GsDelegate<void ()> OnClearAllState;

	//清除所有状态
	void ClearAllState();

protected:
	string m_sName;
	string m_sDataEnv;
	int m_nIndex;
};
