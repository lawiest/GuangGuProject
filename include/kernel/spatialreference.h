#pragma once
#include "../utility/preconfig.h" 
#include <object.h>
#include <filesystem.h>
KERNEL_NS  

/// \brief 空间参考数据格式类型
enum GsSpatialReferenceFormat
{
	/// \brief 未知的空间参考定义格式
	eUnknownFormat,
	/// \brief 标准WKT格式
	eWktFormat,
	/// \brief XML格式
	eXMLFormat,
	/// \brief proj.4格式
	eProj4Format,
	/// \brief ESRI的Wkt格式
	eESRIWktFormat,
	/// \brief GeoStar5以逗号分隔的空间参考描述格式
	eGeoStar5Format,
};

/// \brief 已知常用空间参考
enum GsWellKnownSpatialReference
{
	/// \brief WGS84空间参考
	eWGS84,
	/// \brief CGCS2000空间参考
	eCGCS2000,
	/// \brief web墨卡托空间参考
	eWebMercator,
};

/// \brief 坐标单位
enum  GsUnits
{
	/// \brief 未知单位
	eUnknownUnit,

	/// \brief 米
	eMeter = 9001,

	/// \brief foot 1 ft = 0.3048 m
	eFoot = 9002,

	///US survey foot 1 USft = 0.30480060960122 m
	eUSSurveyFoot = 9003,

	/// \brief Clarke's foot 其与米之前的转换关系 1 Clarke's foot = 0.3047972654 m
	eClarkeFoot = 9005,

	/// \brief German legal metre,1 German legal metre= 1.0000135965 m
	eGermanMetre = 9031,

	/// \brief Indian foot,Indian Foot = 0.99999566 British feet (A.R.Clarke 1865). British yard (= 3 British feet) taken to be J.S.Clark's 1865 value of 0.9144025 metres.
	eIndianFoot = 9080,

	/// \brief sexagesimal DMS
	eSexagesimalDMS = 9110,

	/// \brief 度
	eDegree = 9102,

	/// \brief Unity
	eUnity = 9201,

	/// \brief 毫米
	eMM,
	/// \brief 英寸
	eInch,
	/// \brief 厘米
	eCM,
	/// \brief 千米
	eKM,
	/// \brief 英里
	eMile,
	// \brief 海里
	eNatuicalMile,
	// \brief 点
	ePoint,

};

/// \brief 坐标轴方向
enum GsAxisOrientation
{
	/// \brief 不确定方向
    eOther=0,
	/// \brief 朝北
    eNorth=1,
	/// \brief 朝南
    eSouth=2,
	/// \brief 朝东
    eEast=3,
	/// \brief 朝西
    eWest=4,
	/// \brief 朝上
    eUp=5,
	/// \brief 朝下
    eDown=6
};

/// \brief 坐标轴名称
enum GsAxisName
{
	/// \brief 不确定
    eUnknown=0,
	/// \brief 纬度
	eLat=1,
	/// \brief 经度
	eLong=2,
	/// \brief X
	eX=3,
	/// \brief Y
	eY=4,
	/// \brief E
	eE=5,
	/// \brief N
	eN=6
};

/// \brief 坐标系信息
struct GsCoordinateSystem
{
	/// \brief 坐标系的编号
	int ID;
	/// \brief 坐标系的名称
	Utility::GsString Name;
	/// \brief 坐标系的类型
	Utility::GsString Type;
	/// \brief 坐标系的维度
	int Dim;
	/// \brief 坐标系的信息来源
	Utility::GsString InformationSource;	
	/// \brief 坐标系的数据来源
	Utility::GsString DataSource;
};

/// \brief 坐标轴信息
struct GsCoordinateAxis
{
	/// \brief 坐标系的编号
	int CoordinateSystemID;
	/// \brief 坐标轴的编号
	int ID;
	/// \brief 坐标轴的方向
	Utility::GsString Orientation;
	/// \brief 坐标轴的名称
	Utility::GsString Name;
	/// \brief 坐标轴名称的缩写
	Utility::GsString Abbreviation;
	/// \brief 坐标轴单位的编号
	GsUnits UnitID;
	/// \brief 坐标轴的顺序
	int Order;
};

/// \brief 椭球信息
struct GsEllipsoid
{
	/// \brief 椭球的编号
	int ID;
	/// \brief 椭球的名称
	Utility::GsString Name;
	/// \brief 椭球的长半轴
	double EquatorialRadiusA;
	/// \brief 椭球的短半轴
	double PolarRadiusB;
	/// \brief 椭球的扁率的倒数  扁率 = （长半轴-短半轴）/长半轴
	double FlattenInverseF;
	/// \brief 椭球的单位编号，此处指长半轴、短半轴的单位
	GsUnits UnitID;
	/// \brief 椭球的信息来源
	Utility::GsString InformationSource;	
	/// \brief 椭球的数据来源
	Utility::GsString DataSource;
};

/// \brief 基准面
struct GsDatum
{
	/// \brief 基准面编号
	int ID;
	/// \brief 基准面对应的椭球编号
	int EllipsoidID;
	/// \brief 基准面名称
	Utility::GsString Name;
	/// \brief 基准面类型
	Utility::GsString Type;
	/// \brief 基准面主午线ID
	int PrimaryMeridianID;
	/// \brief 基准面信息来源
	Utility::GsString InformationSource;
	/// \brief 基准面数据来源
	Utility::GsString DataSource;
	/// \brief 基准面是否存在7参数
	bool Exist7parameters;
	/// \brief 基准面7参数：在x轴上，将椭球中心相对于WGS 84椭球中心相对移动的米数
	double ShiftX;
	/// \brief 基准面7参数：在Y轴上，将椭球中心相对于WGS 84椭球中心相对移动的米数
    double ShiftY;
	/// \brief 基准面7参数：在z轴上，将椭球中心相对于WGS 84椭球中心相对移动的米数
    double ShiftZ;
	/// \brief 基准面7参数:绕x轴旋转的弧秒数
    double RotateX;
	/// \brief 基准面7参数:绕Y轴旋转的弧秒数
    double RotateY;
	/// \brief 基准面7参数:绕Z轴旋转的弧秒数
    double RotateZ;
	/// \brief 基准面7参数:尺度参数
    double ScaleAdjust;
};

/// \brief 坐标参考系统信息
struct GsCoordinateReferenceSystem
{
	/// \brief 坐标参考系统编号
	int ID;
	/// \brief 坐标参考系统名称
	Utility::GsString Name;
	/// \brief 坐标参考系统类型
	Utility::GsString Type;
	/// \brief 坐标系编号
	int CoordinateSystemID;
	/// \brief 椭球编号
	int DatumID;
	/// \brief 基础地理坐标系的椭球编号
	int GeographicDatumID;
	/// \brief 坐标参考系统信息来源
	Utility::GsString InformationSource;	
	/// \brief 坐标参考系统数据来源
	Utility::GsString DataSource;
};

