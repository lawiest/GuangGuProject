#pragma once
#include "../utility/preconfig.h"
#include <object.h> 
#include <byteendian.h> 
#include <tinyxml2.h> 
#include <mathhelp.h> 
#include <json.h> 
#include <any.h> 
#include "spatialreference.h" 
#include "../geomathse/gobject.inl"
#include "../geomathse/GeomathSE.inl"
#include "locker.h"

KERNEL_NS    

class GsGeometryCollection;

/// \brief 轻量级点对象
struct GS_API GsRawPoint
{
	///\brief x坐标
	double X;
	///\brief y坐标
	double Y;

	/// \brief 缺省构造函数
	GsRawPoint();
	/// \brief 从x，y坐标构造
	/// \param x x坐标
	/// \param y y坐标
	GsRawPoint(double x,double y);
	
	/// \brief 拷贝构造函数
	/// \param rhs 要拷贝的对象
	GsRawPoint(const GsRawPoint& rhs);

	/// \brief 计算当前点和传入点之间的中点
	GsRawPoint Middle(const GsRawPoint& rhs) const;

	/// \brief 设置x、y坐标
	/// \param x x坐标
	/// \param y y坐标
	void Set(double x,double y);
	 
	/// \brief 判断点是否相等的操作符
	bool operator == (const GsRawPoint& rhs)const; 

	/// \brief 判断点是否不等的操作符
	bool operator != (const GsRawPoint& rhs)const;

	/// \brief 赋值操作符
	GsRawPoint& operator = (const GsRawPoint& rhs);
	/// \brief +=操作符
	GsRawPoint& operator += (const GsRawPoint& rhs);
	/// \brief -=操作符
	GsRawPoint& operator -= (const GsRawPoint& rhs);

	/// \brief +操作符
	GsRawPoint operator + (const GsRawPoint& rhs) const;
	/// \brief -操作符
	GsRawPoint operator - (const GsRawPoint& rhs) const;


	/// \brief 计算和另外一个点的距离的平方
	double Distance2(const GsRawPoint& rhs)const;

	/// \brief 将点坐标当作度转换为弧度点
	GsRawPoint ToRadian()  const;
	/// \brief 将点坐标当作弧度转换为度
	GsRawPoint ToDegree()  const;

	/// \brief 相对于输入的点向量角度
	double VectorAngle(const GsRawPoint& o);


};


/// \brief 设定边界的情况
enum GsSpatialRelation
{
	/// \brief 没有内部与边界的限制
	eSpatialRelationExBoundary = 0,
	/// \brief 几何对象的边界相交
	eSpatialRelationExClementini = 1,
	/// \brief 几何对象的边界可以不相交
	eSpatialRelationExProper = 2
};

/// \brief 轻量级三维点对象
struct GS_API GsRawPoint3D:public GsRawPoint
{
	///\brief z坐标
	double Z;
	GsRawPoint3D();
	
	/// \brief 从geomathd的点构造
	GsRawPoint3D(double x,double y,double z = 0);
	GsRawPoint3D(const GsRawPoint3D& rhs);
	GsRawPoint3D(const GsRawPoint& rhs);

	/// \brief 计算当前点和传入点之间的中点
	GsRawPoint3D Middle(const GsRawPoint3D& rhs) const;

	void Set3D(double x,double y ,double z);
	GsRawPoint3D& operator =(const GsRawPoint3D& rhs);
	bool operator == (const GsRawPoint3D& rhs)const;
	bool operator != (const GsRawPoint3D& rhs)const;


	/// \brief 赋值操作符
	GsRawPoint3D& operator = (const GsRawPoint& rhs);
	/// \brief +=操作符
	GsRawPoint3D& operator += (const GsRawPoint& rhs);
	/// \brief -=操作符
	GsRawPoint3D& operator -= (const GsRawPoint& rhs);

	/// \brief +操作符
	GsRawPoint3D operator + (const GsRawPoint& rhs) const;
	/// \brief -操作符
	GsRawPoint3D operator - (const GsRawPoint& rhs) const;
	 
	/// \brief +=操作符
	GsRawPoint3D& operator += (const GsRawPoint3D& rhs);
	/// \brief -=操作符
	GsRawPoint3D& operator -= (const GsRawPoint3D& rhs);

	/// \brief +操作符
	GsRawPoint3D operator + (const GsRawPoint3D& rhs) const;
	/// \brief -操作符
	GsRawPoint3D operator - (const GsRawPoint3D& rhs) const;
	 
	
	/// \brief 计算和另外一个点的距离的平方
	double SteroDistance2(const GsRawPoint3D& rhs)const;

	/// \brief 将点坐标当作度转换为弧度点
	GsRawPoint3D ToRadian3D()  const;
	/// \brief 将点坐标当作弧度转换为度
	GsRawPoint3D ToDegree3D()  const;

};

/// \brief 轻量级矩形对象
struct GS_API GsBox
{
	double XMin, YMin, XMax, YMax;

	/// \brief 缺省构造
	GsBox();
	/// \brief 从矩形的最大点和最小点两个坐标点构造
	GsBox(const GsRawPoint& Min, const GsRawPoint& Max);

	/// \brief 从坐标范围构造
	GsBox(double xmin, double ymin, double xmax, double ymax);
	/// \brief 拷贝构造
	GsBox(const GsBox& rhs);


	/// \brief 是否和另外一个矩形相交
	bool IsIntersects(const GsBox& rhs)const;

	/// \brief 是否和另外一个矩形相离
	bool IsDisjoin(const GsBox& rhs)const;
	/// \brief 是否和另外一个点相离
	bool IsDisjoin(double x,double y)const;
	/// \brief 是否和另外一个点相离
	bool IsDisjoin(const GsRawPoint& pt)const;

	/// \brief 判断自身是否完全包含另外一个矩形
	bool IsContain(const GsBox& rhs)const;

	/// \brief 判断自身是否包含点
	bool IsContain(double x, double y)const;
	/// \brief 判断自身是否包含点
	bool IsContain(const GsRawPoint& pt)const;


	/// \brief 合并另外一个矩形的范围到自身
	/// \return 返回自身的引用
	GsBox& Union(const GsBox& rhs);

	/// \brief 计算和另外一个矩形的交集
	/// \return 返回自身的引用
	GsBox& Intersect(const GsBox& rhs);


	/// \brief 判断矩形是否为有效矩形
	/// \details 有效矩形最大值大于最小值
	/// \return 返回自身是否有效
	bool IsValid()const;

	/// \brief 矩形的宽度
	/// \return 返回宽度值
	double Width()const;

	/// \brief 矩形的高度
	/// \return 返回高度值
	double Height()const;

	/// \brief 矩形的中心坐标
	/// \return 返回中心坐标点
	GsRawPoint Center() const;

	/// \brief 偏移矩形
	/// \param off 偏移的xy量
	/// \return 返回自身的引用
	GsBox& Offset(const GsRawPoint& off);

	/// \brief 偏移矩形
	/// \param x x方向上偏移量
	/// \param y y方向上偏移量
	/// \return 返回自身的引用
	GsBox& Offset(double x, double y);


	/// \brief 设置矩形的范围
	/// \return 返回自身的引用
	GsBox& Set(double xmin, double ymin, double xmax, double ymax);

	/// \brief 以矩形中心按照特定的比率缩放
	GsBox Scale(double scale);
	/// \brief 以指定中心按照特定的比率缩放
	GsBox Scale(const GsRawPoint& at, double scale);

	GsBox Expand(double tx, double ty);
};

/// \brief 三角型
struct GS_API GsTriangulate
{
	GsRawPoint Point[3];
	GsTriangulate();
	GsTriangulate(const GsRawPoint& p1, const GsRawPoint& p2, const GsRawPoint& p3);
	GsTriangulate(const GsTriangulate& rhs);

	GsRawPoint& PointAt(int i);

};
/// \brief 三角型
struct GS_API GsTriangulate3D
{
	GsRawPoint3D Point[3];
	GsTriangulate3D();
	GsTriangulate3D(const GsRawPoint3D& p1, const GsRawPoint3D& p2, const GsRawPoint3D& p3);
	GsTriangulate3D(const GsTriangulate3D& rhs);

	GsRawPoint3D& PointAt(int i);
};

/// \brief 三角化类型
enum GsTriangulationType
{
	/// \brief 狄罗尼三角网
	eDelaunayTriangulation,
	/// \brief 面内的三角网
	ePolygonTriangulation,
};
/// \brief 三角型集合
class GS_API GsTriangulateCollection:public Utility::GsRefObject
{
protected:
	GsTriangulateCollection();
public:
	virtual ~GsTriangulateCollection();
	/// \brief 集合中三角形的数量
	virtual int Count() = 0;
	/// \brief 获取集合中的一个三角形
	virtual GsTriangulate Triangulate(int i) = 0;
	
	/// \brief 集合中三角形节坐标地址
	virtual double* NodePtr() = 0;
	/// \brief 三角形节点的坐标维度（2或3）
	virtual int CoordinateDimension() = 0;
	/// \brief 节点的数量
	virtual int NodeCount() = 0;

	/// \brief 获取指定三角形的节点索引
	virtual int* TriangulateIndex(int i) = 0;

	/// \brief 获取所有的三角形节点索引
	virtual int* TriangulateIndex() = 0;

	/// \brief 选择和矩形相交的三角形形成三角形子集
	virtual Utility::GsSmarterPtr<GsTriangulateCollection> Select(const GsBox& extent) = 0;
};

/// \brief GsTriangulateCollectionPtr
GS_SMARTER_PTR(GsTriangulateCollection);

	/// \brief 几何对象类型
enum GsGeometryType
{
	/// \brief 未知或无效几何类型
	eGeometryTypeUnknown,
	/// \brief 单点
	eGeometryTypePoint,
	/// \brief 单段线
	eGeometryTypePath,
	/// \brief 多边形
	eGeometryTypePolygon,
	/// \brief 集合
	eGeometryTypeCollection,
	/// \brief 复合点、多点
	eGeometryTypeMultiPoint,
	/// \brief 多段折线
	eGeometryTypePolyline,
	/// \brief 注记
	eGeometryTypeAnnotation,
	/// \brief 矩形
	eGeometryTypeEnvelope,
	/// \brief 两点线段
	eGeometryTypeLine,
	/// \brief 三点圆弧
	eGeometryTypeCircleArc,
	/// \brief 单圈
	eGeometryTypeRing 
};

/// \brief segment扩展类型
enum GsSegmentExtension
{
	/// \brief segment不扩展
    eNoExtension = 0,
	/// \brief segment被沿着起点的切点无限扩展
    eExtendTangentAtFrom = 1,
	/// \brief segment沿着起点扩展，圆弧扩展后为一个圆，椭圆弧扩展后为一个椭圆，线段扩展后为一条射线
    eExtendEmbeddedAtFrom = 2,
	/// \brief segment被沿着终点的切点无限扩展
    eExtendTangentAtTo = 4,
	/// \brief segment沿着起点扩展，圆弧扩展后为一个圆，椭圆弧扩展后为一个椭圆，线段扩展后为一条射线
    eExtendEmbeddedAtTo = 8,
	/// \brief eExtendTangentAtFrom And/Or eExtendTangentAtTo
    eExtendTangents = 5,
	/// \brief eExtendEmbeddedAtFrom And/Or eExtendEmbeddedAtTo
    eExtendEmbedded = 10,
	/// \brief eExtendTangentAtFrom Or eExtendEmbeddedAtFrom
    eExtendAtFrom = 3,
	/// \brief eExtendTangentAtTo Or eExtendEmbeddedAtTo
    eExtendAtTo = 12
};

struct GS_API GeometryBlobHead//blob的头结构
{
	int		oid;
	int		oid2;
	GsGeometryType	type;
	int		coordnum;
	int		dim;
	int		infonum;
} ; 

/// \brief 几何数据库封装对象
/// \details 以Oracle Spatial Geometry数据描述结构封装对象
class GS_API GsGeometryBlob:public Utility::GsGrowByteBuffer
{ 
public:
	/// \brief 缺省构造函数
	GsGeometryBlob();
	/// \brief 获取64位OID
	/// \return 返回64位的oid
	long long OID() const;
	/// \brief 第一个32位oid
	/// \return 返回32位的oid
	int OID1() const;
	/// \brief 第2个32位oid
	/// \return 返回32位的oid
	int OID2() const;
	/// \brief 设置第一个32位oid
	void OID1(int oid);
	/// \brief 设置第2个32位oid
	void OID2(int oid);

	/// \brief 设置64位oid
	void OID(long long oid);

	/// \brief 几何数据的头
	/// \return 几何数据的头指针
	GeometryBlobHead *HeadPtr();

	/// \brief 几何类型
	/// \return 返回几何类型
	virtual GsGeometryType GeometryType()const;
	
	/// \brief 设置几何类型
	virtual void GeometryType(GsGeometryType eType);
	
	/// \brief 获取坐标维度
	/// \return 返回坐标维度2、3
	virtual int CoordinateDimension()const;
	/// \brief 设置坐标维度
	/// \param  nDim 坐标维度2、3
	virtual void CoordinateDimension(int nDim);

	/// \brief 获取解释串指针
	/// \return 返回解释串的指针
	virtual int* Interpret()const;
	
	/// \brief 获取解释串长度
	/// \return 返回解释串的长度
	virtual int InterpretLength() const;

	/// \brief 获取坐标串
	/// \return 返回坐标串的指针
	virtual double* Coordinate()const;
	/// \brief 获取坐标串长度
	/// \return 返回坐标串的长度
	virtual int CoordinateLength()const; 
	
	/// \brief 设置坐标串和解释串数据
	/// \param pInter 解释串指针
	/// \param nIntLen 解释串长度
	/// \param pCoord 坐标串指针
	/// \param nCoordLen 坐标串长度
	virtual void SetCoordinate(int* pInter,int nIntLen,double* pCoord,int nCoordLen);

	
	/// \brief 坐标转换
	/// \return 返回是否成功转换。
	virtual bool Transform(GsCoordinateTransformation* pTrans);
	
	/// \brief 判断是否有效
	bool IsValid();

	/// \brief 恢复为空状态
	void Empty();

