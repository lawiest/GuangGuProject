#pragma once
#include "../utility/preconfig.h" 
#include "geometry.h"
#include "spatialreference.h"
#include "canvas.h" 
#include <image.h>
#include <wordanalyzer.h>
#include "serialize.h"
KERNEL_NS   
/// \brief 符号的类型
enum GsSymbolType
{
	/// \brief 点符号
	ePointSymbol,
	/// \brief 线符号
	eLineSymbol,
	/// \brief 面符号
	eFillSymbol,
	/// \brief 文本符号
	eTextSymbol,

};
  

/// \brief 显示坐标转换对象
/// \details 用于实现设备和地图之间的坐标以及长度单位的转换
class GS_API GsDisplayTransformation:public Utility::GsRefObject
{
	/// \brief 参考比例尺
	double					m_RefScale;
	
	/// \brief 地图范围
	GsBox					m_mapExtent;
	/// \brief 设备范围
	Utility::GsRect			m_deviceExtent;

	/// \brief 分辨率，每个像素代表多少地理长度
	double m_dblResolution;
	/// \brief 地图比例尺。
	double m_dblScale;

	/// \brief 设备分辨率，缺省为96
	float m_fDPI;
	
	/// \brief 设备空间参考
	GsSpatialReferencePtr m_ptrSR;

	
	/// \brief 计算单位相对于米的比例。
	double UnitScale(GsUnits eUnit);
public:	
	/// \brief 从设备范围和地图范围构造
	GsDisplayTransformation(const GsBox& mapExtent,const Utility::GsRect& deviceExtent);
	virtual ~GsDisplayTransformation();
/// \brief 当前空间参考
	GsSpatialReference* SpatialReference();
	
	/// \brief 设置当前空间参考
	void SpatialReference(GsSpatialReference* sr);

	/// \brief 地图范围
	GsBox MapExtent() const;
	/// \brief 设置地图范围
	void MapExtent(const GsBox& extent);
	
	/// \brief 设备分辨率
	float DPI();
	/// \brief 设置设备分辨率
	void DPI(float dpi);
	
	/// \brief 地图分辨率
	double Resolution();
	
	/// \brief 设置地图分辨率
	void Resolution(double res);

	/// \brief 设备范围
	Utility::GsRect DeviceExtent()const ;
	/// \brief 设置范围
	void DeviceExtent(const Utility::GsRect& extent);

	/// \brief 转换设备坐标到地图坐标
	void ToMap(float x,float y,double& mapx,double& mapy);
	/// \brief 转换设备坐标到地图坐标
	void ToMap(int x,int y,double& mapx,double& mapy);
	/// \brief 转换设备坐标到地图坐标
	void ToMap(float* pScreenCoord,int nCount,int nDim,double* pMapCoord);
	/// \brief 转换设备坐标到地图坐标
	void ToMap(int* pScreenCoord,int nCount,int nDim,double* pMapCoord);
	
	/// \brief 转换设备坐标到地图坐标
	GsBox ToMap(const Utility::GsRect& rect);
	/// \brief 转换设备坐标到地图坐标
	GsBox ToMap(const Utility::GsRectF& rect);
	/// \brief 转换设备坐标到地图坐标
	GsRawPoint ToMap(const Utility::GsPT& pt);
	/// \brief 转换设备坐标到地图坐标
	GsRawPoint ToMap(const Utility::GsPTF& pt);
	/// \brief 转换设备坐标到地图坐标
	GsRawPoint ToMap(int x,int y);
	/// \brief 转换设备坐标到地图坐标
	GsRawPoint ToMap(float x, float y);

	/// \brief 转换地图坐标到设备坐标
	Utility::GsPTF FromMap(const GsRawPoint& pt);
	/// \brief 转换地图坐标到设备坐标
	Utility::GsRect FromMap(const GsBox& box);
	/// \brief 转换地图坐标到设备坐标
	Utility::GsRectF FromMap(const GsBox& box,Utility::GsRectF& rect );

	/// \brief 转换地图坐标到设备坐标
	Utility::GsPTF FromMap(double x,double y);