/// \brief 坐标转换规则
struct GsCoordinateConversionsRule
{
	/// \brief 坐标转换规则名称
	Utility::GsString  Name;
	/// \brief 坐标转换规则类型
    Utility::GsString  Type;
	/// \brief 坐标转换规则编号
    int  ID;
};

/// \brief 坐标转换参数信息
struct GsCoordinateConversionsParameter
{
	/// \brief 坐标转换参数名称
	Utility::GsString Name;
	/// \brief 坐标转换规则编号
    int CoordinateConversionsRuleID;
	/// \brief 坐标转换参数编号
    int ID;
	/// \brief 坐标转换参数值
    int Value;
	/// \brief 坐标转换参数单位编号
    GsUnits UnitID;
};

/// \brief 空间参考
class GS_API GsSpatialReference:public Utility::GsRefObject
{
	mutable Utility::GsString m_strWkt;
	void* m_SR;  
	Utility::GsString m_strName;
	Utility::GsVector<GsCoordinateConversionsParameter> m_vecCoordParams;
public:
	~GsSpatialReference();
	/// \brief 从已知的空间参考类型构造
	GsSpatialReference(GsWellKnownSpatialReference eWellKnown = eWGS84);
	/// \brief 从字符串和指定类型进行构造
	GsSpatialReference(const char* strData,GsSpatialReferenceFormat eFormat = eWktFormat); 
	/// \brief 从EPSG编号构造
	/// \details 使用全局配置项Kernel/SpatialRererence/EPSG设置EPSG.txt所在的路径
	/// \code{.cpp}
	///  GsGlobalConfig::Instance().Child("Kernel/SpatialRererence/EPSG").Value("...SpatialReferenceDataFolder\\EPSG.txt");
	/// 
	/// GsSpatialReferencePtr ptrSR = new GsSpatialReference(4326);
	/// {....}
	/// \endcode
	GsSpatialReference(int nEPSG);
	
	/// \brief 返回内部对象指针
	void* Handle();
	
	/// \brief 空间参考的名称
	Utility::GsString Name() const;
	/// \brief 空间参考的名称
	void Name(const char* strName);

	/// \brief 坐标单位
	GsUnits Units()const;

	/// \brief 坐标单位
	Utility::GsString ExportToWKT(bool bPretty=false)const;
	/// \brief 以XML的格式输出
	Utility::GsString ExportToXml()const;
	/// \brief 以Proj4的格式输出
	Utility::GsString ExportToProj4()const; 

	/// \brief 是否为空
	bool IsNull()const;
	 
	/// \brief 是否是相同的坐标系
	bool IsSame(GsSpatialReference* sr)const;
	/// \brief 是否是相同的投影坐标系
	bool IsSameProjCS(GsSpatialReference* sr)const;
	/// \brief 是否是相同的地理坐标系
	bool IsSameGeogCS(GsSpatialReference* sr)const;
	/// \brief 是否是相同的空间参数，可以作为空间参考是否相同的依据，但不绝对
	bool IsSameParameter(GsSpatialReference* sr) const;
	/// \brief 是否是相同的椭球参数
	bool IsSameEllipsoid(GsSpatialReference* sr) const;
	/// \brief 是否是本地坐标系
	bool IsLocalCS()const;
	/// \brief 是否是地理坐标系
	bool IsGeographic()const;
	/// \brief 是否是投影坐标系
	bool IsProjected()const;
	/// \brief 是否是地心坐标系
	bool IsGeocentric() const;
	
	/// \brief 返回空间参考对应的地理空间参考
	/// \details 投影坐标系返回对应的地理坐标系，
	/// 地理坐标系返回自身,地心坐标系范围对应的地理坐标系
	Utility::GsSmarterPtr<GsSpatialReference> Geographic();

	/// \brief 返回空间参考对应的地心坐标系空间参考
	/// \details 投影坐标系和地理坐标系返回对应的地心坐标系，
	/// 地心坐标系返回自身
	Utility::GsSmarterPtr<GsSpatialReference> Geocentric();

	/// \brief 椭球长半轴(公里 km)
	double EquatorialRadiusA()const; 

	/// \brief 椭球短半轴(公里 km)
	double PolarRadiusB()const; 

	/// \brief 椭球扁率的倒数
	double FlattenInverseF()const; 

	/// \brief 椭球平均半径(公里 km)
	double MeanRadius()const; 
	
	/// \brief EPSG
	int EPSG()const; 

	/// \brief 椭球名称
	Utility::GsString Ellipsoid()const;

	/// \brief 投影名称
	Utility::GsString Projection()const;

	/// \brief 获取坐标轴的方向信息
	/// \param i 坐标轴的顺序（0或者1） 
	/// \param peOrientation 输出坐标轴的方向
	/// \return  返回坐标轴的名称或者空字符串
	Utility::GsString  Axis(int i,GsAxisOrientation* peOrientation);

	/// \brief 获取坐标系信息
	GsCoordinateSystem CoordinateSystem();

	/// \brief 获取坐标轴信息
	/// \param i 坐标轴的顺序（0或者1）
	GsCoordinateAxis CoordinateAxis(int i);

	/// \brief 获取椭球信息
	GsEllipsoid EllipsoidInfo();

	/// \brief 获取基准面信息
	GsDatum Datum();

	/// \brief 获取基础地理坐标系统信息
	GsCoordinateReferenceSystem GeographicCoordinateReferenceSystem();

	/// \brief 获取坐标转换规则信息
	GsCoordinateConversionsRule CoordinateConversionsRule();

	/// \brief 获取坐标转换参数信息
	/// \param nIndex 坐标转换参数的序号（从0开始） 
	GsCoordinateConversionsParameter CoordinateConversionsParameter(int nIndex);

	/// \brief 获取坐标转换参数数量
	int CoordinateConversionsParameterCount();


	/// \brief Clone空间参考
	virtual Utility::GsSmarterPtr<GsSpatialReference> Clone();

};
/// \brief GsSpatialReferencePtr
GS_SMARTER_PTR(GsSpatialReference);


/// \brief 大地线，计算两个经纬度坐标的测地线长度，或称为大圆距离、球面距离
class GS_API GsGeodesic
{ 
protected:
  struct geodesic {
    double a;                   /**< the equatorial radius */
    double f;                   /**< the flattening */
    /**< @cond SKIP */
    double f1, e2, ep2, n, b, c2, etol2;
    double A3x[6], C3x[15], C4x[21];
    /**< @endcond */
  };
  geodesic m_gd;

public:
	/// \brief 从空间参考初始化
	GsGeodesic(GsSpatialReference* sr);
	