	/// \brief 点的数量
	int PointCount() const;

	/// \brief 获取点的头指针。
	template<class T>
	T* PointHead() const
	{
		return (T*)Coordinate();
	}
	/// \brief 获取SE对象,此方法已经弃用，建议使用 ToGeoSEObject方法替代
	/// \return 返回生成SE对象
	geostar::gobjptr GeoSEObject();
	/// \brief 获取内存引用GeometryBlob的SE对象 
	/// \return 返回生成SE对象,此方法已经弃用，建议使用 ToReferenceSEObject方法替代
	geostar::gobjptr ReferenceSEObject();

	/// \brief 获取SE对象 
	/// \return 返回生成SE对象
	geostar::gobjptr ToGeoSEObject(bool bRef = false);
	/// \brief 获取内存引用GeometryBlob的SE对象 
	/// \return 返回生成SE对象
	geostar::gobjptr ToReferenceSEObject();


	/// \brief 设置SE对象到内存块，方法已经弃用建议使用FromGeoSEObject方法替代
	/// \param p SE对象
	/// \param bNormalize 是否正规化几何类型
	/// \return 返回是否成功
	GS_DEPRECATED(bool GeoSEObject(geostar::geo_object* p,bool bNormalize = false)) ;

	/// \brief 设置SE对象到内存块
	/// \param p SE对象
	/// \param bNormalize 是否正规化几何类型
	/// \return 返回是否成功
	bool FromGeoSEObject(geostar::geo_object* p, bool bNormalize = false);

	/// \brief 外接矩形范围
	/// \return 返回外接矩形对象
	virtual GsBox Envelope();

	/// \brief 清除内存数据
	virtual void Clear();
	/// \brief 释放内存
	virtual void Reset();

	/// \brief 对Geometry数据进行三角化
	GsTriangulateCollectionPtr BuildTriangulate(GsTriangulationType eType);
	/// \brief 对Geometry数据进行三角化
	GsTriangulateCollectionPtr BuildTriangulate(GsTriangulationType eType, const GsBox& extent);

	/// \brief 将几何数据高度压缩到内存块中
	/// \param nPrecision 几何坐标精确到小数点后面的位数。
	bool HighCompressTo(int nPrecision, GsByteBuffer* buff);

	/// \brief 从高度压缩的内存块中解压几何数据。
	bool FromHighCompressed(GsByteBuffer* buff);

};

	/// \brief 几何对象关系结果
enum GsGeometryRelationResult
{
	/// \brief 不符合对象关系
	eIsFalse,
	/// \brief 符合对象关系
	eIsTrue,

	/// \brief 对象间不适合这种关系
	eUnFit,
};

enum GsBufferCapType
{
	/// \brief 末端类型采用内插圆弧
	/// \image html eBufferRoundCap.png "eBufferRoundCap"
 	eBufferRoundCap,
	/// \brief 末端类型采用平头
	/// \image html eBufferFlatCap.png "eBufferFlatCap"
	eBufferFlatCap,
	/// \brief 末端类型采用方头
	/// \image html eBufferSquareCap.png "eBufferSquareCap"
	eBufferSquareCap
};

class GS_API GsPoint;

/// \brief 几何对象基类
// \details 几何对象抽象基类
class GS_API GsGeometry:public Utility::GsRefObject
{

protected:
	/// \brief 几何数据内存块
	mutable GsGeometryBlob m_OGS;
	GsSpatialReferencePtr m_ptrSp;
protected:
	/// \brief 设置坐标串和解释串
	/// \details OGS = Oracle Geometry Struct (Oracle几何对象结构）
	/// \param pBlob 几何数据内存块
	/// \param nLen 内存块长度
	void SetOGS(const unsigned char* pBlob,int nLen);
	
	/// \brief 从坐标串和解释串构造
	/// \param pBlob 几何数据内存块
	/// \param nLen 内存块长度
	GsGeometry(const unsigned char* pBlob,int nLen);
	//缺省构造
	GsGeometry(); 
	///\brief 切割点
	void CutP(geostar::gobjptr& ptrThis, geostar::gobjptr& ptrCut, double dblTol, GsGeometryCollection* ppLeftGeo, GsGeometryCollection* ppRightGeo);
	///\brief 区分左边右边
	void DifferentiatesLeftOrRight(geostar::gobjptr& gSrc, Utility::GsVector<geostar::gobjptr>& vecObjs,GsGeometryCollection* ppLeftGeo,GsGeometryCollection* ppRightGeo,double dblTol);
	
	/// \brief 获取几何对象的空间参考，内部使用
	/// \details 先获取几何自身的空间参考，没有则用系统缺省的空间参考
	GsSpatialReferencePtr GeometrySpatialReference() const;


	/// \brief 按照指定的间距按照测地线加密
	/// \details 几何对象如果没有空间参考则当作地理坐标系数据直接处理
	/// \param  dblInterval 差值间隔，单位米
	virtual void GeodesicInterpolate(double dblInterval,double* pCoord,int n,int nDim, Utility::GsVector<double>& vecRes);

public:
	virtual ~GsGeometry();

	/// \brief 返回正规化的Geometry
	/// \details 如果无需正规化则返回自身，参考GsGeometryFactory::Normalize
	Utility::GsSmarterPtr<GsGeometry> Normalize();

	/// \brief 获取对象的hash值
	///\return 缺省为对象自身的指针
	virtual unsigned long long HashCode();

	/// \brief 克隆自身
	/// \return 返回克隆的新对象
	virtual Utility::GsSmarterPtr<GsGeometry> Clone();

	/// \brief 按照指定的间距按照测地线加密
	/// \details 几何对象如果没有空间参考则当作地理坐标系数据直接处理
	/// \param  dblInterval 差值间隔，单位米
	virtual Utility::GsSmarterPtr<GsGeometry> GeodesicInterpolate(double dblInterval);

	/// \brief 计算测试线缓冲区。
	/// \param  dblRadius 缓冲距离，单位米
	Utility::GsSmarterPtr<GsGeometry> GeodesicBuffer(double dblRadius, GsBufferCapType eType = eBufferRoundCap, double dblTol = FLT_EPSILON);


	/// \brief 获取存储几何数据的指针
	/// \return 返回存储几何数据的指针
	virtual GsGeometryBlob* GeometryBlobPtr() ;

	/// \brief 外接矩形范围
	/// \return 返回外接矩形对象
	virtual GsBox Envelope();
	
	/// \brief 任意方向上最小外接矩形
	/// \return 返回Ring对象或者空。
	Utility::GsSmarterPtr<GsGeometry> MinimumBoundingBox();

	/// \brief Label点
	/// \return 返回Label点
	virtual GsRawPoint LabelPoint();
	
	/// \brief 几何类型
	/// \return 返回几何类型
	virtual GsGeometryType GeometryType()const;

	/// \brief 坐标维度
	/// \return 返回2，只有x，y坐标，返回3，有z坐标
	virtual int CoordinateDimension() const;
	
	/// \brief 设置坐标维度，2或者3
	virtual void CoordinateDimension(int nDim);

	/// \brief 几何对象维度
	/// \return 0表示点，1表示线，2表示面
	virtual int Dimension()const;

	/// \brief 坐标转换
	/// \return 返回是否成功转换。
	virtual bool Transform(GsCoordinateTransformation* pTrans);

	/// \brief 转换几何对象类型为几何维度
	/// \return 返回几何维度（0：点  1：线  2：面）
	static int GeometryTypeDimension(GsGeometryType eType);
		
	/// \brief 计算和另外一个Geometry的距离
	/// \image html Geometry_Distance.png "Geometry最短路径"
	virtual double Distance (GsGeometry* pGeo,double dblTol =  FLT_EPSILON);

    /// \brief 计算几何对象的外壳
	virtual Utility::GsSmarterPtr<GsGeometry> Boundary (double dblTol =  FLT_EPSILON);

	/// \brief 计算几何对象的凸壳
	virtual Utility::GsSmarterPtr<GsGeometry> ConvexHull (double dblTol =  FLT_EPSILON);

	/// \brief	计算几何对象的缓冲区
	/// \param	dblRadius	缓冲区半径
	/// \param	eType	 	缓冲区分析时的节点处理类型，默认使用真圆弧衔接
	/// \param	dblTol 容差
	/// \return	缓冲区多边形
	virtual Utility::GsSmarterPtr<GsGeometry> Buffer (double dblRadius, GsBufferCapType eType = eBufferRoundCap,double dblTol =  FLT_EPSILON);

	/// \brief	计算几何对象的缓冲区
	/// \param	dblRadius	缓冲区半径
	/// \param	nArc	 	缓冲区分析时的节点内插成半圆时内插折线线段的数目
	/// \param	dblTol 容差
	/// \return	缓冲区多边形
	virtual Utility::GsSmarterPtr<GsGeometry> Buffer (double dblRadius, int nArc,double dblTol =  FLT_EPSILON);

	/// \brief	计算几何对象的缓冲区
	/// \param	dblRadius	缓冲区半径
	/// \param	nArc	 	缓冲区分析时的节点内插成半圆时内插折线线段的数目
	/// \param eType		缓冲区端点类型
	/// \param	dblTol 容差
	/// \return	缓冲区多边形
	virtual Utility::GsSmarterPtr<GsGeometry> Buffer ( int nArc, double dblRadius,GsBufferCapType eType,double dblTol =  FLT_EPSILON);

	/// \brief 计算和另外一个几何对象的相交的部分
	virtual Utility::GsSmarterPtr<GsGeometry> Intersection (GsGeometry* pOther,double dblTol =  FLT_EPSILON);

	/// \brief 计算和另外一个几何对象的合并的结果
	virtual Utility::GsSmarterPtr<GsGeometry> Union (GsGeometry* pOther,double dblTol =  FLT_EPSILON);

	/// \brief 计算和另外一个几何对象不同的部分
	virtual Utility::GsSmarterPtr<GsGeometry> Difference (GsGeometry* pOther,double dblTol =  FLT_EPSILON);
	
	/// \brief 计算和另外一个几何对象对称差
	virtual Utility::GsSmarterPtr<GsGeometry> SymDifference (GsGeometry* pOther,double dblTol =  FLT_EPSILON);
	
	/// \brief 判断一个几何对象是否存在重点、自相交等复杂情况
	/// \image html Geometry_IsSimple.png "Geometry是否自相交"
	virtual bool IsSimple (double dblTol =  FLT_EPSILON);

	/// \brief 将复杂的存在重点、自相交的对象简化
	/// \image html Geometry_Simplify.png "简化Geometry"
	virtual Utility::GsSmarterPtr<GsGeometry> Simplify();

	/// \brief 根据传入的分割线，将当前几何对象分割为左右两个几何对象集合
	virtual void Cut(GsGeometry* pCutter, GsGeometryCollection* ppLeftGeo, GsGeometryCollection* ppRightGeo, double dblTol =  FLT_EPSILON);

	/// \brief 用传入的几何对当前几何对象进行剪切
	virtual  Utility::GsSmarterPtr<GsGeometry> Clip(GsGeometry* pCliperGeo, bool bIncludeBoundary, double dblTol =  FLT_EPSILON);

	/// \brief 创建并返回当前几何对象上距离输入点最近的一个点
	virtual Utility::GsSmarterPtr<GsPoint> ReturnNearestPoint(GsPoint * pPoint, GsSegmentExtension extension, double dblTol =  FLT_EPSILON);

	/// \brief 复制当前几何对象上距离输入点最近的一个点到pNearest
	virtual void QueryNearestPoint(GsPoint * pPoint, GsSegmentExtension extension, GsPoint * pNearest, double dblTol =  FLT_EPSILON);
	
	/// \brief 判断geometry是否有效
	virtual bool IsEmpty()  const;

	/// \brief 比较几何对象似乎否相等
	virtual GsGeometryRelationResult IsEqual (GsGeometry * pOther,double dblTol =  FLT_EPSILON);
	
	/// \brief 比较几何对象是否相贴近
	/// \image html Geometry_IsTouch.png "Geometry相贴近"
	virtual GsGeometryRelationResult IsTouch(GsGeometry * pOther,double dblTol =  FLT_EPSILON);

	/// \brief 几何对象是否落入在传入几何对象内部。
	/// \image html Geometry_IsWithin.png "Geometry在内部"
	virtual GsGeometryRelationResult IsWithin(GsGeometry * pOther,double dblTol =  FLT_EPSILON);

	/// \brief 几何对象是否包含传入对象
	/// \image html Geometry_IsContain.png "Geometry包含"
	virtual GsGeometryRelationResult IsContain(GsGeometry * pOther,double dblTol =  FLT_EPSILON);

	/// \brief 几何对象和传入对象是否在线的语义上向穿
	virtual GsGeometryRelationResult IsCross(GsGeometry * pOther,double dblTol =  FLT_EPSILON);

	/// \brief 几何对象和传入几何对象是否在线的语义上重叠
	/// \image html Geometry_IsOverlap.png "Geometry相重叠"
	virtual GsGeometryRelationResult IsOverlap(GsGeometry * pOther,double dblTol =  FLT_EPSILON);

	/// \brief 几何对象和传入几何对象是否向离
	/// \image html Geometry_IsDisjoin.png "Geometry相离"
	virtual GsGeometryRelationResult IsDisjoin(GsGeometry * pOther,double dblTol =  FLT_EPSILON);

	/// \brief 几何对象和传入几何对象是否向交
	virtual GsGeometryRelationResult IsIntersect(GsGeometry * pOther,double dblTol =  FLT_EPSILON);


	/// \brief 计算几何对象集合的合并的结果
	virtual Utility::GsSmarterPtr<GsGeometry> ConstructUnion(GsGeometryCollection* pGeometryCollection,double dblTol =  FLT_EPSILON);

	/// \brief	计算几何对象的缓冲区
	/// \param	dblRadius	缓冲区半径
	/// \param	len			缓冲区半径数组长度
	/// \param	eType	 	缓冲区分析时的节点处理类型，默认使用真圆弧衔接
	/// \param	dblTol 容差
	/// \return	缓冲区多边形
	virtual Utility::GsSmarterPtr<GsGeometryCollection> ConstructBuffers (double* dblRadius,int len, GsBufferCapType eType = eBufferRoundCap,double dblTol =  FLT_EPSILON);

