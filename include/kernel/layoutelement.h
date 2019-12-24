#pragma once
#include "../utility/preconfig.h" 
#include <object.h>
#include "spatialreference.h"
#include "geometry.h"
#include "map.h"
#include <matrix.h>
#include "serialize.h"
KERNEL_NS

/// \brief 元素测试命中的结果
enum GsElementHittestResult
{
	/// \brief 没有命中
	eElementHitNone,

	/// \brief 完整命中全部
	eElementHitWhole = 1,
};
/// \brief 制图对象
class GS_API GsElement :public Utility::GsRefObject
{

protected:
	GsAffineCoordinateTransformationPtr m_ptrTransform;
	GsMatrixD				m_Matrix;//制图的变换矩阵
	bool					m_Visible;
	bool					m_Selected;
	bool					m_CanHit;
	GsGeometryPtr			m_ptrGeometry;
	double					m_dblReferenceScale; // 参考比例尺
	UTILITY_NAME::GsRect	m_PageRect;//纸张在显示设备中的大小。
	UTILITY_NAME::GsString	m_Name;//名字接口
	UTILITY_NAME::GsString	m_ClassName;//实例名字
	GsElement();
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType) = 0;
	/// \brief 任意几何对象中测试
	virtual GsElementHittestResult OnHittest(GsGeometry* geo);

	/// \brief 拷贝
	virtual void Assign(GsElement* pEle);

	/// \brief 计算并修改传入的矩阵，转换前后box的左下角点不变。
	/// \param box 旧的外接矩形范围
	/// \param point 转换之后，新的外接框矩形的最大点(XMax, YMax),其中转换过程中原有的(XMin,YMin)不变。
	/// \param mat 输出参数：计算出转换的矩阵，并且返回
	void CalcParamsMatrix(const GsBox& box, const GsRawPoint&point, GsMatrixD & mat);

	/// \brief 通过传入旧的box和新的box，计算并修改传入的矩阵
	/// \param oldBox 旧的外接矩形范围
	/// \param newBox 新的外接矩形范围
	/// \param mat 输出参数：计算出转换的矩阵，并且返回
	void CalcParamsMatrix(const GsBox& oldBox, const GsBox& newBox,  GsMatrixD & mat);

	/// \brief 序列化
	bool XmlSerialize(UTILITY_NAME::GsSerializeStream * pSerStream);
	/// \brief 反序列化
	bool XmlDeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream);
public:
	virtual ~GsElement();

	/// \brief 获取仿射变换的矩阵
	virtual GsMatrixD Transform();
	/// \brief 设置仿射变换的矩阵
	virtual void Transform(const GsMatrixD& mx);

	/// \brief 单点命中测试
	virtual GsElementHittestResult Hittest(const GsRawPoint& pt, double tol);
	/// \brief 矩形命中测试
	virtual GsElementHittestResult Hittest(const GsBox& box);
	/// \brief 任意几何对象中测试
	virtual GsElementHittestResult Hittest(GsGeometry* geo);


	/// \brief 是否可见
	bool Visible();
	/// \brief 是否可见
	void Visible(bool b);
	/// \brief 是否选中
	bool Selected();
	/// \brief 是否选中
	void Selected(bool bSelect);
	/// \brief 能否被命中
	bool CanHit();
	/// \brief 能否被命中
	void CanHit(bool bCanHit);
	/// \brief 几何对象
	virtual GsGeometry* Geometry();
	/// \brief 几何对象
	void Geometry(GsGeometry* geo);

	/// \brief 得到参考比例尺
	double ReferenceScale();
	/// \brief 设置参考比例尺
	void ReferenceScale(double dblReferenceScale);

	/// \brief 设置纸张在设备中大小。
	void PageRect(UTILITY_NAME::GsRect rect);

	/// \brief 获取纸张在设备中的大小
	UTILITY_NAME::GsRect PageRect();

	/// \brief 设置名字
	void Name(const char* name);

	/// \brief 获取名字
	const char * Name();

	/// \brief 进行几何变换后用于绘制的几何对象
	virtual GsGeometryPtr DisplayGeometry();

	/// \brief 绘制
	virtual void Draw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	/// \brief 获取位置 外接矩形的左下角位置
	virtual GsRawPoint Position();
	/// \brief 设置位置 外接矩形的左下角位置
	virtual void Position(const GsRawPoint & position);
	/// \brief 设置大小
	virtual Utility::GsSizeF Size();
	/// \brief 设置大小
	virtual void Size(const Utility::GsSizeF & sizef);

	/// \brief 对制图元素进行克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();

	//virtual GeoStar::Utility::GsString ClassName();
};
/// \brief GsElementPtr
GS_SMARTER_PTR(GsElement);

/// \brief 地图元素
class GS_API GsMapElement :public GsElement, public UTILITY_NAME::GsSerialize
{
	GsPaintDevicePtr m_ptrPaintDevice;
	GsMapPtr m_ptrMap;
	bool		m_bClipPage;//控制是否裁切纸张。
	
	bool		m_bShowBorder;
	GsSymbolPtr m_ptrBorderSymbol;
	double		m_dblBorderOffset;

	double m_dblFixScale;
	double m_dblLastScale;
	bool m_bFixScale;
	GsDisplayTransformationPtr m_ptrDT;
	UTILITY_NAME::GsSimpleBitmapPtr m_ptrBitmap;
	GsBox  m_Box;
	UTILITY_NAME::GsRect m_Rect;

	void DrawMap(const Utility::GsRect& rect, GsDisplay* disp, GsTrackCancel* pCancel);

public:
	GsMapElement();
	GsMapElement(GsMap* pMap);
	~GsMapElement();
	/// \brief 获取绑定的Map
	GsMap*  Map();

	/// \brief 图廓符号
	GsSymbol* BorderSymbol();
	/// \brief 图廓符号
	void BorderSymbol(GsSymbol* sym);

	/// \brief 是否显示图廓
	bool ShowBorder();
	/// \brief 是否显示图廓
	void ShowBorder(bool bShow);

	/// \brief 图廓显示位置的偏移
	double BorderOffset();
	/// \brief 图廓显示位置的偏移，默认位置是map制图画布的外接矩形。如果是正，则向内偏移；如果是负，则向外偏移
	void BorderOffset(double offset);

	/// \brief 是否裁切纸张
	bool ClipPage();
	/// \brief 是否裁切纸张
	void ClipPage(bool bClip);

	/// \brief 获取是否锁定比例尺
	bool IsFixedScale();
	/// \brief 设置是否锁定比例尺
	void IsFixedScale(bool bFixedScale);

	/// \brief 获取锁定得比例尺, 此比例尺为PageLauout得DT得来
	double FixedScale();
	/// \brief  设置锁定比例尺, 此比例尺为PageLauout得DT得来, 如果想要制图页面一致不改变比例尺不得设置,
	/// \brief  如果需要动态锁定比例尺(Map范围变化后锁定变化后的比例尺,则通过mapelemet中map范围和mapelement的大小重新计算一个用来锁定)
	/// \brief  默认锁定值会从layout的dt中拿一个范围
	void FixedScale(double dblFixedScale);