	/// \brief 转换地图坐标到设备坐标
	void FromMap(double x,double y,float &screenX,float & screenY);
	/// \brief 转换地图坐标到设备坐标
	void FromMap(double x,double y,int &screenX,int & screenY);
	
	/// \brief 转换地图坐标到设备坐标
	void FromMap(double* pMapCoord,int nCount,int nDim,float* pScreenCoord);
	/// \brief 转换地图坐标到设备坐标
	void FromMap(double* pMapCoord,int nCount,int nDim,int* pScreenCoord);

	/// \brief 从纸面单位长度转换为像素单位长度
	double FromPageMeasure(GsUnits u,double dblLen);

	/// \brief 获取比例尺分母的值
	double Scale();
	/// \brief 设置比例尺的值
	void Scale(double scale);

	/// \brief 获取参考比例尺
	double ReferenceScale();
	/// \brief 设置参考比例尺
	void ReferenceScale(double dblScale);

	/// \brief 地图到屏幕的转换矩阵
	/// \details 屏幕到地图的转换矩阵可以通过矩阵Invert获得
	GsMatrix Matrix();

public:
	/// \brief 转换毫米单位长度为像素单位长度
	float MMToPixel(double mm);
	/// \brief 毫米单位转换为字体的单位磅（point)
	float MMToPoint(double mm);
	/// \brief 磅（point)转换为毫米单位
	float PointToMM(double point);

	/// \brief 像素到毫米
	double PixelToMM(double nPixel);

	/// \brief 毫米单位到地图长度
	double MMToMap(double dblMM);
	/// \brief 地图长度到毫米
	double MapToMM(double dblMM);
	/// \brief 像素长度到地图长度
	double PixelToMap(double dblPixel);
	/// \brief 地图长度到像素长度
	double MapToPixel(double dblMap);

private:
	double m_MMToPixel;
	double m_MMToPoint;
	double m_MMToMap;
	double m_PixelToMap;
	
	void Pretreatment();

};
/// \brief GsDisplayTransformationPtr
GS_SMARTER_PTR(GsDisplayTransformation);


/// \brief 基于画布坐标系系的Geomety
class GS_API GsGraphicsGeometry:public Utility::GsRefObject
{
	Utility::GsGrowByteBuffer m_Temp;
	Utility::GsGrowByteBuffer m_Coordinate;
	Utility::GsGrowByteBuffer m_Interpret;
public:
	GsGraphicsGeometry();
	GsGraphicsGeometry(GsGeometry* geo,GsDisplayTransformation* dt);
	GsGraphicsGeometry(GsGeometryBlob* geo, GsDisplayTransformation* dt);
	virtual ~GsGraphicsGeometry();
	
	/// \brief 绑定并转换几何数据
	void Assign(GsGeometry* geo, GsDisplayTransformation* dt);
	/// \brief 绑定并转换几何数据
	void Assign(GsGeometryBlob* geo, GsDisplayTransformation* dt);

	/// \brief 几何分块的数量
	int PartCount()const;

	/// \brief 特定块的坐标指针
	float* PartPtr(int i) const;
	
	/// \brief 特定块的坐标长度
	int PartLength(int i)const;

	/// \brief 总的坐标长度
	int TotalLength()const;

	/// \brief 清空几何对象
	void Clear();

	/// \brief 添加一段几何坐标
	float* Append(const float* pCoord, int nLen);

	/// \brief 预留空间添加一段几何坐标
	float* Append(int nLen);

};
/// \brief GsGraphicsGeometryPtr
GS_SMARTER_PTR(GsGraphicsGeometry);

/// \brief 符号对象基类
class GS_API GsSymbol:public Utility::GsRefObject
{
	void AppendPath(double* pCoord,int nLen,GsGraphicsPath* pPath,int ndim = 2);

	void AppendPolyline( int n_cood,double *p_cood,int cdim,GsGraphicsPath*  path);
	void AppendCurve(Utility::GsVector<GsRawPoint>& pointArray,GsGraphicsPath*  path,bool circle=false);
	void AppendEnv( int n_cood,double *p_cood,int cdim,GsGraphicsPath*  path);

protected:
	/// \brief 绘制的画布
	GsCanvasPtr m_ptrCanvas;
	/// \brief 坐标转换
	GsDisplayTransformationPtr m_ptrDT;
	