	/// \brief 从地球半径和扁率构造
	/// \param a 赤道半径 
	/// \param f 扁率
	GsGeodesic(double a,double f);

	/// \brief 拷贝构造函数
	GsGeodesic(const GsGeodesic& rhs);
	/// \brief 赋值操作符
	GsGeodesic& operator = (const GsGeodesic& rhs);


	/// \brief 计算测地线长度、大圆距离、球面距离
	/// \param fromLongitude 开始点经度
	/// \param fromLatitude 开始点纬度度
	/// \param toLongitude 结束点经度
	/// \param toLatitude 结束点纬度度
	/// \param fromAzimuth 输出开始点的方向角
	/// \param toAzimuth 输出结束点的方向角
	/// \return 测地线长度，单位米
	double Inverse(double fromLongitude,double fromLatitude,double toLongitude,double toLatitude,double* fromAzimuth = NULL,double* toAzimuth = NULL);

	/// \brief 计算一段连续点的测地线长度
	/// \param pCoord 点坐标数组
	/// \param nLen 点坐标数组长度
	/// \param nDim 坐标维度（2或3）
	/// \return 测地线长度，单位米
	double ComputeGeodesic(const double* pCoord, int nLen, int nDim);

	
	/// \brief 根据起点经纬度坐标和方位角沿测试线前进一定长度到终点
	/// \param fromLongitude 开始点经度
	/// \param fromLatitude 开始点纬度度
	/// \param fromAzimuth 开始点方位角（以正北为0顺时针旋转的角度，单位度）
	/// \param length12 沿测地线前进的距离（单位米）
	/// \param toLongitude 输出终点的经度
	/// \param toLatitude 输出终点的纬度
	/// \param toAzimuth 输出终点的方位角
	void Direct(double fromLongitude,double fromLatitude,double fromAzimuth,double length12,
				double* toLongitude,double* toLatitude,double* toAzimuth = NULL);


	/// \brief 寻找起点终点大圆上的t对应的点
	/// \param fromLongitude 开始点经度
	/// \param fromLatitude 开始点纬度度
	/// \param toLongitude 终点经度
	/// \param toLatitude 终点维度
	/// \param t 参数0~1标示起点和终点，可以小于0或者大于1
	/// \param Longitude 插值点的经度
	/// \param Latitude 插值点的维度
	/// \param Azimuth 插值点的方向角
	void Interpolation(double fromLongitude,double fromLatitude,double toLongitude,double toLatitude,double t,
				double* Longitude,double* Latitude,double* Azimuth = NULL);


};
/// \brief 一根测地线，已经确定了起点终点的大地线
class GS_API GsGeodesicLine:public GsGeodesic
{
	struct geodesicline {
	double lat1;                /**< the starting latitude */
	double lon1;                /**< the starting longitude */
	double azi1;                /**< the starting azimuth */
	double a;                   /**< the equatorial radius */
	double f;                   /**< the flattening */
	/**< @cond SKIP */
	double b, c2, f1, salp0, calp0, k2,
		salp1, calp1, ssig1, csig1, dn1, stau1, ctau1, somg1, comg1,
		A1m1, A2m1, A3c, B11, B21, B31, A4, B41;
	double C1a[6+1], C1pa[6+1], C2a[6+1], C3a[6], C4a[6];
	/**< @endcond */
	unsigned caps;              /**< the capabilities */
	};
	geodesicline m_L;
	
public:
	 /**
   * mask values for the \e caps argument to geod_lineinit().
   **********************************************************************/
  enum GeoDMask {
    eGEOD_NONE         = 0U,                     /**< Calculate nothing */
    eGEOD_LATITUDE     = 1U<<7  | 0U,            /**< Calculate latitude */
    eGEOD_LONGITUDE    = 1U<<8  | 1U<<3,         /**< Calculate longitude */
    eGEOD_AZIMUTH      = 1U<<9  | 0U,            /**< Calculate azimuth */
    eGEOD_DISTANCE     = 1U<<10 | 1U<<0,         /**< Calculate distance */
    eGEOD_DISTANCE_IN  = 1U<<11 | 1U<<0 | 1U<<1, /**< Allow distance as input  */
    eGEOD_REDUCEDLENGTH= 1U<<12 | 1U<<0 | 1U<<2, /**< Calculate reduced length */
    eGEOD_GEODESICSCALE= 1U<<13 | 1U<<0 | 1U<<2, /**< Calculate geodesic scale */
    eGEOD_AREA         = 1U<<14 | 1U<<4,         /**< Calculate reduced length */
    eGEOD_ALL          = 0x7F80U| 0x1FU          /**< Calculate everything */
  };
public:
	/// \brief 根据空间参考，起点的经纬度和方向角确定一根测地线
	GsGeodesicLine(GsSpatialReference* sr,double fromLongitude,double fromLatitude,double fromAzimuth,GeoDMask mask);
	/// \brief 拷贝构造函数
	GsGeodesicLine(const GsGeodesicLine& rhs);
	/// \brief 赋值操作符
	GsGeodesicLine& operator = (const GsGeodesicLine& rhs);

	/// \brief 根据距离获得测地线上的点坐标
	/// \param distance 距离起点的距离，单位米
	/// \param toLatitude 终点的经度
	/// \param toLongitude 终点的纬度
	/// \param toAzimuth 终点的方位角
	void Position(double distance,double* toLatitude, double* toLongitude, double* toAzimuth = NULL);

};

/// \brief 测地线多边形，以测地线构成的多边形
class GS_API GsGeodesicPolygon:public GsGeodesic
{ 
  struct GeoDPolygon {
    double lat;                 /**< the current latitude */
    double lon;                 /**< the current longitude */
    /**< @cond SKIP */
    double lat0;
    double lon0;
    double A[2];
    double P[2];
    int polyline;
    int crossings;
    /**< @endcond */
    unsigned num;               /**< the number of points so far */
  };
  GeoDPolygon m_PG;
  bool m_bPolyline;

public:
	/// \brief 构造一个多边形或者Polyline
	GsGeodesicPolygon(GsSpatialReference* sr,bool bPolyline = false);

	/// \brief 拷贝构造
	GsGeodesicPolygon(const GsGeodesicPolygon& rhs);
	/// \brief 赋值操作符
	GsGeodesicPolygon& operator=(const GsGeodesicPolygon& rhs);
	
	/// \brief 重置恢复到缺省状态
	void Reset();