	/// \brief	计算几何对象的缓冲区
	/// \param	dblRadius 缓冲区半径数组  
	/// \param	len 缓冲区半径数组长度
	/// \param	nArc 缓冲区分析时的节点内插成半圆时内插折线线段的数目
	/// \param	dblTol 容差
	/// \return	缓冲区多边形
	virtual Utility::GsSmarterPtr<GsGeometryCollection> ConstructBuffers (double* dblRadius,int len, int nArc,double dblTol =  FLT_EPSILON);

	/// \brief 判断两个Geometry是否符合指定的空间关系
	virtual GsGeometryRelationResult Relation (GsGeometry * pOther, char* strRelationDescription, double dblTol =  FLT_EPSILON);

	/// \brief 几何对象包含另一几何对象
	virtual GsGeometryRelationResult IsContain (GsGeometry * pOther, enum GsSpatialRelation SpatialRelationExEnum, double dblTol =  FLT_EPSILON);

	/// \brief 几何对象被另一几何对象包含
	virtual GsGeometryRelationResult IsWithin (GsGeometry * pOther, enum GsSpatialRelation SpatialRelationExEnum, double dblTol =  FLT_EPSILON);

	/// \brief 设置空间参考
	void SpatialReference(GsSpatialReference *pSp);
	/// \brief 获取空间参考
	GsSpatialReference* SpatialReference() const;
	/// \brief 获取Se对象 polygon 会重写此方法
	virtual geostar::gobjptr ToSE();

	/// \brief 对Geometry数据进行三角化
	GsTriangulateCollectionPtr BuildTriangulate(GsTriangulationType eType);

	/// \brief 对Geometry数据进行三角化
	GsTriangulateCollectionPtr BuildTriangulate(GsTriangulationType eType,const GsBox& extent);

	/// \brief	平滑
	/// \param	double dblValue 内插参数 默认为8
	/// \return	GsGeometry* 平滑处理后的几何地物
	virtual Utility::GsSmarterPtr<GsGeometry> Smooth(int nArc);

	/// \brief	平形
	/// \param	double dblOffset, 距离
	/// \param int nArc 默认为6
	/// \return	GsGeometry* 平滑处理后的几何地物
	virtual Utility::GsSmarterPtr<GsGeometry>  Parallel(double dblOffset,int nArc);
};
/// \brief GsGeometryPtr
GS_SMARTER_PTR(GsGeometry);

/// \brief 点和Geometry碰撞关系类型
enum GsHitType
{
	/// \brief 未能发生碰撞
	eHitNone,
	/// \brief 精确命中某个节点
	eHitNode,
	/// \brief 精确命两个节点的中点
	eHitMiddleNode,
};

/// \brief 点和Geometry碰撞的结果。
struct GS_API GsHitTestResult
{
	/// \brief 碰撞类型
	GsHitType Type;
	GsRawPoint3D Node;
	int			Index;
	GsRawPoint* NodePtr;

	GsHitTestResult();
	GsHitTestResult(GsHitType t);
	GsHitTestResult(GsHitType t, GsRawPoint* node,int idx);
	GsHitTestResult(GsRawPoint* node,  int idx);
	GsHitTestResult(GsHitType t, const GsRawPoint3D& node, int idx);
	GsHitTestResult(const GsRawPoint3D& node, int idx);
	GsHitTestResult(const GsHitTestResult& rhs);
};
/// \brief 判断点和Geometry的碰撞关系判断
class GS_API GsGeometryHitTest
{
	double m_Tol;
	GsGeometryPtr m_ptrGeometry;
	GsHitTestResult HitTestPoint(const GsRawPoint& pt, double tol2);
	
public:
	GsGeometryHitTest(GsGeometry* geo);
	GsGeometryHitTest();

	GsGeometry* Geometry();
	void Geometry(GsGeometry* geo);

	double Tolerance();
	void Tolerance(double tol);
	/// \brief 测试点和Geometry碰撞
	GsHitTestResult HitTest(const GsRawPoint& pt);

	/// \brief 更新Hittest的结果数据
	bool Update(const GsHitTestResult& rhs);

	/// \brief 删除Hittest的结果数据
	bool Delete(const GsHitTestResult& rhs);
};

/// \brief 几何简化结果
enum GsGeometrySimplifyResult
{
	/// \brief 简化成功
	eSimplifySucceed,
	/// \brief 简化失败或者无须简化
	eSimplifyFaild,
	/// \brief 简化结果已经退化
	eSimplefyDegenerated,
};
/// \brief 几何简化器
class GS_API GsGeometrySimplifier:public Utility::GsRefObject
{
protected:
	mutable double m_dblTol;
	mutable Utility::GsVector<int> m_vecInter;
	mutable Utility::GsVector<double> m_vecCoord;
	bool m_bEnableDegenerated;
	GsGeometrySimplifier();
public:

	/// \brief 是否允许退化
	bool EnableDegenerated() const;
	/// \brief 是否允许退化
	void EnableDegenerated(bool bEnable);

	/// \brief 容差
	double Tolerance() const;
	/// \brief 容差
	virtual void Tolerance(double dblTol);
	
	/// \brief 简化几何对象返回简化后的几何对象
    virtual GsGeometryPtr Simplify( GsGeometry* geometry ,bool bClone = true) const;
    /// \brief 简化几何对象
	virtual GsGeometrySimplifyResult SimplifyGeometry( GsGeometry* geometry ) const;
	/// \brief 简化几何对象
	virtual GsGeometrySimplifyResult Simplify( GsGeometryBlob* geo) const;
	/// \brief 简化几何对象
	virtual GsGeometrySimplifyResult Simplify(int* I,int& nI,double* C,int& nC,int dim) const;

	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsGeometrySimplifier> Clone() = 0;
};

/// \brief GsGeometrySimplifierPtr
GS_SMARTER_PTR(GsGeometrySimplifier);

/// \brief 保拓扑简化器
class GS_API GsTopologyPreservingGeometrySimplifier:public GsGeometrySimplifier
{
	geostar::gobjptr m_ptrTrans;
	geostar::geo_trans* m_pTrans;
	geostar::geo_factory* m_pFac;
	geostar::gobjptr m_ptrFac;

public: 
	GsTopologyPreservingGeometrySimplifier(double dblTol = 0);
	using GsGeometrySimplifier::Simplify;
	using GsGeometrySimplifier::Tolerance;

	/// \brief 简化几何对象
	virtual GsGeometrySimplifyResult Simplify(int* I,int& nI,double* C,int& nC,int dim) const;
	/// \brief 容差
	virtual void Tolerance(double dblTol);
	/// \brief 克隆
	virtual Utility::GsSmarterPtr<GsGeometrySimplifier> Clone();


};
/// \brief GsTopologyPreservingGeometrySimplifierPtr
GS_SMARTER_PTR(GsTopologyPreservingGeometrySimplifier);

/// \brief 矩形几何对象
class GS_API GsEnvelope:public GsGeometry
{
public:
	/// \brief 缺省构造函数
	GsEnvelope();
	/// \brief 从内存块构造
	/// \param pBlob 几何数据内存块
	/// \param nLen 内存块长度
	GsEnvelope(const unsigned char* pBlob,int nLen);
	/// \brief 从范围构造
	GsEnvelope(double xmin,double ymin,double xmax,double ymax);
	/// \brief 从范围构造3D
	GsEnvelope(double xmin,double ymin,double zmin,double xmax,double ymax,double zmax);
	/// \brief 从Box构造
	GsEnvelope(const GsBox& rhs); 
	
	/// \brief 返回矩形的轻量对象
	GsBox Box()const;
	/// \brief 是否和另外一个矩形相交
	bool IsIntersects(const GsBox& rhs)const ;
	
	/// \brief 是否和另外一个矩形相交
	bool IsIntersects(GsEnvelope* env)const ;
	
	/// \brief 是否和另外一个矩形相离
	bool IsDisjoin(const GsBox& rhs)const ;
	/// \brief 是否和另外一个矩形相离
	bool IsDisjoin(GsEnvelope* rhs)const ;

	/// \brief 合并另外一个矩形的范围到自身
	/// \return 返回自身的引用
	GsEnvelope* Union(const GsBox& rhs);

	/// \brief 合并另外一个矩形的范围到自身
	/// \return 返回自身的引用
	GsEnvelope* Union(GsEnvelope* rhs);

	/// \brief 与另一box求交，自身改变为求得的交集
	/// \return 返回自身的引用
	GsEnvelope* Intersect(const GsBox& rhs);

	/// \brief 与另一矩形求交，自身改变为求得的交集
	/// \return 返回自身的引用
	GsEnvelope* Intersect(GsEnvelope* rhs);

	/// \brief 判断矩形是否为有效矩形
	/// \details 有效矩形最大值大于最小值
	/// \return 返回自身是否有效
	bool IsValid()const ;

	/// \brief 矩形的宽度
	/// \return 返回宽度值
	double Width()const ;
	
	/// \brief 矩形的高度
	/// \return 返回高度值
	double Height()const ;

	/// \brief 矩形的深度
	/// \return 返回深度值
	double Depht()const; 

	/// \brief 矩形的中心坐标
	/// \return 返回中心坐标点
	GsRawPoint3D Center() const;
	
	/// \brief 偏移矩形
	/// \param off 偏移的xy量
	/// \return 返回自身的引用
	GsEnvelope* Offset(const GsRawPoint& off);
	
	/// \brief 偏移矩形3D
	/// \param off 偏移的xyz量
	/// \return 返回自身的引用
	GsEnvelope* Offset(const GsRawPoint3D& off);

	/// \brief 偏移矩形
	/// \param x x方向上偏移量
	/// \param y y方向上偏移量
	/// \return 返回自身的引用
	GsEnvelope* Offset(double x,double y);

	/// \brief 偏移矩形
	/// \param x x方向上偏移量
	/// \param y y方向上偏移量
	/// \param z z方向上偏移量
	/// \return 返回自身的引用
	GsEnvelope* Offset(double x,double y,double z);	
	
	/// \brief 设置矩形的范围
	/// \return 返回自身的引用
	GsEnvelope* Set(double xmin,double ymin,double xmax,double ymax);
	
	/// \brief 设置矩形的范围
	/// \return 返回自身的引用
	GsEnvelope* Set(double xmin,double ymin,double zmin,double xmax,double ymax,double zmax);

	/// \brief 设置矩形的范围
	/// \return 返回自身的引用
	GsEnvelope* Set(const GsRawPoint3D &minpoint,const GsRawPoint3D &maxpoint);

	/// \brief 设置矩形的范围
	/// \return 返回自身的引用
	GsEnvelope* Set(const GsRawPoint &minpoint,const GsRawPoint &maxpoint);
	

	/// \brief 获取矩形的范围
	/// \return 返回box对象
	GsBox Get()const;

	/// \brief 获取矩形的范围
	void Get(double& xmin,double& ymin,double& xmax,double& ymax)const;

	/// \brief 获取矩形的范围
	void Get(double& xmin,double& ymin,double& zmin,double& xmax,double& ymax,double& zmax)const;

	/// \brief 获取矩形的范围
	void Get(GsRawPoint &minpoint,GsRawPoint &maxpoint)const;

	/// \brief 获取矩形的范围
	void Get(GsRawPoint3D &minpoint,GsRawPoint3D &maxpoint)const;

	/// \brief 设置矩形的范围
	/// \return 返回自身的引用
	GsEnvelope* Set(const GsBox& box);
	
	
	/// \brief 最小X值
	double XMin()const;
	void XMin(double val);

	/// \brief 最大X值
	double XMax()const;
	void XMax(double val);

	/// \brief 最小Y值
	double YMin()const;
	void YMin(double val);

	/// \brief 最大Y值
	double YMax()const;
	void YMax(double val);

	/// \brief 最小Z值
	double ZMin()const;
	void ZMin(double val);

	/// \brief 最大Z值
	double ZMax()const;
	void ZMax(double val);

	
	/// \brief 获取最大的点
	GsRawPoint3D Max()const;

	/// \brief 设置最大的点
	void Max(const GsRawPoint &rhs);

	/// \brief 设置最大的点
	void Max(const GsRawPoint3D &rhs);

	/// \brief 获取最小的点
	GsRawPoint3D Min()const;
	/// \brief 设置最小的点
	void Min(const GsRawPoint &rhs);
	/// \brief 设置最小的点
	void Min(const GsRawPoint3D &rhs);

	/// \brief 按照指定的间距按照测地线加密
	/// \details 几何对象如果没有空间参考则当作地理坐标系数据直接处理
	/// \param  dblInterval 差值间隔，单位米
	virtual GsGeometryPtr GeodesicInterpolate(double dblInterval)override;
	/// \brief 按照指定间距外扩
	GsEnvelope* Expand(double tx, double ty);
};
/// \brief GsEnvelopePtr
GS_SMARTER_PTR(GsEnvelope);

/// \brief 单点几何对象
class GS_API GsPoint:public GsGeometry
{
	
public:
	/// \brief 缺省构造
	/// \details x，y坐标都为0
	GsPoint();

	/// \brief 从内存块构造
	/// \param pBlob 几何数据内存块
	/// \param nLen 内存块长度
	GsPoint(const unsigned char* pBlob,int nLen);
	/// \brief xy坐标构造
	/// \param x x坐标
	/// \param y y坐标
	GsPoint(double x,double y);
	
	/// \brief 从轻量点构造
	GsPoint(const GsRawPoint& pt);
	
	/// \brief 从轻量点构造
	GsPoint(const GsRawPoint3D& pt);


	/// \brief xy坐标构造
	/// \param x x坐标
	/// \param y y坐标
	/// \param z z坐标
	GsPoint(double x,double y,double z);

	/// \brief 设置xy坐标
	GsPoint* Set(double x,double y);

	/// \brief 设置xy,z坐标
	GsPoint* Set(double x,double y,double z);

	/// \brief 设置xy坐标
	GsPoint* Set(const GsRawPoint& pt);
	/// \brief 设置xy,z坐标
	GsPoint* Set(const GsRawPoint3D& pt);

	/// \brief 获取点
	GsRawPoint3D Get()const;

