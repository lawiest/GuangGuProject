#pragma once
#include "../utility/preconfig.h"
#include <map.h>  
#include <list>
#include "layoutelement.h"

KERNEL_NS 



/// \brief 编辑回馈框架对象类型
enum GsFeedbackItemType
{
	/// \brief 普通可以编辑修改的回馈手柄
	eEditableHandle,

	/// \brief  用户修改旋转属性的回馈手柄
	eEditableRotateHandle,

	/// \brief  描绘编辑边界的边框线
	eFrameLine,

	/// \brief  用于延长显示的边框线
	eExtendFrameLine,

	/// \brief 可以编辑修改的功能性回馈手柄
	eEditableFunctionHandle,
};
/// \brief 编辑回馈框架对象的功能
enum GsFeedbackFunction
{
	/// \brief 无特定功能，仅用于渲染
	eNoneFunction,

	/// \brief 仿射变换左上移动功能
	eLeftTopMove,

	/// \brief 仿射变换左下移动功能
	eLeftBottomMove,

	/// \brief 仿射变换左中移动功能
	eLeftCenterMove,

	/// \brief 仿射变换中上移动功能
	eCenterTopMove,

	/// \brief 仿射变换中下移动功能
	eCenterBottomMove,

	/// \brief 仿射变换右上移动功能
	eRightTopMove,

	/// \brief 仿射变换右上移动功能
	eRightBottomMove,

	/// \brief 仿射变换右中移动功能
	eRightCenterMove,

	/// \brief 旋转对象的功能
	eRotateItem = 100,

	//元素自定义的节点, 由各个element自行处理,
	eElementCustomization,
};
/// \brief 编辑回馈对象
struct  GsFeedbackItem
{
	
	GsFeedbackItem(GsFeedbackItemType t, GsGeometry* geo, GsFeedbackFunction f, double tol)
	{
		Type = t;
		Geometry = geo;
		Function = f;
		HittestTolerance = tol;
	}
	/// \brief 回馈对象的类型
	GsFeedbackItemType		Type;
	/// \brief 绘制的几何数据
	GsGeometryPtr			Geometry;
	
	/// \brief 对象功能
	GsFeedbackFunction		Function;

	/// \brief 命中容差
	double					HittestTolerance;

};
/// \brief 元素枚举器
class GS_API GsEnumElement :public Utility::GsRefObject
{

protected:
	GsEnumElement() {}
public:
	virtual ~GsEnumElement() {}

	/// \brief 重置枚举
	virtual void Reset() = 0;
	/// \brief 获取下一个元素
	virtual GsElementPtr Next() = 0;
};
/// \brief GsEnumElementPtr
GS_SMARTER_PTR(GsEnumElement);

/// \brief 过滤特定类型元素的枚举器
template<class T>
class GsTypeFilterEnumElement :public GsEnumElement
{
	GsEnumElementPtr m_ptrBase;
public:
	GsTypeFilterEnumElement(GsEnumElement* pBase)
	{
		m_ptrBase = pBase;
	}
	/// \brief 重置枚举
	virtual void Reset()
	{
		m_ptrBase->Reset();
	}
	/// \brief 获取下一个元素
	virtual GsElementPtr Next()
	{
		GsElementPtr ptrEle = m_ptrBase->Next();
		if (!ptrEle)
			return 0;
		
		T* pFilter = dynamic_cast<T*>(ptrEle.p);
		while (!pFilter)
		{
			ptrEle = m_ptrBase->Next();
			if (!ptrEle)
				return 0;

			pFilter = dynamic_cast<T*>(ptrEle.p);
		}

		return ptrEle;
	}
};
/// \brief 元素编辑回馈框架
class GS_API GsElementEditFeedback :public Utility::GsRefObject
{
	UTILITY_NAME::GsStlMap<GsFeedbackItemType, GsSymbolPtr> m_ItemSymbols;
protected:
	GsEnumElementPtr m_ptrEnumEle;
	//当前操作矩阵,基于开始的box
	GsMatrixD m_Matrix;
	GsBox m_BeginBox;

	UTILITY_NAME::GsVector<GsFeedbackItem> m_FeedBackItems;
	GsElementEditFeedback();
	virtual void BeginDraw();
	void FinishDraw();
public:
	virtual ~GsElementEditFeedback();

	/// \brief 根据变换的矩阵，调整每一项的Geometry.
	virtual GsBox UpdateMatrixFeedBackItems(const GsMatrixD & matrix);
	/// \brief 绘制
	virtual void Draw(GsDisplay* pDisp, GsTrackCancel* pCancel);

	/// \brief 获取特定对象类型的渲染符号	
	GsSymbol* ItemSymbol(GsFeedbackItemType eType);