	double DrawScale();
	/// \brief 对制图元素进行克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual UTILITY_NAME::GsString ClassName();

protected:
	/// \brief 绘制
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	/// \brief 拷贝
	virtual void Assign(GsMapElement* pEle);

	
};
/// \brief GsMapElementPtr
GS_SMARTER_PTR(GsMapElement);
DECLARE_CLASS_CREATE(GsMapElement);


/// \brief 基于特定形状的元素
class GS_API GsShapeElement :public GsElement ,public UTILITY_NAME::GsSerialize
{
protected:
	GsSymbolPtr			m_ptrSymbol;
	/// \brief 绘制
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	/// \brief 拷贝
	virtual void Assign(GsShapeElement* pEle);
	
public:
	GsShapeElement();
	GsShapeElement(GsGeometry* geo, GsSymbol* sym);
	GsShapeElement(GsGeometry* geo);
	~GsShapeElement();
	virtual GsSymbol * Symbol();
	virtual void Symbol(GsSymbol*);
	virtual Utility::GsSmarterPtr<GsElement> Clone();
	

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual UTILITY_NAME::GsString ClassName() ;

};
/// \brief GsShapeElementPtr
GS_SMARTER_PTR(GsShapeElement);
DECLARE_CLASS_CREATE(GsShapeElement);

/// \brief 几何元素
class GS_API GsGeometryElement :public GsShapeElement
{

protected:
	/// \brief 绘制
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	/// \brief 拷贝
	virtual void Assign(GsGeometryElement* pEle);
	
public:
	GsGeometryElement();
	GsGeometryElement(GsGeometry* geo, GsSymbol* sym);
	GsGeometryElement(GsGeometry* geo);
	~GsGeometryElement();
	using GsElement::Transform;

	/// \brief 设置仿射变换的矩阵
	virtual void Transform(const GsMatrixD& mx);

	virtual Utility::GsSmarterPtr<GsElement> Clone();

	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;
};
/// \brief GsGeometyElementPtr
GS_SMARTER_PTR(GsGeometryElement);
DECLARE_CLASS_CREATE(GsGeometryElement);
/// \brief 矩形元素
class GS_API GsEnvelopeElement :public GsGeometryElement
{
protected:
	/// \brief 拷贝
	virtual void Assign(GsEnvelopeElement* pEle);
public:
	GsEnvelopeElement() {}
	GsEnvelopeElement(const GsBox & box);
	GsEnvelopeElement(GsEnvelope* pEnv);
	~GsEnvelopeElement();
	virtual Utility::GsSmarterPtr<GsElement> Clone();
};
GS_SMARTER_PTR(GsEnvelopeElement);
DECLARE_CLASS_CREATE(GsEnvelopeElement);
/// \brief 指北针类型
enum GsNorthArrowType
{
	/// \brief 指北针，上方没有N标识
	eNorthArrowAllNoText,
	/// \brief 左边是白色的指北针，上方有N标识
	eNorthArrowLeft,
	/// \brief 右边是白色的指北针，上方有N标识
	eNorthArrowRight,
	/// \brief 全部黑色的指北针，上方有N标识
	eNorthArrowAll,
	/// \brief 四个方向的指北针，上方有N标识
	eNorthArrowFour,
	/// \brief 基于点符号得指北针,//geostar.ttf code:21-27
	eNorthArrowByPointSymbol,
};
/// \brief 指北针
class GS_API GsNorthArrow :public GsGeometryElement
{
	//➢➣➤
	/// \brief 旋转角度
	double m_dblAngle;
	/// \brief 记录外接框
	GsBox m_Box;

	/// \brief 记录原有的box，转换样式的时候使用
	GsBox m_BoxOrigin;

	/// \brief 绘制过程中记录左边三角形
	GsRingPtr m_ptrRingLeft;
	/// \brief 绘制过程中记录右边三角形
	GsRingPtr m_ptrRingRight;
	
	/// \brief 绘制四个方向指北针的时候 记录GsRing 
	GsRingPtr m_ptrRingFourLeft;
	/// \brief 绘制四个方向指北针的时候 记录GsRing 
	GsRingPtr m_ptrRingFourRight;
	/// \brief 绘制过程中记录文字外接框的矩阵
	GsRingPtr m_ptrRingText;

	/// \brief 绘制指北针的类型
	GsNorthArrowType m_NorthArrowType;
	/// \brief 绘制指北针文字的符号。
	GsTextSymbolPtr m_ptrTextSymbol;
	
	/// \brief 绘制指北针填充的颜色。
	GsColor m_Color;
	/// \brief 文字和指北针之间的间距。
	double m_Gap;

	/// \brief绘制指北针
	void DrawNorthArrow(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	/// \brief绘制四个箭头样式的指北针
	void DrawNorthArrowFour(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);

	/// \brief 绘制指北针上方的"N"文字。
	/// \param box 绘制指北针之前的外接框。
	/// \param coff 文字放大系数。以高除以高来计算。
	GsBox DrawTextN(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType, GsBox box, double coff = 1.0);

	/// \brief 初始化m_ptrRingLeft,m_ptrRingRight,m_ptrRingText;
	void InitNorthArraw();
protected:
	/// \brief 拷贝
	virtual void Assign(GsNorthArrow* pEle);
	/// \brief 绘制
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	
public:
	GsNorthArrow();
	GsNorthArrow(double x, double y, double w, double h, GsNorthArrowType type = eNorthArrowRight);
	~GsNorthArrow();

	/// \brief 获取指北针的样式
	GsNorthArrowType NorthArrowType();
	/// \brief 设置指北针的样式
	void NorthArrowType(GsNorthArrowType type);


	/// \brief 颜色
	virtual GsColor Color();
	/// \brief 设置颜色
	virtual void Color(const GsColor& c);

	/// \brief 旋转角度,单位是度
	virtual double Angle();
	/// \brief 旋转角度,单位是度
	virtual void Angle(double Angle);

	/// \brief 获取文字"N"的大小
	virtual double TextSize();
	/// \brief 设置文字"N"的大小
	virtual void TextSize(double size);
	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

};
GS_SMARTER_PTR(GsNorthArrow);
DECLARE_CLASS_CREATE(GsNorthArrow);

/// \brief 图片
class GS_API GsPictureElement :public GsGeometryElement
{
	Utility::GsImagePtr        m_ptrImage;
	double m_dHeight;
	double m_dWidth;
	GsRawPoint m_ptCenter;
	/// \brief 图廓符号
	GsSymbolPtr m_ptrBorderSymbol; 

	void Init(double x, double y, Utility::GsImage *pImg);
public:
	GsPictureElement();
	/// \brief 从图片对象构造
	GsPictureElement(double x, double y,GeoStar::Utility::GsImage * pImg);
	/// \brief 从图片内存块构造
	GsPictureElement(double x, double y, const unsigned char *pBlob, int nLen);
	/// \brief 从图片文件构造
	GsPictureElement(double x, double y, const char* pImageFile);
	~GsPictureElement();

	/// \brief 得到图片的高度
	double Height();
	/// \brief 设置图片的高度
	void Height(double dHeight);

	/// \brief 得到图片的宽度
	double Width();
	/// \brief 得到设置的宽度
	void Width(double dWidth);

