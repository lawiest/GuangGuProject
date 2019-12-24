#pragma once
#include "matrix.h"
#include "geometry.h"
#include <image.h>
#include <configuration.h>

KERNEL_NS    
/// \brief 颜色对象
struct  GS_API GsColor
{
	/// \brief 缺省构造
	GsColor();
	/// \brief 根据32位整数构造
	GsColor(unsigned int c);
	/// \brief 根据32位整数构造
	GsColor(int c);
	 
	/// \brief 根据R，G，B，A通道构造
	/// \param r 红色通道[0,255]
	/// \param g 绿色通道[0,255]
	/// \param b 蓝色通道[0,255]
	/// \param a 透明通道[0,255]
	GsColor(unsigned char r,unsigned char g,unsigned char b,unsigned char a=255);
	  
	/// \brief 拷贝构造
	GsColor(const GsColor& rhs );
	/// \brief 改变透明度构造已有颜色
	GsColor(const GsColor& rhs ,unsigned char a);
	
	/// \brief 交换RGB中R和B的通道
	void FlipRGB();

	/// \brief 从CSS颜色描述构造
	static GsColor FromCSS(const char* csscolor);
	
	/// \brief 从Win32的RGB颜色构造
	static GsColor FromCOLORREF(unsigned int rgb);
	
	/// \brief 根据R，G，B，A通道构造
	/// \param r 红色通道[0,255]
	/// \param g 绿色通道[0,255]
	/// \param b 蓝色通道[0,255]
	/// \param a 透明通道[0,255]
	static GsColor FromARGB(unsigned char r,unsigned char g,unsigned char b,unsigned char a=255);
	
	/// \brief 从浮点R，G，B，A通道构造
	/// \param r 红色通道[0,1]
	/// \param g 绿色通道[0,1]
	/// \param b 蓝色通道[0,1]
	/// \param a 透明通道[0,1]
	static GsColor FromARGBF(float r,float g,float b,float a=1.0f);
	
	/// \brief 改变透明度构造已有颜色
	static GsColor FromColor(const GsColor& rhs ,unsigned char a);
	/// \brief 改变透明度构造已有颜色
	static GsColor FromColorF(const GsColor& rhs,float a);
	
	/// \brief 从颜色名称构造 
	static GsColor FromName(const char* strName);

	/// \brief 颜色的名称，如果存在的话
	Utility::GsString Name() const;
	
	/// \brief 是否是已知的颜色
	bool IsKnownColor()const;


	/// \brief 生成一个随机的颜色
	static GsColor Random();

	/// \brief 从HSV颜色构造
	/// \param h Hue  色调
	/// \param s Saturation 饱和度
	/// \param v Value 亮度
	/// \param a 透明度
	static GsColor FromHSV(float h, float s, float v, unsigned char a = 255);

	/// \brief 用Win32 RGB颜色设置颜色值
	void SetCOLORREF(unsigned int rgb,unsigned char a= 255);
	/// \brief 转换为win32 RGB颜色值
	unsigned int ToCOLORREF() const;

	/// \brief 以html的形式返回颜色值#RRGGBB
	Utility::GsString ToHtml() const;
	/// \brief 以html的形式返回颜色值rgba(r,g,b,af);
	Utility::GsString ToHtmlRGBA() const;

	/// \brief 转换为32位无符号整数
	unsigned int ToUInt() const;
	
	/// \brief 转换HSV的颜色
	bool ToHSV(float *h ,float *s , float *v );

	/// \brief 转换为32位符号整数
	int ToInt() const;

	
	/// \brief 根据R，G，B，A通道设置颜色值
	void SetARGB(unsigned char r,unsigned char g,unsigned char b,unsigned char a=255);
	/// \brief 根据R，G，B，A通道设置颜色值
	void SetARGBF(float r,float g,float b,float a=1.0f);
	/// \brief 根据ARGB值设置
	void SetARGB(unsigned int argb);
	/// \brief 从HSV颜色设置
	/// \param h Hue  色调
	/// \param s Saturation 饱和度
	/// \param v Value 亮度
	/// \param a 透明度
	void SetHSV(float h, float s, float v, unsigned char a = 255);

	/// \brief 浮点数R通道值
	/// \return 返回R通道值[0,1]
	float RedF()const;
	/// \brief 浮点数G通道值
	/// \return 返回G通道值[0,1]
	float GreenF()const;
	/// \brief 浮点数A通道值
	/// \return 返回A通道值[0,1]
	float AlphaF()const;
	/// \brief 浮点数B通道值
	/// \return 返回B通道值[0,1]
	float BlueF()const;

	/// \brief 等号操作符
	GsColor &operator=(unsigned int &argb);
    /// \brief 等号操作符
	GsColor &operator=(int &argb);
	/// \brief 等号操作符
	GsColor &operator=(const GsColor &rhs);
    /// \brief 颜色是否相等
	bool operator==(const GsColor &c) const;
    /// \brief 颜色是否不等
	bool operator!=(const GsColor &c) const;
	 
	/// \brief 无符号整数重载操作符
	operator unsigned int() const;
	/// \brief 整数重载操作符
	operator int()const;

	/// \brief 颜色数据结构
	union
	{
		/// \brief ARGB颜色值
		unsigned int Argb;
		struct
		{
			unsigned char B,G,R,A;
		};
	};

