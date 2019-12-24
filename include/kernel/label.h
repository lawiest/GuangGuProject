#pragma once
//////////////////////////////////////////////////////////////////////////                                          
//!  \file label.h
//!  \brief GeoStar Kernel GIS内核
//!  \details 自动标注模块
//!  \author xiaochao
//!  \attention 
//!   Copyright (c) Wuda Geoinformatics Co.,Ltd. <br> 
//!   All Rights Reserved.
//!   版权所有(c) 武大吉奥信息技术有限公司 保留所有权利
//!  \version 1.1
//////////////////////////////////////////////////////////////////////////
#include "../utility/preconfig.h" 
#include<object.h>
#include<geodatabase.h>
#include<geometry.h>
#include<symbol.h>
#include "pointsymbol.h"

KERNEL_NS  

/// \brief 点标注九宫格位置
///        ４２５
///        １★０
///        ６３７
enum GsPointLabelPlace
{
	///\brief 标注位于符号右边
	///
	///         ★Label
	///        
	eRight,
	///\brief 标注位于符号左边
	///
	///       Label★
	///        
	eLeft,
	///\brief 标注位于符号顶部
	///        Label
	///         ★
	///
	eTop,
	///\brief 标注位于符号底部
	///
	///         ★
	///        Label
	eBottom,

	///\brief 标注位于符号左上
	///    Label
	///         ★
	///        
	eTopLeft,
	///\brief 标注位于符号右上
	///           Label
	///         ★
	///        
	eTopRight,
	///\brief 标注位于符号左下
	///           
	///         ★
	///    Label   
	eBottomLeft,
	///\brief 标注位于符号右下
	///           
	///         ★
	///           Label   
	eBottomRight,
};

/// \brief 点标注位置优先级别
enum GsPlaceOrderPriority
{
	/// \brief 放弃
	eAbandonPriority,
	/// \brief 高优先级
	eHighPriority,
	/// \brief 中等优先级
	eMiddlePriority,
	/// \brief 低先级
	eLowPriority,
};

/// \brief 线标注类型
enum GsLineLabelType
{
	/// \brief 无
	eUnKnown,
	/// \brief 水平标注
	eHorizon,
	/// \brief 垂直于线标注
	eVerticalLine,
	/// \brief 沿线标注
	eAlongLine,
	/// \brief 沿线标注，文字水平
	eAlongLineWithHorizon,
};

/// \brief 线标注起始点位置
enum GsLineLabelStartPos
{
	/// \brief 缺省位置 中点起算
	eDefaultPos,
	/// \brief 线起点位置
	eStartPos,
	/// \brief 线尾点位置
	eEndPos,
};

/// \brief 线与标注位置关系
enum GsLineLabelPos
{
	/// \brief 自动位置
	eAutoPlace,
	/// \brief 线上
	eTopLine,
	/// \brief 线下
	eBottomLine,
	/// \brief 线中
	eCenterLine,
};

/// \brief 标注背景框形状
enum GsLabelBackgroundFrame
{
	/// 无
	eNone,
	/// \brief 矩形
	eRectangle,
	/// \brief 圆角矩形
	eRoundRectangle,
	/// \brief 椭圆
	eEllipse,
};

/// \brief 标注对象
class GS_API GsLabel:public Utility::GsRefObject
{
protected:
	/// \brief 标注文字符号
	GsTextSymbolPtr m_ptrSymbol;
	/// \brief 标注点符号
	GsPointSymbolPtr m_ptrPointSymbol;
	/// \brief 标注ID
	int m_nID;
	/// \ brief 标注所属几何标识
	long long m_nOwnID;
	/// \brief 标注属性标识
	long long m_nOrder;
	/// \brief 标注优先级
	int m_nPriority;
	/// \brief 标注文本
	Utility::GsString m_strText;
	GsGeometryBlob m_GeometryBlob;
	GsRawPoint m_Point;
	/// \brief 几何值
	double m_dGeoValue;
	/// \brief 标注背景框
	GsLabelBackgroundFrame m_eBgFrame;
	/// \brief 标注背景符号
	GsFillSymbolPtr m_ptrBgSymbol;

