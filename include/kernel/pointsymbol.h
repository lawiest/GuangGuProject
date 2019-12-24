#pragma once
#include "symbol.h"
#include "geometry.h"
#include <qrcode.h>
#include  <mathhelp.h>
KERNEL_NS    
/// \brief 制图线端套类型
enum GsLineCapStyle
{
	eLineCapButt = 512,
	eLineCapRound = 0,
	eLineCapSquare = 256
};

/// \brief 制图线接合类型
enum GsLineJoinStyle
{
	eLineJoinMiter = 8192,
	eLineJoinRound = 0,
	eLineJoinBevel = 4096
};
/// \brief 点符号
class GS_API GsPointSymbol:public GsSymbol, public UTILITY_NAME::GsSerialize
{
protected:
	/// \brief 大小
	double m_dblSize;

	/// \brief 颜色
	GsColor m_Color;
	/// \brief 旋转角
	double m_dblAngle;

	/// \brief xy的偏移
	GsRawPoint m_Offset;

protected:
	
	/// \brief 当绘制的时候发生
	/// \details 子类通过覆盖此方法实现实际的绘制
	virtual void OnDraw(GsGeometryBlob* pBuffer);

	/// \brief 当绘制点的时候发生
	/// \details 子类通过覆盖此函数实现点的绘制。基类已经实现了地理坐标到屏幕坐标的转换
	virtual void OnPointDraw(const float* pScreenPoint,int nLen);
	virtual void OnPointDraw();
	
	/// \brief 重新计算符号的Size
	void RecalculateSize();

	/// \brief 缺省构造
	GsPointSymbol();
public:
	virtual ~GsPointSymbol();
	/// \brief 符号类型
	virtual GsSymbolType Type();
	/// \brief 符号大小
	virtual double Size();
	/// \brief 设置符号大小
	virtual void Size(double dblSize);

	/// \brief 符号大小
	virtual double DesignSize();

	/// \brief 颜色
	virtual GsColor Color();
	/// \brief 设置颜色
	virtual void Color(const GsColor& c);

	/// \brief 获取旋转角
	virtual double Angle();
	/// \brief 设置旋转角
	virtual	void Angle(double angle);

	/// \brief x偏移
	double OffsetX();
	/// \brief y偏移
	double OffsetY();
	/// \brief x偏移
	void OffsetX(double val);
	/// \brief y偏移
	void OffsetY(double val);
	/// \brief 偏移
	GsRawPoint Offset();
	/// \brief 偏移
	void Offset(const GsRawPoint& val);
	/// \brief 偏移
	void Offset(double x,double y);

	/// \brief 点符号的矩形范围
	virtual GsBox Envelope();

	// 通过 GsSerialize 继承
	virtual bool Serialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual bool DeSerialize(UTILITY_NAME::GsSerializeStream * pSerStream) override;

	virtual UTILITY_NAME::GsString ClassName();

};
/// \brief GsPointSymbolPtr
GS_SMARTER_PTR(GsPointSymbol);

/// \brief 简单点符号
class GS_API GsSimplePointSymbol:public GsPointSymbol
{
	GsSolidBrushPtr m_ptrBrush;
protected:

	virtual void OnPointDraw();
	
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();
public:
	GsSimplePointSymbol();
	/// \brief 从颜色和size构造
	GsSimplePointSymbol(const GsColor& c,double dblSize = 1);
	virtual ~GsSimplePointSymbol();
	 
};
GS_SMARTER_PTR(GsSimplePointSymbol);
DECLARE_CLASS_CREATE(GsSimplePointSymbol)

 
/// \brief 面类型的点符号抽象基类
class GS_API GsSurfacePointSymbol:public GsPointSymbol
{
protected:
	GsBrushStyle m_FillType;
	GsColor			m_nFillColor;
	float 			m_fLineWidth;
	GsSurfacePointSymbol();
public:
	virtual ~GsSurfacePointSymbol();
	/// \brief 填充的画笔类
	GsBrushStyle FillType();
	/// \brief 填充的画笔类
	void FillType(GsBrushStyle eType);
	/// \brief 填充的画笔类
	GsColor FillColor();
	/// \brief 填充的画笔类
	void FillColor(GsColor c);
	 

	/// \brief 线的宽度
	float LineWidth();
	void LineWidth(float fWidth);

};
GS_SMARTER_PTR(GsSurfacePointSymbol);