	/// \brief 预制颜色值。
	enum
	{
		AliceBlue            = 0xFFF0F8FF,
        AntiqueWhite         = 0xFFFAEBD7,
        Aqua                 = 0xFF00FFFF,
        Aquamarine           = 0xFF7FFFD4,
        Azure                = 0xFFF0FFFF,
        Beige                = 0xFFF5F5DC,
        Bisque               = 0xFFFFE4C4,
        Black                = 0xFF000000,
        BlanchedAlmond       = 0xFFFFEBCD,
        Blue                 = 0xFF0000FF,
        BlueViolet           = 0xFF8A2BE2,
        Brown                = 0xFFA52A2A,
        BurlyWood            = 0xFFDEB887,
        CadetBlue            = 0xFF5F9EA0,
        Chartreuse           = 0xFF7FFF00,
        Chocolate            = 0xFFD2691E,
        Coral                = 0xFFFF7F50,
        CornflowerBlue       = 0xFF6495ED,
        Cornsilk             = 0xFFFFF8DC,
        Crimson              = 0xFFDC143C,
        Cyan                 = 0xFF00FFFF,
        DarkBlue             = 0xFF00008B,
        DarkCyan             = 0xFF008B8B,
        DarkGoldenrod        = 0xFFB8860B,
        DarkGray             = 0xFFA9A9A9,
        DarkGreen            = 0xFF006400,
        DarkKhaki            = 0xFFBDB76B,
        DarkMagenta          = 0xFF8B008B,
        DarkOliveGreen       = 0xFF556B2F,
        DarkOrange           = 0xFFFF8C00,
        DarkOrchid           = 0xFF9932CC,
        DarkRed              = 0xFF8B0000,
        DarkSalmon           = 0xFFE9967A,
        DarkSeaGreen         = 0xFF8FBC8B,
        DarkSlateBlue        = 0xFF483D8B,
        DarkSlateGray        = 0xFF2F4F4F,
        DarkTurquoise        = 0xFF00CED1,
        DarkViolet           = 0xFF9400D3,
        DeepPink             = 0xFFFF1493,
        DeepSkyBlue          = 0xFF00BFFF,
        DimGray              = 0xFF696969,
        DodgerBlue           = 0xFF1E90FF,
        Feldspar			 = 0xFFD19275, //Html
		Firebrick            = 0xFFB22222,
        FloralWhite          = 0xFFFFFAF0,
        ForestGreen          = 0xFF228B22,
        Fuchsia              = 0xFFFF00FF,
        Gainsboro            = 0xFFDCDCDC,
        GhostWhite           = 0xFFF8F8FF,
        Gold                 = 0xFFFFD700,
        Goldenrod            = 0xFFDAA520,
        Gray                 = 0xFF808080,
        Green                = 0xFF008000,
        GreenYellow          = 0xFFADFF2F,
        Honeydew             = 0xFFF0FFF0,
        HotPink              = 0xFFFF69B4,
        IndianRed            = 0xFFCD5C5C,
        Indigo               = 0xFF4B0082,
        Ivory                = 0xFFFFFFF0,
        Khaki                = 0xFFF0E68C,
        Lavender             = 0xFFE6E6FA,
        LavenderBlush        = 0xFFFFF0F5,
        LawnGreen            = 0xFF7CFC00,
        LemonChiffon         = 0xFFFFFACD,
        LightBlue            = 0xFFADD8E6,
        LightCoral           = 0xFFF08080,
        LightCyan            = 0xFFE0FFFF,
        LightGoldenrodYellow = 0xFFFAFAD2,
        LightGray            = 0xFFD3D3D3,
		LightGrey            = 0xFFD3D3D3,//html
        LightGreen           = 0xFF90EE90,
        LightPink            = 0xFFFFB6C1,
        LightSalmon          = 0xFFFFA07A,
        LightSeaGreen        = 0xFF20B2AA,
        LightSkyBlue         = 0xFF87CEFA,
		LightSlateBlue	     = 0xFF8470FF, //html
		LightSlateGray       = 0xFF778899,
        LightSteelBlue       = 0xFFB0C4DE,
        LightYellow          = 0xFFFFFFE0,
        Lime                 = 0xFF00FF00,
        LimeGreen            = 0xFF32CD32,
        Linen                = 0xFFFAF0E6,
        Magenta              = 0xFFFF00FF,
        Maroon               = 0xFF800000,
        MediumAquamarine     = 0xFF66CDAA,
        MediumBlue           = 0xFF0000CD,
        MediumOrchid         = 0xFFBA55D3,
        MediumPurple         = 0xFF9370DB,
        MediumSeaGreen       = 0xFF3CB371,
        MediumSlateBlue      = 0xFF7B68EE,
        MediumSpringGreen    = 0xFF00FA9A,
        MediumTurquoise      = 0xFF48D1CC,
        MediumVioletRed      = 0xFFC71585,
        MidnightBlue         = 0xFF191970,
        MintCream            = 0xFFF5FFFA,
        MistyRose            = 0xFFFFE4E1,
        Moccasin             = 0xFFFFE4B5,
        NavajoWhite          = 0xFFFFDEAD,
        Navy                 = 0xFF000080,
        OldLace              = 0xFFFDF5E6,
        Olive                = 0xFF808000,
        OliveDrab            = 0xFF6B8E23,
        Orange               = 0xFFFFA500,
        OrangeRed            = 0xFFFF4500,
        Orchid               = 0xFFDA70D6,
        PaleGoldenrod        = 0xFFEEE8AA,
        PaleGreen            = 0xFF98FB98,
        PaleTurquoise        = 0xFFAFEEEE,
        PaleVioletRed        = 0xFFDB7093,
        PapayaWhip           = 0xFFFFEFD5,
        PeachPuff            = 0xFFFFDAB9,
        Peru                 = 0xFFCD853F,
        Pink                 = 0xFFFFC0CB,
        Plum                 = 0xFFDDA0DD,
        PowderBlue           = 0xFFB0E0E6,
        Purple               = 0xFF800080,
        Red                  = 0xFFFF0000,
        RosyBrown            = 0xFFBC8F8F,
        RoyalBlue            = 0xFF4169E1,
        SaddleBrown          = 0xFF8B4513,
        Salmon               = 0xFFFA8072,
        SandyBrown           = 0xFFF4A460,
        SeaGreen             = 0xFF2E8B57,
        SeaShell             = 0xFFFFF5EE,
        Sienna               = 0xFFA0522D,
        Silver               = 0xFFC0C0C0,
        SkyBlue              = 0xFF87CEEB,
        SlateBlue            = 0xFF6A5ACD,
        SlateGray            = 0xFF708090,
        Snow                 = 0xFFFFFAFA,
        SpringGreen          = 0xFF00FF7F,
        SteelBlue            = 0xFF4682B4,
        Tan                  = 0xFFD2B48C,
        Teal                 = 0xFF008080,
        Thistle              = 0xFFD8BFD8,
        Tomato               = 0xFFFF6347,
        Transparent          = 0x00FFFFFF,
        Turquoise            = 0xFF40E0D0,
        Violet               = 0xFFEE82EE,
		VioletRed			 = 0xFFD02090,//html
        Wheat                = 0xFFF5DEB3,
        White                = 0xFFFFFFFF,
        WhiteSmoke           = 0xFFF5F5F5,
        Yellow               = 0xFFFFFF00,
        YellowGreen          = 0xFF9ACD32,
	};
};

/// \brief 渐变色
class GS_API GsGradsColor:public Utility::GsRefObject
{
protected:
	Utility::GsString m_strName;
	int m_nCount;
	GsColor m_from,m_to;
	virtual void CreateColor(){};
public:
	/// \brief 获取渐变色
	virtual GsColor Color( int nIndex) = 0;