	/// \brief 画布坐标系Geomety
	GsGraphicsGeometryPtr m_ptrGraphGeo;

	/// \brief 符号名称
	Utility::GsString m_strName;
	/// \brief 符号版本
	Utility::GsString m_strVersion;
	/// \brief 符号描述
	Utility::GsString m_strDesc;


	/// \brief 符号编码
	long long m_nCode; 
	/// \brief 图示编号，字符串的编码
	Utility::GsString m_strCode;

	/// \brief 转换显示用的屏幕坐标
	Utility::GsGrowByteBuffer m_ScreenBuff;  
	
protected:
	/// \brief 由空间分析的path构建成可绘制的path
	GsGraphicsPathPtr CreateGraphicsPath(GsGeometryBlob *pBlob);

	
	/// \brief 转换地理坐标为屏幕坐标
	virtual Utility::GsGrowByteBuffer* ToScreen(double* pCoord,int nLen,int nDim);
	virtual float* ToScreenPtr(double* pCoord,int nLen,int nDim);

	/// \brief 当开始绘制的时候发生
	/// \details 子类通过覆盖此方法实现自定义的数据准备
	virtual void OnStartDrawing();
	/// \brief 当结束绘制的时候发生
	/// \details 子类通过覆盖此方法实现自定义的数据回收过程
	virtual void OnEndDrawing();

	/// \brief 当绘制的时候发生
	/// \details 子类通过覆盖此方法实现实际的绘制
	virtual void OnDraw(GsGeometryBlob* pBuffer);

	/// \brief 绘制Canvas的path
	virtual void OnDraw(GsGraphicsPath* pPath,GsGeometryBlob *pBlob) ;

	/// \brief 几何数据操作
	/// \details 子类通过覆盖此函数实现绘制前对几何数据的处理，例如计算平行线
	virtual geostar::gobjptr GeometryOperator(geostar::gobjptr& pPath);
	/// \brief 缺省构造
	GsSymbol();
public:
	virtual ~GsSymbol();
	
	/// \brief 符号名称
	virtual Utility::GsString Name();
	/// \brief 设置符号名称
	virtual void Name(const char* strName);
	
	/// \brief 符号版本
	virtual Utility::GsString Version();
	/// \brief 设置符号版本
	virtual void Version(const char* strVer);
	
	/// \brief 符号描述
	virtual Utility::GsString Description();
	/// \brief 设置符号描述
	virtual void Description(const char* strDesc);
	
	/// \brief 符号编码
	virtual long long Code();
	/// \brief 设置符号编码
	virtual void Code(long long nCode);
	
	/// \brief 图式编号
	virtual Utility::GsString SchemaCode();
	/// \brief 设置图式编号
	virtual void SchemaCode(const char* strCode);
	
	/// \brief 是否已经调用了StartDrawing方法
	virtual bool HasStartDrawing();
	/// \brief 开始绘制
	virtual bool StartDrawing(GsCanvas* pCanvas,GsDisplayTransformation *pDT) ;
	/// \brief 结束绘制
	virtual bool EndDrawing() ;

	/// \brief 绘制几何对象
	virtual bool Draw(GsGeometry* pGeo);

	/// \brief 绘制几何内存块
	virtual bool DrawBlob(GsGeometryBlob* pBuffer);

	/// \brief 绘制画布坐标系下的几何对象
	virtual bool DrawGraphics(GsGraphicsGeometry* pGeo, GsGeometryBlob* pBuffer);

	/// \brief 符号是否有效
	/// \details 符号是否会在画布上绘制内容，如果符号不会在画布上绘制任何的内容则不需要执行绘制过程。
	virtual bool IsValid();

	/// \brief 符号的类型
	virtual GsSymbolType Type() = 0;

	
	/// \brief 对符号进行克隆
	virtual Utility::GsSmarterPtr<GsSymbol> Clone();
};
/// \brief GsSymbolPtr
GS_SMARTER_PTR(GsSymbol);
 