	/// \brief 增加一个边界点
	/// \param lon 经度
	/// \param lat 纬度
	void AddPoint(double lon,double lat);

	/// \brief 以方位角、长度增加一条边
	void AddEdge(double dblAzimuth,double dblLen);

	/// \brief 计算多边形的球面面积以及周长
	/// \param bReverse 是否反向，如果为true（反向）则认为逆时针多边形得到负的面积值。(标准积分面积逆时针方向的点返回正面积）
	/// \param bSign  是否带符号返回面积，如果时则在方向正确时返回正面积不正确时返回负面积， 否则将返回多边形外的地球面积。
	/// \param pArea  当构造时使用非Polyline参数则返回球面面积，否则不返回
	/// \param pPerimeter    返回多边形或者polyline的周长
	/// \return 返回多边形或者折线有多少个点
	unsigned int Compute(bool bReverse,bool bSign,double* pArea,double *pPerimeter = NULL);

};



/// \brief 空间参考管理对象
/// \details 枚举现有的空间参考，增加新空间参考等等能力
class GS_API GsSpatialReferenceManager
{
public:
	/// \brief 特殊分类
	enum GsSpecialCategory
	{
		/// \brief 标准EPSG分类
		eStandardEPSG,
		/// \brief EPSG别名分类
		eAliasOfEPSG,
	};

	/// \brief 空间参考的分类
	class GS_API GsSpatialReferenceCategory
	{
	public:
		virtual ~GsSpatialReferenceCategory();

		/// \brief 类别的名称
		virtual Utility::GsString Name() = 0;
		
		/// \brief 空间参考的数量
		virtual int Count() = 0;
		
		/// \brief根据索引获取空间参考对象
		virtual GsSpatialReferencePtr SpatialReference(int i) = 0;
	};
private:
	Utility::GsString m_DataFolder;

	Utility::GsVector<GsSpatialReferenceCategory*> m_Categorys;

public:
	/// \brief 指定特定目录构造空间参考管理对象
	GsSpatialReferenceManager(const char* strSRDataFolder = NULL);
	~GsSpatialReferenceManager();

	/// \brief 获取空间参考数据目录
	Utility::GsString  DataFolder();

	/// \brief 空间参考的分类数量
	int CategoryCount();

	/// \brief 获取空间参考的一个分类
	GsSpatialReferenceCategory*  Category(int i );

	/// \brief 获取特殊的空间参考分类
	GsSpatialReferenceCategory*  SpecialCategory(GsSpecialCategory eType );

};


/// \brief 坐标转换基类
class GS_API GsCoordinateTransformation:public Utility::GsRefObject
{
public:
	~GsCoordinateTransformation();
	/// \brief 对x数组和y数组以及Z数组分别转换
	virtual bool Transformation(double* pX,double *pY,double *pZ,int nPointCount,int nPointOff) = 0;
	virtual bool Transformation(double* pX,double *pY,int nPointCount,int nPointOff);
	/// \brief 对x，y，或者z构成的坐标数组进行转换
	virtual bool Transformation(double* pCoord,int nPointCount,int nDim);

	/// \brief 对x，y进行转换
	virtual bool Transformation(double &x,double &y);
	/// \brief 对x，y，z进行转换
	virtual bool Transformation(double &x, double &y,double& z);

	/// \brief 对x，y，或者z构成的坐标数组进行转换,转换结果输出到新的数组中
	virtual bool Transformation(const double* pCoord,int nPointCount, int nDim, double* pCoordResult);

	/// \brief 输入结果数据，获取源坐标
	virtual bool ReverseTransformation(double* pX, double *pY, double *pZ, int nPointCount, int nPointOff);

	virtual bool ReverseTransformation(const double* pCoord, int nPointCount, int nDim, double* pCoordResult);
};
/// \brief GsCoordinateTransformationPtr
GS_SMARTER_PTR(GsCoordinateTransformation);

/// \brief 投影坐标转换
class GS_API GsProjectCoordinateTransformation:public GsCoordinateTransformation
{
protected:
	void* m_pTrans;
	bool m_bParalle;
	GsSpatialReferencePtr m_ptrSource;
	GsSpatialReferencePtr m_ptrTarget;
	void CreateHandle();
public:
	using GsCoordinateTransformation::Transformation;
	GsProjectCoordinateTransformation(GsSpatialReference* pSource,GsSpatialReference* pTarget);
	~GsProjectCoordinateTransformation();

	/// \brief 对x数组和y数组以及Z数组分别转换
	virtual bool Transformation(double* pX,double *pY,double *pZ,int nPointCount,int nPointOff);
	/// \brief 投影转换源空间参考
	GsSpatialReference* Source();
	/// \brief 设置投影转换源空间参考
	void Source(GsSpatialReference* pSR);

	/// \brief 投影转换目标空间参考
	GsSpatialReference* Target();
	/// \brief 设置投影转换目标空间参考
	void Target(GsSpatialReference* pSR);
};
/// \brief GsProjectCoordinateTransformationPtr
GS_SMARTER_PTR(GsProjectCoordinateTransformation);


/// \brief 坐标转换方法
enum GsCoordinateTransformationMethod
{
	/// \brief 位置矢量法
	///\details 七参数赫尔默特（Helmert）转换法，依据布尔莎（Bursa-Wolf）公式
	///\details 由国际大地测量协会（IAG）通过ISO19111标准定义
    ///\details 与坐标框架法不同的是7参数中三个旋转参数的方向相反
	ePositionVector,

	/// \brief 坐标框架法
	///\details 七参数赫尔默特（Helmert）转换法，依据布尔莎（Bursa-Wolf）公式
	///\details 由EPSG（欧洲石油勘探组织）定义
	///\details 与位置矢量法不同的是7参数中三个旋转参数的方向相反
    eCoordinateFrame,

	/// \brief 基于地心的三参数转换法
	eGeocentricTranslation,
};

/// \brief 参数化的投影坐标转换
/// \details 定义不同投影间的转换方法和转换参数
/// \details 从一个大地经纬度坐标系变换到另一个大地经纬度坐标系一般需要经过三个环节：大地经纬度坐标系到空间直角坐标系>空间直角坐标系到空间直角坐标系>空间直角坐标系到大地经纬度坐标系
/// \details 其中的中间环节，空间直角坐标到空间直角坐标采用七参数赫尔默特（Helmert）转换法，其转换矩阵为著名的布尔沙-沃尔夫（Bursa-Wolf）公式,或者三参数等不同的转换方法
class GS_API GsParameterProjectCoordinateTransformation:public GsProjectCoordinateTransformation
{
	/// \brief 转换参数
	Utility::GsVector<double> m_vecParam;
	/// \brief 转换参数
	GsCoordinateTransformationMethod m_eMethod;
public:
	using GsCoordinateTransformation::Transformation;
	/// \brief 从转换方法和转换参数构造
	GsParameterProjectCoordinateTransformation(GsSpatialReference* pSource,GsSpatialReference* pTarget,
							GsCoordinateTransformationMethod eMethod,double* pParam);
	