	/// \brief 获取名称
	Utility::GsString Name();
	/// \brief 设置名称
	void Name(const char* strName);
	
	/// \brief 颜色数量
	virtual int Count();
	/// \brief 颜色数量
	virtual void Count(int nCount); 
	
	/// \brief 开始颜色
	GsColor From();
	/// \brief 开始颜色
	void From(const GsColor & color);

	/// \brief 结束颜色
	GsColor To();
	/// \brief 结束颜色
	void To(const GsColor & color);

};

/// \brief 方程式渐变色
enum GsFormulaGradesColorStyle
{
    eHSVFormula = 0,
    eHFormula = 1,
    eSFormula = 2,
    eVFormula = 3
};
/// \brief 方程式渐变色
class GS_API GsFormulaGradsColor:public GsGradsColor
{
	GsFormulaGradesColorStyle m_eStyle;
public:
	GsFormulaGradsColor(const GsColor& from,const GsColor& to);
	virtual GsColor Color( int nIndex);
	/// \brief 变换类型
	GsFormulaGradesColorStyle Style();
	/// \brief 变换类型
	void Style(GsFormulaGradesColorStyle style);
};


class GS_API GsRandomGradsColor:public GsGradsColor
{  
	GsColor m_from,m_to;
	Utility::GsVector<GsColor> m_Color;
	virtual void CreateColor();
public:
	GsRandomGradsColor(const GsColor& from,const GsColor& to);
	virtual GsColor Color( int nIndex);
};


/// \brief 画笔的类型
enum GsPenStyle
{ 
	/// \brief 实线
	/// \image html eSolidLine.png "SolidLine"
	eSolidLine,
    /// \brief 虚线
	/// \image html eDashLine.png "SolidLine"
	eDashLine,
    /// \brief 点线
	/// \image html eDotLine.png "SolidLine"
	eDotLine,
	/// \brief 点划线
    /// \image html eDashDotLine.png "SolidLine"
	eDashDotLine,
    /// \brief 点点划线
	/// \image html eDashDotDotLine.png "SolidLine"
	eDashDotDotLine,
    /// \brief 自定义线
	/// \image html eCustomDashLine.png "SolidLine"
	eCustomDashLine,
};

/// \brief 画笔端头类型
enum GsPenCapStyle
{
	/// \brief 平头
	/// \image html eFlatCap.png "FlatCap"
	eFlatCap,
    /// \brief 方头
	/// \image html eSquareCap.png "SquareCap"
	eSquareCap,
	/// \brief 圆头
    /// \image html eRoundCap.png "RoundCap"
	eRoundCap
};

/// \brief 画笔线段衔接类型
enum GsPenJoinStyle 
{  
    /// \brief 尖
    /// \image html eMiterJoin.png "MiterJoin"
	eMiterJoin,
    /// \brief 楔形
	/// \image html eBevelJoin.png "BevelJoin"
	eBevelJoin,
    /// \brief 圆头
    /// \image html eRoundJoin.png "RoundJoin"
	eRoundJoin,
};

/// \brief 画笔绘制的类型
enum GsBrushStyle 
{  
	eHorizontalBrush,
	eVerticalBrush,
	eFdiagonalBrush,
	eBdiagonalBrush,
	eCrossBrush,
	eDiagCrossBrush,
	eEmptyBrush,
	eSolidBrush,
} ; 
/// \brief 字体的式样
enum GsFontStyle
{
	eFontStyleRegular = 0,
	eFontStyleBlack = 0x01,
	eFontStyleBold = 0x02,
	eFontStyleItalic = 0x04,
	eFontStyleBoldItalic = eFontStyleBold | eFontStyleItalic,
	eFontStyleBlackItalic = eFontStyleBlack | eFontStyleItalic,
	eFontStyleUnderline = 0x08,
    eFontStyleStrikeout = 0x10,
};

/// \brief 字体的倾斜
enum GsItalic
{
	/// \brief 不倾斜
	eNormalItalic,
    /// \brief 右倾斜
	eRightItalic,
    /// \brief 左倾斜
	eLeftItalic
};
/// \brief 字符对齐类型
enum GsStringAlignment
{
	eStringAlignmentNear = 0,
    eStringAlignmentCenter = 1,
    eStringAlignmentFar = 2
};

/// \brief 字符串格式化对象
class GS_API GsStringFormat:public Utility::GsRefObject
{
protected:
	Utility::GsString m_strFont;
	float m_fSize;
	GsFontStyle m_FontStyle;
	//字符串行对齐
	GsStringAlignment m_eLineAlignment;
	//字符串行对齐
	GsStringAlignment m_eAlignment;
public:
	GsStringFormat();
	virtual ~GsStringFormat();
	
	/// \brief 设置字体名称
	virtual void Font(const char* strFont);
	/// \brief 获取字体名称
	virtual Utility::GsString Font();

	/// \brief 设置字体大小,单位磅
	virtual void FontSize(float fSize);
	/// \brief 设置字体大小,单位磅
	virtual float FontSize();

	/// \brief 字体式样
	virtual GsFontStyle FontStyle();
	/// \brief 字体式样
	virtual void FontStyle(GsFontStyle eStyle);

	/// \brief 列对齐
	virtual GsStringAlignment Alignment();
	/// \brief 列对齐
	virtual void Alignment (GsStringAlignment e);

	/// \brief 行对齐
	virtual GsStringAlignment LineAlignment();
	/// \brief 行对齐
	virtual void LineAlignment (GsStringAlignment e);
};
/// \brief GsStringFormatPtr
GS_SMARTER_PTR(GsStringFormat);  


/// \brief 路径对象
/// \details 复杂线、面绘制数据对象，从GsCanvas::CreatePath()创建
class GS_API GsGraphicsPath:public  Utility::GsRefObject
{
protected:
	GsGraphicsPath();
	GsMatrix m_Mx;
public:
	virtual ~GsGraphicsPath();
	/// \brief 开始一个子绘制段
	virtual bool StartFigure() = 0;
	/// \brief 封闭一个子绘制段
	virtual bool CloseFigure()= 0;
	/// \brief 恢复路径对象为初始状态
	virtual bool Reset()= 0;
	/// \brief 增加一根线段
	virtual bool AddLine(int x1,int y1,int x2,int y2);
	/// \brief 增加一根线段
	virtual bool AddLine(float x1,float y1,float x2,float y2);
	/// \brief 增加一段连续的线段
	virtual bool AddLines(float* pCoord,int nPointCount);
	/// \brief 增加一段连续的线段
	virtual bool AddLines(int* pCoord,int nPointCount);