/// \brief 类椭圆点符号基类
template<class T>
class GsEllipsePointSymbolT:public T
{
protected:
	double m_dblLongAxis, m_dblShortAxis;
	GsEllipsePointSymbolT()
	{
		m_dblLongAxis = 1;
		m_dblShortAxis = 1; 
	}
public:
	virtual ~GsEllipsePointSymbolT(){}
	
	/// \brief 获取椭圆属性
	void EllipseParameter(double* longAxis,double* shortAxis,double* angle)
	{
		if(longAxis)
			*longAxis = m_dblLongAxis;
		if(shortAxis)
			*shortAxis = m_dblShortAxis;
		if(angle)
			*angle = this->m_dblAngle;
	}
	/// \brief 设置椭圆属性
	void EllipseParameter(double dblLongAxis, double dblShortAxis,double dblAngle = 0.0)
	{
		m_dblLongAxis  = dblLongAxis;
		m_dblShortAxis = dblShortAxis;
		this->m_dblSize = 2* Utility::GsMath::Max(m_dblShortAxis,m_dblLongAxis);
		this->m_dblAngle =  dblAngle;
	}
	/// \brief 获取长轴长度
	double LongAxis()
	{
		return m_dblLongAxis;
	}
	/// \brief 获取短轴长度
	double ShortAxis()
	{
		return m_dblShortAxis;
	}
	/// \brief 设置长轴长度
	void LongAxis(double val)
	{
		m_dblLongAxis = fabs(val);
		this->m_dblSize = 2 * Utility::GsMath::Max(m_dblShortAxis,m_dblLongAxis);
	}
	/// \brief 设置短轴长度
	void ShortAxis(double val)
	{
		m_dblShortAxis = fabs(val);
		this->m_dblSize = 2 * Utility::GsMath::Max(m_dblShortAxis,m_dblLongAxis);
	}
	/// \brief 获取符号大小
	virtual double Size()
	{ 
		this->m_dblSize = 2 * Utility::GsMath::Max(m_dblShortAxis,m_dblLongAxis); 
		return this->m_dblSize;
	}
	/// \brief 设置符号大小
	virtual void Size(double dblSize)
	{
		if(dblSize <=0 || Utility::GsMath::IsEqual(dblSize,this->m_dblSize))
			return;
		double dblScale = dblSize / this->m_dblSize;
		m_dblLongAxis*=dblScale;
		m_dblShortAxis*=dblScale;

		T::Size(dblSize);
	}

	/// \brief 点符号的矩形范围
	virtual GsBox Envelope()
	{
		GsBox env(-m_dblLongAxis,-m_dblShortAxis,m_dblLongAxis,m_dblShortAxis);	
		//旋转
		if(fabs(this->m_dblAngle) > DBL_EPSILON)
		{
			double f[8] = {-m_dblLongAxis,-m_dblShortAxis,
				m_dblLongAxis,-m_dblShortAxis,
				m_dblLongAxis,m_dblShortAxis,
				-m_dblLongAxis,m_dblShortAxis,};
			GsMatrix mat;
			mat.Rotate(-this->m_dblAngle * Utility::GsMath::Pi() / 180);		
			mat.TransformPointsT(f,4);
			for (int i = 0; i < 2; i++)
			{
				env.Union(GsBox(f[4*i],f[4*i+1],f[4*i+2],f[4*i+3]));
			}
		}

		env.Offset(this->m_Offset);
		return env;
	}
}; 

/// \brief 椭圆的点符号。
class GS_API GsEllipsePointSymbol:public GsEllipsePointSymbolT<GsSurfacePointSymbol>
{
	GsSolidBrushPtr m_ptrBrush;
	GsPenPtr m_ptrPen;
protected:	
	/// \brief 当绘制点的时候发生
	/// \details 子类通过覆盖此函数实现点的绘制。基类已经实现了地理坐标到屏幕坐标的转换
	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();
	
public:
	GsEllipsePointSymbol();
	GsEllipsePointSymbol(double dblLongAxis, double dblShortAxis, double dblAngle = 0.0);
	~GsEllipsePointSymbol();
};
/// \brief GsEllipsePointSymbolPtr
GS_SMARTER_PTR(GsEllipsePointSymbol);
DECLARE_CLASS_CREATE(GsEllipsePointSymbol)

/// \brief pie符号
class GS_API GsPiePointSymbol:public GsEllipsePointSymbolT<GsSurfacePointSymbol>
{
	GsPenPtr m_ptrPen;
	GsSolidBrushPtr	m_ptrBrush; 
	double               m_StartAngle;       //弦形起始角
	double               m_EndAngle;         //弦形终止角
protected:
	virtual void OnPointDraw();