	/// \brief 设置特定对象类型的渲染符号
	void ItemSymbol(GsFeedbackItemType eType, GsSymbol* sym);
};
/// \brief GsElementEditFeedbackPtr
GS_SMARTER_PTR(GsElementEditFeedback);

/// \brief 进行仿射变换编辑的回馈框架
class GS_API GsAffineElementEditFeedback :public GsElementEditFeedback
{

	GsEnvelopePtr m_ptrTmpEnv;
	volatile bool m_bStart, m_bMouseMove;
	GsRawPoint m_First, m_Second;
	//屏幕
	GsActiveViewPtr m_ptrView;
	//当前选中的节点
	GsFeedbackItem * m_HitFeedBackItem;
	//中间贴移动变换的兼容性位图
	GsImageCanvasPtr m_ptrFeedBackCanvas;
	//选中的element是否随外界边框一起变化
	bool m_bElementbeAffine;
	bool HitFeedBackItem(const GsRawPoint & point, double tol= 0);
	//计算八点拖拽缩放矩阵参数
	bool CalcScale(const GsBox& box, GsFeedbackFunction fun, const GsRawPoint &p1, const GsRawPoint& p2,  GsMatrixD& m);
	void FireFaceBackEdit(const GeoStar::Utility::GsPT & pt);

	GeoStar::Utility::GsImage* FeedBackMoveImage(GsEnumElement* pEnumEle, GsDisplayTransformation* pDT);
public:
	GsAffineElementEditFeedback();
	GsAffineElementEditFeedback(GsActiveView* ptrView,bool ElementbeAffine = true);
	~GsAffineElementEditFeedback();
	/// \brief 绑定编辑的元素
	void BindElement(GsEnumElement* pEle);
	//
	GsEnumElement* EnumElement();
	/// \brief 鼠标移动到选中的element上的事件,通知上层鼠标移动到了哪里 
	Utility::GsDelegate<void(GsFeedbackItem*) >OnAffineElementFeedback;

	/// \brief 重置
	virtual void Reset();
	/// \brief 分离
	virtual void Detach();
	/// \brief 接收鼠标按下消息
	virtual void OnMouseDown(GsButton eButton, int nKeyboard, const Utility::GsPT& pt);
	/// \brief 接收鼠标弹起消息
	virtual void OnMouseUp(GsButton eButton, int nKeyboard, const Utility::GsPT& pt);
	/// \brief 接收鼠标移动消息
	virtual void OnMouseMove(GsButton eButton, int nKeyboard, const Utility::GsPT& pt);
	virtual void OnFeedbackDraw(GsDisplay* pDisplay);
};
/// \brief GsAffineElementEditFeedbackPtr
GS_SMARTER_PTR(GsAffineElementEditFeedback);


/// \brief 元素容器
class GS_API GsElementContainer :public Utility::GsRefObject
{
	Utility::GsList<GsElementPtr> m_vecElements;
public:
	GsElementContainer();
	virtual ~GsElementContainer();

	/// \brief  元素数量
	int Count() const;

	/// \brief  元素数量
	void Clear();

	/// \brief  增加元素
	void Add(GsElement* pEle);

	/// \brief  插入元素
	bool Insert(GsElement* pEle, int i);

	/// \brief  移除元素
	GsElementPtr Remove(GsElement* pEle);

	/// \brief  移除元素
	GsElementPtr Remove(int i);

	/// \brief  获取元素在容器中的索引，-1表示元素不在容器中
	int IndexOf(GsElement* ele);

	/// \brief 下移元素
	int MoveDown(GsElement* ele);

	/// \brief 上移元素
	int MoveUp(GsElement* ele);

	/// \brief 移动元素到最顶层
	int MoveTop(GsElement* ele);

	/// \brief 移动元素到最底层
	int MoveBottom(GsElement* ele);

	/// \brief 枚举所有的元素
	GsEnumElementPtr EnumElement();

	bool Swap(GsElement* pEle1, GsElement* pEle2);

	/// \brief 枚举特定类型的的元素。
	template<class T>
	GsEnumElementPtr TypeFilterElement()
	{
		return new GsTypeFilterEnumElement<T>(EnumElement());
	}
};
/// \brief GsElementContainerPtr
GS_SMARTER_PTR(GsElementContainer);


/// \brief 复合元素
class GS_API GsMultiElement:public GsElement
{
	GsElementContainerPtr m_ptrContainer;
public:
	GsMultiElement();
	~GsMultiElement();
	
	GsElementContainer* Elements();
	/// \brief 设置仿射变换的矩阵
	virtual void Transform(const GsMatrixD& mx);
	/// \brief 对制图元素进行克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();
protected:

	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
};
/// \brief GsMultiElementPtr
GS_SMARTER_PTR(GsMultiElement);