	GsLabel();
public:
	
	virtual  ~GsLabel();
	/// \brief 是否和另外一个label堆叠
	virtual bool IsIntersects(GsLabel* rhs) const = 0;

	/// \brief 绘制标注
	virtual void Draw(GsDisplayTransformation* pDT,GsCanvas* pCanvas)  = 0;
	
	/// \brief 标准绘制的Geometry
	GsGeometryBlob* Geometry();
	
	/// \brief 标注的点
	void LabelPoint(double x,double y);

	/// \brief 标注的点
	GsRawPoint LabelPoint();

	/// \brief 重置标注为初始状态
	virtual void Reset();

	/// \brief 标注唯一标识
	int ID() const;
	/// \brief 标注唯一标识
	void ID(int n);

	/// \brief 获取标注的优先级
	int Priority()const;
	/// \brief 设置标注的优先级
	void Priority(int n);

	/// \brief 标注属性标识
	long long Order()const;
	/// \brief 标注属性标识
	void Order(long long n);

	/// \brief 标注所属几何标识
	long long OwnID()const;
	/// \brief 标注所属几何标识
	void OwnID(long long id);

	/// \brief 标注绘制的符号
	GsTextSymbol* Symbol();
	/// \brief 标注绘制的符号
	void Symbol(GsTextSymbol* sym);

	/// \brief 标注点符号
	GsPointSymbol* PointSymbol();
	/// \brief 标注点符号
	void PointSymbol(GsPointSymbol* sym);

	/// \brief 背景框形状
	void BackgroundFrame(GsLabelBackgroundFrame eFrame);
	/// \brief 背景框形状
	GsLabelBackgroundFrame BackgroundFrame();

	/// \brief 背景框符号
	void BackgroundSymbol(GsFillSymbol *pSymbol);
	/// \brief 背景框符号
	GsFillSymbol* BackgroundSymbol();

	/// \brief 标注的文本
	const char* Text()const;
	/// \brief 标注的文本
	void Text(const char* str);

	/// \brief 几何值（长、面积）
	double GeometryValue();
	/// \brief 几何值（长、面积）
	void GeometryValue(double dVal);
};
/// \brief GsLabelPtr
GS_SMARTER_PTR(GsLabel);
class GsTrackCancel;
/// \brief 自动标注避让容器抽象基类
class GS_API GsLabelContainer:public Utility::GsRefObject
{
protected:
	/// \brief 已经安排位置的标注
	Utility::GsList<GsLabelPtr> m_lstPlaced;
	/// \brief 未能安排位置的标注
	Utility::GsList<GsLabelPtr> m_lstUnPlaced;

	/// \brief 标注对象池
	Utility::GsQueue<GsLabelPtr> m_LabelPool;
	
	/// \brief 最大存储的标注
	int m_nMax;

	/// \brief 标注容器是否可用
	bool m_bEnabled;

	/// \brief 是否自动标注
	bool m_bAutoLabel;

	/// \brief 是否去除重复标注
	bool m_bRemoveRepeatLabel;

	/// \brief 障碍物管理对象
	void *m_pObstacleManager;

	int m_nBaseId;
public:
	GsLabelContainer();
	virtual ~GsLabelContainer();

	/// \brief 准备避让
	virtual void Begin();

	/// \brief 增加一个标注到容器
	virtual bool Add(GsLabel* label);

	/// \brief 结束避让
	virtual void End();

	/// \brief 绘制标注
	virtual void DrawLabel(GsDisplayTransformation* pDT,GsCanvas* pCanvas, GsTrackCancel* cancel = NULL);

	/// \brief 最大存储的标注
	int MaxLabel();
	/// \brief 最大存储的标注
	void MaxLabel(int n);

	/// \brief 创建或者复用一个Label对象。
	GsLabelPtr CreateLabel();

	/// \brief 是否去除重复标注
	void RemoveRepeatLabel(bool bRemove);
	/// \brief 是否去除重复标注
	bool RemoveRepeatLabel();

	/// \brief 障碍物管理类
	void* ObstacleMgr();