	virtual void OnStartDrawing();
	virtual void OnEndDrawing();
public:
	GsPiePointSymbol();
	virtual ~GsPiePointSymbol();
	   
	/// \brief 设置Pie开始角度
	void StartAngle(double angle);
	/// \brief 获取Pie开始角度
	double StartAngle();
	/// \brief 设置Pie结束角度
	void EndAngle(double angle);
	/// \brief 获取Pie结束角度
	double EndAngle(); 
};
/// \brief GsPiePointSymbolPtr
GS_SMARTER_PTR(GsPiePointSymbol);
DECLARE_CLASS_CREATE(GsPiePointSymbol)


/// \brief 线的点符号。
class GS_API GsLinePointSymbol:public GsPointSymbol
{
	GsPenPtr m_ptrPen;
	double m_dblWidth;
	GsRawPoint m_StartPoint, m_EndPoint;
	float m_Line[4];
	GsLineCapStyle     m_CapStyle;     //符号端套类型
	GsLineJoinStyle    m_JoinStyle;     //线段接头类型
	double               m_dblMiterLimit;	  //线端斜面接合限值属性
protected:	
	/// \brief 当绘制点的时候发生
	/// \details 子类通过覆盖此函数实现点的绘制。基类已经实现了地理坐标到屏幕坐标的转换
	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();

public:
	/// \brief 默认构造函数
	GsLinePointSymbol();
	/// \brief 从颜色和线宽构造
	GsLinePointSymbol(const GsColor& color,double dblWidth);
	virtual ~GsLinePointSymbol();

	/// \brief 设置LinePoint的起点坐标
	void StartPoint(double x, double y);
	/// \brief 获取LinePoint的起点
	GsRawPoint StartPoint();

	/// \brief 设置LinePoint的终点坐标
	void EndPoint(double x, double y);
	/// \brief 获取LinePoint的终点
	GsRawPoint EndPoint();

	/// \brief 设置线宽度
	void Width(double dblWidth);
	/// \brief 获取线宽度
	double Width();

	/// \brief 符号大小
	virtual double Size();
	/// \brief 设置符号大小
	virtual void Size(double dblSize);

	/// \brief 点符号的矩形范围
	virtual GsBox Envelope();
	/// \brief 获取端套类型
	GsLineCapStyle	CapStyle();
	/// \brief 设置端套类型
	void CapStyle(GsLineCapStyle Style);
	/// \brief 获取接头类型
	GsLineJoinStyle JoinStyle();
	/// \brief 设置接头类型
	void JoinStyle(GsLineJoinStyle Style);

	/// \brief 获取制图线的线端斜面接合限值
	double MiterLimit();
	/// \brief 设置制图线的线端斜面接合限值
	void MiterLimit(double Limit);

};
/// \brief GsLinePointSymbolPtr
GS_SMARTER_PTR(GsLinePointSymbol);
DECLARE_CLASS_CREATE(GsLinePointSymbol)


/// \brief 支持多个点构成曲线、曲线面的点符号模板基类
template<class T>
class GS_API GsCurvePointSymbolT:public T
{
protected:
	UTILITY_NAME::GsVector<GsRawPoint> m_vecPoints;
public:
	
	/// \brief 获取节点类型数组
	void CurvePoints(UTILITY_NAME::GsVector<GsRawPoint> &vecPoints)
	{
		if(vecPoints.empty())
			vecPoints = m_vecPoints;
		else
			m_vecPoints = vecPoints;

	}
	/// \brief 获取的节点类型数组
	UTILITY_NAME::GsVector<GsRawPoint>  CurvePoints()
	{
		return m_vecPoints;
	}
	
	/// \brief 设置点
	void CurvePoints(GsRawPoint* pPoint,int nCount)
	{
		m_vecPoints.clear();
		if(!pPoint  || nCount <=0)
			return;
		m_vecPoints.insert(m_vecPoints.begin(),pPoint,pPoint+ nCount);
		GsPointSymbol::RecalculateSize();
	}