	/// \brief 得到图片的中心点位置
	GsRawPoint CenterPoint();
	/// \brief 设置图片的中心点位置
	void CenterPoint(const GsRawPoint& pt);

	/// \brief 设置图廓符号
	void BorderSymbol(GsSymbol* pSym);
	/// \brief 得到图廓符号
	GsSymbol* BorderSymbol();

	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

protected:
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	/// \brief 拷贝
	virtual void Assign(GsPictureElement* pEle);
};
GS_SMARTER_PTR(GsPictureElement);
DECLARE_CLASS_CREATE(GsPictureElement);


/// \brief 接幅表
//  \brief 三行三列 九宫格
class GS_API GsJoinMapTableElement :public GsGeometryElement
{
	/// \brief 接幅表中间面填充符号,默认斜线填充
	GsFillSymbolPtr m_ptrFillSymbol;
	/// \brief 接幅表文字绘制符号
	GsTextSymbolPtr m_ptrTextSymbol;
	/// \brief 接幅表边线绘制符号
	GsLineSymbolPtr m_ptrLineSymbol;
	/// \brief 存储需要绘制的文字
	UTILITY_NAME::GsVector< UTILITY_NAME::GsVector<UTILITY_NAME::GsString> > m_Rows;

	/// \brief 九宫格的交叉点
	GsRawPoint m_PointInter[4][4];
	/// \brief 九宫格的每个格子的中心点。
	GsRawPoint m_PointCenter[3][3];

	/// \brief 记录外接框的大小。
	GsBox m_box;

	void InitJoinMapTable();

	void DrawLine(GsDisplay* pDisp, const GsRawPoint& point1, const GsRawPoint& point2, GsSymbol* pSymbol = NULL );
public:
	GsJoinMapTableElement();
	/// \brief 接幅表构造函数
	GsJoinMapTableElement(double x, double y);
	/// \brief 接幅表析构函数
	~GsJoinMapTableElement();
	/// \brief 设置某一位置的值,row和col的取值为0,1,2
	void Value(int row, int col, const char* text);
	/// \brief 获取某一位置的值，row和col的取值为0,1,2
	UTILITY_NAME::GsString Value(int row, int col);
	/// \brief 接幅表中间面填充符号,默认斜线填充
	GsFillSymbol* FillSymbol();
	/// \brief 接幅表中间面填充符号,默认斜线填充
	void FillSymbol(GsFillSymbol *pFillSymbol);
	/// \brief 接幅表边线绘制符号
	GsLineSymbol* FrameLineSymbol();
	/// \brief 接幅表边线绘制符号
	void FrameLineSymbol(GsLineSymbol *pLineSymbol);
	/// \brief 接幅表文字绘制符号
	GsTextSymbol* TextSymbol();
	/// \brief 接幅表文字绘制符号
	void TextSymbol(GsTextSymbol *pTextSymbol);
	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

protected:
	/// \brief 绘制
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	/// \brief 拷贝
	virtual void Assign(GsJoinMapTableElement* pEle);
};
GS_SMARTER_PTR(GsJoinMapTableElement);
DECLARE_CLASS_CREATE(GsJoinMapTableElement);


/// \brief 坡度尺的类型
enum GsSlopeRulerType
{	
	eSlopeRulerTypeNone = 0,
	/// \brief 一种是绘相邻六条等高线时，从第五度开始绘制
	eSlopeRulerTypeFive ,

	/// \brief 另一种从第十度开始绘制，对应的描述信息的位置都会随着改变
	eSlopeRulerTypeTen,

	
};

/// \brief 坡度尺
class GS_API GsSlopeRulerElement :public GsGeometryElement
{
	/// \brief 尺子的横向分隔值
	double   m_dfIntervalGap;
	/// \brief 标题和尺之间的间隔距离
	double   m_dfTitleGap;
	/// \brief 标注和尺子之间的间隔距离
	double   m_dfLabelRulerGap;
	/// \brief 等高距
	double   m_dfContourDist;
	/// \brief 是否显示正切标注
	bool     m_bShowTanlabel;
	/// \brief 坡度尺的类型
	GsSlopeRulerType m_enumType;
	/// \brief 说明信息字体高度
	double   m_dRulerInfoHeight;
	/// \brief 标题文字高度
	double   m_dTitleNameHeight;
	/// \brief 标注文字高度
	double   m_dLabelTextHeight;

	/// \brief 绘制线的符号Symbol
	GsLineSymbolPtr m_ptrLineSym;

	/// \brief 绘制的起点
	GsRawPoint m_ptStar;

	/// \brief 外边框
	GsGeometryPtr m_ptrFrameGeometry;


	// 下面的参数为该类的临时成员变量
protected:
	/// \brief 绘制坡度尺的线的临时成员变量
	GsLinePtr   m_ptrLine;	
	/// \brief 绘制线的起点
	GsRawPoint m_ptFrom;
	/// \brief 绘制线的终点
	GsRawPoint m_ptTo;
	/// \brief 所有需要绘制的线的集合
	UTILITY_NAME::GsVector<GsGeometryPtr> m_vecDrawLineGeos;
	/// \brief 所有需要绘制的坡度符号的位置的集合 (如1° 30°）
	UTILITY_NAME::GsVector<GsGeometryPtr> m_vecSlopeGeos;
	/// \brief 所有坡度注释符号的位置（如 “量相邻两条等高线时用”文字位置）
	UTILITY_NAME::GsVector<GsGeometryPtr> m_vecSlopenoteGeos;
	/// \brief 所有需要绘制正切值标注的位置的集合
	UTILITY_NAME::GsVector<GsGeometryPtr> m_vecTanlabelGeos;
	/// \brief 标题的位置
	GsGeometryPtr m_ptrTitleGeo;	
	/// \brief 所有需要绘制文字的Element集合
	UTILITY_NAME::GsVector<GsElementPtr> m_vecTestElements;
	/// \brief 临时计算出的标题与底线之间的高度
	double m_dRulerHeigh;

public:
	GsSlopeRulerElement();
	GsSlopeRulerElement(double x, double y);
	~GsSlopeRulerElement();

	/// \brief 得到尺子的横向分隔值
	double IntervalGap();
	/// \brief 设置尺子的横向分隔值
	void IntervalGap(double dIntervalGap);

	/// \brief 得到尺子的横向分隔值
	double TitleGap();
	/// \brief 设置尺子的横向分隔值
	void TitleGap(double dTitleGap);

	/// \brief 得到标注和尺子之间的间隔距离
	double LabelRulerGap();
	/// \brief 设置标注和尺子之间的间隔距离
	void LabelRulerGap(double dLabelRulerGap);

	/// \brief 得到等高距
	double ContourDist();
	/// \brief 设置等高距
	void ContourDist(double dContourDist);

	/// \brief 得到是否显示正切标注
	bool ShowTanlabel();
	/// \brief 设置是否显示正切标注
	void ShowTanlabel(bool bShowTanlabel);

	/// \brief 得到坡度尺的类型
	GsSlopeRulerType SlopeRulerType();
	/// \brief 设置坡度尺的类型
	void SlopeRulerType(GsSlopeRulerType eType);

	/// \brief 得到说明信息字体高度
	double RulerInfoHeight();
	/// \brief 得到说明信息字体高度
	void RulerInfoHeight(double dRulerInfoHeight);