	// 检查冲突回调函数
	Utility::GsDelegate<bool(GsLabel*)> CheckConflictCallBack;

public:
	/// \brief 标注容器是否可用
	bool Enabled();
	/// \brief 标注容器是否可用
	void Enabled(bool b);

	/// \brief 标注是否自动避让
	bool AutoLabel();
	/// \brief 标注是否自动避让
	void AutoLabel(bool b);

protected:
	/// \brief 回收标注
	void Recovery(Utility::GsList<GsLabelPtr>& lst);
	void Recovery(GsLabel* label);

	/// \brief 准备避让
	virtual void OnBegin();

	/// \brief 增加一个标注到容器
	virtual bool OnAdd(GsLabel* label);

	/// \brief 结束必避让
	virtual void OnEnd();
};
/// \brief GsLabelContainerPtr
GS_SMARTER_PTR(GsLabelContainer);


class GS_API GsSimpleLabelContainer:public GsLabelContainer
{
protected:
	/// \brief 准备避让
	virtual void OnBegin();

	/// \brief 增加一个标注到容器
	virtual bool OnAdd(GsLabel* label);

	/// \brief 结束必避让
	virtual void OnEnd();

public:
	GsSimpleLabelContainer();
	virtual ~GsSimpleLabelContainer();
};
/// \brief GsSimpleLabelContainerPtr
GS_SMARTER_PTR(GsSimpleLabelContainer);

/// \brief 进阶版标注容器，采用更为复杂的避让规则
class GS_API GsAdvancedLabelContainer:public GsSimpleLabelContainer
{
protected:
	/// \brief 准备避让
	virtual void OnBegin();

	/// \brief 增加一个标注到容器
	virtual bool OnAdd(GsLabel* label);

	/// \brief 结束必避让
	virtual void OnEnd();
public:
	GsAdvancedLabelContainer();
	virtual ~GsAdvancedLabelContainer();

};
/// \brief GsAdvancedLabelContainerPtr
GS_SMARTER_PTR(GsAdvancedLabelContainer);

/// \brief 自动标注计算基类
class GS_API GsLabelProperty:public Utility::GsRefObject
{
protected:
	/// \brief 标注符号的Clone，用于计算标注范围
	GsTextSymbolPtr m_ptrSymbolClone;

	///\brief 公用的SE计算对象
	geostar::safe_ga m_ga;

	/// \brief 缓存一个Label的位置
	Utility::GsVector<Utility::GsRectF> m_vecLabelRects;

	/// \brief 当前DT
	GsDisplayTransformationPtr m_ptrDT;
	/// \brief 计算标注内容
	GsCanvasPtr			m_ptrTextCanvas;

	/// \brief 标注容器
	GsLabelContainerPtr m_ptrContainer;
	// 记录容器原始状态
	bool m_bOldRepeatLabelState;

	//合并标注参数
	/// \brief 待合并的几何对象
	Utility::GsVector<GsGeometryPtr> m_vecUnionGeo;
	/// \brief 合并标注文本
	Utility::GsString m_strUnionLabel;
	/// \brief 合并标注当前字段值
	Utility::GsString m_strUnionFieldVal;
	/// \brief 合并几何符号
	GsSymbolPtr m_ptrUnionSymbol;

	double m_dbSimpleTol;
	double m_dbUnionTol;
	double m_fBgSpace;

protected:
	/// \brief 标注绘制的符号
	GsTextSymbolPtr m_ptrSymbol;

	/// \brief 标注优先级
	int m_nLabelPriority;
	/// \brief 标注的字段名
	Utility::GsString m_strFieldName;
	/// \brief 标注字段索引
	int m_nFieldIndex;

	/// \brief 是否允许重复标注
	bool m_bRemoveRepeatLabel;

	/// \brief 标注间距（单位mm）
	float m_fLabelDistance;
	double m_dGeoLabelDistance;

	/// \brief 背景框类型
	GsLabelBackgroundFrame m_eBgFrame;
	/// \brief 背景框符号
	GsFillSymbolPtr m_ptrBgSymbol;

	/// \brief 标注最小要素大小(mm)
	float m_fLimitSize;
	float m_fGeoLimitSize;