	/// \brief 设置符号大小
	virtual void Size(double dblSize)
	{
		if(dblSize <=0 || Utility::GsMath::IsEqual(dblSize,this->m_dblSize))
			return;
		double dblScale = dblSize / this->m_dblSize;

		for(int i = 0;i<m_vecPoints.size();i++)
		{ 
			//计算resize后polygon上的点
			m_vecPoints[i].X = m_vecPoints[i].X * dblScale;
			m_vecPoints[i].Y = m_vecPoints[i].Y * dblScale; 
		}
		this->m_Offset.X *= dblScale;
		this->m_Offset.Y *= dblScale;
		this->m_dblSize = dblSize;
	}
	/// \brief 获取符号大小
	virtual double Size()
	{
		return this->m_dblSize;
	}

	/// \brief 符号的矩形范围
	virtual GsBox Envelope()
	{
		GsBox env;
		if (m_vecPoints.empty())
			return env; 


		GsMatrix mat;
		mat.Rotate(-this->m_dblAngle * Utility::GsMath::Pi() / 180);
		float f[2];

		Utility::GsVector<GsRawPoint>::iterator it = m_vecPoints.begin();
		for(;it != m_vecPoints.end();it++)
		{
			f[0] = it->X;
			f[1] = it->Y;
			//角度大于0才做矩阵乘法，否则效率太低了
			if(fabs(this->m_dblAngle) > DBL_EPSILON)
				mat.TransformPointsT(f,1);

			env.Union(GsBox(f[0],f[1],f[0],f[1]));
		}
		env.Offset(this->m_Offset);
		return env;
	}
};
/// \brief 曲线点符号
class GS_API GsCurvelinePointSymbol : public GsCurvePointSymbolT<GsPointSymbol>
{
	GsPenPtr             m_ptrPen;
	GsGraphicsPathPtr    m_ptrGraphicsPath;
	double               m_dblWidth;

protected:
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();
	virtual void OnPointDraw();
public:
	GsCurvelinePointSymbol();
	GsCurvelinePointSymbol(UTILITY_NAME::GsVector<GsRawPoint>& vec);
	GsCurvelinePointSymbol(UTILITY_NAME::GsVector<GsRawPoint>& vec,const GsColor& c);
	virtual ~GsCurvelinePointSymbol();
	 
	/// \brief 获取线宽度
	double Width();
	/// \brief 设置线宽度
	void Width(double dblWidth);
};
GS_SMARTER_PTR(GsCurvelinePointSymbol);
DECLARE_CLASS_CREATE(GsCurvelinePointSymbol);

/// \brief 面点符号
class GS_API GsPolygonPointSymbol:public GsCurvePointSymbolT<GsSurfacePointSymbol>
{
	GsGraphicsPathPtr m_ptrPath; 
	GsSolidBrushPtr m_ptrBrush;
	GsPenPtr m_ptrPen;

protected:
	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();
public:
	GsPolygonPointSymbol();

	/// \brief 从颜色和size构造
	GsPolygonPointSymbol(UTILITY_NAME::GsVector<GsRawPoint>& vec,const GsColor& c);
	GsPolygonPointSymbol(GsRawPoint* pts,int nCount,const GsColor& c);

	virtual ~GsPolygonPointSymbol();
};
GS_SMARTER_PTR(GsPolygonPointSymbol);
DECLARE_CLASS_CREATE(GsPolygonPointSymbol);

/// \brief 圆弧点符号
class GS_API GsArcPointSymbol:public GsEllipsePointSymbolT<GsPointSymbol>
{
	double		m_dblStartAngle;
	double		m_dblEndAngle;
	float		m_fLineWidth;
	GsPenPtr	m_ptrPen;
protected:	
	/// \brief 当绘制点的时候发生
	/// \details 子类通过覆盖此函数实现点的绘制。基类已经实现了地理坐标到屏幕坐标的转换
	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();

public:
	GsArcPointSymbol();
	GsArcPointSymbol(double dblStartAngle, double dblEndAngle);
	~GsArcPointSymbol();
	/// \brief 截取圆中arc的起始角度
	double StartAngle();

	/// \brief 截取圆中arc的终止角度
	double EndAngle();

	/// \brief 设置截取圆中arc的起始角度
	void StartAngle(double dblStartAngle);
	
	/// \brief 设置截取圆中arc的终止角度
	void EndAngle(double dblEndAngle);

	/// \brief 符号的线宽
	float LineWidth();

	/// \brief 设置符号的线宽
	void LineWidth(float linewidth);

};
/// \brief GsArcPointSymbolPtr
GS_SMARTER_PTR(GsArcPointSymbol);
DECLARE_CLASS_CREATE(GsArcPointSymbol);