/// \brief 线符号基类
class GS_API GsLineSymbol:public GsSymbol, public UTILITY_NAME::GsSerialize
{
protected:
	/// \brief 线的宽度
	double m_dblWidth;
	/// \brief 线的颜色
	GsColor m_Color;
protected:
	GsLineSymbol();
public:
	using GsSymbol::DrawGraphics;
	virtual ~GsLineSymbol();

	/// \brief 绘制Canvas的path
	virtual void DrawGraphics(GsGraphicsPath* pPath, GsGeometryBlob *pBlob);

	/// \brief 符号的类型
	virtual GsSymbolType Type();
	
	/// \brief 线符号的宽度,单位毫米
	virtual double Width();
	/// \brief 设置线符号的宽度,单位毫米
	virtual void Width(double dblWidth);

	/// \brief 线符号颜色
	virtual GsColor Color();
	/// \brief 设置线符号颜色
	virtual void Color(const GsColor& c);

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual UTILITY_NAME::GsString ClassName();

};
GS_SMARTER_PTR(GsLineSymbol);


/// \brief 面符号基类
class GS_API GsFillSymbol:public GsSymbol, public UTILITY_NAME::GsSerialize
{
protected:
	/// \brief 填充颜色
	GsColor m_Color; 
	/// \brief 面的边线符号
	GsLineSymbolPtr m_ptrLineSymbol;
	/// \brief 面的边界线是否转化为线数据
	bool m_GenerateLine;
	 
protected: 
	/// \brief 当开始绘制的时候发生
	virtual void OnStartDrawing();
	/// \brief 当结束绘制时发生
	virtual void OnEndDrawing();
	 
public:
	GsFillSymbol();
	virtual ~GsFillSymbol();
	
	/// \brief 绘制Canvas的path
	virtual void DrawGraphics(GsGraphicsPath* pPath, GsGeometryBlob *pBlob);
	/// \brief 绘制Canvas的path
	virtual bool DrawGraphics(GsGraphicsGeometry* pPath, GsGeometryBlob *pBlob);

	/// \brief 符号类型
	virtual GsSymbolType Type();
	/// \brief 填充的颜色
	virtual GsColor FillColor();
	/// \brief 设置填充的颜色
	virtual void FillColor(const GsColor& c);
	/// \brief 边线符号
	virtual GsLineSymbol* Outline();
	/// \brief 设置边线符号
	virtual  void Outline(GsLineSymbol* pSym);

	/// \brief 设置面边界是否转为线符号
	virtual void GenerateLine(bool bGenerateLine);

	/// \brief 获取面边界是否转为线符号
	virtual bool GenerateLine();

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual UTILITY_NAME::GsString ClassName();
};
GS_SMARTER_PTR(GsFillSymbol);



/// \brief 文本风格（左倾、右倾、左耸、右耸）
enum GsTextStyle
{
	/// \brief 无倾斜
	/// \details  ━━
	/// \details ┃  ┃
	/// \details ┃  ┃
	/// \details  ━━
	/// \details 
	eTextStyleNormal,
	/// \brief 左倾
	/// \details  ━━
	/// \details  ╲   ╲
	/// \details    ╲   ╲
	/// \details       ━━
	eTextStyleLeftItalic,
	/// \brief 右倾
	/// \details        ━━
	/// \details      ╱  ╱
	/// \details    ╱  ╱
	/// \details   ━━
	eTextStyleRightItalic,
	/// \brief 左耸
	/// \details ┃╲    
	/// \details ┃  ╲  
	/// \details  ╲   ┃
	/// \details     ╲┃
	eTextStyleLeftShrug,
	/// \brief 右耸
	/// \details    ╱┃   
	/// \details  ╱  ┃  
	/// \details ┃  ╱
	/// \details ┃╱
	eTextStyleRightShrug 
};