	/// \brief 获取点
	void Get(double &x,double &y)const;
	/// \brief 获取点
	void Get(double &x,double &y,double &z)const;


	/// \brief 得到this和o的距离的平方
	double Distance2(GsPoint* o)const;

	/// \brief 得到this和o的距离的平方
	double Distance2(const GsRawPoint& o)const;

	/// \brief 得到this和o的距离
	double Distance(GsPoint* o)const;

	/// \brief 得到this和o的距离
	double Distance(const GsRawPoint& o)const;

    /// \brief 返回this和o的一次距离
	double Abs(GsPoint *o)const;

	/// \brief 返回this和o的一次距离
	double Abs(const GsRawPoint &o)const;

	/// \brief 使得this到原点的距离为fabs(r)
	void Stand(double r);


    /// \brief 返回复数this和复数a的乘积
	GsPoint* Mul(const GsRawPoint &a);

	/// \brief 返回复数this和复数a的乘积
	GsPoint* Mul(GsPoint *a);

	/// \brief 获取x坐标
	double X() const;
	/// \brief 设置x坐标
	void X(double x);

	/// \brief 获取y坐标
	double Y() const;
	/// \brief 设置y坐标
	void Y(double y);

	/// \brief 获取z坐标
	double Z() const;
	/// \brief 设置z坐标
	void Z(double z);


};
/// \brief GsPointPtr
GS_SMARTER_PTR(GsPoint);

 
/// \brief 几何对象集合
class GS_API GsGeometryCollection:public GsGeometry
{
	 
public:
	GsGeometryCollection();
	virtual ~GsGeometryCollection();
	 

	/// \brief 几何对象的数量
	/// \return 返回几何对象的数量
	virtual unsigned int Count() const;
	
	/// \brief 根据索引获得几何对象
	/// \param nIndex 索引
	/// \return 返回几何对象的指针
	virtual GsGeometryPtr Geometry(int nIndex);

	/// \brief 增加一个几何对象
	/// \param pGeo 几何对象指针
	virtual void Add(GsGeometry* pGeo);
	 

	/// \brief 增加一个几何对象集合
	/// \param pCollection 几何对象集合指针
	virtual void Add(GsGeometryCollection* pCollection);

	/// \brief 将传入Geometry集合中的元素添加到这个集合
	virtual void AddCollection(GsGeometryCollection* pCollection);

	/// \brief 插入一个几何对象
	/// \param nIndex 插入位置
	/// \param pGeo   几何对象指针
	virtual void Insert(int nIndex, GsGeometry* pGeo);

	/// \brief 插入多个几何对象
	/// \param nIndex   插入位置
	/// \param nCount   几何对象个数
	/// \param pGeoList 几何对象队列指针
	virtual void Inserts(int nIndex, int nCount, GsGeometry** pGeoList);

	/// \brief 插入一个几何对象集合
	/// \param nIndex      插入位置
	/// \param pCollection 几何对象集合指针
	virtual void InsertCollection(int nIndex, GsGeometryCollection* pCollection);

	/// \brief 移除一个几何对象
	/// \param nIndex 几何对象索引
	virtual GsGeometryPtr RemoveAt(int nIndex);
	
	/// \brief 根据位置替换一个几何对象
	/// \param nIndex 几何对象索引
	/// \param pGeo   几何对象指针
	virtual void ReplaceAt(int nIndex, GsGeometry* pGeo);

	/// \brief 清除几何对象
	virtual void Clear();
	
	/// \brief 获取存储几何数据的指针
	/// \return 返回存储几何数据的指针
	virtual GsGeometryBlob* GeometryBlobPtr() override  ;


	/// \brief 按照指定的间距按照测地线加密
	/// \details 几何对象如果没有空间参考则当作地理坐标系数据直接处理
	/// \param  dblInterval 差值间隔，单位米
	virtual GsGeometryPtr GeodesicInterpolate(double dblInterval)override;

protected:
	/// \brief 几何数据对象化
	void GeometryObjectification()   ;
	/// \brief 几何数据序列化
	void GeometrySerialization()  ;

	/// \brief 开始对象化
	virtual void OnObjectification();
	/// \brief 开始序列化
	virtual void OnSerialization();

	/// \brief 过滤几何类型
	/// \details 由GsGeometry子类实现各自类型的过滤
	/// \param pGeo 被过滤的几何对象指针
	/// \return 返回过滤结果
	virtual bool FilterGeometryType(GsGeometry* pGeo);

	/// \brief 数据改变
	virtual void Dirty();

protected:
	Utility::GsVector<GsGeometryPtr> m_vtGeometry;
};
/// \brief GsGeometryCollectionPtr
GS_SMARTER_PTR(GsGeometryCollection);

	
/// \brief 多点几何对象
class GS_API GsMultiPoint:public GsGeometry
{
public:
	/// \brief 默认构造函数
	GsMultiPoint();
	/// \brief 从内存块构造
	/// \param pBlob 几何数据内存块
	/// \param nLen 内存块长度
	GsMultiPoint(const unsigned char* pBlob,int nLen);
	/// \brief 从一个点构造
	/// \param pt GsRawPoint点
	GsMultiPoint(const GsRawPoint& pt);
	/// \brief 从一个点构造
	/// \param pt GsRawPoint3D点
	GsMultiPoint(const GsRawPoint3D& pt);
	/// \brief 从一个点构造
	/// \param x x坐标
	/// \param y y坐标
	GsMultiPoint(double x,double y);
	/// \brief 从一个点构造
	/// \param x x坐标
	/// \param y y坐标
	/// \param z z坐标
	GsMultiPoint(double x,double y,double z);
	/// \brief 从一组点构造
	GsMultiPoint(const GsRawPoint* pt,int nCount);
	/// \brief 从一组点构造
	GsMultiPoint(const GsRawPoint3D* pt,int nCount);

	/// \brief 设置MultiPoint
	void Set(const GsRawPoint* pt,int nCount);

	/// \brief 设置MultiPoint
	void Set(const GsRawPoint3D* pt,int nCount);

	/// \brief 添加一个点
	void Add(const GsRawPoint& pt);
	/// \brief 添加一个点
	void Add(const GsRawPoint3D& pt);
	/// \brief 添加一个点
	void Add(double x,double y);
	/// \brief 添加一个点
	void Add(double x,double y,double z);
	/// \brief 添加一组点
	void Add(const GsRawPoint* pt,int nCount);
	/// \brief 添加一组点
	void Add(const GsRawPoint3D* pt,int nCount);

	/// \brief 插入一个点
	void Insert(int nIndex,const GsRawPoint& pt);
	/// \brief 插入一个点
	void Insert(int nIndex,const GsRawPoint3D& pt); 
	/// \brief 插入一个点
	void Insert(int nIndex,double x,double y);
	/// \brief 插入一个点
	void Insert(int nIndex,double x,double y,double z);
	/// \brief 插入一组点
	void Insert(int nIndex,const GsRawPoint* pt,int nCount);
	/// \brief 插入一组点
	void Insert(int nIndex,const GsRawPoint3D* pt,int nCount);
	
	/// \brief 更新点
	void Replace(int nIndex,const GsRawPoint& pt);
	/// \brief 更新点
	void Replace(int nIndex,const GsRawPoint3D& pt);
	/// \brief 更新点
	void Replace(int nIndex,double x,double y);
	/// \brief 更新点
	void Replace(int nIndex,double x,double y, double z);
	/// \brief 通过index查询点
	GsRawPoint3D PointAt(int nIndex) const;

	/// \brief 几何对象的数量
	/// \return 返回几何对象的数量
	unsigned int Count() const;

	/// \brief 移除一个几何对象
	/// \param nIndex 几何对象索引
	void RemoveAt(int nIndex);

	/// \brief 清除几何对象
	void Clear();
	
	virtual bool IsEmpty()  const override;
};
/// \brief GsMultiPointPtr
GS_SMARTER_PTR(GsMultiPoint);
 

/// \brief 线段基类
class GS_API GsSegment:public GsGeometry
{	
protected:
	GsSegment();
public:
	virtual ~GsSegment();

	/// \brief	获得Segment的长度
	/// \return	Segment的长度
	virtual double Length() const = 0;

	/// \brief	计算测地线长度
	/// \details 坐标单位必须为度
	/// \return	测地线长度
	virtual double GeoDesicLength() const ;
	/// \brief	计算特定椭球下测地线长度
	/// \details 坐标单位必须为度
	/// \return	测地线长度
	virtual double GeoDesicLength(GsGeodesic& desic) const ;

	/// \brief	求出给定点到Segment最近点所在Segment上的位置
	/// \param inPoint	给定点
	/// \return	给定点到Segment上最近点所在Segment上的位置
	virtual double Nearest(const GsRawPoint3D& inPoint) const = 0;
	/// \brief	求出给定点到Segment最近点所在Segment上的位置
	/// \param inPoint	给定点
	/// \return	给定点到Segment上最近点所在Segment上的位置
	virtual double Nearest(const GsRawPoint& inPoint) const;

	/// \brief	求出给定点到Segment上的最短距离
	/// \param inPoint	给定点
	/// \return	给定点到Segment上的最短距离
	virtual double Distance2(const GsRawPoint3D& inPoint) const = 0;
	virtual double Distance2(const GsRawPoint& inPoint) const;

	/// \brief	得到Segment在指定位置处的点.
	/// \param	parameter	点在Segment上的比例参数.
	/// \return	Segment在指定位置处的点
	virtual GsRawPoint3D Value(double parameter) const = 0;

	/// \brief	求点pt在Segment的左右边，返回1表示左边，返回2表示右边，0表示共线
	/// \param	pt  	给定的点
	/// \param	tol2	容差
	/// \return	返回1表示左边，返回2表示右边，0表示共线
	virtual int LeftRight(const GsRawPoint3D& pt, double tol2) const = 0;
	virtual int LeftRight(const GsRawPoint& pt, double tol2) const;
	

	/// \brief	判断组成Segment的所有点是不是共点
	/// \return	如果共点返回true，如果不共点返回false
	virtual bool IsPoint() const = 0;

	/// \brief	Segment最大比例，Line返回1，CircleARC返回2
	/// \return	1或者2
	virtual double MaxParameter() const;

	/// \brief	对Segment进行翻转
	GsSegment* Reverse();

	/// \brief	获取Segment的fromPoint
	/// \return	Segment的fromPoint
	GsRawPoint3D FromPoint() const;

	/// \brief	获取Segment的inPoint
	/// \return	Segment的inPoint
	GsRawPoint3D ToPoint() const;

	/// \brief	设置对象的FromPoint
	/// \param	point	对象的FromPoint
	void FromPoint(const GsRawPoint& point);
	void FromPoint(const GsRawPoint3D& point);

	/// \brief	设置对象的ToPoint
	/// \param	point	对象的ToPoint
	void ToPoint(const GsRawPoint& point);
	void ToPoint(const GsRawPoint3D& point);
};
/// \brief GsSegmentPtr
GS_SMARTER_PTR(GsSegment);

/// \brief 连续折线
class GS_API GsPath:public GsGeometry
{ 
public:
	/// \brief path中点的角色类型
	enum GsPathPointRole
	{
		eErrorPointRole,
		eLinePointRole,
		eArcPointRole,
		eCtrlPointRole,
	};
public:
	/// \brief 缺省构造
	GsPath(int coordim = 2);
	/// \brief 从内存中构造path
	GsPath(const unsigned char* pBlob,int nLen);
	
	/// \brief 从一个矩形构造
	GsPath(GsEnvelope* pEnv);
	/// \brief 从一个矩形构造
	GsPath(const GsBox& env);
	
	/// \brief 从一个圆心和半径构造圆
	GsPath(const GsRawPoint3D& ptCenter,double dblRadius);
	/// \brief 从一个圆心和半径构造圆
	GsPath(const GsRawPoint& ptCenter,double dblRadius);

	/// \brief 从3点构造一个圆
	GsPath(const GsRawPoint3D& pt1,const GsRawPoint3D& pt2,const GsRawPoint3D& pt3);
	/// \brief 从3点构造一个圆
	GsPath(const GsRawPoint& pt1,const GsRawPoint& pt2,const GsRawPoint& pt3);

	/// \brief 从一个椭圆构造
	GsPath(const GsRawPoint& ptCenter, double dblSemimajor, double dblSemiminor, double angle = 0);
	/// \brief 从一个椭圆构造
	GsPath(const GsRawPoint3D& ptCenter, double dblSemimajor, double dblSemiminor, double angle = 0);

	/// \brief 构造折线
	GsPath(const double *pCoord,int nLen,int nDim);
	/// \brief 构造折线
	GsPath(const GsRawPoint *ptArray, int count);
	/// \brief构造折线
	GsPath(const GsRawPoint3D *ptArray, int count);


	virtual ~GsPath();

	/// \brief	对Path进行翻转.
	GsPath* Reverse();

	/// \brief 构造折线
	bool  Set(const double *pCoord, int nLen, int nDim);
	/// \brief 构造折线
	bool  Set(const GsRawPoint *ptArray, int count);
	/// \brief构造折线
	bool  Set(const GsRawPoint3D *ptArray, int count);

	/// \brief 将自身数据设置为一个矩形
	bool  Set(GsEnvelope* pEnv);
	bool  Set(const GsBox& env);

	/// \brief 根据圆心和半径设置自身为圆
	bool  Set(const GsRawPoint3D& ptCenter,double dblRadius);
	/// \brief 根据圆心和半径设置自身为圆
	bool  Set(const GsRawPoint& ptCenter,double dblRadius);

	/// \brief 根据3点设置自身为圆
	/// \return 如果3点共线则返回false
	bool  Set(const GsRawPoint3D& pt1,const GsRawPoint3D& pt2,const GsRawPoint3D& pt3);
	/// \brief 根据3点设置自身为圆
	/// \return 如果3点共线则返回false
	bool  Set(const GsRawPoint& pt1,const GsRawPoint& pt2,const GsRawPoint& pt3);

	/// \brief 根据圆心、长短半轴以及旋转角设置为椭圆
	bool  Set(const GsRawPoint3D& ptCenter, double dblSemimajor, double dblSemiminor, double angle, double dblTol = 0.0);
	/// \brief 根据圆心、长短半轴以及旋转角设置为椭圆
	bool  Set(const GsRawPoint& ptCenter, double dblSemimajor, double dblSemiminor, double angle, double dblTol = 0.0);

