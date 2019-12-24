#pragma once
#include "symbol.h"
#include "linesymbol.h"

KERNEL_NS    
/// \brief 简单面符号
class GS_API GsSimpleFillSymbol:public GsFillSymbol
{
	
	GsSolidBrushPtr m_ptrBrush;
	GsBrushStyle m_Style;
protected:

	virtual void OnDraw(GsGraphicsPath* pPath,GsGeometryBlob * pBlob);

	virtual void OnStartDrawing();
	virtual void OnEndDrawing();
public:
	GsSimpleFillSymbol();
	GsSimpleFillSymbol(const GsColor& c);
	virtual ~GsSimpleFillSymbol();
	
	
	/// \brief 符号是否有效
	/// \details 符号是否会在画布上绘制内容，如果符号不会在画布上绘制任何的内容则不需要执行绘制过程。
	virtual bool IsValid();

	/// \brief 画笔的式样
	GsBrushStyle  Style();
	/// \brief 画笔的式样
	void Style(GsBrushStyle eStyle);
};
GS_SMARTER_PTR(GsSimpleFillSymbol);

DECLARE_CLASS_CREATE(GsSimpleFillSymbol);


/// \brief 点符号填充式样
enum GsPointFillStyle
{
	
    /// \brief 格网填充
	ePFS_Grid = 0,
    /// \brief 品形填充
	ePFS_Triangle = 1,
    /// \brief 随机填充
	ePFS_Random = 2,
    /// \brief Label点填充
	ePFS_LabelPoint = 3
};


/// \brief 点填充符号
class GS_API GsPointFillSymbol:public GsFillSymbol
{
	GsPointSymbolPtr m_ptrPointSymbol;
	GsPointFillStyle m_eStyle;
	double m_dblGridAngle;
	GsRawPoint m_Offset;
	double m_dblXSeparation,m_dblYSeparation;
	Utility::GsImagePtr  m_ptrImage;
	GsTextureBrushPtr m_ptrTexture;
	double m_dblPointAngle;

protected:
	virtual void OnDraw(GsGraphicsPath* pPath,GsGeometryBlob * pBlob);
	virtual void DrawRandomPoint(GsGraphicsPath* pPath,geostar::gobjptr &pGeoPath);
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();
	virtual void DrawCentralPoint(GsGraphicsPath* pPath,geostar::gobjptr &pGeoPath);
protected:
	
	/// \brief 点符号生成位图
	bool CreateBitmap();

public:
	GsPointFillSymbol();
	GsPointFillSymbol(GsPointSymbol* pPt,GsPointFillStyle eStyle);
	
	/// \brief 获取点符号
	GsPointSymbol* PointSymbol();
	/// \brief 设置点符号
	void PointSymbol(GsPointSymbol* pt);
	/// \brief 获取点符号填充式样
	GsPointFillStyle Style();
	/// \brief 设置点符号填充式样
	void Style(GsPointFillStyle eStyle);

	/// \brief 获取格网填充时格网的角度
	double GridAngle();
	/// \brief 获取格网角度
	void GridAngle(double angle);

	/// \brief 偏移
	GsRawPoint Offset();
	/// \brief 偏移
	void Offset(const GsRawPoint& offset);

	/// \brief x方向上间隔
	double XSeparation();
	/// \brief x方向上间隔
	void XSeparation(double val);

	/// \brief y方向上间隔
	double YSeparation();
	/// \brief y方向上间隔
	void YSeparation(double val);

	/// \brief 获取图片
	Utility::GsImage* Picture();

	/// \brief 符号是否有效
	/// \details 符号是否会在画布上绘制内容，如果符号不会在画布上绘制任何的内容则不需要执行绘制过程。
	virtual bool IsValid();
};
GS_SMARTER_PTR(GsPointFillSymbol);
DECLARE_CLASS_CREATE(GsPointFillSymbol);

/// \brief 线填充符号
class GS_API GsLineFillSymbol:public GsFillSymbol
{
	double m_dblOffset;
	double m_dblAngle;
	GsLineSymbolPtr m_ptrLineSymbol;
	double m_dblSeparation;
	GsTextureBrushPtr m_ptrTexture;
	GsPenPtr m_ptrPen;
//	GsColor m_color;
public:
	GsLineFillSymbol();
	GsLineFillSymbol(GsLineSymbol* pLineSymbol);
	/// \brief 获取线填充的角度
	double Angle();
	/// \brief 设置线填充的角度
	void Angle(double angle);

	/// \brief 获取线符号
	GsLineSymbol* LineSymbol();
	/// \brief 设置线符号
	void LineSymbol(GsLineSymbol* pLine);

	/// \brief 获取线填充的间隔
	double Separation();
	/// \brief 设置线填充的间隔
	void Separation(double dblValue);
	