	/// \brief 获取数据转换的方法
	GsCoordinateTransformationMethod Method();
	
	/// \brief 设置坐标转换的方法和参数值
	void Parameter(GsCoordinateTransformationMethod method,double* pParam);

};
/// \brief GsParameterProjectCoordinateTransformationPtr
GS_SMARTER_PTR(GsParameterProjectCoordinateTransformation);


/// \brief 仿射坐标转换
/// \details 仿射变换公式
/// \details x_new = m11 * x + m21 * y + dx;
/// \details y_new = m12 * x + m22 * y + dy;
/// \details 反射变换矩阵常规表达
/// \details [m11,m12,0]     
/// \details [m21,m22,0]	  
/// \details [dx ,dy ,1]
class GS_API GsAffineCoordinateTransformation:public GsCoordinateTransformation
{
	Utility::GsVector<double> m_Source;
	Utility::GsVector<double> m_Target;
	double m_Matrix[6];
public:
	using GsCoordinateTransformation::Transformation;
	/// \brief 根据源和目标同名点组构成仿射变换参数
	/// \details 最低需要3个点的坐标,即nLen为6
	GsAffineCoordinateTransformation(const double* pSourcePoint,const double* pTargetPoint,int nLen =  6);

	/// \brief 根据访射变换矩阵构造
	/// \details 访射变换矩阵为6个double 每个元素对应矩阵元素：[m11][m12][m21][m22][dx][dy]
	GsAffineCoordinateTransformation(const double* pMatirx);

	/// \brief 根据平移参数构造
	GsAffineCoordinateTransformation(double dx, double dy);

	/// \brief 根据旋转点和旋转参数构造
	/// \param ox,oy为旋转中心点d的坐标值
	/// \param rotationAngle为旋转角,单位角度
	GsAffineCoordinateTransformation(double ox,double oy,double rotationAngle);

	/// \brief 设置同名点参数
	bool Parameter(const double* pSourcePoint,const double* pTargetPoint,int nLen =  6);
	
	/// \brief 获取仿射变换矩阵的值。
	/// \param m 用于输出矩阵值得数组，长度6
	/// \details 访射变换矩阵为6个double 每个元素的意义如下：[m11][m12][m21][m22][dx][dy]
	bool Elements(double* m);

	/// \brief 获取访射变换矩阵指针
	/// \details 访射变换矩阵为6个double 每个元素的意义如下：[m11][m12][m21][m22][dx][dy]
	const double* Matrix()const ;

	/// \brief 同名点数量
	int IdenticalPointCount()const ;

	/// \brief 源的同名点坐标
	const double* SourcePoint()const ;
	/// \brief 目标的同名点坐标
	const double* TargetPoint()const ;
	/// \brief 对x数组和y数组以及Z数组分别转换
	virtual bool Transformation(double* pX,double *pY,double *pZ,int nPointCount,int nPointOff);

};
/// \brief GsAffineCoordinateTransformationPtr
GS_SMARTER_PTR(GsAffineCoordinateTransformation);

/// \brief 从WGS84坐标系转GCJ02(火星）坐标系。
/// \details https://en.wikipedia.org/wiki/Restrictions_on_geographic_data_in_China
class GS_API GsWGS84ToGCJ02CoordinateTransformation:public GsCoordinateTransformation
{
public:
	using GsCoordinateTransformation::Transformation;
	/// \brief 对x数组和y数组以及Z数组分别转换
	virtual bool Transformation(double* pX,double *pY,double *pZ,int nPointCount,int nPointOff);
};
/// \brief GsWGS84ToGCJ02CoordinateTransformationPtr
GS_SMARTER_PTR(GsWGS84ToGCJ02CoordinateTransformation);


/// \brief 转从GCJ02(火星）坐标系反转为WGS84
/// \details https://en.wikipedia.org/wiki/Restrictions_on_geographic_data_in_China
class GS_API GsGCJ02ToWGS84CoordinateTransformation:public GsCoordinateTransformation
{
protected:
	double m_dblPrecision; 
public:
	using GsCoordinateTransformation::Transformation;
	/// \brief 从转换精度构造
	/// \param dblPrecision 反转的精确度，单位米
	GsGCJ02ToWGS84CoordinateTransformation(double dblPrecision=0.001);

	/// \brief 对x数组和y数组以及Z数组分别转换
	virtual bool Transformation(double* pX,double *pY,double *pZ,int nPointCount,int nPointOff);

	/// \brief 获取反算精度
	virtual double Precision();
	/// \brief 设置反算精度
	virtual void Precision(double val);
	  
};

/// \brief GsGCJ02ToWGS84CoordinateTransformationPtr
GS_SMARTER_PTR(GsGCJ02ToWGS84CoordinateTransformation);

/// \brief 从WGS84坐标系转BD09(百度）坐标系。
/// \details https://en.wikipedia.org/wiki/Restrictions_on_geographic_data_in_China
class GS_API GsWGS84ToBD09CoordinateTransformation:public GsWGS84ToGCJ02CoordinateTransformation
{
public:
	using GsCoordinateTransformation::Transformation;
	/// \brief 对x数组和y数组以及Z数组分别转换
	virtual bool Transformation(double* pX,double *pY,double *pZ,int nPointCount,int nPointOff);
};
/// \brief GsWGS84ToGCJ02CoordinateTransformationPtr
GS_SMARTER_PTR(GsWGS84ToBD09CoordinateTransformation);


/// \brief 转从BD09(百度）坐标系反转为WGS84
/// \details https://en.wikipedia.org/wiki/Restrictions_on_geographic_data_in_China
class GS_API GsBD09ToWGS84CoordinateTransformation:public GsGCJ02ToWGS84CoordinateTransformation
{
	
public:
	using GsCoordinateTransformation::Transformation;
	/// \brief 从转换精度构造
	/// \param dblPrecision 反转的精确度，单位米
	GsBD09ToWGS84CoordinateTransformation(double dblPrecision);

	/// \brief 对x数组和y数组以及Z数组分别转换
	virtual bool Transformation(double* pX,double *pY,double *pZ,int nPointCount,int nPointOff);
};
/// \brief GsBD09ToWGS84CoordinateTransformationPtr
GS_SMARTER_PTR(GsBD09ToWGS84CoordinateTransformation);


/// \brief 坐标精度转换
class GS_API GsCoordinatePrecision :public Utility::GsRefObject
{
public:
	virtual ~GsCoordinatePrecision() {}