	/// \brief 判断path是否构成了一个正圆，由两个三点圆弧构成圆。
	bool IsCircle()const;

	/// \brief 是否封闭了
	bool IsClosed()const;

	/// \brief 路径是否为空
	/// \return 返回路径是否为空。如果path中一个点也没有保存则为空
	virtual bool IsEmpty()const  override;

	/// \brief 路径是否有效
	/// \return 返回路径是否有效，path中至少存在一个线段才是有效，因此最低需要2个点。
	bool IsValid()const;

	/// \brief 是否是完全由线构成的
	/// \return 是否是完全由线构成的
	bool IsOnlyLine() const;

	/// \brief 是否是完全由圆弧构成的
	/// \return 是否是完全由圆弧构成的
	bool IsOnlyArc()const;

	/// \brief 是否是由圆弧和线组合的
	/// \return 是否是由圆弧和线组合的
	bool IsCompound()const;

	/// \brief 最大的参数
	/// \return 最大的参数
	virtual  double MaxParameter()const;

	/// \brief 根据参数获得点坐标
	/// \return 返回参数对应的点坐标。
	virtual GsRawPoint3D Value(double t);

	/// \brief 计算距离点最近的线上的参数
	/// \return 返回参数。
	virtual double Closet(const GsRawPoint& pt);

	/// \brief 计算距离点最近的线上的参数
	/// \return 返回参数。
	virtual double Closet(const GsRawPoint3D& pt);


	/// \brief 增加一个线段
	/// \param pSeg 线段
	virtual void Add(GsSegment* pSeg);
	/// \brief 增加一个点
	void Add(const GsRawPoint& r);
	/// \brief 增加一个3D点
	void Add(const GsRawPoint3D& r); 


	/// \brief 线段的数量
	/// \return 返回Path中线段的数量
	virtual unsigned int SegmentCount() const;
	
	/// \brief 根据索引移除一个Segment
	/// \return 返回被移除的Segment的对象
	virtual GsSegmentPtr RemoveSegment(int nIndex);

	/// \brief 根据索引获取一个线段
	/// \return 返回线段的对象或者空
	virtual GsSegmentPtr Segment(int i) const;

	/// \brief 在尾部增加一段线。
	virtual void LineTo(const GsRawPoint& pt);
	/// \brief 在尾部增加一段线。
	virtual void LineTo(const GsRawPoint3D& pt);

	/// \brief 在尾部增加一段三点圆弧
	virtual void ArcTo(const GsRawPoint& middle,const GsRawPoint& end);
	/// \brief 在尾部增加一段三点圆弧
	virtual void ArcTo(const GsRawPoint3D& middle,const GsRawPoint3D& end);


	/// \brief 返回点的数量
	/// \return 返回点的数量
	virtual unsigned int PointCount()const;

	/// \brief 获取第i个点
	GsRawPoint3D Point(int i)const;
	
	/// \brief 根据索引获得点的角色
	GsPathPointRole PointRole(int i)const;

	/// \brief 得到起点
	GsRawPoint3D StartPoint()const;
	/// \brief 设置为起点
	void StartPoint(const GsRawPoint& rhs);
	/// \brief 设置3D起点
	void StartPoint(const GsRawPoint3D& rhs); 

	/// \brief 得到终点
	GsRawPoint3D EndPoint()const;
	/// \brief 设置终点
	void EndPoint(const GsRawPoint& rhs);
	/// \brief 设置3D终点
	void EndPoint(const GsRawPoint3D& rhs); 
	
	/// \brief 插入一个点
	bool Insert(int nIndex,const GsRawPoint& r);
	/// \brief 插入一个3D点
	bool Insert(int nIndex,const GsRawPoint3D& r);
	
	/// \brief 清除所有的点。
	void Clear();

	/// \brief 减少一个点
	void Remove(int nIndex);
	/// \brief 替换一个点
	void Replace(int nIndex,const GsRawPoint& r);
	/// \brief 替换一个3D点
	void Replace(int nIndex,const GsRawPoint3D& r);

	/// \brief 将自身中的圆弧内插成折线
	/// \param num 180度内插的点数量
	/// \return 返回是否内插
	virtual bool Interpolate(int num);

    /// \brief 将自身中的圆弧内插成折线，内插后的折线和圆弧之间的最大距离平方不超过tol2
	/// \param tol2 内插后折线距离圆弧的最大距离
	/// \return 返回是否内插
	virtual bool  Interpolate(double tol2);

	/// \brief 按照指定的间距按照测地线加密
	/// \details 几何对象如果没有空间参考则当作地理坐标系数据直接处理
	/// \param  dblInterval 差值间隔，单位米
	virtual GsGeometryPtr GeodesicInterpolate(double dblInterval)override;

	/// \brief 得到长度
	/// \return 返回Polyline长度
	double Length() const;
	/// \brief	计算测地线长度.
	/// \details 坐标单位必须为度
	/// \return	测地线长度
	virtual double GeoDesicLength() const  ;
	/// \brief	计算特定椭球下测地线长度.
	/// \details 坐标单位必须为度
	/// \return	测地线长度
	virtual double GeoDesicLength(GsGeodesic& desic) const  ;
	/// \brief 得到指定位置的斜率。
	/// \param parameter 在path中的位置，从0开始，一段Segment加1，其中小数是指在线段中间。
	/// \return 返回指定位置的斜率 k=y/x = point.Y/point.X
	virtual GsRawPoint TangAt(double parameter);
};
/// \brief GsPathPtr
GS_SMARTER_PTR(GsPath);

/// \brief 单圈面
class GS_API GsRing:public GsPath
{
	geostar::gobjptr					m_Spi;
	double m_dblArea;
public:
	using GsPath::Set;
	GsRing(int nDim = 2);
	/// \brief 从内存中构造path
	GsRing(const unsigned char* pBlob,int nLen);
	/// \brief 从坐标串构造
	GsRing(const double *pCoord, int nLen, int nDim);
	/// \brief 从坐标串构造
	GsRing(const GsRawPoint *ptArray, int count);
	/// \brief 从坐标串构造
	GsRing(const GsRawPoint3D *ptArray, int count);


	/// \brief 从一个矩形构造
	GsRing(GsEnvelope* pEnv);
	/// \brief 从一个矩形构造
	GsRing(const GsBox& env);

	/// \brief 从一个圆心和半径构造圆
	GsRing(const GsRawPoint3D& ptCenter,double dblRadius);
	/// \brief 从一个圆心和半径构造圆
	GsRing(const GsRawPoint& ptCenter,double dblRadius);

	/// \brief 从3点构造一个圆
	GsRing(const GsRawPoint3D& pt1,const GsRawPoint3D& pt2,const GsRawPoint3D& pt3);
	/// \brief 从3点构造一个圆
	GsRing(const GsRawPoint& pt1,const GsRawPoint& pt2,const GsRawPoint& pt3);

	/// \brief 从一个椭圆构造
	GsRing(const GsRawPoint& ptCenter, double dblSemimajor, double dblSemiminor, double angle = 0,double dblTol = 0.0);
	/// \brief 从一个椭圆构造
	GsRing(const GsRawPoint3D& ptCenter, double dblSemimajor, double dblSemiminor, double angle = 0, double dblTol = 0.0);

	/// \brief 从扇形构造
	GsRing(double r, double startAngle, double endAngle, const GsRawPoint& ptCenter);
	/// \brief 从扇形构造
	GsRing(double r, double startAngle, double endAngle, const GsRawPoint3D& ptCenter);


	virtual ~GsRing();

	/// \brief 从扇形构造
	void Set(double r, double startAngle, double endAngle, const GsRawPoint& ptCenter);
	/// \brief 从扇形构造
	void Set(double r, double startAngle, double endAngle, const GsRawPoint3D& ptCenter);



	/// \brief 强制封闭
	bool ForceClose();

	/// \brief 积分面积
	double Area();
	/// \brief 椭球面积
	/// \details 几何对象坐标系必须为地理坐标
	double GeodesicArea(GsGeodesicPolygon &geodesic);

	/// \brief 椭球面积
	/// \details 几何对象坐标系必须为地理坐标
	double GeodesicArea();

	/// \brief 获取圈的方向是否是逆时针方向
	bool IsCounterClockwise();

	/// \brief Label点
	/// \return 返回Label点
	virtual GsRawPoint LabelPoint() override;

	/// \brief 重心点
	/// \return 返回重心点
	virtual GsRawPoint Centroid();

	/// \brief 是否是外圈
	/// \return 返回是否是外圈
	virtual bool IsExterior();

	/// \brief	将自身内插为折线 
	/// \param	dblTol2	 内插的折线距离圆弧的的最大距离的平方
	/// \param	vec	输出点.
	virtual void Interpolate(double dblTol2, Utility::GsVector<GsRawPoint3D>& vec) const;

	/// \brief 将自身中的圆弧内插成折线
	/// \param num 180度内插的点数量
	/// \return 返回是否内插
	virtual bool Interpolate(int num)override;

	/// \brief 将自身中的圆弧内插成折线，内插后的折线和圆弧之间的最大距离平方不超过tol2
	/// \param tol2 内插后折线距离圆弧的最大距离
	/// \return 返回是否内插
	virtual bool  Interpolate(double tol2)override;

	/// \brief 是否在空间上覆盖
	/// \return 返回是否覆盖
	bool IsCover(GsRing * ring);
};
/// \brief GsRingPtr
GS_SMARTER_PTR(GsRing);
 
/// \brief 两点线段
class GS_API GsLine:public GsSegment
{
public:
	/// \brief	默认构造函数，创建由两个二维原点（0， 0）构成的GsLine
	GsLine();

	/// \brief	构造函数，根据内存块构造对象.
	/// \param	pBlob	几何数据内存块.
	/// \param	nLen 	内存块长度.
	GsLine(const unsigned char* pBlob,int nLen);

	/// \brief	构造函数，根据起始点构造对象.
	/// \param fromPoint	from point.
	/// \param toPoint  	to point.
	GsLine(const GsRawPoint& fromPoint, const GsRawPoint& toPoint);
	GsLine(const GsRawPoint3D& fromPoint, const GsRawPoint3D& toPoint);

	/// \brief	根据点坐标构造对象.
	/// \param	x1	FromPoint的X坐标.
	/// \param	y1	FromPoint的Y坐标.
	/// \param	x2	ToPoint的X坐标.
	/// \param	y2	ToPoint的Y坐标.
	GsLine(double x1, double y1, double x2, double y2);
	GsLine(double x1, double y1, double z1, double x2, double y2, double z2);

	/// \brief	拷贝构造函数.
	/// \param	line 要拷贝的线.
	GsLine(GsLine* line);

	/// \brief	析构函数
	~GsLine();

	/// \brief	设置Line的FromPoint和ToPoint.
	/// \param	fromPoint	from point.
	/// \param	toPoint  	to point.
	/// \return	返回Set点坐标以后的Line对象.
	GsLine* Set(const GsRawPoint& fromPoint, const GsRawPoint& toPoint);
	GsLine* Set(const GsRawPoint3D& fromPoint, const GsRawPoint3D& toPoint);

	/// \brief	获取3D的Line的FromPoint和ToPoint.
	/// \param	fromPoint	from point.
	/// \param	toPoint  	to point.
	void Get(GsRawPoint& fromPoint, GsRawPoint& toPoint) const;
	void Get(GsRawPoint3D& fromPoint, GsRawPoint3D& toPoint) const;

	/// \brief	获得Line的角度.
	/// \return	角度值.
	double Angle() const;
	
	/// \brief	获得Segment的长度.
	/// \return	Segment的长度.
	virtual double Length() const override;
	 
	/// \brief	求出给定点到Segment最近点所在Segment上的位置
	/// \param inPoint	给定点
	/// \return	给定点到Segment上最近点所在Segment上的位置
	virtual double Nearest(const GsRawPoint3D& inPoint) const override;
	

	/// \brief	求出给定点到Segment上的最短距离
	/// \param inPoint	给定点
	/// \return	给定点到Segment上的最短距离 
	virtual double Distance2(const GsRawPoint3D& inPoint) const override;
	

	/// \brief	得到Segment在指定位置处的点.
	/// \param	parameter	点在Segment上的比例参数.
	/// \return	Segment在指定位置处的点.
	virtual GsRawPoint3D Value(double parameter) const override;

	/// \brief	求点pt在直线me的左右边，返回1表示左边，返回2表示右边，0表示共线
	/// \param	pt  	给定的点
	/// \param	tol2	容差
	/// \return	返回1表示左边，返回2表示右边，0表示共线
	virtual int LeftRight(const GsRawPoint3D& pt, double tol2) const override;

	/// \brief	判断组成Line的所有点是不是共点
	virtual bool IsPoint() const override;
	    
};
/// \brief GsLinePtr
GS_SMARTER_PTR(GsLine);

class GS_API GsCircleArc:public GsSegment
{

public:
	/// \brief 构造函数
	GsCircleArc();
	/// \brief 从内存块构造
	/// \param pBlob 几何数据内存块
	/// \param nLen 内存块长度
	GsCircleArc(const unsigned char* pBlob,int nLen); 
	/// \brief 从三个二维点构造
	GsCircleArc(const GsRawPoint &from,const GsRawPoint &middle,const GsRawPoint &end);
		/// \brief 从三个三维点构造
    GsCircleArc(const GsRawPoint3D &from,const GsRawPoint3D &middle,const GsRawPoint3D &end);

	/// \brief 从圆心，半径、起始角度构造
	GsCircleArc(const GsRawPoint &center, double r,double startAngle,double endAngle);
	/// \brief 从圆心，半径、起始角度构造
	GsCircleArc(const GsRawPoint3D &center, double r, double startAngle, double endAngle);

	GsCircleArc(double fromx,double fromy,double middlex,double middley,double tox,double toy);
	GsCircleArc(double fromx, double fromy,double fromz, double middlex, double middley,double middlez, double tox, double toy,double toz );