/// \brief 星形的类型
enum GsAsteriskType
{
	/// \brief 普通型
	eAsteristkOrdination,
	/// \brief 交叉型
	eAsteristkCross,
	/// \brief 内接型
	eAsteristkInternal,
	/// \brief 辐射型
	eAsteristkRadiation,
}; 
/// \brief 星形点符号
class GS_API GsAsteriskPointSymbol:public GsEllipsePointSymbolT<GsSurfacePointSymbol>
{
	/// \brief 边线
	GsPenPtr m_ptrPen;
	/// \brief 填充
	GsSolidBrushPtr	m_ptrBrush; 

	/// \brief 类型
	GsAsteriskType      m_Asterisk;
	/// \brief 角数量
	int                  m_nCorners;

	/// \brief 比例
	double               m_rate;
	/// \brief 开始外交
	double               m_InterStartAngle; 
	/// \brief 开始内角
	double               m_InnerStartAngle; 
	 
	/// \brief 星形路径
	GsGraphicsPathPtr	m_ptrPath;
protected:
	/// \brief 绘制点
	virtual void OnPointDraw();

	/// \brief 符号开始绘制
	virtual void OnStartDrawing();
	/// \brief 符号结束绘制
	virtual void OnEndDrawing();

public:
	GsAsteriskPointSymbol();
	~GsAsteriskPointSymbol(); 

	/// \brief 获取比例
	double Rate ();
    /// \brief 设置比例
	void Rate (double pRation );
	/// \brief 获取星型的类型
	GsAsteriskType AsterType ();
    /// \brief 设置星型的类型
	void AsterType (GsAsteriskType eType);
    /// \brief 获取星型角的数量
	int Corners ();
	/// \brief 设置星型角的数量
	void Corners (int nVal);
    
	/// \brief 获取外角的开始角度
	double InterStartAngle ();
	/// \brief 设置外角的开始角度
	void InterStartAngle (double dAngle);
	
	/// \brief 获取内角的开始角度
	double InnerStartAngle ();
	/// \brief 设置内角的开始角度
	void InnerStartAngle (double dAngle);  
};
/// \brief GsAsteriskPointSymbolPtr
GS_SMARTER_PTR(GsAsteriskPointSymbol);
DECLARE_CLASS_CREATE(GsAsteriskPointSymbol)
/// \brief 弦点符号
class GS_API GsChordPointSymbol:public GsEllipsePointSymbolT<GsSurfacePointSymbol>
{
	double		m_dblStartAngle;
	double		m_dblEndAngle; 
	GsPenPtr	m_ptrPen;
	GsSolidBrushPtr m_ptrBrush;
	GsGraphicsPathPtr m_ptrPath;

protected:	
	/// \brief 当绘制点的时候发生
	/// \details 子类通过覆盖此函数实现点的绘制。基类已经实现了地理坐标到屏幕坐标的转换
	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();

public:
	GsChordPointSymbol();
	GsChordPointSymbol(double dblStartAngle, double dblEndAngle);
	~GsChordPointSymbol();
	 
   /// \brief 截取圆中arc的起始角度
	double StartAngle();

    /// \brief 设置开始角度
	void StartAngle (double pAngle );
    /// \brief 获取结束角度
	double EndAngle ();
    /// \brief 设置结束角度
	void EndAngle (double pAngle );

	/// \brief 设置符号大小,跟据设置的大小，调整长短轴
	virtual void Size(double dblSize);
};
/// \brief GsChordPointSymbolPtr
GS_SMARTER_PTR(GsChordPointSymbol);
DECLARE_CLASS_CREATE(GsChordPointSymbol);

/// \brief 由贝塞尔曲线构成的面点符号
class GS_API GsCurvegonPointSymbol:public GsCurvePointSymbolT<GsSurfacePointSymbol>
{ 
	GsPenPtr             m_ptrPen;
	GsGraphicsPathPtr    m_ptrGraphicsPath;
	GsSolidBrushPtr      m_ptrBrush;
public:
	GsCurvegonPointSymbol();
	GsCurvegonPointSymbol(UTILITY_NAME::GsVector<GsRawPoint>& vec);
	GsCurvegonPointSymbol(UTILITY_NAME::GsVector<GsRawPoint>& vec,const GsColor& c);
	~GsCurvegonPointSymbol();
protected:
	/// \brief 当绘制点的时候发生
	/// \details 子类通过覆盖此函数实现点的绘制。基类已经实现了地理坐标到屏幕坐标的转换
	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();
};
/// \brief GsCurvegonPointSymbolPtr
GS_SMARTER_PTR(GsCurvegonPointSymbol);
DECLARE_CLASS_CREATE(GsCurvegonPointSymbol)