/// \brief 纸张类型
/// \details https://en.wikipedia.org/wiki/Paper_size#Loose_sizes
enum GsPageType
{
	/// \brief North American paper sizes 216 × 279mm
	ePageLetter = 1,
	/// \brief North American paper sizes 216 × 356mm
	ePageLegal = 2,
	/// \brief North American paper sizes 279 × 432mm
	ePageTabloid = 3,
	/// \brief ANSI C 432 × 559mm
	ePageC = 4,
	/// \brief ANSI D 559 × 864mm
	ePageD = 5,
	/// \brief ANSI E 864 × 1118mm
	ePageE = 6,
	/// \brief ISO paper sizes, A series 148 × 210mm
	ePageA5 = 7,
	/// \brief ISO paper sizes, A series 210 × 297mm
	ePageA4 = 8,
	/// \brief ISO paper sizes, A series 297 × 420mm
	ePageA3 = 9,
	/// \brief ISO paper sizes, A series 420 × 594mm
	ePageA2 = 10,
	/// \brief ISO paper sizes, A series 594 × 841mm
	ePageA1 = 11,
	/// \brief ISO paper sizes, A series 841 × 1189mm
	ePageA0 = 12,


	/// \brief ISO paper sizes, A series 105 × 148mm
	ePageA6,
	/// \brief ISO paper sizes, A series 74 × 105mm
	ePageA7,
	/// \brief ISO paper sizes, A series 52 × 74mm
	ePageA8,
	/// \brief ISO paper sizes, A series 37 × 52mm
	ePageA9,
	/// \brief ISO paper sizes, A series 26 × 37mm
	ePageA10,

	/// \brief ISO paper sizes, B series 1000 × 1414mm
	ePageB0,
	/// \brief ISO paper sizes, B series 707 × 1000mm
	ePageB1,
	/// \brief ISO paper sizes, B series 500 × 707	mm
	ePageB2,
	/// \brief ISO paper sizes, B series 353 × 500mm
	ePageB3,
	/// \brief ISO paper sizes, B series 250 × 353	mm
	ePageB4,
	/// \brief ISO paper sizes, B series 176 × 250	mm
	ePageB5,
	/// \brief ISO paper sizes, B series 125 × 176	mm
	ePageB6,
	/// \brief ISO paper sizes, B series 88 × 125	mm
	ePageB7,
	/// \brief ISO paper sizes, B series 62 × 88	mm
	ePageB8,
	/// \brief ISO paper sizes, B series 44 × 62mm
	ePageB9,
	/// \brief ISO paper sizes, B series 31 × 44	mm
	ePageB10, 

	/// \brief ISO paper sizes, C series 917 × 1297mm
	ePageC0,
	/// \brief ISO paper sizes, C series 648 × 917mm
	ePageC1,
	/// \brief ISO paper sizes, C series 458 × 648mm
	ePageC2,
	/// \brief ISO paper sizes, C series 324 × 458	mm
	ePageC3,
	/// \brief ISO paper sizes, C series 229 × 324mm
	ePageC4,
	/// \brief ISO paper sizes, C series 162 × 229	mm
	ePageC5,
	/// \brief ISO paper sizes, C series 114 × 162	mm
	ePageC6,
	/// \brief ISO paper sizes, C series 81 × 114mm
	ePageC7,
	/// \brief ISO paper sizes, C series 57 × 81mm
	ePageC8,
	/// \brief ISO paper sizes, C series 40 × 57mm
	ePageC9,
	/// \brief ISO paper sizes, C series 28 × 40mm
	ePageC10,

	ePageCustom=1000,
};


/// \brief 页面元素
class GS_API GsPage :public GsElement
{
	GsPageType m_eType;
	Utility::GsSizeF m_PageSize;
	GsFillSymbolPtr m_PageSymbol;
	GsFillSymbolPtr m_ShadowSymbol;
	UTILITY_NAME::GsRect	m_PageRect;//纸张在显示设备中的大小。
	
	void CreatePage(const Utility::GsSizeF& s);

public:
	GsPage(GsPageType eType = ePageA4);
	~GsPage();
	
	/// \brief 根据纸张类型获取纸张大小
	/// \details https://en.wikipedia.org/wiki/Paper_size#Loose_sizes
	static Utility::GsSizeF PageSize(GsPageType e);

	/// \brief 获取纸张大小
	Utility::GsSizeF PageSize();
	/// \brief 设置自定义的纸张大小
	void PageSize(const Utility::GsSizeF& s);
	/// \brief GsPage
	GsPageType PageType();
	/// \brief GsPage
	void PageType(GsPageType type);
	