	/// \brief 得到标题文字高度
	double TitleNameHeight();
	/// \brief 设置标题文字高度
	void  TitleNameHeight(double dTitleNameHeight);

	/// \brief 得到标注文字高度
	double LabelTextHeight();
	/// \brief 设置标注文字高度
	void LabelTextHeight(double dLabelTextHeight);

	/// \brief 得到绘制线的符号Symbol
	GsLineSymbol* LineSymbol();
	/// \brief 设置绘制线的符号Symbol
	void LineSymbol(GsLineSymbol* pLineSym);


	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;
protected:
	/// \brief 绘制
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	/// \brief 拷贝
	virtual void Assign(GsSlopeRulerElement* pEle);

private:
	/// \bief 初始化需要绘制的数据
	void InitDrawData();
	/// \brief 初始化时计算绘相邻两条等高线时的位置信息
	void CaculateTowContourLines();
	/// \brief 初始化时计算绘相邻六条等高线时的位置信息
	void CaculateSixContourLines();
	/// \brief 根据设置的参数计算外框
	void CaculateFrameBox();
	/// \brief 拖动改变边框重新计算内部数据的尺寸长宽 
	void InitSlopeRulerFormatGap();

};
GS_SMARTER_PTR(GsSlopeRulerElement);
DECLARE_CLASS_CREATE(GsSlopeRulerElement);


/// \brief数据表格
class GS_API GsTableElement :public GsGeometryElement
{
public:
	GsTableElement();
};
GS_SMARTER_PTR(GsTableElement);


/// \brief TextElement中文字填充类型
enum GsTextTensile
{
	//TextElement中的文字如何填充传入的矩形框
	/// \brief 完全拉伸，宽高不变
	eFullTensile,
	/// \brief 以宽高的最小值作为文字大小
	eMinTensile,
	/// \brief 以宽作为文字大小
	eWidthTensile,
	/// \brief 以高作为文字大小
	eHeightTensile
};


/// \brief 锚点的位置
enum GsTextAnchors
{
	//用户传入的锚点位置为坐标点
	//如旋转，开始绘制等都以锚点为参考
	//*    *    *
	//*    *    *
	//*    *    *

	/// \brief 第一行左边点
	eTextTopLeft,
	/// \brief 第一行中间点
	eTextTopCenter,
	/// \brief 第一行右边点
	eTextTopRight,
	/// \brief 第二行左边点
	eTextCenterLeft,
	/// \brief 第二行中间点
	eTextCenterCenter,
	/// \brief 第二行右边点
	eTextCenterRight,
	/// \brief 第三行左边点
	eTextBottomLeft,
	/// \brief 第三行中间点
	eTextBottomCenter,
	/// \brief 第三行右边点
	eTextBottomRight
};

/// \brief 文本元素
class GS_API GsTextElement : public GsGeometryElement
{
	/// \brief 锚点位置
	GsTextAnchors m_TextAnchors;
	/// \brief 锚点坐标
	GsRawPoint m_TextAnchorsPoint;

	/// \brief 记录文本框在不旋转的时候的形状
	GsRingPtr m_TextRing;
	/// \brief 记录文本框旋转后的形状
	GsRingPtr m_TextAngleRing;

	/// \brief 记录文本框中的文本值
	Utility::GsString m_sText;
	/// \brief 记录外接轮廓的宽
	double m_Width;
	/// \brief 记录外接轮廓的高
	double m_Height;
	/// \brief 记录左上角点的坐标，绘制开始的位置
	GsPointPtr m_Point;
	/// \brief 记录文本框旋转角度
	double m_Angle;
public:
	GsTextElement();
	/// \brief 根据字符串来设置外接矩形的宽高，然后根据锚点来设置外接矩形的坐标值
	GsTextElement(double pX, double pY, const char* pStr = "", GsTextAnchors pTextAnchors = eTextBottomLeft);
	/// \brief 根据文本符号来设置外接矩形的宽高，然后根据锚点来设置外接矩形的坐标值
	GsTextElement(double pX, double pY, GsTextSymbol* pText, GsTextAnchors pTextAnchors = eTextBottomLeft);
	~GsTextElement();

	/// \brief 返回文本符号
	GsTextSymbolPtr TextSymbol();
	/// \brief 设置文本符号
	void TextSymbol(GsTextSymbol* pTextSym);

	/// \brief 返回锚点所在坐标
	GsRawPoint TextAnchorsPoint();
	/// \brief 设置锚点坐标
	void TextAnchorsPoint(const GsRawPoint& p);
	/// \brief 设置锚点坐标
	void TextAnchorsPoint(const GsRawPoint& p, GsTextAnchors pTextAnchors);

	/// \brief 返回锚点类型
	GsTextAnchors TextAnchors();
	/// \brief 设置锚点类型
	void TextAnchors(GsTextAnchors pTextAnchors);

	/// \brief 返回文本框的宽
	double Width();
	/// \brief 设置文本框的宽
	void Width(double pWidth);

	/// \brief 返回文本框的高
	double Height();
	/// \brief 设置文本框的高
	void Height(double pHeight);

	/// \brief 返回文本框旋转角度，单位为度
	double Angle();
	/// \brief 设置文本框旋转角度，单位为度
	void Angle(double pAngle);

	/// \brief 返回文本框的字符串
	Utility::GsString Text();
	/// \brief 设置文本框的字符串
	void Text(const char* str);

	/// \brief 返回外接矩形框的轮廓
	GsBox Envelope();
	/// \brief 根据传入的字符串计算新的外接矩形
	GsBox Envelope(const char* strText);

	/// \brief 设置文本制图的外接矩形框
	/// \param pBox 外接矩形框的位置和大小
	/// \param pTextTensile	文本制图中的文字填充类型
	void Envelope(double pBoxWidth, double pBoxHeight, GsTextTensile pTextTensile);

	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

protected:
	/// \brief 绘制
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	/// \brief 拷贝
	virtual void Assign(GsTextElement* pEle);

private:
	/// \brief 计算字符串的外接矩形
	Utility::GsSizeF MeasureString(GsCanvas* pCanvas, GsDisplayTransformation* pDisplayTrans, GsTextSymbol* pTextSym);

	/// \brief 根据传入的矩形和旋转角度，计算新的矩形坐标，同时得到左上角点的坐标
	/// \param pRing 未做任何旋转的矩形
	/// \param pAngle 旋转角度
	GsRingPtr EnvelopeAngle(GsRing* pRing, double dAngle);

	/// \brief 根据传入的宽高，锚点位置，返回矩形的坐标
	GsBox EnvelopeTransform(double dWidth, double dHeight, const GsRawPoint& pRawPoint, GsTextAnchors pTextAnchors);