/// \brief 文本符号
class GS_API GsTextSymbol : public GsSymbol, public UTILITY_NAME::GsSerialize
{  
	GsTextStyle				m_eTextStyle;					//倾斜类型
	bool					m_bBold;						//加粗标识
	bool					m_bHollow;						//空心标识
	bool					m_bUnderLine;					//下划线标识
	bool					m_bStrikeOut;					//删除线标识
	GsSolidBrushPtr			m_ptrBrush;						//注记填充画刷
	GsPenPtr				m_ptrHaloPen;					//注记光晕画笔
	GsPenPtr				m_ptrPen;						//注记描边画笔
	GsPenPtr				m_ptrWhitePen;					//绘制不偏移阴影效果时的白色画刷
	GsColor					m_textColor;					//注记颜色
	Utility::GsString		m_strFont;						//字体名称
	float					m_fSize;						//字体大小
	Utility::GsString		m_strText;						//注记内容
	GsStringFormatPtr		m_ptrTextFormat;				//注记样式
	GsStringAlignment		m_eHAlignment;					//水平对齐方式
	GsStringAlignment		m_eVAlignment;					//垂直对齐方式
	double					m_dblWidth;						//注记宽度
	double					m_dblHeight;					//注记高度
	double					m_dblStyleAngle;				//字体风格的角度参数，例左倾斜的角度
	double					m_dblAngle;						//旋转角度
	double					m_offsetX;						//X偏移
	double					m_offsetY;						//Y偏移
	double					m_dblHExtra;					//水平间距
	double					m_dblVExtra;					//垂直间距
	GsColor					m_backColor;					//背景色（为空时表示透明）
	GsSolidBrushPtr			m_ptrBackBrush;					//背景填充画刷
	GsColor					m_shadowColor;					//阴影色（为空时表示无阴影效果）
	GsSolidBrushPtr			m_ptrShadowBrush;				//阴影填充画刷
	double					m_shadowOffsetX;				//阴影X偏移
	double					m_shadowOffsetY;				//阴影Y偏移
	GsMatrix				m_WordStyleMatrix;				//单个字的式样变换矩阵实现倾斜和缩放。
	int						m_iLabelOffset;					//LineLabelPlacement Offset。
	Utility::GsSizeF		m_WordSize;
	Utility::GsSizeF		m_StringSize;
	UTILITY_NAME::GsVector<Utility::GsString> m_SplitWords;
	bool					m_SimpleDraw;
	Utility::GsWordAnalyzerPtr		m_ptrWordAnalyzer;
	GsColor					m_HaloColor;					//光晕颜色
	double					m_HaloSize;						//光晕的大小
private:
	/// \brief 绘制一个文本单元
	void DrawText(const Utility::GsPTF& loc,double angle,const UTILITY_NAME::GsVector<Utility::GsString>& vecWords);
	
	/// \brief 将字符串进行整体的绘制
	void DrawText(const Utility::GsPTF& loc);
	
	/// \brief 符号是否可以用简单绘制实现
	bool CanSimpleDraw();

protected:
	/// \brief 当绘制的时候发生
	/// \details 子类通过覆盖此方法实现实际的绘制
	virtual void OnDraw(GsGeometryBlob* pBuffer);
	/// \brief 当开始绘制的时候发生
	/// \details 子类通过覆盖此方法实现自定义的数据准备
	virtual void OnStartDrawing();
	/// \brief 当结束绘制的时候发生
	/// \details 子类通过覆盖此方法实现自定义的数据回收过程
	virtual void OnEndDrawing();
	 
public:
	/// \brief 默认构造函数
	GsTextSymbol();
	/// \brief 析构函数
	virtual ~GsTextSymbol();

	/// \brief 获取光晕的颜色
	GsColor HaloColor();
	/// \brief 设置光晕的颜色
	void HaloColor(const GsColor& halo);

	/// \brief 获取光晕的大小，单位毫米
	double HaloSize();
	/// \brief 设置光晕的大小，单位毫米
	void HaloSize(double size);


	/// \brief 设置倾斜类型
	void TextStyle(GsTextStyle style);
	/// \brief 获取倾斜类型
	GsTextStyle TextStyle();

	/// \brief 设置加粗标识
	void Bold(bool bBold);
	/// \brief 获取加粗标识
	bool Bold();