/// \brief 锁定类型
enum GsPictureKeepOrigin
{
    ePictureNoKeep,
	ePictureKeepWidth,
	ePictureKeepHeight,
	ePictureKeepAll= 255,
};
/// \brief 图片点符号
class GS_API GsPicturePointSymbol:public GsPointSymbol
{
	/// \brief 是否使用透明色
	bool m_bTrans;
	/// \brief 要透明的颜色
	GsColor m_TransColor;
	/// \brief 是否使用背景色
	bool m_bUseBackColor;
	/// \brief 背景色
	GsColor m_BackColor;
	/// \brief 宽度
	double m_dblWidth;
	/// \brief 高度
	double m_dblHeight;
	/// \brief 矩形框
	Utility::GsRectF m_Rect;
	/// \brief 锁定类型
	GsPictureKeepOrigin m_OriginalState;
	/// \brief 图片对象
	Utility::GsImagePtr m_ptrImage,m_ptrImageDrawing;
	/// \brief 图片的真实数据
	Utility::GsGrowByteBuffer m_ImageData;
	/// \brief 画刷
	GsSolidBrushPtr m_ptrBrush;

	/// \brief x方向上的间隔
	double m_IntervalX;
	/// \brief y方向上的间隔
	double m_IntervalY;
protected:

	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();

	/// \brief 点符号的矩形范围
	virtual GsBox Envelope();

	/// \brief 处理透明色
	void ProcessTransparentColor();
public:
	/// \brief 无参构造函数
	GsPicturePointSymbol();
	/// \brief 路径导入图片构造函数
	GsPicturePointSymbol(const char* strFileName);
	
	GsPicturePointSymbol(const unsigned char* pBlob, int nLen);

	/// \brief 获取是否使用透明色
	bool Transparent ();
    /// \brief 设置是否使用透明色
	void Transparent (bool bTrans);
    /// \brief 获取要透明的颜色
	GsColor TransparentColor ();
    /// \brief 设置要透明的颜色
	void TransparentColor (const GsColor& color);
	
	/// \brief 获取是否使用背景色
	bool UseBackColor ();
	/// \brief 设置是否使用背景色
	void UseBackColor (bool bUse);
	/// \brief 获取背景色
	GsColor BackColor ();
	/// \brief 设置背景色
	void BackColor (const GsColor& color);
	
	/// \brief 获取图片对象
	Utility::GsImage* Picture ();
	/// \brief 获取图片的数据
	Utility::GsGrowByteBuffer* PictureData ();
	/// \brief 设置图片对象
	void Picture(Utility::GsImage* pImage);
	/// \brief 获取图片宽度(单位毫米)
	double Width ();
	/// \brief 设置图片宽度(单位毫米)
	void Width (double dblWidth);
	
	/// \brief 获取图片高度(单位毫米)
	double Height();
	/// \brief 设置图片高度(单位毫米)
	void Height(double dblHeight);

	/// \brief 获取图片锁定状态
	GsPictureKeepOrigin OriginalState ();
	/// \brief 设置图片锁定状态
	void OriginalState (GsPictureKeepOrigin  eState);
	
	/// \brief 从文件载入图片
	bool LoadPicture (const char* strFileName);
    /// \brief 从内存块载入图片。
	bool LoadPicture(const unsigned char* pBlob,int nLen);

	/// \brief x方向上间隔
	virtual void XSeparation(double dVal);
	/// \brief x方向上间隔
	virtual double XSeparation();
	/// \brief y方向上间隔
	virtual void YSeparation(double dVal);
	/// \brief y方向上间隔
	virtual double YSeparation();
};
/// \brief GsPicturePointSymbolPtr
GS_SMARTER_PTR(GsPicturePointSymbol);
DECLARE_CLASS_CREATE(GsPicturePointSymbol);

/// \brief 折线点符号
class GS_API GsPolylinePointSymbol:public GsCurvePointSymbolT<GsPointSymbol>
{
	GsGraphicsPathPtr m_ptrPath; 
	GsPenPtr m_ptrPen;
	double m_dblWidth;

protected:
	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();
public:
	GsPolylinePointSymbol();

	/// \brief 从颜色和size构造
	GsPolylinePointSymbol(UTILITY_NAME::GsVector<GsRawPoint>& vec,const GsColor& c);
	GsPolylinePointSymbol(GsRawPoint* pts,int nCount,const GsColor& c);