	/// \brief 设置文本符号，来设置外接矩形的宽高
	bool SetTextSymbol(GsTextSymbol* pTextSym);
};
GS_SMARTER_PTR(GsTextElement);
DECLARE_CLASS_CREATE(GsTextElement);


/// \brief 格网,方里网格网显示风格
enum GsGridLineShowType
{
	/// \brief 显示格网线
	eShowLines,
	/// \brief 显示格网线交点十字
	eShowTicks,
	/// \brief 不显示内部格网线
	eShowNone,
};

/// \brief 格网,方里网四边显示相关信息(主刻度，分刻度，标注),控制四个方向是否显示
enum GsGridOrientation
{
	/// \brief 上部显示
	eGridTop = 1,
	/// \brief 下部显示
	eGridBottom = 2,
	/// \brief 左部显示
	eGridLeft = 4,
	/// \brief 右部显示
	eGridRight = 8,
};

/// \brief 格网,方里网文字显示方向 不论是水平还是垂直都是自西向东,自南向北排列字
enum GsGridLabelOrientation
{
	/// \brief 水平显示
	eHorizontal,
	/// \brief 垂直显示
	eVerticalLabel,
};
/// \brief 格网,方里网四边显示文字的样式
enum GsGridLabelStringFormat
{
	/// \brief 六十进制显示度分秒 100°12′12″ N 或100°12′12″ E
	eSexagesimal,
	/// \brief 十进制 显示 100.7889,尾部字体分割大小显示
	eDecimalDivisional,
	/// \brief 自定义,暂未实现
	eCustomDefine,
};

/// \brief 格网,分刻度位置
enum GsGridScalePosition
{
	/// \brief 显示在格网线内部
	eInside,
	/// \brief 显示在格网线外部
	eOutside,
};
/// \brief 格网,方里网
class GS_API GsGridElement :public GsGeometryElement
{
	/// \brief 记录外接框的大小。
	GsBox m_box;
	/// \brief 记录格网显示的最大范围，默认是经度-180到180 维度-90到90，如果是0则无限制
	GsBox m_boxRange;
	/// \brief 十字交叉的外接圆的半径。
	double m_dfCrossTickSize;
	/// \brief 格网显示风格
	GsGridLineShowType m_LineShowType;
	/// \brief 控制四个方向是否显示标注
	GsGridOrientation m_GridLabelAxes;
	/// \brief 文字显示方向
	GsGridLabelOrientation  m_LabelOrientation;
	/// \brief 	四边显示文字的样式
	GsGridLabelStringFormat m_LabelFormat;
	/// \brief 是否根据Map范围变化调整格网绘制原点,默认位false,如果为true,地图范围变化将根据地图范围动态改变原点
	bool m_FixedOrigin;
	/// \brief x方向间距间距经度纬度
	double m_XIntervalSize;
	/// \brief Y方向间距 间距经度纬度
	double m_YIntervalSize;
	/// \brief x方向间距  实际绘制间距
	double m_XIntervalDrawSize;
	/// \brief Y方向间距 实际绘制间距
	double m_YIntervalDrawSize;
	/// \brief X轴原点的经纬度
	double m_XOrigin;
	/// \brief Y轴原点的经纬度
	double m_YOrigin;
	/// \brief 绑定的Map对象
	GsMapPtr m_ptrMap;
	/// \brief 格网线的符号
	GsLineSymbolPtr m_ptrLineSymbol;
	/// \brief 外边框符号
	GsLineSymbolPtr m_ptrBorderSymbol;

	/// \brief 主刻度的长度
	double m_dfMainScaleLength;
	/// \brief 四边是否绘制主刻度
	GsGridOrientation m_OrientMainScaleLine;
	/// \brief 主刻度的位置，格网外还是格网内
	GsGridScalePosition m_PosMainScale;
	
	/// \brief 分刻度将主刻度切分的数量
	int m_nDivisionScaleCount;
	/// \brief 分刻度的长度
	double m_dfDivisionScaleLength;
	/// \brief 四边是否绘制分刻度
	GsGridOrientation m_OrientDivisionScaleLine;
	/// \brief 分刻度的位置，格网外还是格网内
	GsGridScalePosition m_PosDivisionScale;

	/// \brief 标注偏移
	double m_dfTextOffset;
	/// \brief 文字标注符号
	GsTextSymbolPtr m_ptrTextSymbol;
	/// \brief label分段显示符号
	GsTextSymbolPtr m_ptrDecimalDivisionalTextSymbol;

	/// \brief x,y原点(经纬度)对应的实际坐标的原点
	GsRawPoint m_pointOrigin;

	/// \brief 绘制直线段
	/// \param point1 端点1
	/// \param point2 端点2
	/// \param pSymbol 绘制线段的符号，默认：简单线黑色
	void DrawLine(GsDisplay* pDisp, const GsRawPoint& point1, const GsRawPoint& point2, GsSymbol* pSymbol = NULL);

	/// \brief 绘制十字交叉
	/// \param point 中心点.
	/// \param size 中心点到外接框的边线的距离。
	/// \param pSym 绘制的符号
	void DrawCrossTicks(GsDisplay* pDisp, const GsRawPoint& point, double size, GsSymbol * pSym);

	/// \brief 初始化外接框对应的经纬度。
	/// \param rect 对应的设备范围
	/// \return 返回经纬度最大范围(-180, -90, 180, 90),对应图纸上面的坐标范围
	GsBox InitGridElement(Utility::GsRect rect);

	/// \brief 绘制文字
	/// \param pTextSym 文字符号
	/// \param text 文字
	/// \param point 锚点
	/// \param eAnchors 锚点类型
	/// \param angle 角度
	GsBox DrawText(GsDisplay* pDisp, GsTrackCancel * pCancel, GsDrawPhase drawType, GsTextSymbol* pTextSym, const char* text, const GsRawPoint& point, GsTextAnchors eAnchors, double angle);

protected:
	/// \brief 绘制
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);

	/// \brief 拷贝
	virtual void Assign(GsGridElement* pEle);
public:
	GsGridElement();
	/// \brief 按照Map范围构造
	/// \param ptrMap 绑定一个GsMap
	/// \param box 格网的box(设置建议和map制图的范围一致)
	GsGridElement(GsMap* ptrMap, GsBox box);
	/// \brief 构造格网制图
	/// \param double dblXOrigin x轴起始点(经纬度)
	/// \param double dblYOrigin y轴起始点(经纬度)
	/// \param double XIntervalSize x轴起间距(经纬度)
	/// \param double YIntervalSize  y轴起间距(经纬度)
	/// \param ptrMap 绑定一个GsMap
	/// \param box 格网的box(设置建议和map制图的范围一致)
	GsGridElement(double dblXOrigin, double dblYOrigin, double XIntervalSize, double YIntervalSize, GsMap* ptrMap, GsBox box);

	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();
	/// \brief 格网线的符号
	GsLineSymbol* LineSymbol();
	/// \brief 外边框符号
	GsLineSymbol* BorderSymbol();
	/// \brief 文字标注符号
	GsTextSymbol* TextSymbol();
	/// \brief label分段显示文字标注符号
	GsTextSymbol* DecimalDivisionalTextSymbol();

	/// \brief 设置格网线的符号
	void LineSymbol(GsLineSymbol* pLineSymbol);
	/// \brief 设置外边框符号
	void BorderSymbol(GsLineSymbol* pLineSymbol);
	/// \brief 设置文字标注符号
	void TextSymbol(GsTextSymbol* pTextSymBol);
	/// \brief label分段显示的文字标注符号
	void DecimalDivisionalTextSymbol(GsTextSymbol* pTextSymbol);
	///// \brief label分段显示符号位置
	//int TextBreakPoint();