	/// \brief 设置空心标识
	void Hollow(bool bStroke);
	/// \brief 获取空心标识
	bool Hollow();

	/// \brief 设置下划线标识
	void UnderLine(bool bUnderLine);
	/// \brief 获取下划线标识
	bool UnderLine();

	/// \brief 设置删除线标识
	void StrikeOut(bool bStrikeOut);
	/// \brief 获取删除线标识
	bool StrikeOut();

	/// \brief 返回符号的颜色
	GsColor Color();
	/// \brief 设置符号的颜色
	void Color(const GsColor& color);

	/// \brief 返回符号的字体
	Utility::GsString Font();
	/// \brief 设置符号的字体
	void Font(const char* strFont);

	/// \brief 返回符号的大小
	float Size();
	/// \brief 设置符号的大小 注记的宽度和高度也改变为符号的大小，也可通过其他方法直接改变注记的宽度和高度
	void Size(float iSize);

	/// \brief 返回符号的文本
	Utility::GsString Text();
	/// \brief 设置符号的文本
	void Text(const char* strText);

	/// \brief 获取水平对齐方式
	GsStringAlignment HorizonAlign();
	/// \brief 设置水平对齐方式
	void HorizonAlign(GsStringAlignment eAlignment);

	/// \brief 获取垂直对齐方式
	GsStringAlignment VerticalAlign();
	/// \brief 设置垂直对齐方式
	void VerticalAlign(GsStringAlignment eAlignment);

	/// \brief 获取注记宽度
	double Width();
	/// \brief 设置注记高度
	void Width(double width);

	/// \brief 获取注记高度
	double Height();
	/// \brief 设置注记高度
	void Height(double height);

	/// \brief 获取字体风格的倾斜角度
	double StyleAngle();
	/// \brief 设置字体风格的倾斜角度
	void StyleAngle(double angle);

	/// \brief 获取注记旋转角度
	double Angle();
	/// \brief 设置注记旋转角度
	void Angle(double angle);

	/// \brief 获取注记X偏移
	double OffsetX();
	/// \brief 设置注记X偏移
	void OffsetX(double offset);

	/// \brief 获取注记Y偏移
	double OffsetY();
	/// \brief 设置注记Y偏移
	void OffsetY(double offset);

	/// \brief 获取注记水平间距
	double HorizonExtra();
	/// \brief 设置注记水平间距
	void HorizonExtra(double extra);

	/// \brief 获取注记垂直间距
	double VerticalExtra();
	/// \brief 设置垂直水平间距
	void VerticalExtra(double extra);

	/// \brief 返回注记的背景色
	GsColor BackgroundColor();
	/// \brief 设置注记的背景色
	void BackgroundColor(const GsColor& color);

	/// \brief 返回注记阴影的颜色
	GsColor ShadowColor();
	/// \brief 设置注记阴影的颜色
	void ShadowColor(const GsColor& color);

	/// \brief 返回注记阴影的X偏移
	double ShadowOffsetX();
	/// \brief 设置注记阴影的X偏移
	void ShadowOffsetX(double offset);

	/// \brief 返回注记阴影的Y偏移
	double ShadowOffsetY();
	/// \brief 设置注记阴影的Y偏移
	void ShadowOffsetY(double offset);

	/// \brief 设置文字内容中的方框大小
	void StringSize(const Utility::GsSizeF& pSize);

	/// \brief 返回符号的类型
	virtual GsSymbolType Type();
	
	/// \brief 是否有效
	virtual bool IsValid();

	/// \brief 对符号进行克隆
	virtual Utility::GsSmarterPtr<GsSymbol> Clone();

	/// \brief 返回分词器
	Utility::GsWordAnalyzer* WordAnalyzer();
	/// \brief 设置分词器
	void WordAnalyzer(Utility::GsWordAnalyzer* pAnalyzer);

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual UTILITY_NAME::GsString ClassName();

};

/// \brief GsTextSymbolPtr
GS_SMARTER_PTR(GsTextSymbol);
DECLARE_CLASS_CREATE(GsTextSymbol);