	virtual ~GsPolylinePointSymbol();
	/// \brief 获取折线点的宽度
	double Width ();

	/// \brief 设置折线点的宽度
	void Width (double dblWidth);
};
/// \brief GsPolylinePointSymbolPtr
GS_SMARTER_PTR(GsPolylinePointSymbol);
DECLARE_CLASS_CREATE(GsPolylinePointSymbol);


/// \brief 矩形点符号
class GS_API GsRectanglePointSymbol:public GsEllipsePointSymbolT<GsSurfacePointSymbol>
{
private: 
	GsSolidBrushPtr m_ptrBrush;
	GsPenPtr m_ptrPen;
	Utility::GsRectF m_SymRect;
public:
	GsRectanglePointSymbol();
	GsRectanglePointSymbol(double width, double height, double angle = 0.0); 
protected:	
	/// \brief 当绘制点的时候发生
	/// \details 子类通过覆盖此函数实现点的绘制。基类已经实现了地理坐标到屏幕坐标的转换
	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();


};
/// \brief GsRectanglePointSymbolPtr
GS_SMARTER_PTR(GsRectanglePointSymbol);
DECLARE_CLASS_CREATE(GsRectanglePointSymbol);

/// \brief 文本点符号
class GS_API GsTextPointSymbol:public GsEllipsePointSymbolT<GsSurfacePointSymbol>
{
private: 
	GsItalic  m_pItalic;          //倾斜类型
	bool m_bBold;                 //加粗标识
	bool m_Stroke;                //空心标识
	Utility::GsString  m_strFont; //字体
	Utility::GsString  m_strText; //字符内容
	Utility::GsPTF m_Loc;                  //绘制范围
	GsSolidBrushPtr m_ptrBrush;   //画刷
	GsPenPtr m_ptrPen;
	GsPenPtr m_ptrHaloPen;			//光晕画笔
	GsStringFormatPtr m_ptrStrf;   //字符的stringformat
	GsGraphicsPathPtr m_ptrPath;  //绘制轮廓用字体path
	GsMatrix			m_Matrix;
public:
	GsTextPointSymbol();
	GsTextPointSymbol(double width, double height, double angle = 0.0); 

    void TextFlag (GsItalic* pItalic,bool *bBold,bool* bStroke);
	void TextFlag (GsItalic pItalic,bool bBold,bool bStroke);

	/// \brief 获取字体
	Utility::GsString  Font ();
	/// \brief 设置字体
	void Font (const char* strFont);
	
	/// \brief 获取字符内容
	Utility::GsString Text ();
	/// \brief 设置字符内容
	void Text(const char* strText);
	 
	/// \brief 设置加粗标识
	void Bold(bool bBold);
	/// \brief 获取加粗标识
	bool Bold();

	/// \brief 设置空心标识
	void Stroke(bool mStroke);
	/// \brief 获取空心标识
	bool Stroke();
	 
	/// \brief 设置倾斜类型
	void Italic(GsItalic pItalic);
	/// \brief 获取倾斜类型
	GsItalic Italic();
protected:	
	/// \brief 当绘制点的时候发生
	/// \details 子类通过覆盖此函数实现点的绘制。基类已经实现了地理坐标到屏幕坐标的转换
	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();

};
/// \brief GsTextPointSymbolPtr
GS_SMARTER_PTR(GsTextPointSymbol); 
DECLARE_CLASS_CREATE(GsTextPointSymbol) 



/// \brief 二维码点符号
class GS_API GsQRCodePointSymbol:public GsPointSymbol
{
	Utility::GsGrowByteBuffer m_Buffer;
	Utility::GsGrowByteBuffer m_CenterImage;
	Utility::GsImagePtr m_ptrImage;
	Utility::GsQRCode m_QRCode;
	GsImageCanvasPtr m_ptrQRImage;
	int m_nPageIndex;

	Utility::GsQRCodeDataType m_eCodeDataType;
	double m_dblBorderSize;
	double m_dblRatio;
	GsColor m_BKColor;
protected:	
	/// \brief 当绘制点的时候发生
	/// \details 子类通过覆盖此函数实现点的绘制。基类已经实现了地理坐标到屏幕坐标的转换
	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();
public:
	GsQRCodePointSymbol();
	GsQRCodePointSymbol(unsigned char* pData,int nLen);
	