	/// \brief 合并标注字段
	Utility::GsString m_strUnionField;
	/// \brief 合并标注字段索引
	int m_nUnionFieldIndex;

protected:
	GsLabelProperty();

public:
	
	virtual ~GsLabelProperty();
	
	/// \brief 准备开始标注计算
	virtual void Begin(GsDisplayTransformation* pDT, GsLabelContainer* pContainer);
	/// \brief 计算标注
	virtual bool CalculateLabel(GsFeature* pFea, GsSymbol* pSym);
	/// \brief 计算标注
	/// \param pGeo 几何对象
	/// \param strLabel	标注文本
	/// \param pSym	几何符号
	/// \param strUnionValue 合并标注字段值（如果为NULL则跟标注文本一致）
	virtual bool CalculateLabel(GsGeometry *pGeo, const char* strLabel, GsSymbol* pSym, const char* strUnionValue=0);
	/// \brief 结束标注计算
	virtual void End();

	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsLabelProperty> Clone();
	
public:
	/// \brief 标注优先级
	void LabelPriority(int nPriority);
	/// \brief 标注优先级
	int LabelPriority();

	/// \brief 要标注的字段名称（支持多字段标注的表达式）
	Utility::GsString LabelField();
	/// \brief 要标注的字段名称（支持多字段标注的表达式）
	void LabelField(const char* strField);
	
	/// \brief 标注绘制的符号
	GsTextSymbol* Symbol();
	/// \brief 标注绘制的符号
	void Symbol(GsTextSymbol* sym);

	/// \brief 是否去除重复标注
	void RemoveRepeatLabel(bool bRemove);
	/// \brief 是否去除重复标注
	bool RemoveRepeatLabel();

	/// \brief 标注间距（单位mm）
	void LabelDistance(float fDist);
	/// \brief 标注间距（单位mm）
	float LabelDistance();

	/// \brief 背景框形状
	void BackgroundFrame(GsLabelBackgroundFrame eFrame);
	/// \brief 背景框形状
	GsLabelBackgroundFrame BackgroundFrame();

	/// \brief 背景框符号
	void BackgroundSymbol(GsFillSymbol *pSymbol);
	/// \brief 背景框符号
	GsFillSymbol* BackgroundSymbol();

	/// \brief 要素大小限制（小于尺寸的几何不生成标注）
	void LimitSize(float fSize);
	/// \brief 要素大小限制（小于尺寸的几何不生成标注）
	float LimitSize();

	/// \brief 合并标注字段
	void UnionLabelField(const char* szUnionField);
	/// \brief 合并标注字段
	Utility::GsString UnionLabelField();

protected:
	/// \brief 计算标注
	/// \param pGeo 几何对象
	/// \param strLabel	标注文本
	/// \param pSym	几何符号
	virtual bool OnCalculateLabel(GsGeometry *pGeo, const char* strLabel, GsSymbol* pSym) = 0;

	/// \brief 过滤标注位置
	virtual bool FilterLabelPosition(GsLabel *pLabel);

	/// \brief 计算标注点的位置
	virtual GsRawPoint LabelPoint(GsGeometry* pGeo, int nFlag = geostar::prop_geo_label);

	/// \brief 计算背景框距文字边界距离
	virtual Utility::GsPTF CalBackgroundSpace(float dWidth, float dHeight);

	/// \brief 获取字段值
	Utility::GsString FieldText(GsFeature* pFea, const Utility::GsString& strFieldName, int nColumn);

	/// \brief 创建一个标注对象
	GsLabelPtr CreateLabel();

	/// \brief 计算合并几何标注
	void CalUnionGeoLabel();

	/// \brief 计算几何值（长度、面积）
	double CalGeometryValue(GsGeometry *pGeo);

	/// \brief 裁切几何
	GsGeometryPtr FilterGeometry(GsGeometry *pSrcGeo);

	/// \brief 合并几何
	GsGeometryPtr UnionGeometry(const Utility::GsVector<GsGeometryPtr>& vecGeo, double tol);

	/// \breif 转化几何数组对象
	geostar::gobjptr ConvertObjectArray(const Utility::GsVector<GsGeometryPtr>& vecGeo);
};