	/// \brief 增加一段圆弧
	/// \param rect 圆弧所在椭圆的的矩形范围
	/// \param fStartAngle 圆弧开始的角度（单位度）
	/// \param fSweepAngle 圆弧旋转的角度（单位度）
	virtual bool AddArc(const Utility::GsRect& rect,float fStartAngle,float fSweepAngle);
	virtual bool AddArc(const Utility::GsRectF& rect,float fStartAngle,float fSweepAngle);
	
	/// \brief 增加三次贝塞尔曲线
	virtual bool AddBezier(float* pCoord,int nPointCount);
	/// \brief 增加三次贝塞尔曲线
	virtual bool AddBezier(int* pCoord,int nPointCount);

	/// \brief 增加二次贝塞尔曲线
	virtual bool AddQuadraticBezier (float* pCoord,int nPointCount);
	/// \brief 增加二次贝塞尔曲线
	virtual bool AddQuadraticBezier (int* pCoord,int nPointCount);
	
	/// \brief 向path中添加一个字符串
	virtual bool AddString(const char* str,int nLen,GsStringFormat* pStrFormat,const Utility::GsRectF &layoutRect);

	/// \brief 获取仿射变换矩阵
	virtual GsMatrix Transform();
	/// \brief 设置仿射变换矩阵
	virtual void Transform(const GsMatrix& rhs);
	
	/// \brief 重置仿射变换矩阵为单位矩阵
	virtual void ResetTransform();

	/// \brief 范围
	virtual Utility::GsRectF Envelope();


};
/// \brief GsGraphicsPathPtr
GS_SMARTER_PTR(GsGraphicsPath);

/// \brief 区域对象
/// \details 标示一个范围，可用于裁切
class GS_API GsRegion:public  Utility::GsRefObject
{
protected:
	GsRegion(){}

public:
	virtual ~GsRegion(){}

	/// \brief 区域的外接矩形范围
	virtual Utility::GsRect Bounds() = 0;
};
/// \brief GsRegionPtr
GS_SMARTER_PTR(GsRegion);

/// \brief 画刷对象基类
class GS_API GsBrush:public  Utility::GsRefObject
{
};
/// \brief GsBrushPtr
GS_SMARTER_PTR(GsBrush);

/// \brief 颜色填充画刷对象
class GS_API GsSolidBrush:public  GsBrush
{
protected:
	/// \brief 画刷的颜色
	GsColor m_Color;
	 
	GsBrushStyle m_Style;
public:

	/// \brief 从颜色构造画刷
	GsSolidBrush(const GsColor& c);
	virtual ~GsSolidBrush();
	/// \brief 画刷的颜色
	virtual GsColor Color();
	/// \brief 设置画刷的颜色
	virtual void Color(const GsColor& c);
	
	/// \brief 画笔的式样
	GsBrushStyle  Style();
	/// \brief 画笔的式样
	void Style(GsBrushStyle eStyle);
};
/// \brief GsSolidBrushPtr
GS_SMARTER_PTR(GsSolidBrush);

/// \brief 根据刷子样式获得最小范围(8*8)的像素块，用来填充
class GS_API GsBrushStylePixels
{
public:
	GsBrushStylePixels(GsBrushStyle eStyle,const GsColor &c);
	virtual ~GsBrushStylePixels();

public:
	virtual void CopyPattern(unsigned char* pBuffer);

protected:
	GsColor m_Color;
	GsBrushStyle m_eStyle;

	void WriteColor(const GsColor &c, unsigned char* pStream);
};
/// \brief GsBrushStylePixels
GS_SMARTER_PTR(GsBrushStylePixels);

/// \brief 使用图片填充的画刷
class GS_API GsTextureBrush:public  GsBrush
{
protected:
	Utility::GsImagePtr m_ptrImage;
	GsMatrix m_Matrix;
public:
	/// \brief 从图片构造画刷
	GsTextureBrush(Utility::GsImage* pImage);

	/// \brief 获取画刷的图像
	virtual  Utility::GsImage* Image();
	/// \brief 设置画刷的图像
	virtual  void Image(Utility::GsImage* pImg);

	/// \brief 获取变换矩阵
	virtual GsMatrix Transform();
	/// \brief 设置变换矩阵
	virtual void Transform(const GsMatrix& mx);
	
};
/// \brief GsTextureBrushPtr
GS_SMARTER_PTR(GsTextureBrush);


/// \brief 使用线型渐变填充的画刷
class GS_API GsLinearGradientBrush:public  GsBrush
{
protected:
	/// \brief 画刷的开始结束颜色
	GsColor m_StartColor;
	GsColor m_EndColor;
	/// \brief 画刷的开始结束位置
	Utility::GsPTF m_StartPoint;
	Utility::GsPTF m_EndPoint;

public:
	/// \brief 默认构造画刷
	GsLinearGradientBrush();
	/// \brief 从颜色和区域构造画刷
	GsLinearGradientBrush(const GsColor& StartColor, const GsColor& EndColor);

	virtual ~GsLinearGradientBrush();
	/// \brief 画刷的颜色
	virtual GsColor StartColor();
	/// \brief 设置画刷的颜色
	virtual void StartColor(const GsColor& c);
	/// \brief 画刷的颜色
	virtual GsColor EndColor();
	/// \brief 设置画刷的颜色
	virtual void EndColor(const GsColor& c);
	/// \brief 渐变的开始位置
	virtual Utility::GsPTF StartPoint();
	/// \brief 设置渐变的开始位置
	virtual void StartPoint(const Utility::GsPTF& c);
	/// \brief 渐变的结束位置
	virtual Utility::GsPTF EndPoint();
	/// \brief 设置渐变的结束位置
	virtual void EndPoint(const Utility::GsPTF& c);
};
/// \brief GsLinearGradientBrushPtr
GS_SMARTER_PTR(GsLinearGradientBrush);

/// \brief 使用辐射渐变填充的画刷
class GS_API GsRadialGradientBrush:public  GsBrush
{
protected:
	/// \brief 画刷的开始结束颜色
	GsColor m_StartColor;
	GsColor m_EndColor;
	/// \brief 辐射中心
	Utility::GsPTF m_CenterPoint;
	/// \brief 辐射半径
	float m_Radius;

public:
	/// \brief 默认构造画刷
	GsRadialGradientBrush();
	/// \brief 从颜色和范围构造画刷
	GsRadialGradientBrush(const GsColor& StartColor, const GsColor& EndColor);