	/// \brief 设置格网显示风格
	void GridLineShowType(GsGridLineShowType eGridLineType);
	/// \brief 获取格网显示风格
	GsGridLineShowType GridLineShowType();
	/// \brief 设置四个方向是否显示标注
	void GridLabelAxes(GsGridOrientation eGridLabelAxes);
	/// \brief 获取四个方向是否显示标注
	GsGridOrientation GridLabelAxes();
	/// \brief 设置文字显示方向
	void GridLabelOrientation(GsGridLabelOrientation eGridLabelAxes);
	/// \brief 获取文字显示方向
	GsGridLabelOrientation GridLabelOrientation();
	/// \brief 设置文字的样式
	void GridLabelStringFormat(GsGridLabelStringFormat eGridLabelAxes);
	/// \brief 获取文字的样式
	GsGridLabelStringFormat GridLabelStringFormat();

	/// \brief 设置是否根据Map范围变化调整格网绘制原点,默认位false,如果为true,地图范围变化将根据地图范围动态改变原点
	void FixedOrigin(bool bFixedOrigin);
	/// \brief 获取是否根据Map范围变化调整格网绘制原点,默认位false,如果为true,地图范围变化将根据地图范围动态改变原点
	bool FixedOrigin();

	/// \brief 设置四边是否绘制主刻度
	void MainScalePosition(GsGridScalePosition ePos);
	/// \brief 获取四边是否绘制主刻度
	GsGridScalePosition MainScalePosition();
	/// \brief 设置主刻度的位置，格网外还是格网内
	void MainScaleOrientation(GsGridOrientation eOri);
	/// \brief 获取主刻度的位置，格网外还是格网内
	GsGridOrientation MainScaleOrientation();
	/// \brief 主刻度的长度
	double MainScaleLength();
	/// \brief 主刻度的长度
	void MainScaleLength(double len);

	/// \brief 设置四边是否绘制分刻度
	void DivisionScalePosition(GsGridScalePosition ePos);
	/// \brief 获取四边是否绘制分刻度
	GsGridScalePosition DivisionScalePosition();
	/// \brief 设置分刻度的位置，格网外还是格网内
	void DivisionScaleOrientation(GsGridOrientation eOri);
	/// \brief 获取分刻度的位置，格网外还是格网内
	GsGridOrientation DivisionScaleOrientation();
	/// \brief 分刻度的长度
	double DivisionScaleLength();
	/// \brief 分刻度的长度
	void DivisionScaleLength(double len);
	/// \brief 获取分刻度将主刻度切分的数量
	int DivisionScaleCount();
	/// \brief 设置分刻度将主刻度切分的数量
	void DivisionScaleCount(int count);

	/// \brief 获取x间距
	double XIntervalSize();
	/// \brief 设置x间距
	void XIntervalSize(double size);
	/// \brief 设置x间距
	void XIntervalSize(double degree, double minute, double second);
	/// \brief x原点
	double XOrigin();
	/// \brief x原点
	void XOrigin(double origin);

	/// \brief 获取Y间距
	double YIntervalSize();
	/// \brief 设置Y间距
	void YIntervalSize(double size);
	/// \brief 设置Y间距
	void YIntervalSize(double degree, double minute, double second);
	/// \brief Y原点
	double YOrigin();
	/// \brief Y原点
	void YOrigin(double origin);
	/// \brief 记录格网显示的最大范围，默认是经度-180到180 维度-90到90，如果是0,0,0,0则无限制
	GsBox Range();
	/// \brief 记录格网显示的最大范围，默认是经度-180到180 维度-90到90，如果是0,0,0,0则无限制
	void Range(GsBox box);

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

};
GS_SMARTER_PTR(GsGridElement);
DECLARE_CLASS_CREATE(GsGridElement);

/// \brief 比例尺
class GS_API GsScaleBarElement :public GsGeometryElement
{
	/// \brief 记录矩形的大小
	GsBox m_Box;
	/// \brief 比例尺文字符号
	GsTextSymbolPtr m_ptrTextSymbol;
	/// \brief 比例尺单位符号
	GsTextSymbolPtr m_ptrTextSymbolUnit;

	/// \brief 比例尺单位显示的文本
	Utility::GsString m_sUnit;
	/// \brief 比例尺单位类型
	GsUnits m_eUnit;

	/// \brief 单位左边的间隙
	double m_Gap;
	/// \brief 比例尺单位元素
	GsTextElementPtr m_TextElementUnit;
	/// \brief 底部线符号
	GsSymbolPtr m_BottomLineSymbol;
	/// \brief 上面的线符号
	GsSymbolPtr m_ScaleLineSymbol;

	/// \brief 竖直线的长度， Big 是比较长的竖直线。
	double m_dfBigBarHeight;
	/// \brief 竖直线的长度， Small 是比较短的竖直线。 
	double m_dfSmallBarHeight;

	/// \brief  级别数量
	int m_nLevel;
	/// \brief 单个级别被切分的数量
	int m_nDivisions;

	/// \brief 单个级别的实际绘制宽度
	double m_dfDivisionsValue;

	/// \brief 左端点的刻度。默认值为0
	int m_nMinScale;
	/// \brief 右端点的刻度。
	int m_nMaxScale;

	/// \brief 记录比例尺绑定的Map。
	GsMapPtr m_PtrMap;

public:
	GsScaleBarElement();
	GsScaleBarElement(double x, double y, GsMap* pMap = NULL);

	~GsScaleBarElement();

	/// \brief  获取底部线符号
	GsSymbolPtr BottomLineSymbol();
	/// \brief  获取底部线符号
	void BottomLineSymbol(GsSymbol* symbol);

	/// \brief  返回级别数量
	int Level();
	/// \brief  设置级别数量
	void Level(int nLevel);

	/// \brief  返回单个级别被切分的数量
	int Division();
	/// \brief  设置单个级别被切分的数量
	void Division(int division);

	/// \brief 设置比例尺的单位 
	void Unit(const char* unit);
	/// \brief 获取比例尺的单位
	Utility::GsString Unit();

	/// \brief 设置比例尺的单位 
	void UnitType(GsUnits eUnit);
	/// \brief 获取比例尺的单位
	GsUnits UnitType();

	/// \brief 设置比例尺绘制的长度(底部水平线的长度)
	void ScaleLength(double len);
	/// \brief 获取比例尺绘制的长度(底部水平线的长度)
	double ScaleLength();

	/// \brief 设置比例尺中最小刻度
	void MinScale(int nMinScale);
	/// \brief 获取比例尺中最小刻度
	int MinScale();
	/// \brief 设置比例尺中最大刻度
	void MaxScale(int nMaxScale);
	/// \brief 获取比例尺中最大刻度
	int MaxScale();

	/// \brief 获取单位左边的间隙
	double Gap();
	/// \brief 设置单位左边的间隙
	void Gap(double gap);

	/// \brief 获取绑定的Map
	GsMapPtr Map();
	/// \brief 设置绑定的Map
	void Map(GsMap* map);

	/// \brief 设置显示的刻度文字符号
	void TextSymbol(GsTextSymbol* ptrTextSym);	
	/// \brief 获取显示的刻度文字符号
	GsTextSymbolPtr TextSymbol();