template<typename T>
class GsMultiSymbolT:public T
{
protected:
	UTILITY_NAME::GsVector<Utility::GsSmarterPtr<T> > m_Symbols;
	GsMultiSymbolT(){}
	
	/// \brief 当开始绘制的时候发生
	/// \details 子类通过覆盖此方法实现自定义的数据准备
	virtual void OnStartDrawing()
	{
		typename UTILITY_NAME::GsVector<Utility::GsSmarterPtr<T> >::iterator it = m_Symbols.begin();
		for(;it != m_Symbols.end();it++)
		{
			(*it)->StartDrawing(this->m_ptrCanvas,this->m_ptrDT);
		}
	}
	/// \brief 当结束绘制的时候发生
	/// \details 子类通过覆盖此方法实现自定义的数据回收过程
	virtual void OnEndDrawing()
	{
		typename UTILITY_NAME::GsVector<Utility::GsSmarterPtr<T> >::iterator it = m_Symbols.begin();
		for(;it != m_Symbols.end();it++)
		{
			(*it)->EndDrawing();
		}
	}

	/// \brief 当绘制的时候发生
	/// \details 子类通过覆盖此方法实现实际的绘制
	virtual void OnDraw(GsGeometryBlob* pBuffer)
	{
		typename UTILITY_NAME::GsVector<Utility::GsSmarterPtr<T> >::iterator it = m_Symbols.begin();
		for(;it != m_Symbols.end();it++)
		{
			(*it)->DrawBlob(pBuffer);
		}
	}
	
public:
	virtual ~GsMultiSymbolT(){}
	/// \brief 绘制画布坐标系下的几何对象
	virtual bool DrawGraphics(GsGraphicsGeometry* pGeo, GsGeometryBlob* pBuffer)
	{
		typename UTILITY_NAME::GsVector<Utility::GsSmarterPtr<T> >::iterator it = m_Symbols.begin();
		for (; it != m_Symbols.end(); it++)
		{
			if(!(*it)->DrawGraphics(pGeo,pBuffer) )
				(*it)->DrawBlob(pBuffer);
		}
		return true;
	}

	/// \brief 增加子符号
	virtual void Add(T* pSymbol)
	{
		m_Symbols.push_back(pSymbol);
	}

	/// \brief 清空所有的符号
	virtual void Clear()
	{
		m_Symbols.clear();
	}

	/// \brief 子符号的数量
	virtual int Count()
	{
		return m_Symbols.size();
	}
	/// \brief 子符号的数量
	virtual void RemoveAt(int i)
	{
		if(i <0 || i >= m_Symbols.size())
			return;
		m_Symbols.erase(m_Symbols.begin() + (Utility::GsVector<GsSymbolPtr>::difference_type)i);
	}

	/// \brief 获取子符号
	T* ElementAt(int i)
	{
		if(i <0 || i >= m_Symbols.size())
			return NULL;
		return m_Symbols[i].p;
	}
	/// \brief 符号是否有效
	/// \details 符号是否会在画布上绘制内容，如果符号不会在画布上绘制任何的内容则不需要执行绘制过程。
	virtual bool IsValid()
	{
		if(m_Symbols.empty())
			return false;
		//所有的子符号，如果一个有效则整个有效
		typename UTILITY_NAME::GsVector<Utility::GsSmarterPtr<T> >::iterator it = m_Symbols.begin();
		for(;it != m_Symbols.end();it++)
		{
			if((*it)->IsValid())
				return true;
		}
		return false;
	}
	

};


/// \brief 复合线符号
class GS_API GsMultiLineSymbol:public GsMultiSymbolT<GsLineSymbol>
{
public:
	/// \brief 获取线符号的宽度,单位毫米
	virtual double Width();
	/// \brief 设置线符号的宽度,单位毫米
	virtual void Width(double dblWidth);

	/// \brief 获取线符号颜色
	virtual GsColor Color();
	/// \brief 设置线符号颜色
	virtual void Color(const GsColor& c);
};
/// \brief GsMultiLineSymbolPtr
GS_SMARTER_PTR(GsMultiLineSymbol);
DECLARE_CLASS_CREATE(GsMultiLineSymbol);