/// \brief GsLabelPropertyPtr
GS_SMARTER_PTR(GsLabelProperty);


/// \brief 点自动标注计算类
class GS_API GsPointLabelProperty:public GsLabelProperty
{
public:
	/// \brief 换行符参数
	struct GsLineBreakParam
	{
		GsLineBreakParam()
			: m_nMinWords(0), m_nMaxRows(0)
			, m_strLineBreak("")
			, m_bWordGroup(false)
		{
			m_ptrWordAnalyzer = new Utility::GsSimpleWordAnalyzer();
		}

		bool IsValid()
		{
			if ((m_nMaxRows <= 0 || m_nMinWords < 0) && m_strLineBreak.empty() && !m_bWordGroup)
				return false;
			return true;
		}
		/// \brief 每行允许的最小字数
		int m_nMinWords;
		/// \brief 允许的最大行数
		int m_nMaxRows;
		/// \brief 换行符
		Utility::GsString m_strLineBreak;
		/// \brief 是否分词组
		bool m_bWordGroup;
		/// \brief 分词器
		Utility::GsWordAnalyzerPtr m_ptrWordAnalyzer;
	};
protected:
	/// \brief 九宫格优先级
	Utility::GsVector<Utility::GsPair<GsPointLabelPlace,GsPlaceOrderPriority> > m_PlaceOrder;

	/// \brief 点与标注间距（单位mm）
	float m_fPointSpaceByLabel;
	/// \brief 是否换行
	bool m_bLineBreak;
	/// \brief 换行符参数
	GsLineBreakParam m_LineBreakParam;
	/// \brief 点标注与符号同时绘制
	bool m_bDrawLabelAndPoint;

	/// \brief 标注避让的最小比例尺范围（1：x）
	double m_dAvoidMinScale;
	/// \brief 标注避让的最大比例尺范围（1：x）
	double m_dAvoidMaxScale;

	/// \brief 避让数据列表
	UTILITY_NAME::GsVector<void*> m_vecAvoidData;
	UTILITY_NAME::GsStlMap<Utility::GsString, GsFeatureClassPtr> m_mapAvoidFeaClass;
	void* m_pObstacle;
public:
	using GsLabelProperty::CalculateLabel;
	GsPointLabelProperty();
	virtual ~GsPointLabelProperty();

	/// \brief 准备开始标注计算
	virtual void Begin(GsDisplayTransformation* pDT,GsLabelContainer* pContainer);
	
	/// \brief 结束标注计算
	virtual void End();

	/// \brief 克隆
	virtual GsLabelPropertyPtr Clone();

	/// \brief 存储避让数据
	/// \brief param strAvoidDataName避让数据名称
	/// \brief param pFeatureClass 数据集对象
	void StoreAvoidDataSource(const char* strAvoidDataName, GsFeatureClass* pFeatureClass);

public:
	/// \brief 获取标注位置的优先级
	GsPlaceOrderPriority PlaceOrder(GsPointLabelPlace e);
	/// \brief 设置标注位置的优先级
	void PlaceOrder(GsPointLabelPlace e,GsPlaceOrderPriority order);

	/// \brief 点与标注间距
	void PointSpaceByLabel(float fSpace);
	/// \brief 点与标注间距
	float PointSpaceByLabel();

	/// \brief 是否换行
	void LineBreak(bool bEnable);
	/// \brief 是否换行
	bool LineBreak();

	/// \brief 换行符参数
	GsLineBreakParam& LineBreakParam();

	/// \brief 同时绘制点符号和标注
	void DrawPointAndLabel(bool bEnable);
	/// \brief 同时绘制点符号和标注
	bool DrawPointAndLabel();
	
	/// \brief 标注避让的最小比例尺范围（1：x）
	void AvoidMinScale(double dScale);
	/// \brief 标注避让的最小比例尺范围（1：x）
	double AvoidMinScale();

	/// \brief 标注避让的最大比例尺范围（1：x）
	void AvoidMaxScale(double dScale);
	/// \brief 标注避让的最大比例尺范围（1：x）
	double AvoidMaxScale();