	/// \brief 根据起点、中间点和终点设定弧段
	void Set(const GsRawPoint &from,const GsRawPoint &middle,const GsRawPoint &end);
	void Set(const GsRawPoint3D &from,const GsRawPoint3D &middle,const GsRawPoint3D &end);
	/// \brief 从圆心，半径、起始角度构造
	void Set(const GsRawPoint &center, double r, double startAngle, double endAngle);
	/// \brief 从圆心，半径、起始角度构造
	void Set(const GsRawPoint3D &center, double r, double startAngle, double endAngle);

	/// \brief 外接矩形范围
	/// \return 返回外接矩形对象
	virtual GsBox Envelope() override;
	
	/// \brief	求出给定点到Segment最近点所在Segment上的位置
	/// \param inPoint	给定点
	/// \return	给定点到Segment上最近点所在Segment上的位置
	virtual double Nearest(const GsRawPoint3D& inPoint) const override;

	/// \brief	求出给定点到Segment上的最短距离
	/// \param inPoint	给定点
	/// \return	给定点到Segment上的最短距离
	virtual double Distance2(const GsRawPoint3D& inPoint) const override;

	/// \brief	求点pt在直线me的左右边，返回1表示左边，返回2表示右边，0表示共线
	/// \param	pt  	给定的点
	/// \param	tol2	容差
	/// \return	返回1表示左边，返回2表示右边，0表示共线
	virtual int LeftRight(const GsRawPoint3D& pt, double tol2) const override;

	/// \brief 获取圆弧弦高
	double ChordHeight() const;

	/// \brief 获取圆弧的起始角度和终止角度
   void Angle(double& startAngle, double& endAngle) const;

	/// \brief 获取圆弧的半径
	double Radius() const;
	
	/// \brief  获取圆弧的圆心
	GsRawPoint3D  CenterPoint() const;

	/// \brief 获取圆弧是否逆时针方向
	bool IsCounterClockwise() const;

	/// \brief 获取圆弧是否直线
	bool IsLine() const;

	/// \brief 获取圆弧是优弧或劣弧
	bool IsMinor() const;

	/// \brief 获取圆弧是否为同一点
	virtual bool IsPoint() const override;

	/// \brief 得到弧段的补弧，起点，终点不变
	void Complement( );
	 
	/// \brief  获取中间点,三点圆弧第二个点
	GsRawPoint3D MiddlePoint() const;
	
	/// \brief  设置三点圆弧第二个点
	void MiddlePoint(const GsRawPoint3D& rhs);
	/// \brief  设置三点圆弧第二个点
	void MiddlePoint(const GsRawPoint& rhs);

	 
	/// \brief 查询圆弧的起点、中间点和终点
	void Get(GsRawPoint& from, GsRawPoint&  pMiddle, GsRawPoint& pTo) const;
	void Get(GsRawPoint3D& from, GsRawPoint3D&  pMiddle, GsRawPoint3D&  pTo) const;

	///\brief 获取圆弧的长度
	virtual double Length() const  override;

	/// \brief 按照指定的间距按照测地线加密
	/// \details 几何对象如果没有空间参考则当作地理坐标系数据直接处理
	/// \param  dblInterval 差值间隔，单位米
	virtual Utility::GsSmarterPtr<GsGeometry> GeodesicInterpolate(double dblInterval)override;

	using GsSegment::GeoDesicLength;
	/// \brief	计算特定椭球下测地线长度.
	/// \details 坐标单位必须为度
	/// \return	测地线长度
	virtual double GeoDesicLength(GsGeodesic& desic) const override;

	/// \brief	得到Segment在指定位置处的点.
	/// \param	parameter	点在Segment上的比例参数.
	/// \return	Segment在指定位置处的点.
	virtual GsRawPoint3D Value(double parameter) const override;

	/// \brief	将自身内插为折线，
	/// \param	num	180度内插的点数量
	/// \param	vec	输出点.
	void Interpolate(int num, Utility::GsVector<GsRawPoint3D>& vec)  const;

	/// \brief	将自身内插为折线
	/// \param	dblTol2	 内插的折线距离圆弧的的最大距离的平方
	/// \param	vec	输出点.
	void Interpolate(double dblTol2, Utility::GsVector<GsRawPoint3D>& vec) const;


};
/// \brief GsCircleArcPtr
GS_SMARTER_PTR(GsCircleArc);


/// \brief 多边形对象
class GS_API GsPolygon:public GsGeometryCollection
{
public:
	/// \brief 从内存块构造
	/// \param pBlob 几何数据内存块
	/// \param nLen 内存块长度
	GsPolygon(const unsigned char* pBlob,int nLen); 

	/// \brief 默认构造
	GsPolygon();

	/// \brief 是否闭合
	bool IsClosed();

	/// \brief 强制闭合
	bool ForceClose();
	
	/// \brief 获取内环的外环
	GsRingPtr FindExteriorRing(GsRing* pInteriorRing);

	/// \brief 获取外环的内环个数
	int InteriorRingCount(GsRing* pExteriorRing);

	/// \brief 获取外环的所有内环
	GsGeometryCollectionPtr InteriorRings(GsRing* ppCollection );
	
	/// \brief 获取外环个数
	int ExteriorRingCount();

	/// \brief 获取所有的外环
	GsGeometryCollectionPtr ExteriorRings() ;
	
	
	/// \brief 获取面积
	double Area();

	/// \brief 椭球面积
	/// \details 几何对象坐标系必须为地理坐标
	double GeodesicArea(GsGeodesicPolygon &geodesic);
	/// \brief 椭球面积
	/// \details 几何对象坐标系必须为地理坐标
	double GeodesicArea();

	/// \brief 得到轮廓线长度
	/// \return 返回轮廓线长度
	double Length();
	/// \brief	计算轮廓线的测地线长度.
	/// \details 坐标单位必须为度
	/// \return	测地线长度
	virtual double GeoDesicLength() const  ;
	/// \brief	计算特定椭球下轮廓线的测地线长度.
	/// \details 坐标单位必须为度
	/// \return	测地线长度
	virtual double GeoDesicLength(GsGeodesic& desic) const  ;

	/// \brief 获取面状几何对象的一个内部点
	virtual GsRawPoint LabelPoint() override;

	/// \brief 获取面的重心
	GsRawPoint Centroid();

	/// \brief 判断是否为空
	virtual bool IsEmpty() const override;

	/// \brief 是否是复合多边形（具有多个外圈的多边形）
	bool IsMultiPolygon();

	/// \brief 将复合多边分解为为多个简单多边形。
	GsGeometryCollectionPtr ToSimplePolygon();

	/// \brief 获取Se对象 polygon 会重写此方法
	virtual geostar::gobjptr ToSE()override;
	
protected:
	/// \brief 过滤几何类型
	/// \param pGeo 被过滤的几何对象指针
	/// \return 返回过滤结果
	virtual bool FilterGeometryType(GsGeometry* pGeo) override;

	/// \brief 强制方向良好
	void ForceGoodDirect();
	 
	/// \brief 数据改变
	virtual void Dirty()override;

	/// \brief 是否需要重新整理
	bool m_bNeedArrange;
	/// \brief 组织所有geoobject
	Utility::GsVector<geostar::gobjptr> SeparateRing();
	/// \brief 查找gobj
	geostar::gobjptr FindRing(geostar::gobjptr& obj);
	/// \brief 将集合内的envlope变为ring,解决
	void RegularizationRings();

};
/// \brief GsPolygonPtr
GS_SMARTER_PTR(GsPolygon);
  

/// \brief 多段线
class GS_API GsPolyline:public GsGeometryCollection
{
public:
	/// \brief 缺省构造
	GsPolyline();

	/// \brief 从内存块构造
	/// \param pBlob 几何数据内存块
	/// \param nLen 内存块长度
	GsPolyline(const unsigned char* pBlob,int nLen);
	 
	
	/// \brief 设置为起点
	/// \param rhs 二维点对象
	void StartPoint(const GsRawPoint& rhs);

	/// \brief 设置起点
	/// \param rhs 三维点对象
	void StartPoint(const GsRawPoint3D& rhs);

	/// \brief 得到起点
	/// \return 返回三维起点
	GsRawPoint3D StartPoint();

	/// \brief 设置终点
	/// \param rhs 二维点对象
	void EndPoint(const GsRawPoint& rhs);

	/// \brief 设置终点
	/// \param rhs 三维点对象
	void EndPoint(const GsRawPoint3D& rhs); 

	/// \brief 得到终点
	/// \return 返回三维起点
	GsRawPoint3D EndPoint();

	/// \brief 得到长度
	/// \return 返回Polyline长度
	virtual double Length() const;
	/// \brief	计算测地线长度.
	/// \details 坐标单位必须为度
	/// \return	测地线长度
	virtual double GeoDesicLength() const  ;
	/// \brief	计算特定椭球下测地线长度.
	/// \details 坐标单位必须为度
	/// \return	测地线长度
	virtual double GeoDesicLength(GsGeodesic& desic) const  ;
	/// \brief 反转Polyline
	/// \return 返回反转后的Polyline
	GsPolyline* Reverse();

	/// \brief 计算距离点最近的线上的参数
	/// \return 返回参数。
	virtual double Closet(const GsRawPoint& pt);

	/// \brief 计算距离点最近的线上的参数
	/// \return 返回参数。
	virtual double Closet(const GsRawPoint3D& pt);

	/// \brief	得到Polyline在指定位置处的点
	/// \param	parameter	点在Polyline上的比例参数.
	/// \return	Polyline在指定位置处的点.
	virtual GsRawPoint3D Value(double parameter);

	/// \brief	得到子部分
	/// \param	dblFromDis	输入的起点参数位置
	/// \param	dblToDis	输入的终点参数位置
	/// \return	新的Polyline
	virtual Utility::GsSmarterPtr<GsPolyline> SubCurve(double dblFromDis, double dblToDis);
	
	virtual bool IsEmpty() const override;
protected:
	/// \brief 过滤几何类型
	/// \param pGeo 被过滤的几何对象指针
	/// \return 返回过滤结果
	virtual bool FilterGeometryType(GsGeometry* pGeo)override;

protected:
	/// \brief 触发序列化行为
	void TriggerSerialization();

};
/// \brief GsPolylinePtr
GS_SMARTER_PTR(GsPolyline);


/// \brief 几何对象类工厂
class GS_API  GsGeometryFactory
{
public:
	
	/// \brief 返回正规化的几何对象
	/// \details 正规化Geometry只返回Point,MultiPoint,Polyline,Polygon
	static GsGeometryPtr Normalize(GsGeometry* geo);
	/// \brief 返回正规化的几何对象
	/// \details 正规化Geometry只返回Point,MultiPoint,Polyline,Polygon
	static GsGeometryType Normalize(GsGeometryType eType);


	/// \brief 创建指定类型的几何对象
	static GsGeometryPtr CreateGeometry(GsGeometryType t);

	/// \brief 从存储用途的字节序转换为可读几何内存块
	/// \param pBlob 几何数据内存块
	/// \param nLen 内存块长度
	/// \param eEndian 字节序
	static void ConvertByteOrderFromStorageBlob(const unsigned char* pBlob, int nLen, Utility::GsEndian eEndian = Utility::eBigEndian);
	// \brief 转换几何内存块为存储用途的字节序
	/// \param pBlob 几何数据内存块
	/// \param nLen 内存块长度
	/// \param eEndian 字节序
	static void ConvertByteOrderToStorageBlob(const unsigned char* pBlob, int nLen, Utility::GsEndian eEndian = Utility::eBigEndian);

	// \brief 根据几何内存块以及字节序创建几何对象
	// \details 如果传入字节序为存储用途字节序则会转换为当前系统的字节序
	/// \param pBlob 几何数据内存块
	/// \param nLen 内存块长度
	/// \param eEndian 几何内存块的字节序类型
	/// \return 返回创建的几何对象指针
	static GsGeometryPtr CreateGeometryFromBlob(const unsigned char* pBlob, int nLen, Utility::GsEndian eEndian = Utility::eLittleEndian);

	// \brief 根据几何内存块创建几何对象
	static GsGeometryPtr CreateGeometryFromBlob(GsGeometryBlob* pBlob);

	// \brief 从oracle的坐标串和解释串构建Geometry对象
	static GsGeometryPtr CreateGeometryFromOracle(int* pInter, int nInterLen, double* pCoord, int nCoordLen, int nDim);

	// \brief 直接从geometryse对象创建为Geometry对象
	static GsGeometryPtr CreateGeometryFromGeoObject(geostar::geo_object* p);

	// \brief 根据解释串获得几何类型
	static GsGeometryType GeometryTypeFromInterpret(int* pInter, int nInterLen, double* pCoord, int nCoordLen, int nDim, bool bNormalize = false);

	// \brief 创建点对象
	static GsPointPtr CreatePoint(double x, double y);
	// \brief 创建点对象
	static GsPointPtr CreatePoint(double x, double y, double z);
	// \brief 创建点对象
	static GsPointPtr CreatePoint(const GsRawPoint& pt);
	// \brief 创建点对象
	static GsPointPtr CreatePoint(const GsRawPoint3D& pt);

	// \brief 创建点对象
	static GsMultiPointPtr CreateMultiPoint(const double* pCoord,int nCount,int nDim = 2);
	// \brief 创建点对象
	static GsMultiPointPtr CreateMultiPoint(const GsRawPoint* pt,int nCount);
	// \brief 创建点对象
	static GsMultiPointPtr CreateMultiPoint(const GsRawPoint3D* pt, int nCount);

	// \brief 创建两点线段
	static GsLinePtr CreateLine(double x1,double y1,double x2,double y2);
	// \brief 创建两点线段
	static GsLinePtr CreateLine(double x1, double y1,double z1, double x2, double y2,double z2);
	// \brief 创建两点线段
	static GsLinePtr CreateLine(const GsRawPoint& pt1, const GsRawPoint& pt2);
	// \brief 创建两点线段
	static GsLinePtr CreateLine(const GsRawPoint3D& pt1, const GsRawPoint3D& pt2);

	// \brief 根据起点方位角和长度创建两点线段
	static GsLinePtr CreateLine(const GsRawPoint& pt1, double angle,double length);
	// \brief 根据起点方位角和长度创建两点线段
	static GsLinePtr CreateLine(const GsRawPoint3D& pt1, double angle, double length);