	/// \brief 设置单位的文字符号
	void TextSymbolUnit(GsTextSymbol* ptrTextSym);
	/// \brief 获取单位的文字符号
	GsTextSymbolPtr TextSymbolUnit();

	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

protected:
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	/// \brief 拷贝
	virtual void Assign(GsScaleBarElement* pEle);

private:
	
	void DrawLine(GsDisplay* pDisp, const GsRawPoint& point1, const GsRawPoint& point2, GsSymbol* pSymbol = NULL);
	void InitScaleElementBox(double x, double y);
	void InitScale();
};
GS_SMARTER_PTR(GsScaleBarElement);
DECLARE_CLASS_CREATE(GsScaleBarElement);

/// \brief 三北方向
class GS_API GsThreeNorthElement :public GsGeometryElement
{
	/// \brief 记录矩形的大小
	GsBox m_Box;
	GsRawPoint m_pointLeftTop;//磁子午线左上点。
	GsRawPoint m_pointRightTop;//坐标纵线右上点。
	GsRawPoint m_pointBottonCenter; //三北线底部交叉点，真子午线最低点。
	GsRawPoint m_pointTopCenter; //真子午线最高点,五角星所在的位置。

	double m_dfLineLength;//三个北线的长度
	double m_dfAsteriskSize;//五角星大小
	double m_dfArrowSize;//箭头大小
	double m_dfGraphAngle;//倾斜度数,单位度
	double m_dfMagnetAngle;//磁偏角
	double m_dfMeridianAngle;//子午线收敛角
	double m_dfArcFirstSpace;//第一个拱形(下面那个拱形，一个箭头)的半径。以m_pointBottonCenter为圆心
	double m_dfArcSecondSpace;//第二个拱形(上面那个拱形，二个箭头)的半径。以m_pointBottonCenter为圆心
	/// \brief 拱形虚线符号
	GsCartographicLineSymbolPtr m_pArcDottedLine;
	/// \brief 三个子午线的线符号。
	GsLineSymbolPtr m_pLineSym;
	/// \brief 三个子午线的标注符号。
	GsTextSymbolPtr m_pTextNameSym;
	/// \brief 三个子午线的角度标注符号。
	GsTextSymbolPtr m_pTextAngleSym;

public:
	GsThreeNorthElement();
	GsThreeNorthElement(double x, double y);
	~GsThreeNorthElement();

	/// \brief 设置三个子午线的线符号。
	void LineSymbol(GsSymbol * pSym);
	/// \brief 获取三个子午线的线符号。
	GsSymbolPtr LineSymbol();

	/// \brief 设置三个子午线的线段长度，单位mm
	void LineLength(double dfLength);
	/// \brief 获取三个子午线的线段长度，单位mm
	double LineLength();

	/// \brief 磁偏角。(标注显示的角度值)
	void MagnetAngle(double dfAngle);
	double MagnetAngle();
	
	/// \brief 子午线收敛角。(标注显示的角度值)
	void MeridianAngle(double dfAngle);
	double MeridianAngle();
	
	/// \brief 绘制三北方向时的夹角。(因为三北方向线在绘制时的夹角是示意图，并不是实际的角度值),设置磁子午线和真子午线绘图的夹角，同时设置坐标纵线和真子午线绘图的夹角，两个夹角相等。
	void GraphAngle(double dfAngle);
	/// \brief 获取三北方向时的夹角,获取磁子午线和真子午线绘图的夹角。
	double GraphAngle();

	/// \brief 设置五角星外接圆的半径。
	void AsteriskSize(double dfSize);
	/// \brief 获取五角星外接圆的半径。
	double AsteriskSize();

	/// \brief 设置箭头的长度
	void ArrowSize(double dfSize);
	/// \brief 获取箭头的长度
	double ArrowSize();

	/// \brief 设置拱形虚线符号。
	void ArcDottedLine(GsCartographicLineSymbol * pSym);
	/// \brief 获取拱形虚线符号。
	GsCartographicLineSymbolPtr ArcDottedLine();

	/// \brief 设置名称标注的符号。
	void NameLabelSymbol(GsTextSymbol * pSym);
	/// \brief 获取名称标注的符号。
	GsTextSymbolPtr NameLabelSymbol();

	/// \brief 设置角读标注的符号。
	void AngleLabelSymbol(GsTextSymbol * pSym);
	/// \brief 获取角读标注的符号。
	GsTextSymbolPtr AngleLabelSymbol();

	/// \brief 获取大小
	virtual Utility::GsSizeF Size();
	/// \brief 设置大小
	virtual void Size(const Utility::GsSizeF & sizef);
	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

protected:
	virtual void InnerDraw(GsDisplay* pDisp, GsTrackCancel* pCancel, GsDrawPhase drawType);
	/// \brief 拷贝
	virtual void Assign(GsThreeNorthElement* pEle);

private:
	/// \brief 绘制直线段
	/// \param point1 端点1
	/// \param point2 端点2
	/// \param pSymbol 绘制线段的符号，默认：简单线黑色
	void DrawLine(GsDisplay* pDisp, const GsRawPoint& point1, const GsRawPoint& point2, GsSymbol* pSymbol = NULL);

	/// \brief 绘制拱形虚线
	/// \param pointFrom 起始点
	/// \param pointMiddle 中间点
	/// \param pointEnd 结束点
	void DrawArcDottedLine(GsDisplay* pDisp, const GsRawPoint& pointFrom, const GsRawPoint& pointMiddle, const GsRawPoint& pointEnd);

	/// \brief 绘制箭头
	/// \param point 基准点
	/// \param dfAngle 偏转角
	/// \param length 箭头的长度
	void DrawArrow(GsDisplay* pDisp, const GsRawPoint& point, double dfAngle, double length);

	/// \brief 绘制五角星
	/// \param point 五角星的中心点
	/// \param length 五角星的中心点到顶点的最远距离
	void DrawAsterisk(GsDisplay* pDisp, const GsRawPoint& point, double length);

	/// \brief 初始化四个点的位置和重新初始化m_box
	void InitBox(double x, double y);

};
GS_SMARTER_PTR(GsThreeNorthElement);
DECLARE_CLASS_CREATE(GsThreeNorthElement);


/// \brief GsLegend为整个图例的对象,内包含所有绑定地图的图例信息和对象
/// \details GsLegendFormat为GsLegend样式信息描述
class GS_API GsLegendsFormat :public Utility::GsRefObject
{
private:
	/// \brief  legendgroup直接的垂直距离
	double m_dGroupGap;
	/// \brief  标题和图例之间的垂直距离
	double m_dHeadingGap;
	/// \brief  图例的水平距离，用于不止1列的图例
	double m_dHorizontalItemGap;
	/// \brief  图例画的符号与文字之间的水平距离
	double m_dHorizontalPatchGap;
	/// \brief  层名称与图示图形之间的垂直距离
	double m_dLayerNameGap;
	/// \brief  标题是否是显示
	bool m_bShowTitle;
	/// \brief  标签和描述之间的水平距离。labels and descriptions.
	double m_dTextGap;
	/// \brief 标题和第一个GsLegendGroup之间的垂直距离。
	double m_dTitleGap;
	/// \brief  标题的位置
	GsRectanglePosition m_eRectanglePosition;
	/// \brief  标题文本所使用的GsTextSymbol
	GsTextSymbolPtr m_ptrTitleSymbol;
	/// \brief  GsLegendGroup之间的垂直距离
	double m_dVerticalItemGap;
	/// \brief  图块之间的垂直距离。LegendClass之间的距离
	double m_dVerticalPatchGap;
	/// \brief  图块的高度
	double m_dDefaultPatchHeight;
	/// \brief  图块的宽度
	double m_dDefaultPatchWidth;	
	/// \brief  单列图例的宽度
	double m_dRowWidth;

public:
	GsLegendsFormat();
	~GsLegendsFormat();
	/// \brief  legendgroup之间的距离
	double GroupGap();
	void  GroupGap(double dblGap);
	/// \brief  标题和图例之间的垂直距离
	double HeadingGap();
	void  HeadingGap(double dblGap);
	/// \brief  图例的水平距离，用于不止1列的图例
	double HorizontalItemGap();
	void  HorizontalItemGap(double dblGap);
	/// \brief  图例画的符号与文字之间的水平距离
	double HorizontalPatchGap();
	void  HorizontalPatchGap(double dblGap);