	/// \brief 线的偏移
	double Offset();
	/// \brief 线的偏移
	void Offset(double off);
protected:
	virtual void OnDraw(GsGraphicsPath* pPath,GsGeometryBlob *pBlob);

	virtual void OnStartDrawing();
	virtual void OnEndDrawing();
	void CreateBitmapBrush(GsSymbolPtr ptrSym,double dblWidth,double dblHeight);
	/// \brief 符号是否有效
	/// \details 符号是否会在画布上绘制内容，如果符号不会在画布上绘制任何的内容则不需要执行绘制过程。
	virtual bool IsValid();

};
GS_SMARTER_PTR(GsLineFillSymbol);
DECLARE_CLASS_CREATE(GsLineFillSymbol);


/// \brief 图片填充符号
class GS_API GsPictureFillSymbol:public GsFillSymbol
{
	GsRawPoint m_Offset;
	double m_dblXSeparation, m_dblYSeparation;
	double m_dblXScale, m_dblYScale;

	Utility::GsGrowByteBuffer  m_ImageData;
	Utility::GsImagePtr        m_ptrImage;
	GsTextureBrushPtr          m_ptrTextureBrush;
	/// \brief 背景色
	GsColor                    m_BackColor;
	/// \brief 透明色,显示时会将此颜色设为透明
	GsColor                    m_TransparentColor;
	double                     m_dblAngle;

public:
	GsPictureFillSymbol();
	GsPictureFillSymbol(Utility::GsImage* pImg);
	
	/// \brief 获取图片的旋转
	double Angle();
	/// \brief 设置图片的旋转
	void Angle(double angle);

	/// \brief 获取图片
	Utility::GsImage* Picture();
	/// \brief 设置图片
	void Picture(Utility::GsImage* img);

	/// \brief 背景色
	GsColor  BackColor();
	/// \brief 透明色
	void BackColor(const GsColor& color);

	/// \brief 透明色
	GsColor TransparentColor();
	/// \brief 设置线填充的间隔
	void TransparentColor(const GsColor& color);

	/// \brief x方向上的缩放比率
	double XScale();
	/// \brief x方向上的缩放比率
	void XScale(double scale);

	/// \brief y方向上的缩放比率
	double YScale();
	/// \brief y方向上的缩放比率
	void YScale(double scale);

	/// \brief 从文件载入图片
	void LoadPicture(const char* strFile);
	/// \brief 从内存载入图片
	void LoadPicture(const unsigned char* blob,int nLen);
	
	/// \brief 是否平铺
	bool Tile();
	/// \brief 是否平铺
	void Tile(bool bVal);
	
	/// \brief 偏移
	GsRawPoint Offset();
	/// \brief 偏移
	void Offset(const GsRawPoint& offset);
	
	/// \brief x方向上间隔
	double XSeparation();
	/// \brief x方向上间隔
	void XSeparation(double val);

	/// \brief y方向上间隔
	double YSeparation();
	/// \brief y方向上间隔
	void YSeparation(double val);

	/// \brief 符号是否有效
	/// \details 符号是否会在画布上绘制内容，如果符号不会在画布上绘制任何的内容则不需要执行绘制过程。
	virtual bool IsValid();

protected:
	void InitProperty();

	virtual void OnDraw(GsGraphicsPath* pPath,GsGeometryBlob * pBlob);
	/// \brief 当开始绘制的时候发生
	virtual void OnStartDrawing();
	/// \brief 当结束绘制时发生
	virtual void OnEndDrawing();

};
GS_SMARTER_PTR(GsPictureFillSymbol);
DECLARE_CLASS_CREATE(GsPictureFillSymbol);


enum GsGradsFillStyle
{
    eGFS_LINEAR = 1,
    eGFS_RECTANGULAR = 2,
    eGFS_CIRCULAR = 3,
    eGFS_GFSBUFFERED = 4
};
enum GsColorGradient
{
	eAllGradient =1,
	eToneGradient=2,
	eSaturationGradient=3,
	ePurityGradient=4,
};
/// \brief 渐变填充符号
class GS_API GsGradsFillSymbol:public GsFillSymbol
{
private:	
	int m_Interval;
	double m_Percentage;

	double m_GradsAngle;
	GsGradsFillStyle m_Style;
	GsColorGradient m_ColorGradient;
	GsGradsColor *m_pGradsColor;

	GsColor m_StartColor;
	GsColor m_EndColor;

	GsLinearGradientBrushPtr m_ptrLinearGradientBrush;
	GsRadialGradientBrushPtr m_ptrRadialGradientBrush;

public:
	GsGradsFillSymbol();
	GsGradsFillSymbol(GsGradsColor* grads,GsGradsFillStyle eStyle);
	GsGradsFillSymbol(GsColor& sc,GsColor& ec,GsGradsFillStyle eStyle);

	/// \brief 渐变色填充的角度
	void GradsAngle(double angle);
	/// \brief 渐变色填充的角度
	double GradsAngle();
	