	// \brief 创建三点圆弧
	static GsCircleArcPtr CreateCircleArc(double x1, double y1, double x2, double y2,double x3,double y3);
	// \brief 创建三点圆弧
	static GsCircleArcPtr CreateCircleArc(double x1, double y1,double z1, double x2, double y2,double z2, double x3, double y3,double z3);
	// \brief 创建三点圆弧
	static GsCircleArcPtr CreateCircleArc(const GsRawPoint& pt1, const GsRawPoint& pt2, const GsRawPoint& pt3);
	// \brief 创建三点圆弧
	static GsCircleArcPtr CreateCircleArc(const GsRawPoint3D& pt1, const GsRawPoint3D& pt2, const GsRawPoint3D& pt3);

	// \brief 创建三点圆弧
	static GsCircleArcPtr CreateCircleArc(const GsRawPoint& center, double r,double startangle,double endangle);
	// \brief 创建三点圆弧
	static GsCircleArcPtr CreateCircleArc(const GsRawPoint3D& center, double r, double startangle, double endangle);
	// \brief 根据起点、终点和拱高创建三点圆弧
	static GsCircleArcPtr CreateCircleArc(const GsRawPoint3D& p1, const GsRawPoint3D& p2,double chord);
	// \brief 根据起点、终点和拱高创建三点圆弧
	static GsCircleArcPtr CreateCircleArc(const GsRawPoint& p1, const GsRawPoint& p2, double chord);

	// \brief 创建折线
	static GsPathPtr CreatePath(const double* pCoord,int nLen,int nDim);
	// \brief 创建折线
	static GsPathPtr CreatePath(const GsRawPoint* ptArray, int nCount);
	// \brief 创建折线
	static GsPathPtr CreatePath(const GsRawPoint3D* ptArray, int nCount);
	// \brief 创建折线
	static GsPathPtr CreatePath(const GsBox& box);
	
	// \brief 创建单圈面
	static GsRingPtr CreateRing(const double* pCoord, int nLen, int nDim);
	// \brief 创建单圈面
	static GsRingPtr CreateRing(const GsRawPoint* ptArray, int nCount);
	// \brief 创建单圈面
	static GsRingPtr CreateRing(const GsRawPoint3D* ptArray, int nCount);
	// \brief 创建单圈面
	static GsRingPtr CreateRing(const GsBox& box);

	// \brief 根据中点、长轴、短轴创建椭圆
	static GsPathPtr CreateEllipse(const GsRawPoint& center, double a, double b,bool bSurface = false);
	// \brief 根据中点、长轴、短轴创建椭圆
	static GsPathPtr CreateEllipse(const GsRawPoint3D& center, double a, double b, bool bSurface = false);
	// \brief 创建圆
	static GsPathPtr CreateCircle(const GsRawPoint& center, double r, bool bSurface = false);
	// \brief 创建圆
	static GsPathPtr CreateCircle(const GsRawPoint3D& center, double r, bool bSurface = false);
	// \brief 创建圆
	static GsPathPtr CreateCircle(const GsRawPoint& p1, const GsRawPoint& p2, const GsRawPoint& p3, bool bSurface = false);
	// \brief 创建圆
	static GsPathPtr CreateCircle(const GsRawPoint3D& p1, const GsRawPoint3D& p2, const GsRawPoint3D& p3, bool bSurface = false);


};



	/// \brief OGC规定的几何类型
enum GsOGCGeometryType
{
	eOGCGeometry,// 0 
	eOGCPoint,// 1 
	eOGCLineString,// 2 
	eOGCPolygon,// 3 
	eOGCMultiPoint,// 4 
	eOGCMultiLineString,// 5 
	eOGCMultiPolygon,// 6 
	eOGCGeometryCollection,// 7 
	eOGCCircularString,// 8 
	eOGCCompoundCurve,// 9 
	eOGCCurvePolygon,// 10 
	eOGCMultiCurve,// 11 
	eOGCMultiSurface,// 12 
	eOGCCurve,// 13 
	eOGCSurface,// 14 
	eOGCPolyhedralSurface,// 15 
	eOGCTIN,// 16 
	
	/// \brief 三点圆弧串
	eOGCArcString = 31,
	/// \brief 三点圆弧
	eOGCArc = 32,
	/// \brief 三点圆
	eOGCCircle = 33,
	/// \brief 由控制点和凸度表示的圆弧串
	eOGCArcStringByBulge= 34,		
	/// \brief 由控制点和凸度表示的圆弧
	eOGCArcByBulge= 35,		
	/// \brief B样条曲线
	eOGCBSPLineCurve= 36,
	/// \brief 贝塞尔曲线
	eOGCBezier = 37,	
	/// \brief 矩形
	eOGCEnvelope = 41,

	eOGCGeometryZ =1000 ,
	eOGCPointZ,//  1001 
	eOGCLineStringZ,//  1002 
	eOGCPolygonZ,//  1003 
	eOGCMultiPointZ,//  1004 
	eOGCMultiLineStringZ,//  1005 
	eOGCMultiPolygonZ,//  1006 
	eOGCGeometryCollectionZ,//  1007 
	eOGCCircularStringZ,//  1008 
	eOGCCompoundCurveZ,//  1009 
	eOGCCurvePolygonZ,//  1010 
	eOGCMultiCurveZ,//  1011 
	eOGCMultiSurfaceZ,//  1012 
	eOGCCurveZ,//  1013 
	eOGCSurfaceZ,//  1014 
	eOGCPolyhedralSurfaceZ,//  1015 
	eOGCTINZ,//  1016
	
	/// \brief 三点圆弧串
	eOGCArcStringZ = 1031,
	/// \brief 三点圆弧
	eOGCArcZ = 1032,
	/// \brief 三点圆
	eOGCCircleZ = 1033,
	/// \brief 由控制点和凸度表示的圆弧串
	eOGCArcStringByBulgeZ= 1034,		
	/// \brief 由控制点和凸度表示的圆弧
	eOGCArcByBulgeZ= 1035,		
	/// \brief B样条曲线
	eOGCBSPLineCurveZ= 1036,
	/// \brief 贝塞尔曲线
	eOGCBezierZ = 1037,	
	/// \brief 矩形
	eOGCEnvelopeZ = 1041,
};
/// \brief WKB字节序
enum GsOGCByteOrder
{
	/// \brief 高字节序 Big Endian 
	eXDR,
	/// \brief 低字节序  Little Endian 
	eNDR,
};

/// \brief 圆弧插值为折线的方法。
enum GsCircularInterpolationStyle
{
	/// \brief 不插值，那么圆弧点将会当作折线点来使用 
	eDoNotInterpolationStyle,
	/// \brief 根据圆弧的角度进行插值 
	eInterpolationByAngle,
	/// \brief 根据圆弧和插值结果的最大距离。 
	eInterpolationByDistance,


};

/// \brief OGC几何对象读取
class GS_API GsOGCReader
{
protected:
	Utility::GsVector<GsRawPoint3D> m_vecCachePoint;
	Utility::GsVector<GsRawPoint> m_vecCachePoint2;
	bool m_bInvertedAxis;
protected:
	virtual GsGeometryPtr CreatePoint(int nDim);
	virtual GsGeometryPtr CreateMultiPoint(int nDim);
	virtual GsGeometryPtr CreateLineString(int nDim);
	virtual GsGeometryPtr CreatePolygon(int nDim); 
	virtual GsGeometryPtr CreateMultiLineString(int nDim);
	virtual GsGeometryPtr CreateMultiPolygon(int nDim);
	virtual GsGeometryPtr CreateGeometryCollection(int nDim);
	
	virtual bool ReadPoint(GsRawPoint3D& pt,int& nDim) = 0; 

	virtual GsOGCGeometryType NextGeometry() = 0;
	virtual void EndGeometry(GsOGCGeometryType e) = 0;

protected:
	Utility::GsStack<GsOGCGeometryType> m_stackType;

	void SplitString(const Utility::GsString& strSrc, Utility::GsVector< Utility::GsString> &vtOut);
	/// \brief 解析字符串类型的坐标串
	void ParseCoords(const Utility::GsString& strCoords, Utility::GsVector<double> &vtCoords, int &nDim);
public:
	GsOGCReader();
	virtual ~GsOGCReader(){}
	/// \brief 读取生成一个几何对象
	virtual GsGeometryPtr Read();
	
	/// \brief 获取坐标是否反向读取参数
	///\details 如果为true，读取的时候将xy反向读取,，如果为false，坐标顺序读取
	bool InvertedAxisOrder();

	/// \brief 设置坐标是否反读取向参数
	///\details 如果为true，读取的时候将xy反向读取,，如果为false，坐标顺序读取
	void InvertedAxisOrder(bool bInvertedAxis);
};

/// \brief WKT格式读取
class GS_API GsWKTOGCReader:public GsOGCReader
{
	Utility::GsString m_strWKT;
	int m_nOffset;
	Utility::GsString ReadUntil(char c);
	int SkipTo(char c);
	Utility::GsString ReadUntil(const char* c);
	bool NextCharIs(char c);
	bool NextCharIs(const char* c);

	GsOGCGeometryType ReadGeometryType();
	virtual bool ReadPoint(GsRawPoint3D& pt,int& nDim); 
	virtual GsOGCGeometryType NextGeometry();
	virtual void EndGeometry(GsOGCGeometryType e);
public:
	GsWKTOGCReader(const char* strWKT); 
	
	/// \brief 重新设置一段新的wkt字符串用于解析
	void Begin(const char* strWKT);


};
/// \brief WKB格式读取
class GS_API GsWKBOGCReader:public GsOGCReader
{
	const unsigned char* m_pStart;
	const unsigned char* m_pEnd;
	const unsigned char* m_pCurrent;
	char m_Order;
	char m_OSOrder;
	bool IsEof();
	template<class T>
	T ReadT()
	{
		T val;
		int l = sizeof(T);
		memcpy(&val,m_pCurrent,l);
		if(l > 1 && m_OSOrder != m_Order) GeoStar::Utility::GsEndianConverter::SwapRefT(val);
		m_pCurrent+=l;
		return val;
	}
	GsPointPtr ReadPoint(bool bHasZ);
	GsPathPtr ReadPath(GsPath* path,bool bHasZ,bool bArc = false,int n = -1);
protected:
	virtual bool ReadPoint(GsRawPoint3D& pt,int& nDim); 

	virtual GsOGCGeometryType NextGeometry();
	virtual void EndGeometry(GsOGCGeometryType e);
public:
	GsWKBOGCReader();
	GsWKBOGCReader(Utility::GsByteBuffer* pBuffer); 
	GsWKBOGCReader(const unsigned char* pBuffer,int nLen); 
	/// \brief 重新设置一段新的WKB数据用于解析Geometry
	bool Begin(const unsigned char* pBuffer,int nLen);
	
	/// \brief 读取生成一个几何对象
	virtual GsGeometryPtr Read();

};
/// \brief KML格式读入
class GS_API GsKMLOGCReader:public GsOGCReader
{
public: 
	GsKMLOGCReader(const char * pKML);
	virtual ~GsKMLOGCReader();
	
	/// \brief 重新设置KML字符串开始读取
	void Begin(const char * pKML);

protected:
	virtual GsGeometryPtr CreatePoint(int nDim);
	virtual GsGeometryPtr CreateMultiPoint(int nDim);
	virtual GsGeometryPtr CreateLineString(int nDim);
	virtual GsGeometryPtr CreatePolygon(int nDim); 
	virtual GsGeometryPtr CreateMultiLineString(int nDim);
	virtual GsGeometryPtr CreateMultiPolygon(int nDim);
	virtual GsGeometryPtr CreateGeometryCollection(int nDim);

	virtual bool ReadPoint(GsRawPoint3D& pt,int &nDim); 
	virtual GsOGCGeometryType NextGeometry();
	virtual void EndGeometry(GsOGCGeometryType e);

protected:
	void *m_pMKLDocument;
	struct GeometryUnit
	{
		Utility::GsString strGeometryType;
		void *pGeometryContent;
	};
	Utility::GsVector<GeometryUnit> m_vtGeometryUnit;
	int m_nIndex;

protected:
	GsGeometryPtr CreatePoint(GeometryUnit &stGeoUnit);
	GsGeometryPtr CreateMultiPoint(GeometryUnit &stGeoUnit);
	GsGeometryPtr CreateLineString(GeometryUnit &stGeoUnit);
	GsGeometryPtr CreatePolygon(GeometryUnit &stGeoUnit); 
	GsGeometryPtr CreateMultiLineString(GeometryUnit &stGeoUnit);
	GsGeometryPtr CreateMultiPolygon(GeometryUnit &stGeoUnit);
	GsOGCGeometryType NextGeometry(GeometryUnit &stGeoUnit);

protected:
	GsPointPtr CreatePoint(void *pKMLContent, int &nDim);
	GsPathPtr CreatePath(void *pKMLContent, int &nDim);
	GsPathPtr CreateRing(void *pKMLContent, int &nDim);
};
/// \brief GML格式读入
class GS_API GsGMLOGCReader:public GsOGCReader
{
	tinyxml2::XMLDocument m_Doc;
	tinyxml2::XMLDocument m_Doc2;   //存在两个XMLDocument是有问题的，临时试一下，后面调整；解析两次也是不对的 等池晶和刘显雄修改
	Utility::GsString m_strCoordinate;
	Utility::GsStack<tinyxml2::XMLElement*> m_StackNode;

	tinyxml2::XMLElement* ChildElement(tinyxml2::XMLElement* pEle,const char* strName);
	void PrepareCoordinate();
public:
	GsGMLOGCReader(const char * strGML);
	virtual ~GsGMLOGCReader();