	/// \brief 获取纸张在设备中的大小
	UTILITY_NAME::GsRect PageRect();
	
protected:
	/// \brief 绘制
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);

};
/// \brief GsPagePtr
GS_SMARTER_PTR(GsPage);


/// \brief 选择元素的类型
enum GsSelectElementType
{
	/// \brief 选择结果形成新选择集
	eNewSelection,
	/// \brief 选择结果并入原选择集
	eUnionSelection,
	/// \brief 选择结果和原选择集亦或
	eXorSelection,
};

enum GsSelectElementLocatType
{
	/// \brief 选择所有
	eAllLocatSelection,
	/// \brief 选择最上层
	eTopLocatSelection,
	/// \brief 选择最底层
	eBottomLocatSelection,
	
};


/// \brief 制图对象
class GS_API GsPageLayout:public GsActiveView,Utility::GsWorkThread
{ 
	GsPagePtr m_ptrPage; 
	GsAffineElementEditFeedbackPtr m_ptrEditFeedback;
	GsMultiElementPtr m_ptrAssistElement;
	GsMultiElementPtr m_ptrMainElement;
	GsMultiElementPtr m_ptrSelectionSet;
	GsMultiElementPtr m_ptrLocatSelectionSet;
	bool m_bValid;
	double m_dReferenceScale;
	/// \brief 执行线程，实现此纯虚函数
	///\return 返回执行是否成功
	virtual bool Execute();
public:
	GsPageLayout();
	GsPageLayout(GsScreenDisplay* pDisplay);
	~GsPageLayout();

	/// \brief 元素容器
	GsElementContainer* ElementContainer();

	/// \brief 辅助元素容器
	/// \details 辅助元素如标尺、格网等辅助制图用途的元素
	/// \details 辅助元素绘制与所有制图元素之上
	GsElementContainer* AssistElementContainer();

	/// \brief 存放选中元素的容器
	GsElementContainer* SelectionSet();

	/// \brief 清空选择
	void ClearSelection();

	/// \brief 点选择
	GsElementContainer* SelectElement(const GsRawPoint& pt, GsSelectElementType eType,double tol, GsSelectElementLocatType eSelectLocatType = eAllLocatSelection);
	/// \brief 矩形选择
	GsElementContainer* SelectElement(const GsBox& box, GsSelectElementType eType, GsSelectElementLocatType eSelectLocatType = eAllLocatSelection);
	/// \brief 任意几何对象选择
	GsElementContainer* SelectElement(GsGeometry* geo, GsSelectElementType eType, GsSelectElementLocatType eSelectLocatType = eAllLocatSelection);
	
	/// \brief 编辑回馈对象
	GsElementEditFeedback *EditFeedback();


	/// \brief 获取纸张对象
	GsPage* Page();

	/// \brief 取消绘制
	void Cancel();
	/// \brief 刷新
	virtual void Update();

	/// \brief 输出
	virtual bool Output(GsDisplay* pDisp, GsTrackCancel* pCancel);

	/// \brief 是否在绘制
	bool IsDrawing();


	/// \brief 绘制地图
	/// \details 如果地图数据未失效，则直接绘制缓冲的位图，反之则重新绘制
	virtual void Paint(GsDrawPhase eReason = eDrawNormal);

	/// \brief 使地图失效
	virtual void Invalidate();

	/// \brief 判断是否是否已经失效
	virtual bool IsValid();

	 
	/// \brief 全图范围
	/// \details 所有图层的范围合并的范围
	virtual GsBox FullExtent();

	/// \brief 设置当前显示范围
	/// \details 会取消当前地图绘制，同时使得地图失效，但不会立刻刷新地图，需要额外调用Paint或者Update刷新地图
	virtual void ViewExtent(const GsBox& extent);

	/// \brief 获取当前显示范围
	/// \details 相当于调用方法Display()->DisplayTransformation()->MapExtent();
	virtual GsBox ViewExtent();

	/// \brief 获取参考比例尺
	void ReferenceScale(double dScale);
	/// \brief 设置参考比例尺 
	double ReferenceScale();

protected:
	/// \brief 根据选择的位置得到选择集
	GsElementContainer* SelectLocatElement(GsGeometry* geo, GsSelectElementLocatType eSelectLocatType = eAllLocatSelection);
};
/// \brief GsPageLayoutPtr
GS_SMARTER_PTR(GsPageLayout);

class GS_API GsLayoutParser
{
	Utility::GsString m_strFile;
	GsPageLayoutPtr m_ptrMap;
public:
	GsLayoutParser();
	~GsLayoutParser();

	/// \brief 解析制图模板为地图
	bool Read(const char* strFilePath, GsPageLayout* pLayout);

	/// \brief 保存到文件
	bool Save(GsPageLayout* pLayout, const char* strFilePath);

};
KERNEL_ENDNS