	/// \brief 增加避让数据
	void AddAvoidData(const char* strName, const char* strWhere="", double dSize = 0.0);
	/// \brief 删除避让数据
	void RemoveAvoidData(const char* strName);
	/// \brief 获取避让数据名称
	UTILITY_NAME::GsVector< Utility::GsString > AvoidDataName();
	/// \brief 清空避让数据
	void ClearAvoidData();

protected:
	/// \brief 计算标注
	/// \param pGeo 几何对象
	/// \param strLabel	标注文本
	/// \param pSym	几何符号
	/// \param strUnionValue 合并标注字段值（如果为NULL则跟标注文本一致）
	virtual bool OnCalculateLabel(GsGeometry *pGeo, const char* strLabel, GsSymbol* pSym);
	/// \brief 过滤标注位置
	virtual bool FilterLabelPosition(GsLabel *pLabel);
	// 保存障碍物数据
	void StoreAvoidData();
	// 计算标注候选位置
	bool CalLabelPosition(GsGeometry *pGeo, GsLabel* pLabel);
	// 标注位置是否冲突
	bool IsLabelPositionConflict(GsLabel *pLabel);

	Utility::GsPTF Offset(GsPointLabelPlace  e,float w,float h,float sw,float sh, Utility::GsPTF distXY);
	float OffsetX(GsPointLabelPlace e,float w,float h,float sw,float sh, double dDist);
	float OffsetY(GsPointLabelPlace e,float w,float h,float sw,float sh, double dDist);

	/// \brief 格式化标注文本
	Utility::GsString FormatLabelText(const char* strLabel);
	// 按照换行符打断字符串
	void BreakString(const Utility::GsWString& str, int nStart, int nEnd, UTILITY_NAME::GsVector<Utility::GsWString>& vecLines);
	// 通过行列数格式化字符串
	void LimitRowAndCol(const UTILITY_NAME::GsVector<UTILITY_NAME::GsVector<Utility::GsWString>>& vecSubStr, UTILITY_NAME::GsVector<Utility::GsWString>& vecLines);
};
/// \brief GsPointLabelPropertyPtr
GS_SMARTER_PTR(GsPointLabelProperty);


struct LabelInfo;
/// \brief 线自动标注计算类
class GS_API GsLineLabelProperty:public GsLabelProperty
{
protected:
	/// \brief 线标注类型
	GsLineLabelType m_eLineLabelType;
	/// \brief 标注与线位置关系
	GsLineLabelPos m_ePosType;
	/// \brief 线标注起点位置
	GsLineLabelStartPos m_eStartPos;
	/// \brief 是否标注多次
	bool m_bMultiLabel;
	/// \brief 线偏移量（单位mm）
	float m_fOffset;
	// 地理坐标线偏移量
	double m_dGeoLineOffset;
	//同一条地物线标注多次, 标注次数, 不是单个地物分段标注多次
	int m_MultiSubPosCount;
	//同一条地物线标注多次, 标注起始段索引
	int m_MultiSubPosIndex;
public:
	using GsLabelProperty::CalculateLabel;
	GsLineLabelProperty();
	virtual ~GsLineLabelProperty();

	void Begin(GsDisplayTransformation* pDT,GsLabelContainer* pContainer);

	/// \brief 克隆
	virtual GsLabelPropertyPtr Clone();

public:
	/// \brief 线标注类型
	void LineLabelType(GsLineLabelType eType);
	/// \brief 线标注类型
	GsLineLabelType LineLabelType();

	/// \brief 标注与线位置关系
	void LabelPos(GsLineLabelPos ePos);
	/// \brief 标注与线位置关系
	GsLineLabelPos LabelPos();

	/// \brief 线标注起始点位置
	void StartPos(GsLineLabelStartPos ePos);
	/// \brief 线标注起始点位置
	GsLineLabelStartPos StartPos();

	/// \brief 是否标注多次
	void MultiLabel(bool bValue);
	/// \brief 是否标注多次
	bool MultiLabel();

	/// \brief 偏移量（单位mm）
	void Offset(float fOffset);
	/// \brief 偏移量（单位mm）
	float Offset();