	void Begin(const char* strGML);
	/// \brief 读取生成一个几何对象
	virtual GsGeometryPtr Read();
	GsGeometryPtr ReadElement(tinyxml2::XMLElement *pElement);

protected:
	virtual GsOGCGeometryType NextGeometry();
	virtual void EndGeometry(GsOGCGeometryType e);
	virtual bool ReadPoint(GsRawPoint3D& pt,int& nDim);


protected: 
	tinyxml2::XMLElement *m_pRoot;
	int ReadDimension(tinyxml2::XMLElement *pElement);
	GsGeometryPtr ReadPoint(tinyxml2::XMLElement *pElement);
	GsGeometryPtr ReadMultiPoint(tinyxml2::XMLElement *pElement);
	GsGeometryPtr ReadPolygon(tinyxml2::XMLElement *pElement);
	GsGeometryPtr ReadMultiPolygon(tinyxml2::XMLElement *pElement);
	GsGeometryPtr ReadRing(tinyxml2::XMLElement *pElement);
	GsGeometryPtr ReadLineString(tinyxml2::XMLElement *pElement);
	GsGeometryPtr ReadPolyLine(tinyxml2::XMLElement *pElement);
	GsGeometryPtr ReadGeometryCollection(tinyxml2::XMLElement *pElement);
	GsGeometryPtr ReadEnvelope(tinyxml2::XMLElement *pElement); 
	GsGeometryPtr ReadMultiCurve(tinyxml2::XMLElement *pElement);
	GsGeometryPtr ReadMultiSurface(tinyxml2::XMLElement *pElement);

};
/// \brief GeoJSON格式读入
class GS_API GsGeoJSONOGCReader:public GsOGCReader
{
	Json::Value * m_Value;//geometry
	Json::Value * m_rootValue;//整个node
	GsOGCGeometryType ReadGeometryType();
	Utility::GsStack<int> m_stackSubIt;
	Utility::GsStack<Json::Value*> m_stackSubValue;
	Utility::GsStlMap<Utility::GsString, Utility::GsAny> m_mapAttribute;
	int m_Crs;
protected:
	virtual bool ReadPoint(GsRawPoint3D& pt,int& nDim) ;
	virtual GsGeometryPtr CreatePolygon(int nDim);
	virtual GsOGCGeometryType NextGeometry();
	virtual void EndGeometry(GsOGCGeometryType e);
	void ReadAttributes(Json::Value * pValue);

public: 
	GsGeoJSONOGCReader(const char* strGeoJSON);
	~GsGeoJSONOGCReader();
	
	/// \brief 从GeoJson读取的属性字段的数量
	virtual int AttributeSize();
	/// \brief 根据索引获取属性数据的名称
	virtual const char* AttributeName(int i);

	/// \brief 根据索引获取属性数据
	virtual const Utility::GsAny& Attribute(int i);
	//epsg id
	int CrsID();
	/// \brief 重新设置GeoJson字符串用于解析几何对象
	void Begin(const char* strGeoJSON);

	
};

/// \brief OGC几何对象写入
class GS_API GsOGCWriter
{
	int m_nIndex;
	bool WriteOne(GsPoint* pt);
	bool WriteOne(GsMultiPoint* pt);
	bool WriteOne(GsGeometryCollection* coll);
	bool WriteOne(GsPath* p);
	bool WriteOne(GsRing* p);
	bool WriteOne(GsLine* p);
	bool WriteOne(GsCircleArc* p);
	bool WriteOne(GsEnvelope* p); 
	bool WriteOne(GsPolyline* pt);
	bool WriteOne(GsPolygon* p); 
	bool WriteOne(GsGeometryCollection* p,int nIndex,int nDim); 
	GsPathPtr Interpolation(GsPath* pPath);
protected:
	
	/// \brief 开始写入一个Geometry的，由子类继承。
	virtual void BeginGeometry(GsOGCGeometryType eType, int nIndex, int nEleCount) = 0;
	
	/// \brief 写入坐标。
	virtual void WriteCoodinate(double* pCoord,int nCount,int nDim) = 0;

	/// \brief 结束一个Geometry
	virtual void EndGeometry()  = 0;
	GsOGCWriter();

	GsCircularInterpolationStyle m_eInterpolationStyle;
	double						m_nInterpolationParameter;
	Utility::GsStack<GsOGCGeometryType> m_LastType;
	bool m_bInvertedAxis;
public:
	virtual ~GsOGCWriter(){}
	/// \brief 获取圆弧的插值类型
	GsCircularInterpolationStyle InterpolationStyle();
	
	/// \brief 设置圆弧的插值类型
	void InterpolationStyle(GsCircularInterpolationStyle  eType);

	/// \brief 圆弧插值参数
	///\details 当插值类型为eInterpolationByAngle时参数代表每180（PI)的角度插值多少点，当插值类型为eInterpolationByDistance时参数代表插值后的折线距离圆弧的最大距离
	double InterpolationParameter();
	
	/// \brief 设置圆弧插值参数
	///\details 当插值类型为eInterpolationByAngle时参数代表每180（PI)的角度插值多少点，当插值类型为eInterpolationByDistance时参数代表插值后的折线距离圆弧的最大距离
	void InterpolationParameter(double value);
	
	/// \brief 获取坐标是否反向参数
	///\details 如果为true，输出的时候将xy反向输出，如果为false，xy坐标不反向直接输出
	bool InvertedAxisOrder();

	/// \brief 设置坐标是否反向参数
	///\details 如果为true，输出的时候将xy反向输出，如果为false，xy坐标不反向直接输出
	void InvertedAxisOrder(bool bInvertedAxis);

	/// \brief 写入一个几何对象
	virtual bool Write(GsGeometry* pGeo);
	
	/// \brief 重置Writer恢复到初始状态
	virtual void Reset() = 0;

	/// \brief 获取Geometry对象对应的OGC类型
	static GsOGCGeometryType OGCGeometryType(GsGeometry* pGeo);
};

/// \brief WKT格式写入
class GS_API GsWKTOGCWriter:public GsOGCWriter
{
	Utility::GsStringStream m_ss;
protected:
	/// \brief 开始写入一个Geometry的，由子类继承。
	virtual void BeginGeometry(GsOGCGeometryType eType,int nIndex, int nEleCount);
	
	/// \brief 写入坐标。
	virtual void WriteCoodinate(double* pCoord,int nCount,int nDim);

	/// \brief 结束一个Geometry
	virtual void EndGeometry();
public:
	GsWKTOGCWriter();
	
	/// \brief 获取写入的wkt字符串
	Utility::GsString WKT();
	
	/// \brief 重置Writer恢复到初始状态
	virtual void Reset();


};
/// \brief WKB格式写入
class GS_API GsWKBOGCWriter:public GsOGCWriter
{
protected:
	unsigned char m_eByteOrder;
	Utility::GsGrowByteBuffer* m_pBuffer;
	bool m_bNeedConvertByteOrder;
	int OrderValue(int i);
	double OrderValue(double d);

	/// \brief 开始写入一个Geometry的，由子类继承。
	virtual void BeginGeometry(GsOGCGeometryType eType,int nIndex,int nEleCount);
	
	/// \brief 写入坐标。
	virtual void WriteCoodinate(double* pCoord,int nCount,int nDim);

	/// \brief 结束一个Geometry
	virtual void EndGeometry();
public:
	GsWKBOGCWriter(Utility::GsGrowByteBuffer* pBuffer,GsOGCByteOrder eByteOrder = eXDR);
	
	/// \brief 缓存
	Utility::GsGrowByteBuffer*  WKB();

	/// \brief 重置Writer恢复到初始状态
	virtual void Reset();

};
 
/// \brief GML格式写入
class GS_API GsGMLOGCWriter:public GsOGCWriter
{ 
protected:
	Utility::GsStringStream m_ss;
	int m_signtemp;
	/// \brief 开始写入一个Geometry的，由子类继承。
	virtual void  BeginGeometry(GsOGCGeometryType eType, int nIndex, int nEleCount);
	/// \brief 写入坐标。
	virtual void WriteCoodinate(double* pCoord,int nCount,int nDim);
	/// \brief 结束一个Geometry
	virtual void EndGeometry();
public:
	/// \brief 获取写入的gml字符串
	Utility::GsString GML();
	GsGMLOGCWriter();
	/// \brief 重置Writer恢复到初始状态
	virtual void Reset();
};

/// \brief GML格式写入
class GS_API GsGML3OGCWriter:public GsGMLOGCWriter
{ 
protected:
	/// \brief 开始写入一个Geometry的，由子类继承。
	virtual void  BeginGeometry(GsOGCGeometryType eType, int nIndex, int nEleCount)override;
		/// \brief 写入坐标。
	virtual void WriteCoodinate(double* pCoord,int nCount,int nDim)override;
	/// \brief 结束一个Geometry
	virtual void EndGeometry()override;
public:
	GsGML3OGCWriter();
};
/// \brief KML格式写入
class GS_API GsKMLOGCWriter:public GsOGCWriter
{   
	Utility::GsStringStream m_ss;
	Utility::GsStack<Utility::GsString> m_stackCloseEle;
	Utility::GsStack<int> m_ElementCount;
	/// \brief 开始写入一个Geometry的，由子类继承
	virtual void  BeginGeometry(GsOGCGeometryType eType, int nIndex, int nEleCount);

	/// \brief 写入坐标。
	virtual void WriteCoodinate(double* pCoord,int nCount,int nDim);

	/// \brief 结束一个Geometry
	virtual void EndGeometry();
public:
	/// \brief 获取写入的gml字符串
	Utility::GsString KML();
	GsKMLOGCWriter();  
	/// \brief 重置Writer恢复到初始状态
	virtual void Reset();
};

/// \brief GeoJSON格式写入
class GS_API GsGeoJSONOGCWriter:public GsOGCWriter
{
	Utility::GsStlMap<Utility::GsString, Utility::GsAny> m_mapAttribute;
	Utility::GsStringStream m_ss;
protected:
	/// \brief 开始写入一个Geometry的，由子类继承。
	virtual void BeginGeometry(GsOGCGeometryType eType, int nIndex, int nEleCount);
	
	/// \brief 写入坐标。
	virtual void WriteCoodinate(double* pCoord,int nCount,int nDim);

	/// \brief 结束一个Geometry
	virtual void EndGeometry();
public:
	GsGeoJSONOGCWriter();
	
	/// \brief GeoJson属性字段的数量
	virtual int AttributeSize();
	
	/// \brief 清除属性字段
	virtual void ClearAttribute();

	/// \brief 根据索引获取属性数据的名称
	virtual const char* AttributeName(int i);

	/// \brief 根据索引获取属性数据
	virtual const Utility::GsAny& Attribute(int i);

	/// \brief 设置属性字段值
	virtual void Attribute(const char* name,const Utility::GsAny& value);
	/// \brief 删除属性字段
	virtual void RemoveAttribute(int i);
	/// \brief 删除属性字段
	virtual void RemoveAttribute(const char* name);

	/// \brief 获取写入的GeoJSON字符串
	Utility::GsString GeoJSON();
	/// \brief 重置Writer恢复到初始状态
	virtual void Reset();
};

/// \brief 针对Geometry封装类，内部存储Geometry索引，加速控件操作比较效率
class GS_API GsIndexGeometry:public Utility::GsRefObject
{
	/// \brief se 索引
	geostar::gobjptr m_Spi;
	/// \brief se 对象
	geostar::gobjptr m_ptrObj;
	/// \brief se 对象
	geostar::safe_ga m_ga;
private:
	GsGeometryRelationResult QuerySpatial(geostar::geo_object*  ptrTar,int type);
public:
	/// \brief 初始化索引几何对象
	GsIndexGeometry(const GsGeometry * pGeo,double dblTol =  FLT_EPSILON);
	/// \brief 初始化索引几何对象
	GsIndexGeometry(const geostar::geo_object * pObj,double dblTol =  FLT_EPSILON);
	/// \brief 获得se几何对象
	geostar::gobjptr ToSeObject();
	/// \brief 比较几何对象是否相贴近
	/// \image html Geometry_IsTouch.png "Geometry相贴近"
	virtual GsGeometryRelationResult IsTouch(GsGeometry * pOther );

	/// \brief 几何对象是否落入在传入几何对象内部。
	/// \image html Geometry_IsWithin.png "Geometry在内部"
	virtual GsGeometryRelationResult IsWithin(GsGeometry * pOther);

	/// \brief 几何对象是否包含传入对象
	/// \image html Geometry_IsContain.png "Geometry包含"
	virtual GsGeometryRelationResult IsContain(GsGeometry * pOther);

	/// \brief 几何对象和传入对象是否在线的语义上相穿
	virtual GsGeometryRelationResult IsCross(GsGeometry * pOther);

	/// \brief 几何对象和传入几何对象是否在线的语义上重叠
	/// \image html Geometry_IsOverlap.png "Geometry相重叠"
	virtual GsGeometryRelationResult IsOverlap(GsGeometry * pOther);

	/// \brief 几何对象和传入几何对象是否相离
	/// \image html Geometry_IsDisjoin.png "Geometry相离"
	virtual GsGeometryRelationResult IsDisjoin(GsGeometry * pOther);

	/// \brief 几何对象和传入几何对象是否相交
	virtual GsGeometryRelationResult IsIntersect(GsGeometry * pOther);

	/// \brief 计算和另外一个几何对象的相交的部分
	virtual Utility::GsSmarterPtr<GsGeometry> Intersection (GsGeometry* pOther);

	/// \brief 计算和另外一个几何对象的合并的结果
	virtual Utility::GsSmarterPtr<GsGeometry> Union (GsGeometry* pOther);

	/// \brief 计算和另外一个几何对象不同的部分
	virtual Utility::GsSmarterPtr<GsGeometry> Difference (GsGeometry* pOther);
	
	/// \brief 计算和另外一个几何对象对称差
	virtual Utility::GsSmarterPtr<GsGeometry> SymDifference (GsGeometry* pOther);
	
	/// \brief 计算和另外一个Geometry的距离
	/// \image html Geometry_Distance.png "Geometry最短路径"
	virtual double Distance (GsGeometry* pGeo);

	/// \brief 用传入的几何对当前几何对象进行剪切
	virtual  Utility::GsSmarterPtr<GsGeometry> Clip(GsGeometry* pCliperGeo, bool bIncludeBoundary);
	
	/// \brief 创建并返回当前几何对象上距离输入点最近的一个点
	virtual Utility::GsSmarterPtr<GsPoint> ReturnNearestPoint(GsPoint * pPoint, GsSegmentExtension extension);
};
GS_SMARTER_PTR(GsIndexGeometry);
KERNEL_ENDNS