	virtual ~GsRadialGradientBrush();
	/// \brief 画刷的颜色
	virtual GsColor StartColor();
	/// \brief 设置画刷的颜色
	virtual void StartColor(const GsColor& c);
	/// \brief 画刷的颜色
	virtual GsColor EndColor();
	/// \brief 设置画刷的颜色
	virtual void EndColor(const GsColor& c);
	
	/// \brief 辐射中心
	virtual Utility::GsPTF CenterPoint();
	/// \brief 设置辐射中心
	virtual void CenterPoint(const Utility::GsPTF& p);
	/// \brief 辐射半径
	virtual float Radius();
	/// \brief 设置辐射半径
	virtual void Radius(const float f);
};
/// \brief GsRadialGradientBrushPtr
GS_SMARTER_PTR(GsRadialGradientBrush);


/// \brief 画笔对象
class GS_API GsPen:public  Utility::GsRefObject
{ 
protected:
	/// \brief 画笔的颜色
	GsColor  m_Color;
	/// \brief 画笔的宽度
	float	m_fWidth;
	/// \brief 画笔的类型
	GsPenStyle m_ePenStyle;
	/// \brief 画笔的模板
	UTILITY_NAME::GsVector<float> m_vecDashPattern;
	/// \brief 画笔偏移长度
	float m_fDashOffset;
	/// \brief 线衔接尖头限制
	float m_fMiterLimit;
	/// \brief 线开始和结束头类型
	GsPenCapStyle m_eStartCap,m_eEndCap;
	/// \brief 线衔接类型
	GsPenJoinStyle m_eLineJoin;
	/// \brief 画笔使用的画刷。
	GsBrushPtr m_ptrBrush;
public:
	/// \brief 从颜色和宽度构造画笔
	GsPen(const GsColor& color,float fWidth =1.0f);
	
	/// \brief 从画刷构建画笔
	GsPen(GsBrush* pBrush,float fWidth =1.0f);

	/// \brief 获取画刷
	virtual GsBrush* Brush();
	/// \brief 设置画刷
	virtual void Brush(GsBrush* pBrush);

	virtual ~GsPen();
	/// \brief 颜色
	virtual GsColor Color() const;
	/// \brief 设置颜色
	virtual void Color(const GsColor &);
	
	/// \brief 宽度
	virtual float Width() const;
	/// \brief 设置宽度
	virtual void Width(float w);

	/// \brief 画笔式样
	virtual GsPenStyle Style() const;
	/// \brief 设置画笔式样
	virtual void Style(GsPenStyle eStyle);
	
	/// \brief 点线、点划线偏移量
	virtual void DashOffset(float offset);
	/// \brief 获取点线、点划线偏移量
	virtual float DashOffset()const;

	/// \brief 设置自定义点划线的模板
	virtual void DashPattern(const UTILITY_NAME::GsVector<float>& pattern);
	/// \brief 自定义点划线的模板
	virtual UTILITY_NAME::GsVector<float> DashPattern() const;
	
	/// \brief 线衔接尖头限制
	virtual float MiterLimit() const;
	/// \brief 设置线衔接尖头限制
	virtual void MiterLimit(float f);

	/// \brief 线起点的头类型
	virtual GsPenCapStyle StartCap() const;
	/// \brief 设置线起点的头类型
	virtual void StartCap(GsPenCapStyle style);

	/// \brief 线终点的头类型
	virtual GsPenCapStyle EndCap() const;
	/// \brief 设置线终点的头类型
	virtual void EndCap(GsPenCapStyle style);

	/// \brief 线衔接类型
	virtual GsPenJoinStyle LineJoin() const;
	/// \brief 设置线衔接类型
	virtual void LineJoin(GsPenJoinStyle style);
};
/// \brief GsPenPtr
GS_SMARTER_PTR(GsPen);

/// \brief 图像效果。
class GS_API GsImageEffect:public Utility::GsRefObject
{
	float m_fTransparency;
public:
	GsImageEffect();
	virtual ~GsImageEffect();

	/// \brief 图像透明度[0-1]，0表示不透明，1表示完全透明
	float Transparency();

	/// \brief 图像透明度[0-1]，0表示不透明，1表示完全透明
	void Transparency(float fTransparency);

	/// \brief 不透明度[0-1],0表示完全透明，1表示完全不透明，与透明度相反，等于1-Transparency();
	float Opacity();

};
/// \brief GsImageEffectPtr
GS_SMARTER_PTR(GsImageEffect);


class GsImageCanvas;
/// \brief 画布的抽象基类
/// \details 通过继承此基类实现不同平台和不同绘制技术的画布
class GS_API GsCanvas:public Utility::GsRefObject
{
protected:
	/// \brief 缺省的矩阵
	GsMatrix m_Matrix; 

	/// \brief 绘制起算的原点
	Utility::GsPT m_RenderingOrigin;

	/// \brief 画布的可视范围
	Utility::GsRectF     m_VisibleBound;
	/// \brief 裁切范围
	GsRegionPtr m_ptrClipRegion;
	
	/// \brief 画布设备类型
	int m_DeviceType;
	GsCanvas();
public:
	virtual ~GsCanvas();

	/// \brief 画布设备类型
	int DeviceType();

	/// \brief 画布设备类型
	void DeviceType(int eType);

	/// \brief 获取绘制起算的原点
	virtual Utility::GsPT RenderingOrigin();
	/// \brief 设置绘制起算的原点
	virtual void RenderingOrigin(const Utility::GsPT& pt);


	/// \brief 获取仿射变换矩阵
	virtual GsMatrix Transform();
	/// \brief 设置仿射变换矩阵
	virtual void Transform(const GsMatrix& rhs);
	
	/// \brief 重置仿射变换矩阵为单位矩阵
	virtual void ResetTransform();

	/// \brief 画布裁切的最大矩形范围或者画布的范围
	/// \details 画布如果设置了裁切范围则返回裁切的最大矩形，否则返回画布的范围
	virtual Utility::GsRect ClipBounds();

	/// \brief 清除裁切范围
	virtual void ClearClip();

	/// \brief 设置矩形的裁切范围
	virtual void Clip(const Utility::GsRect& rhs);

	/// \brief 设置path作为的裁切范围
	virtual void Clip(GsGraphicsPath*  path);

	/// \brief 设置path作为的裁切范围
	virtual void Clip(GsRegion *  region);

	/// \brief 获取裁切范围
	virtual GsRegion* Clip();