	/// \brief 计算输入坐标附近此距离精度下几何坐标距离
	/// \details 以经纬度坐标为例，当距离精度为0.1米时，如果y坐标为赤道坐标（0度）则几何坐标距离为8.9e-6度
	/// \details 如果y坐标为(45度）则几何坐标距离为1.7e-5度
	virtual double PrecisionToCoordinatePrecision(double x, double y, double dblPrecision) = 0;
};

/// \brief GsCoordinatePrecisionPtr
GS_SMARTER_PTR(GsCoordinatePrecision);

/// \brief 非线性变换或数学上无法直接可逆的逆坐标转换
/// \details 单向坐标转换方向上为非线性坐标转换无法简单通过仿射变换进行逆转换
/// \details 单向坐标转换公式为不可逆算法如果采用仿射变换会存在大量同名点计算仿射变换参数导致逆转换性能低
/// \details 本转换通过指定正向转换的算法和距离计算算法实现快速收敛，保证逆转换性能高并且精度也能达到预期。
/// \details 本转换适合单向转换公式确定并且精度确定的情况，如果双向坐标转换都无法精确确定则不适用本转换对象
class GS_API GsConverseCoordinateTransformation :public GsCoordinateTransformation
{ 
	GsCoordinateTransformationPtr m_ptrForwardTrans;
	GsCoordinatePrecisionPtr		 m_ptrCoordPreTrans;
	double						m_dblPrecision;
public:

	using GsCoordinateTransformation::Transformation;
	/// \brief 从转换精度构造
	/// \param pForwardTrans 正向转换算法
	/// \param dblPrecision 转换距离精度，单位由转换算法本身决定。
	/// \param pCoordPreTrans 实现距离精度到坐标几何距离的转换
	GsConverseCoordinateTransformation(GsCoordinateTransformation* pForwardTrans, 
				GsCoordinatePrecision* pCoordPreTrans,double dblPrecision);

	virtual ~GsConverseCoordinateTransformation();

	/// \brief 对x数组和y数组以及Z数组分别转换
	virtual bool Transformation(double* pX, double *pY, double *pZ, int nPointCount, int nPointOff);

	/// \brief 获取反算距离精度
	virtual double Precision();
	/// \brief 设置反算距离精度
	virtual void Precision(double val);
};

/// \brief GsConverseCoordinateTransformationPtr
GS_SMARTER_PTR(GsConverseCoordinateTransformation);

enum GsParameterType
{
	Bursa7Parameter,
	Plane4Parameter,
	Molodensky7Parameter,
	Cube4Parameter,
};

/// \brief 坐标转换类厂
/// \brief 源坐标系与坐标系之间不得跨坐标系
/// \brief 源坐标系为北京54系,则目标坐标系可以为80系,但不能为2000系
/// \brief 例如:Beijing54Ellipsoid<===>Xian80Ellipsoid或者Xian80Ellipsoid<===>China2000Ellipsoid
/// \details Create......Transformation创建坐标转换对象
/// \details Compute.....Parameter计算转换参数
class GS_API GsCoordinateTransformationFactory
{
public:
	/// \brief 从源和目标的空间参考生成投影坐标转换对象
	/// \param pSource源空间参考
	/// \param pTarget目标空间参考
	static GsCoordinateTransformationPtr CreateProjectCoordinateTransformation(GsSpatialReference* pSource, GsSpatialReference* pTarget);

	/// \brief 生成参数化的投影坐标转换对象
	/// \param pSource源空间参考
    /// \param pTarget目标空间参考
	/// \param eMethod坐标转换方法,
	/// \param pParam坐标转换参数
	static GsCoordinateTransformationPtr CreateParameterProjectCoordinateTransformation(GsSpatialReference* pSource, GsSpatialReference* pTarget,
		GsCoordinateTransformationMethod eMethod, double* pParam);

	/// \brief 生成仿射坐标转换对象
	/// \param pSourcePoint源同名点数组
	/// \param pTargetPoint目标同名点数组
	/// \param nLen源和目标同名点数组长度,最低需要3个点的坐标,即nLen为6
	static GsCoordinateTransformationPtr CreateAffineCoordinateTransformation(double* pSourcePoint, double* pTargetPoint, int nLen);

	/// \brief 生成WGS84到GCJ02坐标转换对象
	static GsCoordinateTransformationPtr CreateWGS84ToGCJ02CoordinateTransformation();

	/// \brief 生成GCJ02到WGS84坐标转换对象
	/// \param dblPrecision 反转的精确度，单位米
	static GsCoordinateTransformationPtr CreateGCJ02ToWGS84CoordinateTransformation(double dblPrecision);

	/// \brief 生成WGS84到BD09坐标转换对象
	static GsCoordinateTransformationPtr CreateWGS84ToBD09CoordinateTransformation();

	/// \brief 生成BD09到WGS84坐标转换对象
	/// \param dblPrecision 反转的精确度，单位米
	static GsCoordinateTransformationPtr CreateBD09ToWGS84CoordinateTransformation(double dblPrecision);

	/// \brief   生成非线性变换或数学上无法直接可逆的逆坐标转换对象
	/// \details 单向坐标转换方向上为非线性坐标转换无法简单通过仿射变换进行逆转换
	/// \details 单向坐标转换公式为不可逆算法如果采用仿射变换会存在大量同名点计算仿射变换参数导致逆转换性能低
	/// \details 本转换通过指定正向转换的算法和距离计算算法实现快速收敛，保证逆转换性能高并且精度也能达到预期。
	/// \details 本转换适合单向转换公式确定并且精度确定的情况，如果双向坐标转换都无法精确确定则不适用本转换对象
	static GsCoordinateTransformationPtr CreateConverseCoordinateTransformation(GsCoordinateTransformation* pForwardTrans,
		GsCoordinatePrecision* pCoordPreTrans, double dblPrecision);

	/// \brief 生成大地坐标到地心坐标转换对象
	/// \param CoordinateSystem坐标数据所在的空间参考
	static GsCoordinateTransformationPtr CreateGeodeticToGeocentricCoordinateTransformation(GsSpatialReference* CoordinateSystem);

	/// \brief 生成地心坐标到大地坐标转换对象
	/// \param CoordinateSystem坐标数据所在的空间参考
	static GsCoordinateTransformationPtr CreateGeocentricToGeodeticCoordinateTransformation(GsSpatialReference* CoordinateSystem);