	/// \brief 渐变色填充的式样
	GsGradsFillStyle Style();
	/// \brief 渐变色填充的式样
	void Style(GsGradsFillStyle eStyle);

	/// \brief 渐变色填充
	GsColorGradient ColorGradientStyle();
	/// \brief 渐变色填充
	void ColorGradientStyle(GsColorGradient eStyle);
	/// \brief 渐变色填充的比率
	double Percentage();
	/// \brief 渐变色填充的比率
	void Percentage(double val);
	
	/// \brief 渐变色填充的间隔
	int Interval();
	/// \brief 渐变色填充的间隔
	void Interval(int val);


	/// \brief 渐变色
	GsColor StartColor();
	/// \brief 渐变色
	void StartColor(GsColor &c);
	/// \brief 渐变色
	GsColor EndColor();
	/// \brief 渐变色
	void EndColor(GsColor &c);

	/// \brief 渐变色
	GsGradsColor* GradsColor();
	/// \brief 渐变色
	void GradsColor(GsGradsColor* grads);

	/// \brief 符号是否有效
	/// \details 符号是否会在画布上绘制内容，如果符号不会在画布上绘制任何的内容则不需要执行绘制过程。
	bool IsValid();

protected: 
	void StartAndEndPoint(GsBox &env_in, GsRawPoint& startPoint, GsRawPoint& endPoint);
	void RectGradsFill(geostar::gobjptr& pPath);

	/// \brief 当绘制的时候发生
	/// \details 子类通过覆盖此方法实现实际的绘制
	virtual void OnDraw(GsGeometryBlob * pBlob);
	virtual void OnDraw(GsGraphicsPath* pPath, GsGeometryBlob *pBlob);


	/// \brief 当开始绘制的时候发生
	virtual void OnStartDrawing();
	/// \brief 当结束绘制时发生
	virtual void OnEndDrawing();
};
GS_SMARTER_PTR(GsGradsFillSymbol);
DECLARE_CLASS_CREATE(GsGradsFillSymbol);

/// \brief 建筑符号
class GS_API GsBuildingFillSymbol:public GsFillSymbol
{
public:
	/// \brief 立体房屋的高度获取方式
	enum GsExtrusionHeightMode
	{
		/// \brief 不设置
		NONE=0,
		/// \brief 常数
		CONSTANTNUM,
		/// \brief 字段
		FIELDKEY,
		/// \brief 脚本计算
		AUTOCAL
	};
private:
	int m_iHeight;
	Utility::GsString m_iHeightKey;
	GsExtrusionHeightMode m_iHeightMode;
	int m_iAngle;
	GsBrushStyle m_Style;
	GsSolidBrushPtr m_ptrBrush;
	GsSimpleFillSymbolPtr m_ptrWallFillSym;
	GsSimpleLineSymbolPtr m_ptrWallLineSym;
	GsSimpleFillSymbolPtr m_ptrTopFillSym;
public:
	GsBuildingFillSymbol();

	/// \brief 视角
	int ViewAngle();
	/// \brief 视角
	void ViewAngle(int iAngle);

	/// \brief 基础，必须要小于或者等于高度
	int ExtrusionBase();

	/// \brief 获取高度的方式
	GsExtrusionHeightMode ExtrusionHeightMode();
	/// \brief 高度(地理距离,单位米)
	int ExtrusionHeight();
	/// \brief 高度(地理距离,单位米)
	void ExtrusionHeight(int iHeight);
	/// \brief 高度字段
	Utility::GsString ExtrusionHeightKey();
	/// \brief 高度字段
	void ExtrusionHeightKey(Utility::GsString HeightKey);

	virtual GsSymbolType Type();	

	/// \brief 画笔的式样
	GsBrushStyle  Style();
	/// \brief 画笔的式样
	void Style(GsBrushStyle eStyle);
	
	/// \brief 符号是否有效
	/// \details 符号是否会在画布上绘制内容，如果符号不会在画布上绘制任何的内容则不需要执行绘制过程。
	virtual bool IsValid();

	/// \brief 绘制顶部
	virtual void DrawTop(GsGeometry* pGeo);

protected:
	virtual void OnDraw(GsGraphicsPath* pPath,GsGeometryBlob * pBlob);
	/// \brief 当开始绘制的时候发生
	virtual void OnStartDrawing();
	/// \brief 当结束绘制时发生
	virtual void OnEndDrawing();

protected:
	bool IsViewVisible(double x1, double y1, double x2, double y2, int iAngle, bool bClockWise);	
	void DrawPolygon(Utility::GsVector<double>& vecCoord, GsFillSymbol* ptrSym);
};
GS_SMARTER_PTR(GsBuildingFillSymbol);
DECLARE_CLASS_CREATE(GsBuildingFillSymbol);

KERNEL_ENDNS