	/// \brief 创建一个裁切范围
	virtual GsRegionPtr CreateRegion(GsGraphicsPath*  path)=0;
	/// \brief 创建一个裁切范围
	virtual GsRegionPtr CreateRegion(const Utility::GsRect& rect)=0;

	/// \brief 一个点是否在画布可见
	virtual bool IsVisible(int x,int y);
	/// \brief 一个点是否在画布可见
	virtual bool IsVisible(float x,float y);

	/// \brief 一个点是否在画布可见
	virtual bool IsVisible(Utility::GsPT& pt);
	/// \brief 一个点是否在画布可见
	virtual bool IsVisible(Utility::GsPTF& pt);

	/// \brief 一个矩形是否在画布中可见
	virtual bool IsVisible(const Utility::GsRectF& rect);
	/// \brief 一个矩形是否在画布中可见
	virtual bool IsVisible(const Utility::GsRect& rect);

	/// \brief 一个线段是否在画布中可见
	virtual bool IsVisible(float x1,float y1,float x2,float y2);
	/// \brief 一个线段是否在画布中可见
	virtual bool IsVisible(int x1,int y1,int x2,int y2);
	/// \brief 一个点或者一段线是否在画布中可见
	virtual bool IsVisible(float* pt,int nPointCount);
	/// \brief 一段线是否在画布中可见
	virtual bool IsVisible(int* pt,int nPointCount);


public:

	/// \brief 绘制一根线段
	/// \param x1 线段的起点x坐标
	/// \param y1 线段的起点y坐标
    /// \param x2 线段的终点x坐标
	/// \param y2 线段的终点y坐标
	/// \param pPen 画笔对象
    /// \return 返回绘制是否成功
	virtual bool DrawLine(float x1,float y1,float x2,float y2,GsPen* pPen);
	
	/// \brief 从文件创建存放于内存中的图像对象
	/// \param strFile 要打开的文件名
	/// \return 返回图像对象
	virtual Utility::GsImagePtr CreateImage(const char* strFile);

	/// \brief 从内存块直接创建图像对象
	/// \param strData 图像的内存块
	/// \param nLen  内存块的长度
	/// \return 返回图像对象
	virtual Utility::GsImagePtr CreateImage(const unsigned char* strData,int nLen) = 0;

	
	/// \brief 创建一个和当前画布兼容的Image画布
	/// \param width 画布的高度
	/// \param height 画布的宽度
	virtual Utility::GsSmarterPtr<GsImageCanvas> CreateCompatibleImageCanvas(int width,int height) = 0;

public:
	/// \brief 创建画笔对象
	/// \param color 画笔的颜色
	/// \param  fWidth 画笔的宽度，像素单位
    /// \returns 返回创建的画笔对象指针
	virtual GsPenPtr CreatePen(const GsColor& color,float fWidth = 1.0f);
	
	/// \brief 从画刷创建画笔对象
	/// \param pBrush 画刷
	/// \param fWidth 画笔的宽度，像素单位
    /// \returns 返回创建的画笔对象指针
	virtual GsPenPtr CreatePen(GsBrush* pBrush,float fWidth = 1.0f);
	
	/// \brief 创建颜色画刷
	/// \param color 画刷的颜色 
    /// \returns 返回创建的画刷对象指针
	virtual GsSolidBrushPtr CreateSolidBrush(const GsColor& color) ;
	
	/// \brief 创建纹理画刷
	/// \param pImg 画刷使用的纹理
    /// \returns 返回创建的画刷对象指针
	virtual GsTextureBrushPtr CreateTextureBrush(Utility::GsImage* pImg) ;

	/// \brief 创建颜色线性渐变画刷
	/// \param StartColor 变化开始的颜色
	/// \param EndColor 变化结束的颜色
    /// \returns 返回创建的画刷对象指针
	virtual GsLinearGradientBrushPtr CreateLinearGradientBrush(const GsColor& StartColor, const GsColor& EndColor);
	
	/// \brief 创建颜色辐射渐变画刷
	/// \param StartColor 变化开始的颜色
	/// \param EndColor 变化结束的颜色
    /// \returns 返回创建的画刷对象指针
	virtual GsRadialGradientBrushPtr CreateRadialGradientBrush(const GsColor& StartColor, const GsColor& EndColor);

	/// \brief 创建路径对象 
    /// \returns 返回创建的路径对象指针
	virtual GsGraphicsPathPtr CreatePath() = 0;
	

	/// \brief 创建字符串格式化话对象
    /// \returns 返回字符串格式化对象
	virtual GsStringFormatPtr CreateStringFormat();
public: 
	/// \brief 绘制多个点连续的线 
	/// \param pCoord 坐标数组指针，以X，Y坐标连续存储
	/// \param nPointCount 坐标点的数量
	/// \param pPen 绘制的画笔
	/// \returns 返回绘制是否成功
	virtual bool DrawLine(float* pCoord,int nPointCount,GsPen* pPen) = 0;
	 
	// \brief 绘制一段圆弧
	/// \param rect 圆弧所在椭圆的的矩形范围
	/// \param fStartAngle 圆弧开始的角度（单位度）
	/// \param fSweepAngle 圆弧旋转的角度（单位度）
	/// \param pPen 绘制的画笔
	virtual bool DrawArc(const Utility::GsRectF& rect,float fStartAngle,float fSweepAngle,GsPen* pPen);
	 
	// \brief 绘制一段圆弧
	/// \param rect 圆弧所在椭圆的的矩形范围
	/// \param fStartAngle 圆弧开始的角度（单位度）
	/// \param fSweepAngle 圆弧旋转的角度（单位度）
	/// \param pPen 绘制的画笔
	virtual bool DrawArc(const Utility::GsRect& rect,float fStartAngle,float fSweepAngle,GsPen* pPen);

	/// \brief 绘制椭圆 
	/// \param rect 椭圆的外接矩形范围
	/// \param pPen 绘制的画笔 
	/// \returns 返回绘制是否成功
	virtual bool DrawEllipse(const Utility::GsRectF& rect,GsPen* pPen);
	/// \brief 绘制椭圆 
	/// \param rect 椭圆的外接矩形范围
	/// \param pPen 绘制的画笔 
	/// \returns 返回绘制是否成功
	virtual bool DrawEllipse(const Utility::GsRect& rect,GsPen* pPen);

	 
	/// \brief 填充椭圆 
	/// \param rect 椭圆的外接矩形范围
	/// \param pBrush 填充的画刷 
	/// \returns 返回绘制是否成功
	virtual bool FillEllipse(const Utility::GsRectF& rect,GsBrush* pBrush);
	 