	/// \brief  层名称与图示图形之间的垂直距离
	double LayerNameGap();
	void  LayerNameGap(double dblGap);
	/// \brief  标题是否是显示
	bool ShowTitle();
	void  ShowTitle(bool bShowTitle);

	/// \brief  标签和描述之间的水平距离。labels and descriptions.
	double TextGap();
	void  TextGap(double dblGap);

	/// \brief 标题和第一个GsLegendGroup之间的垂直距离。
	double TitleGap();
	void  TitleGap(double dblGap);
	/// \brief  标题的位置
	GsRectanglePosition TitlePosition();
	void TitlePosition(GsRectanglePosition eType);
	/// \brief  标题文本所使用的GsTextSymbol
	GsTextSymbol* TitleSymbol();
	void TitleSymbol(GsTextSymbol* pSym);

	/// \brief  GsLegendGroup之间的垂直距离
	double VerticalItemGap();
	void  VerticalItemGap(double dblGap);
	/// \brief  图块之间的垂直距离。LegendClass之间的距离
	double VerticalPatchGap();
	void  VerticalPatchGap(double dblGap);

	/// \brief  图块的高度
	double DefaultPatchHeight();
	void DefaultPatchHeight(double dHeight);
	/// \brief  图块的宽度
	double DefaultPatchWidth();
	void DefaultPatchWidth(double dWidth);
	/// \brief  单列图例的宽度
	double RowWidth();
	void RowWidth(double dRowWidth);
	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsLegendsFormat> Clone();
};
GS_SMARTER_PTR(GsLegendsFormat);

/// \brief 图例
/// \brief GsLegend为整个图例的对象,内包含所有绑定地图的图例信息和对象
class GS_API GsLegendsElement :public GsGeometryElement  
{
	Utility::GsVector<GsLegendGroupPtr > m_lstLegendGroup;
	/// \brief  当新的层添加到映射时是否应添加新项
	bool m_bAutoAdd;
	/// \brief  图例项是否应保持与图层相同的顺序
	bool m_bAutoReorder;
	/// \brief  表明如果items should be shown只有当相关层是可见的。
	bool m_bAutoVisibility;
	/// \brief  保留以备将来使用。
	bool m_bFlowRight;
	/// \brief  样式信息描述
	GsLegendsFormatPtr m_ptrLegendsFormat;
	/// \brief  题图map
	GsMapPtr m_ptrMap;
	/// \brief  图例标题
	Utility::GsString m_strTitle;
	/// \brief  图例标题高度
	double m_dTitleHeight;
	/// \brief  图例标题宽度
	double m_dTitleWidth;
	/// \brief  画图例的起始点位置
	GsRawPoint m_ptRawPoint;
	/// \brief 外框图廓
	GsGeometryPtr m_ptrBorderGeometry; 
	/// \brief 图廓符号
	GsSymbolPtr m_ptrBorderSymbol; 	

	/// \brief  边距暂不对外开接口
	/// \brief  上边距
	double m_dUpDistance;
	/// \brief  下边距
	double m_dDownDistance;
	/// \brief  左边距
	double m_dLeftDistance;
	/// \brief  右边距
	double m_dRightDistance;

	/// \brief  图例所有的element
	Utility::GsVector<GsElementPtr> m_vecElements;

public:
 	GsLegendsElement();
	GsLegendsElement(double x,double y,GsMap* pMap);
	~GsLegendsElement();

	/// \brief  得到GsLegendGroup的集合
	Utility::GsVector<GsLegendGroupPtr >* ListLegendGroup();

	/// \brief  得到GsLegendFormat为GsLegend样式描述
	GsLegendsFormat* Format();
	void Format(GsLegendsFormat* pFormat);

	/// \brief  得到Map
	GsMap* Map();
	void Map(GsMap *pMap);

	/// \brief  图例的标题
	Utility::GsString Title();
	void Title(const char * strTitle);

	void Draw(GsDisplay*pDisplay, GsTrackCancel *pTrackCancel, GsEnvelope*pBounds);

	/// \brief  刷新
	void Refresh();

	/// \brief  通过 GsElement 继承
	virtual void InnerDraw(GsDisplay * pDisp, GsTrackCancel * pCancel, GsDrawPhase drawType) override;

	/// \brief 设置图廓符号
	void BorderSymbol(GsSymbol* pSym);
	/// \brief 得到图廓符号
	GsSymbol* BorderSymbol();

	/// \brief  当新的层添加是否应添加新项,同时图例与图层顺序相同
	bool AutoAdd();
	void AutoAdd(bool pAutoAdd);

	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsElement> Clone();

	/// \brief 转换为组合元素的集合
	GsElementPtr ConvertToGraphics();

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;



protected:

	/// \brief  图例项是否应保持与图层相同的顺序
	bool AutoReorder();
	void AutoReorder(bool pAutoReorder);

	/// \brief  表明如果items should be shown只有当相关层是可见的。
	bool AutoVisibility();
	void AutoVisibility(bool pAutoVisibility);
	
	/// \brief  保留以备将来使用。
	bool FlowRight();
	void FlowRight(bool pFlowRight);

	/// \brief  图例的显示范围
	void QueryBounds(GsDisplay*pDisplay, GsTrackCancel *pTrackCancel, GsEnvelope*pBounds);


	/// \brief  根据m_pLegendGroup初始化绘制信息
	void InitElements();
	/// \brief  计算移动缩放后的各个尺寸及间距
	void InitLegendsFormatGap();
	/// \brief 初始化布局
	void InitLayout();
	/// \brief 对单个GsLegendGroup布局，ptPoint为布局起点
	void InitLegendGroupLayout(GsRawPoint* pPoint, GsLegendGroup* pLegendGroup, GsBox* pBoxFrame);
	/// \brief 拷贝
	virtual void Assign(GsLegendsElement* pEle);
	/// \brief 根据layer增加m_lstLegendGroup,增加处理GsMultiLayer的情况
	void AddLegendGroupByLayer(GsLayer* pLayer);

};
GS_SMARTER_PTR(GsLegendsElement);
DECLARE_CLASS_CREATE(GsLegendsElement);

KERNEL_ENDNS