/// \brief 复合面符号
class GS_API GsMultiFillSymbol:public GsMultiSymbolT<GsFillSymbol>
{
public:
	/// \brief 获取填充的颜色
	virtual GsColor FillColor();
	/// \brief 设置填充的颜色
	virtual void FillColor(const GsColor& c);
	/// \brief 获取边线符号
	virtual GsLineSymbol* Outline();
	/// \brief 设置边线符号
	virtual  void Outline(GsLineSymbol* pSym);
};
/// \brief GsMultiFillSymbolPtr
GS_SMARTER_PTR(GsMultiFillSymbol);
DECLARE_CLASS_CREATE(GsMultiFillSymbol);

/// \brief 符号库的格式
enum GsSymbolLibraryFormat
{
	/// \brief 地图定义文件的符号存储格式
	eGeoMapDefineFormat,
	/// \brief 跨平台符号的符号存储格式。
	eGenernalFormat,
};

/// \brief 符号库对象。
/// \details 用于实例化符号库的符号或者生成符号库文件。
class GS_API GsSymbolLibrary
{
	void *m_Doc;
	void LoadSymbol();
	Utility::GsString m_strFileName;
	Utility::GsString m_Name;
	Utility::GsString m_Description;
	UTILITY_NAME::GsVector<GsSymbolPtr> m_vecSymbols;
public:
	/// \brief 从符号库文件或者符号库xml字符串实例化对象
	GsSymbolLibrary(const char* strSymbolLib,bool bFileName = true);
	
	/// \brief 构造一个空的符号库对象
	GsSymbolLibrary();

	virtual ~GsSymbolLibrary();
	
	/// \brief 符号库的路径
	Utility::GsString Path();


	/// \brief 保存符号的集合
	UTILITY_NAME::GsVector<GsSymbolPtr>* Symbols();

	/// \brief 根据符号的Code查找符号
	/// \details 如果存在多个相同的code则返回第一个符号。
	GsSymbolPtr SymbolByCode(long long nCode);

	/// \brief 根据符号的Code查找符号
	/// \param nCode 需要查找的编号
	/// \param eType 需要查找的符号的类型
	/// \return 返回特定类型(eType)下找到的(nCode)编号的第一个符号
	GsSymbolPtr SymbolByCode(long long nCode, GsSymbolType eType);

	/// \brief 根据符号的名称查找符号
	/// \details 支持统配查找
	GsSymbolPtr SymbolByName(const char* strName);

	/// \brief 根据字符串的图式编号查找Code
	/// \details 支持统配查找
	GsSymbolPtr SymbolBySchemaCode(const char* SchemaCode);

	/// \brief 返回特定类型的符号。
	UTILITY_NAME::GsVector<GsSymbolPtr> Symbols(GsSymbolType eType);

	/// \brief 保存符号库到文件
	void Save();
	
	/// \brief 保存符号库到字符串
	Utility::GsString SaveToString(GsSymbolLibraryFormat eFormat=eGenernalFormat);
	
	/// \brief 保存符号库到新的文件。
	void Save(const char* strSymbolLibFile);

	/// \brief 符号库的名称
	Utility::GsString Name();
	
	/// \brief 设置符号库的名称
	void Name(const char* strName);

	/// \brief 符号库的名称
	Utility::GsString Description();
	
	/// \brief 设置符号库的描述信息。
	void Description(const char* strDesc);

	/// \brief 将符号序列化为字符串。
	static Utility::GsString ToString(GsSymbol* pSym,GsSymbolLibraryFormat eFormat = eGenernalFormat);
	
	/// \brief 计算符号哈希值
	static long long HashCode(GsSymbol* pSym);

	/// \brief 从字符串解析生成符号对象
	static GsSymbolPtr ParserSymbol(const char* str);

	/// \brief 为特定的符号解析并配置属性
	static bool Assign(GsSymbol* pSym,const char* strXML);

	/// \brief 为特定的符号解析并配置属性
	static bool Assign(GsSymbol* pSym,tinyxml2::XMLElement* pEle);


};

KERNEL_ENDNS