	/// \brief 填充椭圆 
	/// \param rect 椭圆的外接矩形范围
	/// \param pBrush 填充的画刷 
	/// \returns 返回绘制是否成功
	virtual bool FillEllipse(const Utility::GsRect& rect,GsBrush* pBrush);

	/// \brief 绘制矩形 
	/// \param rect 矩形范围
	/// \param pPen 绘制的画笔 
	/// \returns 返回绘制是否成功
	virtual bool DrawRectangle(const Utility::GsRect& rect,GsPen* pPen);

	/// \brief 绘制矩形 
	/// \param rect 矩形范围
	/// \param pPen 绘制的画笔 
	/// \returns 返回绘制是否成功
	virtual bool DrawRectangle(const Utility::GsRectF& rect,GsPen* pPen);

	/// \brief 填充矩形 
	/// \param rect 矩形范围
	/// \param pBrush 填充的画刷 
	/// \returns 返回绘制是否成功
	virtual bool FillRectangle(const Utility::GsRect& rect,GsBrush* pBrush);

	/// \brief 填充矩形 
	/// \param rect 矩形范围
	/// \param pBrush 填充的画刷 
	/// \returns 返回绘制是否成功
	virtual bool FillRectangle(const Utility::GsRectF& rect,GsBrush* pBrush);

	/// \brief 绘制Pie轮廓
	/// \param rect Pie所在椭圆的外接矩形范围
	/// \param start_angle 饼的开始角度
	/// \param sweepAngle 饼的旋转角度
	/// \param pPen 绘制的画笔
	/// \returns 返回绘制是否成功
	virtual bool DrawPie(const Utility::GsRectF  &rect,float start_angle,float sweepAngle,GsPen* pPen);

	/// \brief 填充Pie
	/// \param rect Pie所在椭圆的外接矩形范围
	/// \param start_angle 饼的开始角度
	/// \param sweepAngle 饼的旋转角度
	/// \param pBrush 填充的画刷 
	/// \returns 返回绘制是否成功
	virtual bool FillPie(const Utility::GsRectF  &rect,float start_angle,float sweepAngle,GsBrush* pBrush);

	/// \brief 绘制路径 
	/// \param pPath 要绘制的路径
	/// \param pPen 绘制的画笔 
	/// \returns 返回绘制是否成功
	virtual bool DrawPath(GsGraphicsPath* pPath,GsPen* pPen) = 0;
	 
	/// \brief 填充路径 
	/// \param pPath 要填充的路径
	/// \param pBrush 填充的画刷 
	/// \returns 返回绘制是否成功
	virtual bool FillPath(GsGraphicsPath* pPath,GsBrush* pBrush) = 0;

	/// \brief 绘制一个位图到指定坐标
	/// \param pImg 要绘制的图片
	/// \param pt 图片左上角对应绘制的坐标y坐标 
	/// \param effect 图像处理的效果
	/// \return 返回绘制是否成功
	virtual bool DrawImage(Utility::GsImage* pImg, const Utility::GsPT& pt = Utility::GsPT(), GsImageEffect* effect = NULL);
	 

	/// \brief 绘制图片的特定部分到特定范围 
	/// \param pImg 要绘制的图片
	/// \param source 图片要绘制的范围 
	/// \param target 要绘制到的目标范围
	/// \returns 返回绘制是否成功
	virtual bool DrawImage(Utility::GsImage* pImg,const Utility::GsRect& source,const Utility::GsRectF& target);
	
	/// \brief 绘制图片的特定部分到特定范围 
	/// \param pImg 要绘制的图片
	/// \param source 图片要绘制的范围 
	/// \param target 要绘制到的目标范围
	/// \returns 返回绘制是否成功
	virtual bool DrawImage(Utility::GsImage* pImg, const Utility::GsRect& source, const Utility::GsRectF& target, GsImageEffect* effect);

	/// \brief 用特定颜色清空整个画布  
	/// \param color 要清空的颜色  
	/// \returns 返回绘制是否成功 
	virtual bool Clear(const GsColor& color) = 0;
	 
	/// \brief 提交未提交的绘制数据   
	/// \returns 返回提交是否成功 
	virtual  bool Flush() = 0;


	/// \brief 绘制字符串
	/// \returns 返回提交是否成功 
	virtual bool DrawString(const char* strText,int nLen,GsBrush* pBrush,const Utility::GsRectF& eLayout,GsStringFormat* pFormat) = 0;

	/// \brief 绘制字符串
	/// \returns 返回提交是否成功 
	virtual bool DrawString(const char* strText,int nLen,GsBrush* pBrush, const Utility::GsPTF& loc,GsStringFormat* pFormat);

	/// \brief 测试字符串绘制的大小
	/// \returns 返回测试是否成功
	virtual bool MeasureString(const char* strText,int nLen,Utility::GsRectF& eLayout,GsStringFormat* pFormat);

}; 
/// \brief GsCanvasPtr  
GS_SMARTER_PTR(GsCanvas);

/// \brief 在位图上会绘制的画布  
class GS_API GsImageCanvas:public GsCanvas
{
public:
	/// \brief 获取绘制的位图
	virtual Utility::GsImage* Image() = 0; 
};
/// \brief GsImageCanvasPtr  
GS_SMARTER_PTR(GsImageCanvas);

/// \brief 基于Cairo渲染引擎的画布
class GS_API GsCairoMemoryImageCanvas :public GsImageCanvas
{
protected:
	Utility::GsImagePtr m_ptrImage;
	void* m_Backend;
	GsCairoMemoryImageCanvas(void* backend);
public:

	using GsCanvas::RenderingOrigin;
	using GsCanvas::Clip;
	using GsCanvas::DrawLine;
	using GsCanvas::DrawImage;
	using GsCanvas::DrawString;
	using GsCanvas::CreateImage;
	using GsCanvas::Transform;

	GsCairoMemoryImageCanvas(int w, int h, float dpi=96.0f);
	GsCairoMemoryImageCanvas(int w, int h,const Utility::GsConfig& config,float dpi = 96.0f);

	virtual ~GsCairoMemoryImageCanvas(); 
	  
	/// \brief 配置对象
	Utility::GsConfig& Config();
	void Config(const Utility::GsConfig& config);

	/// \brief 创建纹理画刷
	/// \param pImg 画刷使用的纹理
	/// \returns 返回创建的画刷对象指针
	virtual GsTextureBrushPtr CreateTextureBrush(Utility::GsImage* pImg);