	/// \brief 同一条地物线标注多次,标注次数,不是单个地物分段标注多次
	void MultiSubPosCount(int nSubsCount);
	/// \brief 同一条地物线标注多次,标注次数,不是单个地物分段标注多次
	int MultiSubPosCount();

	/// \brief 同一条地物线标注多次,标注的段数
	void MultiSubPosIndex(int nSubIndex);
	/// \brief 同一条地物线标注多次,标注的段数
	int MultiSubPosIndex();
protected:
	/// \brief 计算标注
	/// \param pGeo 几何对象
	/// \param strLabel	标注文本
	/// \param pSym	几何符号
	virtual bool OnCalculateLabel(GsGeometry *pGeo, const char* strLabel, GsSymbol* pSym);

	// 转换标注对象
	bool CalLabelPosition(LabelInfo& labelInfo, GsLabel* pLabel);

	// 计算标注点
	void LabelPoints(GsGeometry* pGeo, double dWordLen, int nWordNum, Utility::GsVector<LabelInfo>& vecLabels);

	// 计算标注点
	void LabelPointsForPath(GsPath* pPath, double dWordLen,int nWordNum, Utility::GsVector<LabelInfo>& vecLabels);

	// 重新排列标注点序
	void ReorderLabel(LabelInfo& label, double dTextHeight, double dOffset);

	// 沿线标注类型重新排列点序
	void ReorderForAlongLine(Utility::GsVector<GsRawPoint>& vecPoint, double dTextHeight, double dOffset);
	
	// 沿线标注文字水平重新排列点序
	void ReorderForAlongLineHorizonText(Utility::GsVector<GsRawPoint>& vecPoint, double dTextHeight, double dOffset);
	
	// 水平标注排列点序
	void ReorderForHorizon(Utility::GsVector<GsRawPoint>& vecPoint, double dTextHeight, double dOffset);
	
	// 垂直于线标注排列点序
	void ReorderForVerticalLine(Utility::GsVector<GsRawPoint>& vecPoint, double dTextHeight, double dOffset);
};
/// \brief GsLineLabelPropertyPtr
GS_SMARTER_PTR(GsLineLabelProperty);

/// \brief 面标注类型
enum GsSurfaceLabelType
{
	eNormal,	// 常规
	eSkeletonLine,	// 沿骨架线标注
};

/// \brief 面自动标注计算类
class GS_API GsSurfaceLabelProperty:public GsLineLabelProperty
{
protected:
	GsSurfaceLabelType m_eSurfaceLabelType;
public: 
	using GsLabelProperty::CalculateLabel;
	GsSurfaceLabelProperty();
	virtual ~GsSurfaceLabelProperty();

	/// \brief 克隆
	virtual GsLabelPropertyPtr Clone();

	/// \brief 标注类型
	void SurfaceLabelType(GsSurfaceLabelType eType);
	/// \brief 标注类型
	GsSurfaceLabelType SurfaceLabelType();
protected:
	/// \brief 计算标注
	/// \param pGeo 几何对象
	/// \param strLabel	标注文本
	/// \param pSym	几何符号
	virtual bool OnCalculateLabel(GsGeometry *pGeo, const char* strLabel, GsSymbol* pSym);

	/// \brief 计算标注点的位置
	virtual GsRawPoint LabelPoint(geostar::geo_object* pobj, int nFlag = geostar::prop_geo_label);

	// 转换标注对象
	bool CalLabelPosition(GsGeometry *pGeo, GsLabel *pLabel);
	bool CalLabelPosForNormal(GsGeometry *pGeo, GsLabel *pLabel);
	bool CalLabelPosForSkeletonLine(GsGeometry *pGeo, GsLabel *pLabel);

	// 将Path转化为屏幕坐标系范围
	GsPathPtr TransPathToScreen(GsPath *pPath);

	// 计算标注方向
	GsRawPoint CalLabelDirection(GsPath* pPath);

	// 计算最小斜外接矩形
	GsPathPtr CalMinimumEnvelop(geostar::geo_object* pobj);
};
/// \brief GsSurfaceLabelPropertyPtr
GS_SMARTER_PTR(GsSurfaceLabelProperty);


KERNEL_ENDNS