	/// \brief  计算二维4参数
	/// \param nPointCount为同名点个数
	/// \param nDim为点坐标维度
	/// \param SourcePoint为数组,存入源坐标系的同名点坐标值(x1,y1,x2,y2....)
	/// \param TargetPoint为数组,存入目标坐标系的同名点坐标值(x1,y1,x2,y2....)
	/// \param Param为长度为4的数组,依次存入2个平移参数x0,y0,单位为米;一个旋转参数an,单位为秒;一个尺度参数,单位为ppm
	/// \param Mp为计算得中误差残差值,按照规范Mp值不超过比例尺分母的万分之一(0.0001*MapScale)
	static void ComputePlane4Parameter(int nPointCount, int nDim,const double* SourcePoint,
		const double* TargetPoint, double* Param, double& Mp);
	static double ComputePlane4Parameter(int nPointCount, int nDim, const double* SourcePoint,
		const double* TargetPoint, double* Param);
	/// \brief 生成二维4参数模型坐标转换对象
	/// \param P4Param为长度为4的数组，依次存入二维4参数(包括2个平移参数x0,y0,单位为米;一个旋转参数an,单位为秒;一个尺度参数,单位为ppm)
	static GsCoordinateTransformationPtr CreatePlane4PatameterCoordinateTransformation(const double* P4Param);

	/// \brief  计算三维4参数
	/// \param B0为所在区域中心点的纬度值
	/// \param L0为所在区域中心点的经度值
	/// \param nPointCount为同名点个数
	/// \param nDim为点坐标维度
	/// \param SourcePoint为数组,存入源坐标系的同名点坐标值(x1,y1,z1,x2,y2,z2....)
	/// \param TargetPoint为数组,存入目标坐标系的同名点坐标值(x1,y1,z1,x2,y2,z2....)
	/// \param Param为长度为4的数组,依次存入3个平移参数x0,y0,z0,单位为米;一个旋转参数an,单位为秒
	/// \param Mp为计算得中误差残差值,按照规范Mp值不超过比例尺分母的万分之一(0.0001*MapScale)
	static void ComputeCube4Parameter(GsSpatialReference* pSource, GsSpatialReference* pTarget, double B0, double L0,
		int nPointCount, int nDim, const double* SourcePoint, const double* TargetPoint, double* Param, double& Mp);
	static double ComputeCube4Parameter(GsSpatialReference* pSource, GsSpatialReference* pTarget, double B0, double L0,
		int nPointCount, int nDim, const double* SourcePoint, const double* TargetPoint, double* Param);

	/// \brief 生成三维4参数模型坐标转换对象
	/// \param B0为所在区域中心点的纬度值
	/// \param L0为所在区域中心点的经度值
	/// \param P4Param为长度为4的数组，依次存入二维4参数(包括3个平移参数x0,y0,z0,单位为米;一个旋转参数an,单位为秒)
	static GsCoordinateTransformationPtr CreateCube4PatameterCoordinateTransformation(GsSpatialReference* pSource,
		GsSpatialReference* pTarget, double B0,double L0,const double* P4Param);

	/// \brief 创建平面拟合的坐标转换对象
	/// \param nPoint为同名点个数
	/// \param nDim为点坐标维度
	/// \param SourcePoint为数组,存入源坐标系的同名点坐标值(x1,y1,x2,y2....)
	/// \param TargetPoint为数组,存入目标坐标系的同名点坐标值(x1,y1,x2,y2....)
	static GsCoordinateTransformationPtr CreatePlaneFittingCoordinateTransformation(int nPoint,int nDim,
		const double* SourcePoint,const double* TargetPoint);

	/// \brief 创建椭球面拟合的坐标的转换对象
	/// \param nPoint为同名点个数
	/// \param nDim为点坐标维度
	/// \param SourcePoint为数组,存入源坐标系的同名点坐标
	/// \param TargetPoint为数组,存入目标坐标系的同名点坐标
	static GsCoordinateTransformationPtr CreateGeoidFittingCoordinateTransformation(GsSpatialReference* pSource, GsSpatialReference* pTarget,
		int nPoint, int nDim, const double* SourcePoint, const double* TargetPoint);

	/// \brief 计算二维7参数
	/// \param nPointCount为同名点个数
	/// \param nDim为点坐标维度
	/// \param pSource,pTarget源和目标的空间参考
	/// \param SourcePoint为数组,存入源坐标系的同名点坐标
	/// \param TargetPoint为数组,存入目标坐标系的同名点坐标
	/// \param Param为长度为7的数组,依次存入3个平移参数dx,dy,dz,单位为米,3个旋转参数anx,any,anz,单位为秒;一个尺度参数,单位为ppm
	/// \param Mp为计算得中误差残差值,按照规范Mp值不超过比例尺分母的万分之一(0.0001*MapScale)
	static void ComputePlane7Parameter(GsSpatialReference* pSource, GsSpatialReference* pTarget,int nPointCount, int nDim, 
		 const double* SourcePoint, const double* TargetPoint, double* Param, double& Mp);
	static double ComputePlane7Parameter(GsSpatialReference* pSource, GsSpatialReference* pTarget, int nPointCount, int nDim,
		const double* SourcePoint, const double* TargetPoint, double* Param);

	/// \brief 生成二维7参数模型坐标转换对象
	/// \param P7Param为长度为7的数组,依次存入二维7参数(3个平移参数dx,dy,dz,单位为米,3个旋转参数anx,any,anz,单位为秒;一个尺度参数,单位为ppm)
	/// \param pSource源空间参考
	/// \param pTarget目标空间参考
	static GsCoordinateTransformationPtr CreatePlane7PatameterCoordinateTransformation
	(GsSpatialReference* pSource, GsSpatialReference* pTarget,const double* P7Param);

	/// \brief 计算三维7参数
	/// \param nPointCount为同名点个数
	/// \param nDim为点坐标维度
	/// \param pSource,pTarget源和目标的空间参考
	/// \param SourcePoint为数组,存入源坐标系的同名点坐标
	/// \param TargetPoint为数组,存入目标坐标系的同名点坐标
	/// \param Param为长度为7的数组,依次存入3个平移参数dx,dy,dz,单位为米,3个旋转参数anx,any,anz,单位为秒;一个尺度参数,单位为ppm
	/// \param Mp为计算得中误差残差值,按照规范Mp值不超过比例尺分母的万分之一(0.0001*MapScale)
	static void ComputePlane7Parameter3D(GsSpatialReference* Source, GsSpatialReference* Target,int nPointCount, int nDim, 
		const double* SourcePoint, const double* TargetPoint, double* Param, double& Mp);
	static double ComputePlane7Parameter3D(GsSpatialReference* Source, GsSpatialReference* Target, int nPointCount, int nDim,
		const double* SourcePoint, const double* TargetPoint, double* Param);