	/// \brief 获取二维码边框留白的大小
	double BorderSize ();
	/// \brief 设置二维码边框留白的大小
	void BorderSize (double size);

	/// \brief 获取二维码编码对象
	Utility::GsQRCode* QRCodePtr();
	 
	/// \brief 多个二维码编码对象时编码某个页面
	int PageIndex();
	/// \brief 多个二维码编码对象时编码某个页面
	void PageIndex(int nIndex);

	/// \brief 置于二维码中心的标示图片
	Utility::GsImagePtr LogoPicture();
	void LogoPicture(Utility::GsImage* pImg);
	
	/// \brief 二维码中心图片占用的比率
	/// \details 介于0.1~0.3之间，过大会遮挡过多二维码导致无法识别，过小中心图片看不清
	double LogoPictureRatio();

	/// \brief 二维码中心图片占用的比率
	void LogoPictureRatio(double dblRatio);
	
	/// \brief 背景颜色
	GsColor BackColor();
	/// \brief 背景颜色
	void BackColor(const GsColor& color);


	/// \brief 置于二维码中心的图片数据
	Utility::GsGrowByteBuffer* LogoPictureData();
	
	/// \brief 二维码显示的原始数据
	Utility::GsByteBuffer* QRData();
	/// \brief 设置任意二进制的数据
	void QRData(const unsigned char* pData,int nLen);
	
	/// \brief 设置字符串的数据
	void QRData(const char* str);
	
	/// \brief 获取数据的类型
	Utility::GsQRCodeDataType  DataType();


};
/// \brief GsQRCodePointSymbol
GS_SMARTER_PTR(GsQRCodePointSymbol); 
DECLARE_CLASS_CREATE(GsQRCodePointSymbol) 



/// \brief 复合点符号
class GS_API GsMultiPointSymbol:public GsMultiSymbolT<GsPointSymbol>
{
	bool m_IsDirectionPoint;
	
public: 
	GsMultiPointSymbol();
	/// \brief 符号大小
	virtual double Size();
	/// \brief 设置符号大小
	virtual void Size(double dblSize);

	/// \brief 颜色
	virtual GsColor Color();
	/// \brief 设置颜色
	virtual void Color(const GsColor& c);

	/// \brief 获取旋转角
	virtual double Angle();
	/// \brief 设置旋转角
	virtual	void Angle(double angle);

	/// \brief x偏移
	void OffsetX(double val);
	/// \brief y偏移
	void OffsetY(double val);
	/// \brief 偏移
	void Offset(const GsRawPoint& val);

	/// \brief 点符号的矩形范围
	virtual GsBox Envelope();

	//IPointSymbolProp
	void IsDirectionPoint(bool isDirP);
	bool IsDirectionPoint();

	/// \brief Multi符号外框的大小
	double MultiSize();

};
/// \brief GsMultiPointSymbolPtr
GS_SMARTER_PTR(GsMultiPointSymbol);
DECLARE_CLASS_CREATE(GsMultiPointSymbol)


/// \brief 脚本化的点符号。
class GS_API GsScriptPointSymbol :public GsPointSymbol
{
	Utility::GsString m_strStartDrawingScript;
	Utility::GsString m_strEndDrawingScript;
	Utility::GsString m_strDrawScript;
	GsRefObject*	m_pScriptEngine;
	GsPointPtr		m_ptrPoint;
protected:
	/// \brief 当绘制点的时候发生
	/// \details 子类通过覆盖此函数实现点的绘制。基类已经实现了地理坐标到屏幕坐标的转换
	virtual void OnPointDraw();
	virtual void OnStartDrawing();
	virtual void OnEndDrawing();

public:
	/// \brief 缺省构造
	GsScriptPointSymbol();
	virtual ~GsScriptPointSymbol();
	
	/// \brief 获取开始绘制的脚本
	Utility::GsString StartDrawingScript();
	/// \brief 设置开始绘制的脚本
	void StartDrawingScript(const char* script);

	/// \brief 获取结束绘制的脚本
	Utility::GsString EndDrawingScript();
	/// \brief 设置结束绘制的脚本
	void EndDrawingScript(const char* script);

	/// \brief 获取绘制的脚本
	Utility::GsString DrawScript();
	/// \brief 设置绘制的脚本
	void DrawScript(const char* script);

};

/// \brief GsScriptPointSymbolPtr
GS_SMARTER_PTR(GsScriptPointSymbol);
DECLARE_CLASS_CREATE(GsScriptPointSymbol)

KERNEL_ENDNS