	/// \brief 创建颜色线性渐变画刷
	/// \param StartColor 变化开始的颜色
	/// \param EndColor 变化结束的颜色
	/// \returns 返回创建的画刷对象指针
	virtual GsLinearGradientBrushPtr CreateLinearGradientBrush(const GsColor& StartColor, const GsColor& EndColor);

	/// \brief 创建颜色辐射渐变画刷
	/// \param StartColor 变化开始的颜色
	/// \param EndColor 变化结束的颜色
	/// \returns 返回创建的画刷对象指针
	virtual GsRadialGradientBrushPtr CreateRadialGradientBrush(const GsColor& StartColor, const GsColor& EndColor);



	/// \brief 获取绘制的位图
	virtual Utility::GsImage* Image();
	/// \brief 设置仿射变换矩阵
	virtual void Transform(const GsMatrix& rhs);


	/// \brief 重置仿射变换矩阵为单位矩阵
	virtual void ResetTransform();

	/// \brief 设置绘制起算的原点
	virtual void RenderingOrigin(const Utility::GsPT& pt);

	/// \brief 创建一个裁切范围
	virtual GsRegionPtr CreateRegion(GsGraphicsPath*  path);
	/// \brief 创建一个裁切范围
	virtual GsRegionPtr CreateRegion(const Utility::GsRect& rect);
	/// \brief 设置path作为的裁切范围
	virtual void Clip(GsRegion *  region);
	/// \brief 清除裁切范围
	virtual void ClearClip();

	
	/// \brief 创建一个和当前画布兼容的Image画布
	/// \param width 画布的高度
	/// \param height 画布的宽度
	virtual GsImageCanvasPtr CreateCompatibleImageCanvas(int width, int height);

	
	/// \brief 绘制多个点连续的线 
	/// \param pCoord 坐标数组指针，以X，Y坐标连续存储
	/// \param nPointCount 坐标点的数量
	/// \param pPen 绘制的画笔
	/// \returns 返回绘制是否成功
	virtual bool DrawLine(float* pCoord, int nPointCount, GsPen* pPen);


	/// \brief 绘制Pie轮廓
	/// \param rect Pie所在椭圆的外接矩形范围
	/// \param start_angle 饼的开始角度
	/// \param sweepAngle 饼的旋转角度
	/// \param pPen 绘制的画笔
	/// \returns 返回绘制是否成功
	virtual bool DrawPie(const Utility::GsRectF  &rect, float start_angle, float sweepAngle, GsPen* pPen);

	/// \brief 填充Pie
	/// \param rect Pie所在椭圆的外接矩形范围
	/// \param start_angle 饼的开始角度
	/// \param sweepAngle 饼的旋转角度
	/// \param pBrush 填充的画刷 
	/// \returns 返回绘制是否成功
	virtual bool FillPie(const Utility::GsRectF  &rect, float start_angle, float sweepAngle, GsBrush* pBrush);

	virtual bool DrawRectangle(const Utility::GsRectF& rect, GsPen* pPen);
	virtual bool FillRectangle(const Utility::GsRectF& rect, GsBrush* pBrush);

	virtual bool DrawEllipse(const Utility::GsRectF& rect, GsPen* pPen);
	virtual bool FillEllipse(const Utility::GsRectF& rect, GsBrush* pBrush);

	/// \brief 绘制图片的特定部分到特定范围 
	/// \param pImg 要绘制的图片
	/// \param source 图片要绘制的范围 
	/// \param target 要绘制到的目标范围
	/// \returns 返回绘制是否成功
	virtual bool DrawImage(Utility::GsImage* pImg, const Utility::GsRect& source, const Utility::GsRectF& target, GsImageEffect* effect);


	/// \brief 从内存块直接创建图像对象
	/// \param strData 图像的内存块
	/// \param nLen 内存块的长度
	/// \return 返回图像对象
	virtual Utility::GsImagePtr CreateImage(const unsigned char* strData, int nLen);

	/// \brief 创建路径对象 
	/// \returns 返回创建的路径对象指针
	virtual GsGraphicsPathPtr CreatePath();

	/// \brief 绘制路径 
	/// \param pPath 要绘制的路径
	/// \param pPen 绘制的画笔 
	/// \returns 返回绘制是否成功
	virtual bool DrawPath(GsGraphicsPath* pPath, GsPen* pPen);


	/// \brief 填充路径 
	/// \param pPath 要填充的路径
	/// \param pBrush 填充的画刷 
	/// \returns 返回绘制是否成功
	virtual bool FillPath(GsGraphicsPath* pPath, GsBrush* pBrush);

	/// \brief 绘制圆弧
	/// \param rect 要绘制圆弧的外接矩形
	/// \param dblStartAngle 开始角度
	/// \param dblEndAngle 结束角度
	/// \param pPen 绘制的画笔 
	/// \returns 返回绘制是否成功
	virtual bool DrawArc(const Utility::GsRectF& rect, float dblStartAngle, float dblEndAngle, GsPen* pPen);

	/// \brief 用特定颜色清空整个画布  
	/// \param color 要清空的颜色  
	/// \returns 返回绘制是否成功 
	virtual bool Clear(const GsColor& color);

	/// \brief 提交未提交的绘制数据   
	/// \returns 返回提交是否成功 
	virtual  bool Flush();

	/// \brief 绘制字符串
	/// \returns 返回提交是否成功 
	virtual bool DrawString(const char* strText, int nLen, GsBrush* pBrush, const Utility::GsRectF& eLayout, GsStringFormat* pFormat);
	virtual bool MeasureString(const char* strText, int nLen, Utility::GsRectF &eLayout, GsStringFormat* pFormat);
};
/// \brief GsCairoMemoryImageCanvasPtr  
GS_SMARTER_PTR(GsCairoMemoryImageCanvas);


/// \brief 基于Cairo引擎的内存画布  
class GS_API GsCairoSVGImageCanvas:public GsCairoMemoryImageCanvas
{
	Utility::GsStringStream	m_SS;
	Utility::GsString	m_FileName;
public:
	GsCairoSVGImageCanvas(const char* svgFile,int w, int h, float dpi = 96);
	GsCairoSVGImageCanvas(int w, int h, float dpi = 96);
	virtual ~GsCairoSVGImageCanvas();
	/// \brief 获取绘制的位图
	virtual Utility::GsImage* Image();

	/// \brief 获取SVG的字符串
	/// \details 如果是文件svg则返回文件名称
	Utility::GsString SVG();
};
/// \brief GsCairoSVGImageCanvasPtr  
GS_SMARTER_PTR(GsCairoSVGImageCanvas);



KERNEL_ENDNS