	/// \brief 生成三维7参数模型坐标转换对象
	/// \param P7Param为长度为7的数组,依次存入二维7参数(3个平移参数dx,dy,dz,单位为米,3个旋转参数anx,any,anz,单位为秒;一个尺度参数,单位为ppm)
	/// \param pSource源空间参考
	/// \param pTarget目标空间参考
	static GsCoordinateTransformationPtr Create3DPlane7PatameterCoordinateTransformation(GsSpatialReference* pSource, 
		GsSpatialReference* pTarget,const double* P7Param);

	/// \brief 计算Bursa7参数
	/// \param nPointCount为同名点个数
	/// \param nDim为点坐标维度
	/// \param SourcePoint为数组,存入源坐标系的同名点坐标
	/// \param TargetPoint为数组,存入目标坐标系的同名点坐标 
	/// \param Param为长度为7的数组,依次为3个平移参数dx,dy,dz,单位为米,3个旋转参数anx,any,anz,单位为秒;一个尺度参数,单位为ppm
	/// \param Mp为计算得中误差残差值,按照规范Mp值不超过比例尺分母的万分之一(0.0001*MapScale)
	static void ComputeBursa7Parameter(GsSpatialReference* Source, GsSpatialReference* Target, int nPointCount, int nDim, 
		const double* SourcePoint,const double* TargetPoint, double* Param, double& Mp);
	static double ComputeBursa7Parameter(GsSpatialReference* Source, GsSpatialReference* Target, int nPointCount, int nDim,
		const double* SourcePoint, const double* TargetPoint, double* Param);

	/// \brief 使用Bursa7参数模型生成坐标转换对象
	/// \param P7Param为长度为7的数组,依次存入Bursa7参数(3个平移参数dx,dy,dz,单位为米,3个旋转参数anx,any,anz,单位为秒;一个尺度参数,单位为ppm)
	/// \param pSource源空间参考
	/// \param pTarget目标空间参考
	static GsCoordinateTransformationPtr CreateBursa7CoordinateTransformation(GsSpatialReference* pSource,
		GsSpatialReference* pTarget,const double* P7Param);

	/// \brief 计算Molodensky7参数
	/// \param nPointCount为同名点个数
	/// \param nDim为点坐标维度
	/// \param SourcePoint为数组,存入源坐标系的同名点坐标值
	/// \param TargetPoint为数组,存入目标坐标系的同名点坐标值
	/// \param Param为长度为7的数组,依次为3个平移参数dx,dy,dz,单位为米,3个旋转参数anx,any,anz,单位为秒;一个尺度参数,单位为ppm
	/// \param Mp为计算得中误差残差值,按照规范Mp值不超过比例尺分母的万分之一(0.0001*MapScale)
	/// \param P为过渡点的三维坐标点(x,y,z)，传空则默认为地心原点坐标(0,0,0)
	static void ComputeMolodenskyParameter(GsSpatialReference* Source, GsSpatialReference* Target, int nPointCount, int nDim, 
		const double* SourcePoint,const double* TargetPoint, double* Param, double& Mp,const double* P=0);
	static double ComputeMolodenskyParameter(GsSpatialReference* Source, GsSpatialReference* Target, int nPointCount, int nDim,
		const double* SourcePoint, const double* TargetPoint, double* Param,  const double* P = 0);

	/// \brief 使用Molodensky参数模型生成坐标转换对象
	/// \param P7Param为长度为7的数组,依次存入Bursa7参数(3个平移参数dx,dy,dz,单位为米,3个旋转参数anx,any,anz,单位为秒;一个尺度参数,单位为ppm)
	/// \param P0为过渡点的三维坐标点(x,y,z)，传空则默认为地心原点坐标(0,0,0)
	static GsCoordinateTransformationPtr CreateMolodenskyCoordinateTransformation(GsSpatialReference* pSource, GsSpatialReference* pTarget,
		const double* P7Param,const double* P0);

	/// \brief 计算网格坐标改正值，使用移动坐标转换法，改正量 反距离加权平均
	/// \param pGridCoord坐标网格坐标，数量
	/// \param range移动搜索半径
	/// \param pCoord控制点坐标 LB
	/// \param nPointCount 控制点数
	/// \param Offset 与控制点对应的坐标改正两
	static void ComputeGridCoordinatesOffset(const double * pGridCoord, int nGridPointCount, double range,
		double * pCoord, int nPointCount, double * pOffset, double * pResOffset);

	/// \brief 生成基于图幅内插方法的大地坐标转换对象
	/// \param BL为长度为8的数组，依次存入图幅西北，东北，西南，东南4个角点的经纬度值
	/// \param Offset为长度为8的数组，经计算返回与BL数组对应的经纬度改正值
	static GsCoordinateTransformationPtr CreateGraphsInterpolationCoordinateTransformation(GsSpatialReference* pSource, GsSpatialReference* pTarget, 
		const double* BL, const double* Offset, int nGridPointCount = 4);

	static GsCoordinateTransformationPtr CreateGraphsInterpolationCoordinateTransformation(const double* BL, const double* Offset, int nGridPointCount = 4);

	/// \brief 计算网格坐标改正值，使用移动坐标转换法，改正量 反距离加权平均
	/// \param pSource,pTarget源和目标的空间参考
	/// \param pGridCoord坐标网格点坐标
	/// \param nPointCount坐标网格点数量
	/// \param range移动搜索半径
	/// \param pCoord控制点坐标 
	/// \param nPointCount 控制点数
	/// \param Offset 与控制点对应的坐标改正量
	static GsCoordinateTransformationPtr CreateGraphsInterpolationCoordinateTransformation(
		GsSpatialReference* pSource, GsSpatialReference* pTarget,
		const double * pGridCoord, int nGridPointCount, double range,
		double * pCoord, int nPointCount, double * pOffset);

	/// \brief 反解参数
	/// \param paramtype 参数类型
	/// \param param为数组,参数值
	/// \param result为数组,存入反解的参数值
	/// \param otherparam为数组，存入反解依赖的其他参数，如反解三维4参数时，依次存入区域中心点的B0,L0
	static void ComputeReverseParameter(GsParameterType paramtype,const double* param, double* result,const double* otherparam);


	/// \brief 计算大地坐标改正值
	/// \param pSource,pTarget源和目标的空间参考
	/// \param pParam坐标转换的七参数
	/// \param BL为待转换的经纬度坐标
	/// \param nPointCount坐标个数
	/// \param Offset 经计算返回与BL数组对应的经纬度改正值
	static void ComputeGeodeticCoordinatesOffset(GsSpatialReference* Source, GsSpatialReference* Target, 
		const double* pParam, double* BL, int nPointCount,double* Offset);
};
KERNEL_ENDNS



