#pragma once
#include "../utility/preconfig.h"
#include <object.h> 
#include "geometry.h"
#include "spatialreference.h"
#include "pyramid.h"
#include "weburiparser.h"
#include <dataio.h>
#include <configuration.h>
KERNEL_NS 
	
//前向声明
class GsGeoDatabase;
class GsFeatureClass; 
class GsRasterClass; 
class GsRowClass; 
class GsDataRoomFolder; 

/// \brief 数据源类型
enum GsDataSourceType
{
	/// \brief 未知数据源类型
	eUnknownDataSource,
	/// \brief Sqlite文件数据源
	eSqliteFile,
	/// \brief ESRI Shape文件数据源
	eShapeFile,
	/// \brief OGC GeoPakcage数据源
	eGeoPackage,
	/// \brief OracleSpatial数据源
	eOracleSpatial,
	/// \brief 标准OracleSpatial数据源
	eOracleSpatial2,
	/// \brief MySQL数据源
	eMySQL,
	/// \brief PostgreSQL数据源
	ePostgreSQL,
	/// \brief 所有文件类型的数据源
	eFile,

	/// \brief 达梦数据源
	eDameng,
	/// \brief web数据源
	eWeb,

	/// \brief RVDB 数据源
	eRvdb,
	/// \brief OGR数据源
	eOGR,
	/// \brief GDB数据源
	eGDB
};

/// \brief 空间数据库支持的能力
enum GsGeoDatabaseCapbility
{
	/// \brief 数据库是否支持事务
	eSupportTransaction,

	/// \brief 数据库是否支持矢量地物类的存储
	eSupportStoreFeatureClass,

	/// \brief 数据库是否支持普通二维表的存储
	eSupportStoreRowClass,

	/// \brief 数据库是否支持瓦片类的存储
	eSupportStoreTileClass,

	/// \brief 数据库是数据集目录
	eSupportDataRoomFolder,

	/// \brief 数据库是数据集目录多级递归
	/// \details 数据集目录下是否还可以支持数据集目录
	eSupportRecursiveDataRoomFolder,

	/// \brief 数据源是否支持栅格数据存储
	eSupportStoreRasterClass,

	/// \brief 数据源是否支持镶嵌栅格数据存储
	eSupportStoreMosaicRasterClass,
};

/// \brief 空间数据库查询条件类型
enum GsSpatialFilterType
{
	/// \brief 未知类型，表示非空间查询
	eSpatialUnKnown,

	/// \brief 外接矩形相交
	eSpatialEnvelopeIntersects,

	/// \brief 相交
    eSpatialIntersects,

	/// \brief 相等
    eSpatialEqual,

	/// \brief 被包含
    eSpatialWithin,

	/// \brief 包含
    eSpatialContains,

	/// \brief 相离
    eSpatialDisjoin,

	/// \brief 部分相交
    eSpatialOverlap,

	/// \brief 相接
    eSpatialTouch,

	/// \brief 穿越
    eSpatialCross,
};

////\class GsConnectProperty
///\brief 数据库连接信息
///\details 定义数据库连接的各类信息以及数据库类型等
struct GS_API GsConnectProperty
{
	///\brief 数据库连接信息版本
	int						Version;
	///\brief 数据服务地址或者文件路径
	Utility::GsString				Server;
	///\brief 数据库名称
	Utility::GsString				Database;
	///\brief 用户名
	Utility::GsString				User;
	///\brief 密码
	Utility::GsString				Password;
	///\brief 端口号
	int						Port;
	///\brief 数据源类型
	GsDataSourceType		DataSourceType;

	 
	///\brief 缺省构造函数
	GsConnectProperty();
	/// \brief 拷贝构造函数
	///\param other 需要拷贝的对象
    GsConnectProperty(const GsConnectProperty& other);
	
	/// \brief 根据服务地址、文件路径、url地址构造
	///\param strServer 服务、文件、url地址
    GsConnectProperty(const char* strServer);
	/// \brief 根据服务地址、文件路径、url地址和数据库名称构造
	///\param strServer 服务、文件、url地址
	///\param strDatabase 数据库名称
    GsConnectProperty(const char* strServer,const char* strDatabase);
	/// \brief 根据服务器地址、数据库和用户名密码构造
	///\param strServer 服务、文件、url地址
	///\param strDatabase 数据库名称
	///\param strUser 用户名
	///\param strPwd 密码
    GsConnectProperty(const char* strServer,const char* strDatabase,const char* strUser,const char* strPwd);
	/// \brief 根据服务器地址、数据库和用户名密码、端口构造
	///\param strServer 服务、文件、url地址
	///\param strDatabase 数据库名称
	///\param strUser 用户名
	///\param strPwd 密码
	///\param nPort 端口
	GsConnectProperty(const char* strServer,const char* strDatabase,const char* strUser,const char* strPwd,int nPort);
	/// \brief 根据服务器地址、数据库和用户名密码、端口、数据源类型构造
	///\param strServer 服务、文件、url地址
	///\param strDatabase 数据库名称
	///\param strUser 用户名
	///\param strPwd 密码
	///\param nPort 端口
	///\param sourceType 数据源类型
    GsConnectProperty(const char* strServer,const char* strDatabase,const char* strUser,const char* strPwd,int nPort,GsDataSourceType sourceType);
};

/// \brief 数据集对象类型
enum GsFeatureType
{
	//\brief 矢量地物类
	eSimpleFeature,
	//\brief 注记类
	eAnnotationFeature,


	/// \brief 影像瓦片
	eImageTileFeature = 8,
	/// \brief 地形瓦片
	eTerrainTileFeature = 9,
	
	/// \brief 模型
	eModeTileFeature = 10,

	/// \brief 静态矢量
	ePrevectorTileFeature = 11,
	/// \brief 动态矢量
	eDynvectorTileFeature = 12,
	/// \brief 地名
	ePlaceNameTileFeature = 13,
	/// \brief 地势
	eHypsographyTileFeature = 14,
	/// \brief 矢量瓦片
	eDlgTileFeature = 23,	
	
	/// \brief 实现多时相瓦片数据集将级扩充的
	eTemporalImageTileFeature = 1000,
	eTemporalTerrainTileFeature = 1001,
	eTemporalModleDsTileFeature = 1002,
	eTemporalPreRaserVectorTileFeature = 1003,
	eTemporalDynRaserVectorTileFeature = 1004,
	eTemporalPlaceNameDsTileFeature = 1005,
	eTemporalColourHypsographyMapTileFeature = 1006,	
};

//! \brief 瓦片类型，即瓦片文件后缀名
enum GsTileEncodingType
{
	//! \brief jpg
	eJpgType = 0,

	//! \brief 地形压缩文件
	eZ7Type = 1,

	//! \brief 自定义三维模型打包文件
	eX3dType = 2,

	//! \brief png
	ePngType = 3,

	//! \brief 自定义地名打包文件
	ePlnType = 4,

	//! \brief bmp
	eBmpType = 5,

	//! \brief dds
	eDdsType = 6,

	//! \brief gif
	eGifType = 7,

	//! \brief tiff
	eTiffType = 8,

	//! \brief zlib地形压缩文件
	eZLibType = 9,

	//! \brief Kmz格式
	eKmzType = 10,

	//! \brief protobuf格式
	eProtobuffType = 11,

	//! \brief 以GZip格式压缩后protobuf格式
	eGZipProtobuffType = 12,

	//! \brief texture of model
	eModTexType = 99,

	//! \brief 未知类型
	eUnKnownType = 100
} ; 
 
/// \brief 数据集对象类型
enum GsDataRoomType
{
	/// \brief 矢量地物类
	eFeatureClass,
	/// \brief 瓦片类
	eTileClass,
	/// \brief 二维表
	eRowClass,
	/// \brief 数据集的集合
	eDataRoomFolder,
	/// \brief 栅格类 
	eRasterClass,
};

/// \brief 选择集操作类型
enum GsSelectionSetOperation
{
	/// \brief 选择集求并
	eSelectionSetUnion,
	/// \brief 选择集求交
	eSelectionSetIntersection,
	/// \brief 选择集求差
    eSelectionSetDifference,
	/// \brief 选择集求对称差
    eSelectionSetSymDifference,
};


/// \brief 数据集扩展数据类型
enum GsDataRoomExtensionDataType
{
	/// \brief 扩展数据集合
	eExtensionDataCollection,
	/// \brief 地名数据式样
	ePlaceNameStyle,
	/// \brief 矢量数据式样
	eVectorTileStyle,

};

/// \brief 数据集扩展数据
class GS_API GsDataRoomExtensionData :public Utility::GsRefObject
{
protected:
	GsDataRoomExtensionDataType m_eType;
	GsDataRoomExtensionData(GsDataRoomExtensionDataType eType);
public:
	virtual ~GsDataRoomExtensionData();
	GsDataRoomExtensionDataType Type();
};
/// \brief  GsDataRoomExtensionDataPtr
GS_SMARTER_PTR(GsDataRoomExtensionData);

/// \brief  地名式样扩展数据
class GS_API GsPlaceNameStyleExtensionData :public GsDataRoomExtensionData
{
protected:
	GsPlaceNameStyleExtensionData();
public:
	virtual ~GsPlaceNameStyleExtensionData();

	/// \brief  根据图标的ID获取图标数据
	virtual bool Icon(long long ID, Utility::GsByteBuffer* buff) = 0;
};
/// \brief  GsPlaceNameStyleExtensionDataPtr
GS_SMARTER_PTR(GsPlaceNameStyleExtensionData);


/// \brief  矢量瓦片式样数据
class GS_API GsVectorTileStyleExtensionData:public GsDataRoomExtensionData
{
protected:
	GsVectorTileStyleExtensionData();
public:
	virtual ~GsVectorTileStyleExtensionData();
	
	/// \brief  式样文本数据
	virtual Utility::GsString Style() = 0;

	/// \brief  所有字体的名称
	virtual UTILITY_NAME::GsVector<Utility::GsString> GlyphNames() = 0;

	/// \brief  获取某个字体PBF文件
	/// \param fontname 从GlyphNames获取的名称
	/// \param nIndex 字体PBF文件的索引，0~255
	/// \param buff 存储字体数据的缓冲区
	virtual bool GlyphContent(const char* fontName, int nIndex, Utility::GsByteBuffer* buff) = 0;

	/// \brief  符号描述内容
	/// \param i 1或者2,1表示sprite.json，2表示sprite@2x.json
	virtual Utility::GsString  Sprites(int i) = 0;

	/// \brief  符号描述内容
	/// \param i 1或者2,1表示sprite.json，2表示sprite@2x.json
	/// \param buff 存储符号内容
	virtual bool  SpritesContent(int i, Utility::GsByteBuffer* buff) = 0;

};
/// \brief  GsVectorTileStyleExtensionDataPtr
GS_SMARTER_PTR(GsVectorTileStyleExtensionData);

/// \brief 数据集扩展数据集合
class GS_API GsDataRoomExtensionDataCollection:public GsDataRoomExtensionData
{
	Utility::GsList<GsDataRoomExtensionDataPtr> m_listSub;
public:
	GsDataRoomExtensionDataCollection();
	~GsDataRoomExtensionDataCollection();
	/// \brief 获取存储的集合
	Utility::GsList<GsDataRoomExtensionDataPtr>& List();
};
/// \brief  GsDataRoomExtensionDataCollectionPtr
GS_SMARTER_PTR(GsDataRoomExtensionDataCollection);

/// \brief 数据集对象基类
class GS_API GsDataRoom:public Utility::GsRefObject
{
protected:
	Utility::GsSharedPtr<Utility::GsRWLock>   m_ptrLocker;
	Utility::GsSmarterPtr<GsGeoDatabase> m_ptrGDB;
	Utility::GsString m_strName;
	Utility::GsSmarterPtr<GsDataRoomFolder> m_ptrFolder;
	GsDataRoom();
public:
	virtual ~GsDataRoom();
	
	/// \brief 获取事务对象，如果不支持事务则返回NULL
	virtual Utility::Data::GsTransaction* Transaction();

	/// \brief 获取数据集的扩展对象
	virtual GsDataRoomExtensionData* ExtensionData();

	/// \brief 获取保证多线程安全的线程锁
	Utility::GsRWLock* Lock();

	/// \brief 打断正在运行中的长时间操作
	/// \return 不支持打断操作的能力则返回false，成功打断或者无须打断都返回true
	virtual bool Interrupt();


	/// \brief 数据集所在的目录
	///\return 返回数据集所在的目录，如果为空则标识数据集在GeoDatabase下
	virtual GsDataRoomFolder* Folder();


	/// \brief 数据集名称
	///\return 返回数据集对象的名称
	virtual Utility::GsString Name();
	/// \brief 数据集类型
	///\return 返回数据集对象的类型
	virtual GsDataRoomType Type() = 0;

	/// \brief 数据集所在的数据库对象
	///\return 返回数据库对象指针
	virtual GsGeoDatabase* GeoDatabase();

	/// \brief 删除
	///\return 返回是否删除成功
	virtual bool Delete() = 0;

	/// \brief 元数据的分类名称
	virtual UTILITY_NAME::GsVector<Utility::GsString> MetadataDomain();
	
	/// \brief 获取元数据分类下的元数据名称
	virtual UTILITY_NAME::GsVector<Utility::GsString> MetadataName(const char* strDomainName);

	/// \brief 获取某个分类下的元数据值
	virtual Utility::GsString MetadataItem(const char* strDomainName,const char* strName);

	/// \brief 设置某个分类下的元数据值
	virtual void MetadataItem(const char* strDomainName,const char* strName,const char* strValue);

	/// \brief 修改数据集名称
	/// \param name 需要设置的名称字符串
	/// \return 返回是否修改成功
	virtual bool ReName(const char* name);

	/// \brief 获取数据集别名
	/// \return 返回数据集别名
	virtual Utility::GsString AliasName();

	/// \brief 修改数据集别名
	/// \param name 需要设置的名称字符串
	/// \return 返回是否修改成功
	virtual bool AliasName(const char* name);

};
/// \brief GsDataRoomPtr
GS_SMARTER_PTR(GsDataRoom);

/// \brief 具备地理特性的数据集对象抽象基类
class GS_API GsGeoDataRoom:public GsDataRoom
{
public:
	virtual ~GsGeoDataRoom();

	/// \brief 获取最大外接矩形范围
	///\param bForce 是否强制获取，强制获取将会从数据表中统计最大范围
	///\return 返回矩形对象
	virtual GsBox Extent(bool bForce = false) = 0;

	/// \brief 设置最大外接矩形范围
	///\param box 要设置的外接矩形box
	///\return 返回矩形对象
	virtual bool Extent(const GsBox& box);

	/// \brief 获取数据集的空间参考
	///\return 返回空间参考对象的指针
	virtual GsSpatialReference* SpatialReference() = 0;

	/// \brief 修改数据集的空间参考
	///\return 返回修改是否成功
	virtual bool ChangeSpatialReference(GsSpatialReference* pSR);

};
/// \brief GsGeoDataRoomPtr
GS_SMARTER_PTR(GsGeoDataRoom);

/// \brief 查询过滤的标识。
enum GsQueryFilterFlag
{
	eQFEmptyFlag = 0,
	/// \brief 查询结果不返回几何对象。
	eQFDisableGeometry=1,
	/// \brief 查询结果不返回属性结构
	eQFDisableAttribute=2,
};

/// \brief 属性条件查询
class GS_API GsQueryFilter:public Utility::GsRefObject
{
protected:
	GsQueryFilterFlag		m_Flag;
	/// \brief where子句
	Utility::GsString  m_strWhereClause;
	GsGeometrySimplifierPtr	m_ptrSimplifier;
public:
	/// \brief 从where子句构造
	///\param strWhere SQL查询的where子句，不包含where
	GsQueryFilter(const char* strWhere = NULL);
	virtual ~GsQueryFilter();
	
	/// \brief 判断标志是否设置
	bool FlagMarked(GsQueryFilterFlag flag);

	/// \brief 判断标志是否设置
	/// \details  设置eQFEmptyFlag以清空标识
	void MarkFlag(GsQueryFilterFlag flag,bool bMark= true);


	/// \brief 设置几何简化的方法
	void Simplifier(GsGeometrySimplifier* method);
	
	/// \brief 获取几何简化的方法
	GsGeometrySimplifier* Simplifier() const;

	/// \brief 获取where子句
	///\return 返回where子句字符串
	virtual Utility::GsString WhereClause();

	/// \brief 设置where子句查询条件
	///\param strWhere SQL查询的where子句
	virtual void WhereClause(const char* strWhere);

	/// \brief 获取是否设置了Where子句
	bool ExistsWhereClause();

	/// \brief 以And合并两个过滤条件
	///\param pOther 另外一个合并的条件
	///\return 返回合并是否成功
	virtual bool And(GsQueryFilter* pOther);

	/// \brief 以Or合并两个过滤条件
	///\param pOther 另外一个合并的条件
	///\return 返回合并是否成功
	virtual bool Or(GsQueryFilter* pOther);

	/// \brief 复制过滤条件对象
	virtual Utility::GsSmarterPtr<GsQueryFilter> Clone();

};
/// \brief GsQueryFilterPtr
GS_SMARTER_PTR(GsQueryFilter);

/// \brief 空间属性查询条件
class GS_API GsSpatialQueryFilter:public GsQueryFilter
{
	/// \brief 空间查询几何对象
	GsGeometryPtr m_ptrGeo;
	/// \brief 过滤方式
	GsSpatialFilterType  m_FilterType;
	/// \brief 过滤索引对象，
	void* m_pFilterIndex;

public:
	/// \brief 从几何对象构造
	///\param pGeo 查询的几何对象指针
	GsSpatialQueryFilter(GsGeometry* pGeo = NULL);
	/// \brief 从几何对象和where子句构造
	///\param pGeo 查询的几何对象指针
	///\param strWhere 查询SQL语句where子句
	GsSpatialQueryFilter(GsGeometry* pGeo,const char* strWhere);
	/// \brief 从几何对象,where子句和查询方式字符串构造
	///\param pGeo 查询的几何对象指针
	///\param strWhere 查询SQL语句where子句
	///\param strFilterType 查询方式
	GsSpatialQueryFilter(GsGeometry* pGeo,const char* strWhere, const char *strFilterType);

	///\brief 从几何对象,where子句和查询条件构造
	///\param pGeo 查询的几何对象指针
	///\param strWhere 查询SQL语句where子句
	///\param eFilterType 查询条件类型
	GsSpatialQueryFilter(GsGeometry* pGeo,const char* strWhere, GsSpatialFilterType eFilterType);
	~GsSpatialQueryFilter();

	/// \brief 获取空间查询几何对象
	///\return 返回查询的几何对象指针
	GsGeometry* Geometry();

	/// \brief 获取是否设置了几何过滤条件
	bool ExistsGeometry();

	/// \brief 设置空间查询几何对象
	///\param pGeo 查询的几何对象指针
	void Geometry(GsGeometry* pGeo);

	/// \brief 获取查询方式
	///\return 返回查询方式字符串
	virtual GsSpatialFilterType Filter();
	
	/// \brief 设置查询方式
	///\param eFilterType 查询方式字符串表达式
	virtual void Filter(GsSpatialFilterType eFilterType);

	/// \brief 获取查询方式
	///\return 返回查询方式字符串
	virtual Utility::GsString FilterType();

	/// \brief 设置查询方式
	///\param strFilterType 查询方式字符串表达式
	virtual void FilterType(const char* strFilterType);


	/// \brief 判断一个传入的Geometry是否符合几何过滤条件
	///\param pGeo 要判断的几何对象
	///\return 返回是否匹配几何过滤条件
	virtual bool IsMatch(GsGeometry* pGeo);

	/// \brief 判断一个传入的Geometry是否符合几何过滤条件
	///\param pBlob 要判断的几何对象
	///\return 返回是否匹配几何过滤条件
	virtual bool IsMatch(GsGeometryBlob* pBlob);


	/// \brief 以And合并两个过滤条件
	///\param pOther 另外一个合并的条件
	///\return 返回合并是否成功
	virtual bool And(GsQueryFilter* pOther);

	/// \brief 以Or合并两个过滤条件
	///\param pOther 另外一个合并的条件
	///\return 返回合并是否成功
	virtual bool Or(GsQueryFilter* pOther);

	/// \brief 复制过滤条件对象
	virtual Utility::GsSmarterPtr<GsQueryFilter> Clone();

};
/// \brief GsSpatialQueryFilterPtr
GS_SMARTER_PTR(GsSpatialQueryFilter);

///\brief 字段
struct GS_API GsField
{	
	///\brief 字段名称
	Utility::GsString				Name;
	
	///\brief 字段别名
	Utility::GsString				AliasName;

	///\brief 字段类型
	Utility::Data::GsFieldType		Type;

	///\brief 字段精度
	int								Precision;
	///\brief 小数位
	int								Scale;
	///\brief 是否允许为空
	bool							IsNullable;

	///\brief 字段默认值
	Utility::GsAny					DefaultValue;

	GsField();
	///\brief 拷贝构造函数
	GsField(const GsField& rhs);

	///\brief 从名称和类型构造
	///\param strName 字段名称
	///\param eType 字段类型
	///\param strAliasName 字段别名
	///\param defaultVal 字段默认值
	GsField(const char* strName,Utility::Data::GsFieldType eType,const char* strAliasName = 0,Utility::GsAny defaultVal = Utility::GsAny());

	///\brief 从名称和类型、精度和小数L位构造
	///\param strName 字段名称
	///\param eType 字段类型
	///\param nPrecision 字段精度
	///\param nScale 字段小数位长度
	///\param strAliasName 字段别名
	///\param defaultVal 字段默认值，类型需要与字段类型匹配
	GsField(const char* strName,Utility::Data::GsFieldType eType,int nPrecision,int nScale=0,const char* strAliasName = 0,Utility::GsAny defaultVal = Utility::GsAny());

	
	///\brief 将字段信息序列化为xml字符串
	Utility::GsString ToXml() const;
};
///\brief 字段集合
struct GS_API GsFields
{
	UTILITY_NAME::GsVector<GsField> Fields;

	///\brief 寻找字段的索引
	int FindField(const char* strFieldName)const;
	
	///\brief 将字段信息序列化为xml字符串
	Utility::GsString ToXml()const;

	///\brief 将xml字符串序列化为Fields信息
	void FromXml(const char* strInfo);

}; 

///\brief 几何字段信息
struct GS_API GsGeometryColumnInfo
{
	///\brief 是否存在Z维度
	bool			HasZ; 
	///\brief 几何类型
	GsGeometryType  GeometryType;
	///\brief 地物类型
	GsFeatureType	FeatureType;
	///\brief 比例尺
	double			MapScale;
	///\brief X，Y维度范围
	GsBox			XYDomain;
	///\brief Z维度范围
	double			ZMin,ZMax;
	///\brief 缺省构造
	GsGeometryColumnInfo();
	///\brief 拷贝构造
	GsGeometryColumnInfo(const GsGeometryColumnInfo& rhs);
	///\brief 从几何类型，地物类型和xy维度范围构造
	GsGeometryColumnInfo(GsGeometryType  gType,GsFeatureType fType,const GsBox& xydomain);
	///\brief 从几何类型，地物类型和xy维度范围，z维度范围构造
	GsGeometryColumnInfo(GsGeometryType  gType,GsFeatureType fType,const GsBox& xydomain,double zmin,double zmax);
};


///\brief 瓦片类基本信息
struct GS_API GsTileColumnInfo
{
	///\brief 地物类型
	GsFeatureType	FeatureType; 
	///\brief X，Y维度范围
	GsBox			XYDomain;
	
	///\brief 有效的顶层级别
	int				ValidTopLevel;
	///\brief 有效的底层级别
	int				ValidBottomLevel;

	///\brief 缺省构造
	GsTileColumnInfo();
	///\brief 拷贝构造
	GsTileColumnInfo(const GsTileColumnInfo& rhs);

	///\brief 从地物类型和范围构造
	GsTileColumnInfo(GsFeatureType fType,const GsBox& xydomain);

};	


class GS_API GsRow:public Utility::GsRefObject
{
protected:
	/// \brief 矢量地物对象唯一ID
	long long m_nOID;
	GsRow();
public:
	virtual ~GsRow();

	/// \brief 获取对象唯一ID
	///\return 返回唯一ID，如果ID小于0则表示对象为新的未存储对象
	virtual long long OID();

	/// \brief 设置对象的id
	/// \details 通过设置地物的id为-1改变对象为新对象
	///\param oid 对象唯一ID
	void OID(long long oid);
	
	/// \brief 获取整形值
	///\param nColumn 列索引
	///\return 返回32位整数值
	virtual int ValueInt(int nColumn);
	/// \brief 获取无符号整形值
	///\param nColumn 列索引
	///\return 返回32位无符号整数值
	virtual unsigned int ValueUInt(int nColumn);
	
	/// \brief 获取64位整数值
	///\param nColumn 列索引
	///\return 返回64位整数值
	virtual long long ValueInt64(int nColumn);
	/// \brief 获取64位无符号整数值
	///\param nColumn 列索引
	///\return 返回64位无符号整数值
	virtual unsigned long long ValueUInt64(int nColumn);

	/// \brief 获取单精度浮点值
	///\param nColumn 列索引
	///\return 返回单精度浮点值
	virtual float ValueFloat(int nColumn);
	/// \brief 获取双精度浮点值
	///\param nColumn 列索引
	///\return 返回双精度浮点值
	virtual double ValueDouble(int nColumn);

	/// \brief 获取字符串值
	///\param nColumn 列索引
	///\return 返回字符串值
	virtual Utility::GsString ValueString(int nColumn);
	/// \brief 获取二进制块
	///\param nColumn 列索引
	///\return 返回二进制块
	virtual Utility::GsAny ValueBlob(int nColumn);

	/// \brief 获取日期
	///\param nColumn 列索引
	///\return 返回日期
	virtual Utility::GsDateTime ValueDateTime(int nColumn);

	/// \brief 获取值的指针
	///\param nColumn 列索引
	///\return 返回值的指针
	virtual unsigned char* ValuePtr(int nColumn) = 0;
	/// \brief 获取值的字节长度
	///\param nColumn 列索引
	///\return 返回值的字节长度
	virtual unsigned int ValueSize(int nColumn) = 0;
	/// \brief 获取列的数据类型
	///\param nColumn 列索引
	///\return 返回列的数据类型枚举
	virtual Utility::Data::GsFieldType ValueType(int nColumn) = 0;

	/// \brief 设置Bool值
	///\param nColumn 列索引
	///\param nValue 布尔值
	virtual void Value(int nColumn, bool nValue);

	/// \brief 设置32位整数值
	///\param nColumn 列索引
	///\param nValue 整数值
	virtual void Value(int nColumn,int nValue);
	/// \brief 设置无符号32位整数值
	///\param nColumn 列索引
	///\param value 整数值
	virtual void Value(int nColumn,unsigned int value);

	/// \brief 设置字符串值
	///\param nColumn 列索引
	///\param value 字符串值
	virtual void Value(int nColumn,const char* value);
	/// \brief 设置单精度浮点值
	///\param nColumn 列索引
	///\param value 单精度浮点值
	virtual void Value(int nColumn,float value);
	/// \brief 设置双精度浮点值
	///\param nColumn 列索引
	///\param value 双精度浮点值
	virtual void Value(int nColumn,double value);
	/// \brief 设置二进制值
	///\param nColumn 列索引
	///\param value 二进制值
	///\param nSize 二进制值长度
	virtual void Value(int nColumn,const unsigned char* value,int nSize);
	/// \brief 设置64位整数
	///\param nColumn 列索引
	///\param value 64位整数值
	virtual void Value(int nColumn,long long  value);
	/// \brief 设置无符号64位整数
	///\param nColumn 列索引
	///\param value 无符号64位整数值
	virtual void Value(int nColumn,unsigned long long  value);
	/// \brief 设置日期
	///\param nColumn 列索引
	///\param value 设置日期
	virtual void Value(int nColumn,const Utility::GsDateTime& value);

	/// \brief 设置属性值为空
	///\param nColumn 列索引
	virtual void Value(int nColumn);

	/// \brief 根据字段类型设置任意值
	///\param nColumn 列索引
	///\param value 无符号64位整数值
	///\param nSize 值长度
	///\param eType 字段类型
	virtual void Value(int nColumn,const unsigned char* value,int nSize,Utility::Data::GsFieldType eType) = 0;

	
	/// \brief 保存对象到数据库或文件
	///\return 返回保存是否成功
	virtual bool Store() = 0;

	/// \brief 从数据库或者文件删除对象
	///\return 返回删除是否成功
	virtual bool Delete() = 0;
};
/// \brief GsRowPtr
GS_SMARTER_PTR(GsRow);


/// \brief 瓦片对象
class GS_API GsTile:public GsRow
{
protected:
	GsTile();
public:
	virtual ~GsTile();

	/// \brief 层行列转化为64位Key值，Level占6位，Row占24位，Col占24位
	static long long ToKey(int nLevel, int nRow, int nCol);
	/// \brief 64位key值获取Level值
	static int KeyToLevel(long long nKey);
	/// \brief 64位key值获取Row值
	static int KeyToRow(long long nKey);
	/// \brief 64位key值获取Col值
	static int KeyToCol(long long nKey);

	/// \brief 获取级别
	virtual int Level();
	/// \brief 设置级别
	virtual void Level(int nLevel);
	/// \brief 获取行
	virtual int Row();
	/// \brief 设置行
	virtual void Row(int r);
	/// \brief 获取列
	virtual int Col();
	/// \brief 设置列
	virtual void Col(int c);

	/// \brief 获取瓦片类型
	virtual GsTileEncodingType TileType() = 0;
	/// \brief 设置瓦片类型
	virtual void TileType(GsTileEncodingType eType) = 0;

	/// \brief 分配指定长度瓦片数据缓冲区
	virtual unsigned char*  TileData(int nLen) = 0;

	/// \brief 设置瓦片数据
	virtual void TileData(const unsigned char* pData,int nLen) = 0;
	
	/// \brief 获取瓦片数据指针
	virtual unsigned char* TileDataPtr() = 0;
	
	/// \brief 获取瓦片数据长度
	virtual int TileDataLength() = 0;

};
/// \brief GsTilePtr
GS_SMARTER_PTR(GsTile);



/// \brief 矢量地物对象
class GS_API GsFeature:public GsRow
{
protected: 

	/// \brief 矢量地物几何数据
	GsGeometryPtr m_ptrGeo;

	/// \brief 矢量地物所在的地物类
	Utility::GsSmarterPtr<GsFeatureClass> m_ptrFeaClass;
	int m_nEPSG;
	GsDataSourceType m_DataSource;
	
	/// \brief 几何对象
	GsGeometryBlob		m_OGS;

	GsFeature();
public:
	virtual ~GsFeature();

	/// \brief 判断地物是否存储的Geometry数据
	virtual bool ExistGeometry();

	/// \brief 获取几何数据
	///\return 返回几何对象的指针
	virtual GsGeometry* Geometry();

	/// \brief 设置几何数据
	///\param Geo 几何对象指针，内部不会拷贝
	virtual void Geometry(GsGeometry* Geo);

	/// \brief 设置几何数据
	///\param Geo 几何Blob数据
	virtual void Geometry(GsGeometryBlob* Geo);

	/// \brief 获得Geometry的Blob数据指针
	virtual GsGeometryBlob* GeometryBlob();


	/// \brief 获取地物所在的地物类
	///\return 返回所在的地物类指针
	virtual GsFeatureClass* FeatureClass();

	/// \brief 复制另其他地物的属性数据到当前地物中
	/// \details 仅仅当其他的属性结构和当前地物保持完全一致才能正常工作
	virtual bool AssignAttribute(GsFeature* pOther);

};
/// \brief GsFeaturePtr
GS_SMARTER_PTR(GsFeature);

/// \brief 分析数据封装
class GS_API GsFeatureBuffer
{
protected:
	GsGeometryBlob m_Blob;
	UTILITY_NAME::GsVector<Utility::GsAny> m_Attributes;
public:
	GsFeatureBuffer();
	virtual ~GsFeatureBuffer();

	/// \brief 从地物读取数据
	bool ReadFromFeature(GsFeature* pFea);

	/// \brief 写入数据到地物
	bool WriteToFeature(GsFeature* pFea);


	/// \brief 数据的唯一ID
	long long ID();
	/// \brief 数据的唯一ID
	void ID(long long id);

	/// \brief 直接设置坐标内容
	/// \param nInterCount 解释串长度
	/// \param pInter 解释串数组
	/// \param nDim 几何维度，0=点，1=线，2=面
	/// \param nCDim 坐标维度，2,3
	/// \param nCoordLen 坐标长度
	/// \param pCoord 坐标数组
	virtual void SetGeometry(int nInterCount, int* pInter, int nDim, int nCDim, int nCoordLen, double* pCoord);

	/// \brief 设置Geometry对象
	virtual void SetGeometry(GsGeometry* pGeo);

	/// \brief 用Geometry数据设置Geometry对象
	virtual void SetGeometry(GsGeometryBlob* pBlob);

	/// \brief 获取数据的Geometry对象实例
	virtual GsGeometryPtr GeometryPtr();


	/// \brief 解释串长度
	virtual int InterpretLength();
	/// \brief 解释串指针
	virtual int* Interpret();
	/// \brief 拷贝解释串到输入的数组中
	virtual int  Interpret(int* pInter, int nBuffLen);
	/// \brief 坐标串长度
	virtual int CoordinateLength();
	/// \brief 坐标串
	virtual double* Coordinate();
	/// \brief 拷贝坐标串到输入的数组中
	virtual int Coordinate(double* pCoord, int nBuffLen);

	/// \brief 几何对象的子对象数量，如果几何对象为简单对象则返回1
	virtual int GeometrySubCount();

	/// \brief 获取子Geometry的解释串
	virtual int SubInterpret(int n, int* pInter, int nBuffLen);
	virtual int SubInterpretLength(int n);
	/// \brief 获取子Geometry的解释串
	virtual int SubCoordinate(int n, double* pCoord, int nBuffLen);
	virtual int SubCoordinateLength(int n);


	/// \brief 坐标维度
	virtual int CoordinateDimension();
	/// \brief 几何维度，0=点，1=线，2=面
	virtual int GeometryDimension();

	/// \brief 属性值的长度
	virtual int AttributeLength();

	/// \brief 重置属性
	virtual void ResetAttribute();

	/// \brief 根据列索引获得列类型
	virtual Utility::Data::GsFieldType ValueType(int nColIndex);

	/// \brief 获取整数值
	virtual int IntValue(int nColIndex);
	/// \brief 获取整数值
	virtual long long Int64Value(int nColIndex);
	/// \brief 获取字符串值
	virtual const char* StringValue(int nColIndex);
	/// \brief 获取浮点值
	virtual double DoubleValue(int nColIndex);

	/// \brief 获取Blob值
	virtual const unsigned char* BlobValue(int nColIndex);
	virtual int BlobValue(int nColIndex, unsigned char* buff, int nLen);

	/// \brief 获取Value
	virtual const Utility::GsAny& Value(int nColIndex) const;

	/// \brief 获取Blob值长度
	virtual int BlobValueLength(int nColIndex);


	/// \brief 设置整数值
	virtual void SetValue(int nColIndex, int val);
	/// \brief 设置64位整数值
	virtual void SetValue(int nColIndex, long long val);
	/// \brief 设置字符串
	virtual void SetValue(int nColIndex, const char* val);
	/// \brief 设置浮点值
	virtual void SetValue(int nColIndex, double val);
	/// \brief 设置Blob
	virtual void SetValue(int nColIndex, const unsigned char* val, int nLen);
	/// \brief 设置空值
	virtual void SetValue(int nColIndex);

};

/// \brief 地物数据IO
class GS_API GsFeatureDataIO
{
public:
	virtual ~GsFeatureDataIO() {  }

	/// \brief 获取数据源的几何维度，0=点，1=线，2=面
	virtual int GeometryDimension() { return 0; }

	/// \brief 输入或者输出数据
	/// \return ,当输入空间数据时返回0标示正常输入一条数据，否则为错误或者输入完成。
	virtual int OnData(GsFeatureBuffer* pData) { return -1; }
};



/// \brief 带符号化的矢量地物对象
class GS_API GsSymbolizeFeature:public GsFeature
{
public:
	virtual ~GsSymbolizeFeature(){}
	/// \brief 获取符号的ID
	virtual long long SymbolID() = 0;

	/// \brief 获取符号数据
	virtual Utility::GsAny SymbolData() = 0;
};
/// \brief GsSymbolizeFeaturePtr
GS_SMARTER_PTR(GsSymbolizeFeature);

/// \brief 地物游标
/// \code{.cpp}
///  GsFeaturePtr ptrFea = ptrCursor->Next();
///  do{
///     DoSomeThing(ptrFea);
///	  }while(ptrCursor->Next(ptrFea.p);
/// \endcode
class GS_API GsFeatureCursor:public Utility::GsRefObject
{
public:
	/// \brief 获取下一个地物对象
	/// \details 每次回生成一个新的地物对象
	///\return 返回下一个地物对象的实例或者空
	virtual GsFeaturePtr Next() = 0;
	
	/// \brief 获取下一个地物对象数据而不用产生新的对象
	/// \details 性能更高的方法，以访问者模式将下一个地物的数据设置到传入的地物对象内部。
	///\param pFea 一个非空地物对象的指针
	///\return 返回是否存在下一个地物，如果传入地物对象为空则一定范围false
	virtual bool Next(GsFeature* pFea) = 0;
};
/// \brief GsFeatureCursorPtr
GS_SMARTER_PTR(GsFeatureCursor);

class GS_API GsRowCursor:public Utility::GsRefObject
{
	
public:
	/// \brief 获取下一个行对象
	/// \details 每次回生成一个新的行对象
	///\return 返回下一个行对象的实例或者空
	virtual GsRowPtr Next() = 0;
	
	/// \brief 获取下一个行对象数据而不用产生新的对象
	/// \details 性能更高的方法，以访问者模式将下一个地物的数据设置到传入的行对象内部。
	///\param pRow 一个非空行对象的指针
	///\return 返回是否存在下一个行，如果传入行对象为空则一定范围false
	virtual bool Next(GsRow* pRow) = 0;
};

/// \brief GsRowCursorPtr
GS_SMARTER_PTR(GsRowCursor);



/// \brief 瓦片游标
/// \code{.cpp}
///  GsTilePtr ptrTile = ptrCursor->Next();
///  do{
///     DoSomeThing(ptrTile);
///	  }while(ptrCursor->Next(ptrTile.p);
/// \endcode
class GS_API GsTileCursor:public Utility::GsRefObject
{
	
public:
	/// \brief 获取下一个瓦片对象
	/// \details 每次回生成一个新的瓦片对象
	///\return 返回下一个瓦片对象的实例或者空
	virtual GsTilePtr Next() = 0;
	
	/// \brief 获取下一个瓦片对象数据而不用产生新的对象
	/// \details 性能更高的方法，以访问者模式将下一个地物的数据设置到传入的瓦片对象内部。
	///\param pRow 一个非空瓦片对象的指针
	///\return 返回是否存在下一个行，如果传入行对象为空则一定返回false
	virtual bool Next(GsTile* pRow) = 0;
};

/// \brief GsTileCursorPtr
GS_SMARTER_PTR(GsTileCursor);

/// \brief 地物类OID枚举器
class GS_API GsEnumIDs:public Utility::GsRefObject
{
public:
	/// \brief 获取下一个地物的OID
	/// \details 每次得到下一个地物的OID
	///\return 返回下一个地物的OID
	virtual long long Next() = 0;

	/// \brief 重置枚举器
	/// \details 调用Reset后，下次调用Next会得到第一个OID
	virtual void Reset() = 0;
};
/// \brief GsEnumIDsPtr
GS_SMARTER_PTR(GsEnumIDs);

/// \brief 选择集
class GS_API GsSelectionSet:public Utility::GsRefObject
{
public:
	/// \brief 获得选择集中地物的数量
	/// \details 获得选择集中地物的数量
	///\return 返回包含的地物数量
	virtual unsigned int Count() = 0;

	/// \brief 获取地物OID枚举器
	/// \details 获取选择集中地物OID的集合
	///\return 返回地物OID枚举器
	virtual GsEnumIDsPtr EnumIDs() = 0;

	/// \brief 往选择集中添加一个OID
	///\param OID 要添加的OID值
	virtual void Add (long long OID) = 0;

	/// \brief 往选择集中添加多个OID
	///\param pOID 要添加的多个OID的首地址
	///\param nOIDCount 要添加的OID的数量
	virtual void Add(long long* pOID, unsigned int nOIDCount) = 0;

	/// \brief 清除所有的OID
	virtual void Clear() = 0;

	/// \brief 在选择集中删除多个OID
	///\param pOID 要删除的多个OID的首地址
	///\param nOIDCount 要删除的OID的数量
	virtual void Remove(long long* pOID, unsigned int nOIDCount) = 0;

	/// \brief 在选择集中查询地物
	///\param pFilter 检索过滤条件
	///\return 返回地物游标
	virtual GsFeatureCursorPtr Search (GsQueryFilter* pFilter = 0) = 0;
	
	/// \brief 获得选择集所属的地物类
	///\return 返回地物类
	virtual GsFeatureClass* Target () = 0;

	/// \brief 根据过滤条件并且在当前选择集范围里查询选择集对应的地物类,得到一个新的选择集
	///\param pFilter 检索过滤条件
	///\return 返回新的选择集
	virtual Utility::GsSmarterPtr<GsSelectionSet> Select (GsQueryFilter * pFilter) = 0;

	/// \brief 根据条件将当前 OID 选择集和另一个 OID 选择集进行集合操作，并得到新的 OID 选择集对象
	///\param pSrcSet 需要合并操作的选择集
	///\param eOperation 合并操作类型 
	///\return 返回新的选择集
	virtual Utility::GsSmarterPtr<GsSelectionSet> Combine (GsSelectionSet * pSrcSet, GsSelectionSetOperation eOperation) = 0;

};
/// \brief GsSelectionSetPtr
GS_SMARTER_PTR(GsSelectionSet);

/// \brief 瓦片数据集
/// \code{.cpp}
///  GsTilePtr ptrTile = ptrTileClass->CreateTile();
///  ptrTile->Row(r);
///	 ptrTile->Level(l);
///	 ptrTile->Col(c);
///  ptrTile->Store();
///  GsTileCursorPtr ptrCursor=ptrTileClass->Search();
///  GsTilePtr ptrTile = ptrCursor->Next();
///  do{
///     DoSomeThing(ptrTile);
///	  }while(ptrCursor->Next(ptrTile.p);
/// \endcode
class GS_API GsTileClass:public GsGeoDataRoom
{ 
protected:
	GsTileClass();
public:
	virtual ~GsTileClass();
	
	/// \brief 创建一个新的Tile对象
	/// \return 返回Tile对象的指针
	virtual GsTilePtr CreateTile()=0;

	/// \brief 检索所有的瓦片
	/// \return 返回瓦片游标
	virtual GsTileCursorPtr Search() = 0;

	/// \brief 检索某个级别到某个级别的瓦片
	/// \param nStartLevel 开始的级别
	/// \param nEndLevel 结束的级别
	/// \return 返回瓦片游标
	virtual GsTileCursorPtr Search(int nStartLevel,int nEndLevel) = 0;

	/// \brief 检索某个级别某个行列范围的瓦片
	/// \param nLevel 检索的级别
	/// \param nStartRow 开始行
	/// \param nStartCol 开始列
	/// \param nEndRow 结束行
	/// \param nEndCol 开始列
	/// \return 返回瓦片游标
	virtual GsTileCursorPtr Search(int nLevel,int nStartRow,int nStartCol,int nEndRow,int nEndCol) = 0;

	/// \brief 获取瓦片类的金字塔
	/// \return 返回金字塔对象指针
	virtual GsPyramid* Pyramid() = 0;

	/// \brief 修改瓦片类的金字塔
	/// \param pPyramid 要修改的金字塔对象
	/// \return 返回修改是否成功
	virtual bool ChangePyramid(GsPyramid* pPyramid) = 0;

	
	
	/// \brief 获取瓦片基本元信息
	///\return 返回原信息对象
	virtual GsTileColumnInfo TileColumnInfo() = 0;

	/// \brief 设置瓦片基本元信息
	/// \param tileinfo 要修改的瓦片元信息对象
	///\return 返回是否设置成功
	virtual bool TileColumnInfo(const GsTileColumnInfo& tileinfo) = 0;

	/// \brief 获取所有瓦片的数量
	///\return 返回瓦片总数
	virtual long long TileCount() = 0;

	/// \brief 获取某个级别瓦片的数量
	/// \param nStartLevel 开始级别
	/// \param nEndLevel 结束级别
	///\return 返回该级别瓦片总数
	virtual long long TileCount(int nStartLevel,int nEndLevel) = 0;

	/// \brief 检索某个级别某个行列范围的瓦片
	/// \param nLevel 检索的级别
	/// \param nStartRow 开始行
	/// \param nStartCol 开始列
	/// \param nEndRow 结束行
	/// \param nEndCol 开始列
	///\return 返回该级别瓦片总数
	virtual long long TileCount(int nLevel,int nStartRow,int nStartCol,int nEndRow,int nEndCol) = 0;

	
	/// \brief 根据瓦片行列号检索一个瓦片
	/// \param nLevel 瓦片的级别
	/// \param nRow 瓦片的行
	/// \param nCol 瓦片的列
	///\return 返回该瓦片或者空
	virtual GsTilePtr Tile(int nLevel,int nRow,int nCol);

	/// \brief 根据瓦片行列号检索一个瓦片
	/// \param nLevel 瓦片的级别
	/// \param nRow 瓦片的行
	/// \param nCol 瓦片的列
	/// \param pTile 瓦片
	///\return 返回瓦片是否存在
	virtual bool Tile(int nLevel,int nRow,int nCol,GsTile* pTile);


	/// \brief 根据瓦片行列号检索一个瓦片,可重入，多线程安全
	/// \param nLevel 瓦片的级别
	/// \param nRow 瓦片的行
	/// \param nCol 瓦片的列
	///\return 返回该瓦片或者空
	virtual GsTilePtr ReentrancyTile(int nLevel, int nRow, int nCol);

	/// \brief 根据瓦片行列号检索一个瓦片,可重入，多线程安全
	/// \param nLevel 瓦片的级别
	/// \param nRow 瓦片的行
	/// \param nCol 瓦片的列
	/// \param pTile 瓦片
	///\return 返回瓦片是否存在
	virtual bool ReentrancyTile(int nLevel, int nRow, int nCol, GsTile* pTile);


	


};
/// \brief GsTileClassPtr
GS_SMARTER_PTR(GsTileClass);


/// \brief 基于瓦片化的地图服务的瓦片数据集封装。
class GS_API GsTMSTileClass:public GsTileClass
{
public:
	
protected: 
	Utility::GsString m_strFormat;
	GsTileClassPtr	m_ptrCacheTile;
	GsTileEncodingType m_eType;
	GsDataRoomExtensionDataPtr m_ExtData;
	GsTMSTileClass();
public:
	virtual ~GsTMSTileClass();
	/// \brief 设置数据集的扩展对象
	void ExtensionData(GsDataRoomExtensionData* extData);

	/// \brief 获取数据集的扩展对象
	virtual GsDataRoomExtensionData* ExtensionData();

	/// \brief 获取TMS的url地址模板
	virtual Utility::GsString UrlTemplate();

	/// \brief 获取瓦片类型
	virtual GsTileEncodingType TileType();
	/// \brief 设置瓦片类型
	virtual void TileType(GsTileEncodingType eType);

	/// \brief 设置TMS的url地址模板
	/// \details 地址模板如http://xxx.server.com/${Level}/${Row}/${Col}
	/// ${Level}代表瓦片级别
	/// ${Row}代表瓦片行
	/// ${Col}代表瓦片列
	virtual void UrlTemplate(const char* ); 


	/// \brief 获取缓存瓦片数据的TileClass对象
	virtual GsTileClass* Cache();

	/// \brief 设置缓存瓦片数据的TileClass
	virtual void Cache(GsTileClass* pCacheTileClass); 
};

/// \brief GsTMSTileClassPtr
GS_SMARTER_PTR(GsTMSTileClass);


/// \brief 二维表数据集
class GS_API GsRowClass:public GsDataRoom
{
protected:
	GsRowClass();
public:
	~GsRowClass();
	/// \brief 根据查询过滤条件查询
	///\param pFilter 检索过滤条件
	///\return 返回检索结果的行游标对象
	virtual GsRowCursorPtr Search(GsQueryFilter* pFilter = 0) = 0;

	/// \brief 根据Where子句进行查询
	///\param strWhere 要检索的where子句
	///\return 返回检索结果的行游标对象
	virtual GsRowCursorPtr Search(const char* strWhere);

	/// \brief 获取二维表的字段信息
	///\return 字段信息
	virtual GsFields Fields() = 0;

	/// \brief 创建的一个新的属性对象
	///\return 返回新建的属性对象指针
	virtual GsRowPtr  CreateRow() = 0;

	/// \brief 根据输入的oid获取属性对象
	///\return 返回属性对象
	virtual GsRowPtr  Row(long long nOID) = 0;
	
	/// \brief 根据输入的oid获取属性对象
	/// \param nOID 输入的OID
	/// \param pRow 传入属性对象指针，用于接收属性数据，不能为空
	///\return 返回是否成功获取属性对象
	virtual bool Row(long long nOID,GsRow* pRow) = 0;

	/// \brief 根据查询条件获取属性的数量
	/// \param pFilter 空间或者属性的查询条件
	///\return 返回满足查询条件的属性的数量
	virtual long long RowCount(GsQueryFilter* pFilter = 0) = 0;

	/// \brief 增加字段
	/// \param field 添加的字段
	///\return 是否添加成功
	virtual bool AddField(const GsField& field) = 0;

	/// \brief 删除字段
	/// \param name 需要删除的字段名称
	///\return 是否删除成功
	virtual bool DeleteField(const char* name) = 0;
};
/// \brief GsRowClassPtr
GS_SMARTER_PTR(GsRowClass);


/// \brief 批量地物更新的标识，标识地物更新那些内容
enum GsBatchFeatureUpdateFlag
{
	/// \brief 更新地物的几何数据
	eUpdateGeometry = 1,

	/// \brief 更新地物的属性数据
	eUpdateAttribute = 2,

	/// \brief 更新整个地物
	eUpdateFullFeature = 3,

};
/// \brief 批量地物更新参数
struct GS_API GsBatchFeatureUpdateParameter
{
	/// \brief 最大记录数量
	long long MaxRecordCount;

	/// \brief 最大内存使用
	long long MaxMemoryUse;

	/// \brief 更新标识
	GsBatchFeatureUpdateFlag Flag;

	/// \brief Flag设置eUpdateAttribute时指定要更新的字段
	/// \details 如果期望更新全部字段，可以设置集合为空
	UTILITY_NAME::GsVector<Utility::GsString> UpdateAttributeField;

	GsBatchFeatureUpdateParameter();
};
/// \brief 批量地物更新对象
/// \details 批量将多个地物的增、删、改动作集中保存，一次性批量提交
class GS_API GsBatchFeatureUpdateStatement :public Utility::GsRefObject
{
protected:
	GsBatchFeatureUpdateParameter m_Parameter;
	GsBatchFeatureUpdateStatement(const GsBatchFeatureUpdateParameter &param);
public:
	virtual ~GsBatchFeatureUpdateStatement();
	
	/// \brief 获取参数
	GsBatchFeatureUpdateParameter Parameter();
	
	/// \brief 获取缓存是否已满
	virtual bool IsFull();

	/// \brief 创建新地物
	virtual bool Create(GsFeature* pFea);
	/// \brief 创建新地物
	virtual bool Create(GsFeatureBuffer* pFea);

	/// \brief 更新地物
	virtual bool Update(GsFeature* pFea);
	/// \brief 更新地物
	virtual bool Update(GsFeatureBuffer* pFea);

	/// \brief 删除地物
	virtual bool Delete(GsFeature* pFea);
	/// \brief 删除地物
	virtual bool Delete(long long oid);

	/// \brief 将批量修改的结果提交
	virtual bool Commit();
};

/// \brief GsBatchFeatureUpdateStatementPtr
GS_SMARTER_PTR(GsBatchFeatureUpdateStatement);

/// \brief 矢量地物类
/// \details GIS点、线、面、注记类型的矢量数据读写对象
class GS_API GsFeatureClass:public GsGeoDataRoom
{
public:
	virtual ~GsFeatureClass();

	/// \brief 批量更新
	virtual GsBatchFeatureUpdateStatementPtr BatchUpdate(const GsBatchFeatureUpdateParameter &param);

	/// \brief 获取地物类的字段信息
	///\return 字段信息
	virtual GsFields Fields() = 0;

	/// \brief 创建的一个新的地物对象
	///\return 返回新建的地物对象指针
	virtual GsFeaturePtr  CreateFeature() = 0;

	/// \brief 根据输入的oid获取地物对象
	///\return 返回地物对象
	virtual GsFeaturePtr  Feature(long long nOID) = 0;
	
	/// \brief 根据输入的oid获取地物对象
	/// \param nOID 输入的OID
	/// \param pFea 传入地物对象指针，用于接收地物数据，不能为空
	///\return 返回是否成功获取地物对象
	virtual bool Feature(long long nOID,GsFeature* pFea) = 0;
	

	/// \brief 根据查询过滤条件粗查
	///\param pFilter 检索过滤条件可以是GsSpatialQueryFilter或者GsQueryFilter类型
	///\return 返回检索结果的地物游标对象
	virtual GsFeatureCursorPtr Search(GsQueryFilter* pFilter = 0) = 0;
	/// \brief 根据几何空间范围进行粗查
	///\param pGeo 要检索的几何空间范围
	///\return 返回检索结果的地物游标对象
	virtual GsFeatureCursorPtr Search(GsGeometry* pGeo);
	/// \brief 根据Where子句进行粗查
	///\param strWhere 要检索的where子句
	///\return 返回检索结果的地物游标对象
	virtual GsFeatureCursorPtr Search(const char* strWhere);
	/// \brief 根据where子句和几何空间范围进行粗查
	///\param pGeo 要检索的空间范围
	///\param strWhere 要检索的where子句
	///\return 返回检索结果的地物游标对象
	virtual GsFeatureCursorPtr Search(GsGeometry* pGeo,const char* strWhere);
	
	/// \brief 地物类的几何类型
	///\return 返回地物类的几何类型
	virtual GsGeometryType GeometryType() = 0;

	/// \brief 创建空间索引
	///\return 返回创建索引是否成功
	virtual bool CreateSpatialIndex() = 0;
	/// \brief 删除空间索引
	///\return 返回删除索引是否成功
	virtual bool DeleteSpatialIndex() = 0;

	/// \brief 是否存在空间索引
	///\return 返回空间索引是否存在
	virtual bool HasSpatialIndex() = 0;

	/// \brief 根据根据where子句和几何空间范围进行选择
	///\return 返回删除索引是否成功
	virtual GsSelectionSetPtr Select(GsQueryFilter* pFilter = 0) = 0;


	/// \brief 获取基本几何信息
	///\return 返回基本几何信息对象
	virtual GsGeometryColumnInfo GeometryColumnInfo() = 0;

	
	/// \brief 根据查询条件获取地物的数量
	/// \param pFilter 空间或者属性的查询条件
	///\return 返回满足查询条件的地物的数量
	virtual long long FeatureCount(GsQueryFilter* pFilter = 0) = 0;

	/// \brief 设置基本几何信息
	/// \param geoInfo 几何信息
	///\return 返回是否设置成功
	virtual bool GeometryColumnInfo(const GsGeometryColumnInfo& geoInfo);

	/// \brief 增加字段
	/// \param field 添加的字段
	///\return 是否添加成功
	virtual bool AddField(const GsField& field);

	/// \brief 删除字段
	/// \param name 需要删除的字段名称
	///\return 是否删除成功
	virtual bool DeleteField(const char* name);

	/// \brief 设置字段别名
	/// \brief nIndex 字段索引
	/// \brief strAliasName 别名
	/// \brief 是否设置成功
	virtual bool FieldAliasName(const int nIndex, const char* strAliasName);

	/// \brief 设置字段别名
	/// \brief strName 字段原名
	/// \brief strAliasName 别名
	/// \brief 是否设置成功
	virtual bool FieldAliasName(const char* strName, const char* strAliasName);
};
/// \brief GsFeatureClassPtr
GS_SMARTER_PTR(GsFeatureClass);

/// \brief Web地物类（WFS)
class GS_API GsWebFeatureClass :public GsFeatureClass
{
public:
	/// \brief 服务地址
	virtual void ServerUrl(const char* url) = 0;
	virtual const char* ServerUrl() = 0;
};
/// \brief GsWebFeatureClassPtr
GS_SMARTER_PTR(GsWebFeatureClass);

/// \brief 代理地物类回调
class GS_API GsProxyFeatureClassIO:public Utility::GsRefObject
{
public:
	virtual ~GsProxyFeatureClassIO() {}
	/// \brief 搜索一个地物类返回数据IO
	virtual GsFeatureDataIO* Search(GsQueryFilter* pFilter) {return NULL;}
	
};

/// \brief GsProxyFeatureClassIOPtr
GS_SMARTER_PTR(GsProxyFeatureClassIO);

/// \brief 用于用户实现将任意数据源实现为地物类
/// \details 通过用户实现简单的回调接口实现读取任意数据的能力
class GS_API GsProxyFeatureClass:public GsFeatureClass
{
	GsProxyFeatureClassIOPtr m_ptrIO;
	GsFields m_Fs;
	GsGeometryColumnInfo m_Info;
	GsSpatialReferencePtr m_ptrSR;
public:
	GsProxyFeatureClass(const char* name);
	virtual ~GsProxyFeatureClass();
	
	/// \brief 设置数据回调IO对象
	void DataIO(GsProxyFeatureClassIO* IO);
	/// \brief 获取数据回调IO对象
	GsProxyFeatureClassIO* DataIO();

	/// \brief 设置地物类的字段信息
	virtual void Fields(const GsFields& fs);
	/// \brief 获取地物类的字段信息
	///\return 字段信息
	virtual GsFields Fields();

	/// \brief 创建的一个新的地物对象
	///\return 返回新建的地物对象指针
	virtual GsFeaturePtr  CreateFeature();

	/// \brief 根据输入的oid获取地物对象
	///\return 返回地物对象
	virtual GsFeaturePtr  Feature(long long nOID);

	/// \brief 根据输入的oid获取地物对象
	/// \param nOID 输入的OID
	/// \param pFea 传入地物对象指针，用于接收地物数据，不能为空
	///\return 返回是否成功获取地物对象
	virtual bool Feature(long long nOID, GsFeature* pFea);


	/// \brief 根据查询过滤条件粗查
	///\param pFilter 检索过滤条件可以是GsSpatialQueryFilter或者GsQueryFilter类型
	///\return 返回检索结果的地物游标对象
	virtual GsFeatureCursorPtr Search(GsQueryFilter* pFilter = 0);

	/// \brief 地物类的几何类型
	///\return 返回地物类的几何类型
	virtual GsGeometryType GeometryType();

	/// \brief 创建空间索引
	///\return 返回创建索引是否成功
	virtual bool CreateSpatialIndex();
	/// \brief 删除空间索引
	///\return 返回删除索引是否成功
	virtual bool DeleteSpatialIndex();

	/// \brief 是否存在空间索引
	///\return 返回空间索引是否存在
	virtual bool HasSpatialIndex();

	/// \brief 根据根据where子句和几何空间范围进行选择
	///\return 返回删除索引是否成功
	virtual GsSelectionSetPtr Select(GsQueryFilter* pFilter = 0);


	/// \brief 获取基本几何信息
	///\return 返回基本几何信息对象
	virtual GsGeometryColumnInfo GeometryColumnInfo();


	/// \brief 根据查询条件获取地物的数量
	/// \param pFilter 空间或者属性的查询条件
	///\return 返回满足查询条件的地物的数量
	virtual long long FeatureCount(GsQueryFilter* pFilter = 0);

	/// \brief 设置基本几何信息
	/// \param geoInfo 几何信息
	///\return 返回是否设置成功
	virtual bool GeometryColumnInfo(const GsGeometryColumnInfo& geoInfo);
	 
	/// \brief 获取最大外接矩形范围
	///\param bForce 是否强制获取，强制获取将会从数据表中统计最大范围
	///\return 返回矩形对象
	virtual GsBox Extent(bool bForce = false);

	/// \brief 设置最大外接矩形范围
	///\param box 要设置的外接矩形box
	///\return 返回矩形对象
	virtual bool Extent(const GsBox& box);

	/// \brief 获取数据集的空间参考
	///\return 返回空间参考对象的指针
	virtual GsSpatialReference* SpatialReference();
	/// \brief 设置空间参考
	virtual void SpatialReference(GsSpatialReference* sp);

	/// \brief 数据集类型
	///\return 返回数据集对象的类型
	virtual GsDataRoomType Type();
	 
	/// \brief 删除
	///\return 返回是否删除成功
	virtual bool Delete();
};
/// \brief GsProxyFeatureClassPtr
GS_SMARTER_PTR(GsProxyFeatureClass);


/// \brief 波段类型
enum GsRasterBandType
{
	/// \brief 未定义波段
	eUndefinedBand=0,
    /// \brief Greyscale灰度索引
	eGrayIndexBand=1,
    /// \brief Paletted 调色板索引波段
	ePaletteIndexBand=2,
    /// \brief RGBA 红色波段
	eRedBand=3,
    /// \brief RGBA 绿色波段
	eGreenBand=4,
	/// \brief RGBA 蓝色波段
	eBlueBand=5,
    /// \brief RGBA 透明波段，0标示透明，255标示不透明
	eAlphaBand=6,
    /// \brief HLS Hue波段
	eHueBand=7,
    /// \brief HLS Saturation波段
	eSaturationBand=8,
    /// \brief HLS Lightness波段
	eLightnessBand=9,
    /// \brief CMYK Cyan波段
	eCyanBand=10,
    /// \brief CMYK Magenta波段
	eMagentaBand=11,
    /// \brief CMYK Yellow波段
	eYellowBand=12,
    /// \brief CMLY Magenta波段
	eBlackBand=13,
    /// \brief YCbCr Y亮度波段
	eYCbCr_YBand=14,
    /// \brief YCbCr Cb蓝色偏移量波段
	eYCbCr_CbBand=15,
    /// \brief YCbCr Cr红色偏移量波段
	eYCbCr_CrBand=16,
};
 
// \brief 支持创建的栅格格式类型
enum GsRasterCreateableFormat
{
	// \brief DOM数据格式
	eNSDTF_DOM,
	// \brief GeoTiff数格式
	eGTiff,
};

// \brief 栅格数据类型
// \details 每个波段具有不同的数据类型
enum GsRasterDataType
{
	// \brief 未知栅格数据类型
	eUnknownRDT= 0,
    // \brief 8位无符号整数
	eByteRDT = 1,
    // \brief 16位无符号整数
	eUInt16RDT = 2,
    // \brief 16位整数
	eInt16RDT = 3,
    // \brief 32位无符号整数
	eUInt32RDT = 4,
    // \brief 32位整数
	eInt32RDT = 5,
    // \brief 32位单精度浮点数
	eFloat32RDT = 6,
    // \brief 64位双精度浮点数
	eFloat64RDT = 7,
    // \brief  16位复合整数
	eCInt16RDT = 8,
    // \brief  32位复合整数
	eCInt32RDT = 9,
    // \brief  32位复合浮点数
	eCFloat32RDT = 10,
    // \brief  64位复合浮点数
	eCFloat64RDT = 11,
};
 


// \brief 栅格颜色类型
enum GsRasterColorType
{
	// \brief  灰度颜色
	eGrayColorType,
	// \brief  RGB颜色，顺序RGBA
	eRGBColorType,
	
	// \brief CMYK颜色，顺序Cyan, Magenta, Yellow , Black
	eCMYKColorType,
	// \brief HLS颜色，顺序Hue, Lightness and Saturation
	eHLSColorType, 
};
// \brief 栅格重采样算法
enum GsRasterResampleAlg
{
	// \brief 最临近点采样
	eNearestNeighbour = 0,
    // \brief 双线性插值，2x2像素范围
	eBilinear = 1,
    // \brief 三次卷积插值，4x4像素范围
	eCubic = 2,
    // \brief 三次样条插值，4x4像素范围
	eCubicSpline = 3,
	// \brief 将对称矩阵通过正交相似变换变成对称三对角矩阵的插值算法，6x6像素范围
	eLanczos = 4,
    // \brief 平均值插值
	eAverage = 5,
    // \brief 模式插值，取采样数据中最常出现的值
	eMode = 6,
	// \brief 高斯模糊插值
	eGauss = 7
};

/// \brief 调色板
/// \details 调色板颜色每2个字节(short)标识一个颜色波段
struct GsColorTable
{
	/// \brief 调色板的颜色类型
	GsRasterColorType ColorType;
	/// \brief 调色板颜色
	UTILITY_NAME::GsVector<unsigned long long> ColorTable;
	
	/// \brief 是否是有效的颜色表
	bool IsValid(){return !ColorTable.empty();}

};

///\brief 栅格类基本信息
struct GS_API GsRasterColumnInfo
{
	///\brief X，Y维度范围
	GsBox			XYDomain;
	///\brief 像素宽度
	int				Width;
	///\brief 像素高度
	int				Height;

	///\brief 数据存储块宽度大小
	int				BlockWidth;
	///\brief 数据存储块高度大小
	int				BlockHeight;

	///\brief 波段数据类型
	GsRasterDataType DataType;
	
	///\brief 波段的名称
	UTILITY_NAME::GsVector<GsRasterBandType> BandTypes;
	
	///\brief 缺省构造函数
	GsRasterColumnInfo();
	///\brief 拷贝构造函数
	GsRasterColumnInfo(const GsRasterColumnInfo& rhs);
	///\brief 全部构造
	GsRasterColumnInfo(const GsBox &extent,int width,int height,GsRasterDataType dataType,GsRasterBandType* pBand,int nBandCount);
	
	///\brief 是否有效
	bool IsValid() const;

};

/// \brief 栅格波段
class GS_API GsRasterBand:public Utility::GsRefObject
{
public:
	virtual ~GsRasterBand(){}
	/// \brief 波段所属的栅格类
	virtual GsRasterClass* RasterClass() = 0;

	/// \brief 波段的名称、类型或者为用途
	virtual GsRasterBandType BandType() = 0;
	
	/// \brief 最小值
	virtual double Minimum()  = 0;
	/// \brief 最大值
	virtual double Maximum()  = 0;

	/// \brief 无效值
	virtual double NoDataValue() = 0;

	/// \brief 波段的的索引号
	virtual int Index() = 0;

	/// \brief 波段的数据类型
	virtual GsRasterDataType BandDataType() = 0;

	/// \brief 波段的数类型bit位大小
	static int RasterDataTypeBitSize(GsRasterDataType t);

	/// \brief 获取调色板、颜色表
	virtual GsColorTable ColorTable()=0;

};
/// \brief GsRasterBandPtr
GS_SMARTER_PTR(GsRasterBand);

/// \brief 栅格数据块
class GS_API GsRaster:public Utility::GsRefObject
{
	/// \brief 数据长度
	int m_nLength;
protected:
	/// \brief 栅格数据指针
	unsigned char * m_pDataPtr;
	/// \brief 栅格数据的宽度
	int			m_nWidth;
	/// \brief 栅格数据的高度
	int			m_nHeight;
	/// \brief 数据位置在数据集上的X偏移
	int			m_nOffsetX;
	/// \brief 数据位置在数据集上的Y偏移
	int			m_nOffsetY;

	/// \brief 栅格对象所属于的栅格类
	Utility::GsSmarterPtr<GsRasterClass> m_ptrRasterClass;
public:
	GsRaster();
	virtual ~GsRaster();
	
	/// \brief 获取栅格数据指针
	virtual unsigned char * DataPtr();
	/// \brief 设置栅格数据指针
	virtual void DataPtr(unsigned char * pHead);

	/// \brief 设置栅格数据指针，数据会拷贝一份
	virtual void DataPtr(const unsigned char * pHead,int nLen);

	/// \brief 根据栅格类属性计算的栅格数据长度
	virtual int DataLength();


	/// \brief 获取栅格数据宽度
	virtual int Width();
	/// \brief 设置栅格数据宽度
	virtual void Width(int n);

	/// \brief 获取栅格数据高度
	virtual int Height();
	/// \brief 设置栅格数据高度
	virtual void Height(int n);

	/// \brief 获取栅格数据X偏移
	virtual int OffsetX();
	/// \brief 设置栅格数据X偏移
	virtual void OffsetX(int n);

	/// \brief 获取栅格数据Y偏移
	virtual int OffsetY();
	/// \brief 设置栅格数据Y偏移
	virtual void OffsetY(int n);

	/// \brief 获取关联的栅格类
	virtual GsRasterClass* RasterClass();
	/// \brief 关联栅格类
	virtual void RasterClass(GsRasterClass* rastercls);
};
/// \brief GsRasterPtr
GS_SMARTER_PTR(GsRaster);

/// \brief 栅格查询游标
class GS_API GsRasterCursor:public Utility::GsRefObject
{
protected:
	GsRasterCursor(){}
public:
	/// \brief 游标的数据宽度
	virtual int Width() = 0;
	/// \brief 游标的数据高度
	virtual int Height() = 0;
	
	/// \brief 游标数据开始数据X偏移
	virtual int X() = 0;
	/// \brief 游标数据开始数据Y偏移
	virtual int Y() = 0;

	/// \brief 重置游标位置
	virtual void Reset() = 0;

	/// \brief 获取下一块栅格对象
	virtual bool Next(GsRaster* raster) = 0;
	
	/// \brief 获取关联的栅格类
	virtual GsRasterClass* RasterClass()=0;
};
/// \brief GsRasterCursorPtr
GS_SMARTER_PTR(GsRasterCursor);

/// \brief 栅格类，存储影像、地形等网格数据
class GS_API GsRasterClass:public GsGeoDataRoom
{
protected:
	GsRasterClass(){}
public:
	virtual ~GsRasterClass(){}
	/// \brief 像素宽度
	virtual int Width()  = 0;
	/// \brief 像素高度
	virtual int Height() = 0;
	
	/// \brief 栅格的编码格式
	virtual Utility::GsString Format() = 0;
	
	/// \brief 获取波段的数量
	virtual int BandCount() = 0;

	/// \brief 根据索引获得波段，索引从0开始
	virtual GsRasterBand* RasterBand(int n) = 0;
	
	/// \brief 计算地理范围在栅格中的像素范围
	virtual Utility::GsRect ExtentToRange(const GsBox& extent);
	/// \brief 计算地理范围对应的像素范围
	virtual GsBox RangeToExtent(const Utility::GsRect& range);

	/// \brief 查询一块原始数据
	virtual GsRasterCursorPtr Search(const Utility::GsRect&  range) = 0;
	/// \brief 采样一块数据。
	/// \param range 要采样的原始数据
	/// \param s 采样后的大小
	/// \param alg 采样算法
	virtual GsRasterCursorPtr Search(const Utility::GsRect&  range,const Utility::GsSize& s,GsRasterResampleAlg alg) = 0;

	/// \brief 将一块栅格数据写入到数据集中
	virtual bool WriteRaster(GsRaster* pRaster);

	/// \brief 是否存在金字塔
	virtual bool ExistsPyramid() = 0;

	/// \brief 创建金字塔
	/// \param alg 采样算法
	/// \param nLevel 创建金字塔的级别，金字塔级别以1/2,1/4,1/8 ...递增；影像金字塔没有0级金字塔的概念，0级就是不创建金字塔。
	virtual bool CreatePyramid(GsRasterResampleAlg alg,int nLevel);
	/// \brief 创建金字塔
	/// \param alg 采样算法
	/// \param nCount
	/// \param pLevels
	virtual bool CreatePyramid(GsRasterResampleAlg alg,int nCount,int* pLevels) = 0;

	
	
	/// \brief 获取基本几何信息
	///\return 返回基本几何信息对象
	virtual GsRasterColumnInfo RasterColumnInfo() = 0;


	/// \brief 修改数据集名称
	/// \param name 需要设置的名称字符串
	/// \return 返回是否修改成功
	virtual bool ReName(const char* name)
	{
		return false;
	}

public:
	/// \brief 发生长事务执行的时候进度信息，例如创建金字塔
	Utility::GsDelegate<bool(const char*,double)> OnProgress;

};
/// \brief GsRasterClassPtr
GS_SMARTER_PTR(GsRasterClass);





/// \brief 支持数据集存储、打开的抽象基类
template<class T>
class GS_API GsDataRoomHouse:public T
{
protected:
	GsDataRoomHouse(){}
public:
	virtual ~GsDataRoomHouse(){}

	/// \brief 枚举数据库数据集的名称 
	///\param eDRType 要枚举的数据集的类型
	///\param vecName 要枚举类型数据集的名称
	virtual void DataRoomNames(GsDataRoomType eDRType, UTILITY_NAME::GsVector<Utility::GsString>& vecName) = 0;

	/// \brief 打开矢量地物类对象
	///\param strName 矢量地物的名称
	///\return 返回矢量地物类的指针或者NULL
	virtual Utility::GsSmarterPtr<GsFeatureClass> OpenFeatureClass(const char* strName)
	{
		return OpenDataRoom(strName,eFeatureClass);
	}
	/// \brief 创建一个地物类
	///\param strName 地物类的名称
	///\param fs 地物类的字段
	///\param oColumnInfo 地物类的基本信息
	///\param pSR 地物类的空间参考
	///\return 如果名称存在则返回已经存在的地物类，否则返回新建的地物类对象或者为空
	virtual Utility::GsSmarterPtr<GsFeatureClass>  CreateFeatureClass(const char* strName,const GsFields&  fs,const GsGeometryColumnInfo& oColumnInfo,GsSpatialReference* pSR)
	{
		return 0;
	}


	/// \brief 打开瓦片类对象
	///\param strName 瓦片类的名称
	///\return 返回瓦片类的指针或者NULL
	virtual Utility::GsSmarterPtr<GsTileClass> OpenTileClass(const char* strName)
	{
		return OpenDataRoom(strName,eTileClass);
	}
	/// \brief 创建一个瓦片类
	///\param strName 瓦片类的名称
	///\param pSR 瓦片类的空间参考
	///\param pPyramid 瓦片类的金字塔
	///\param oColumnInfo 瓦片信息
	///\return 如果名称存在则返回已经存在的瓦片类，否则返回新建的瓦片类对象或者为空
	virtual Utility::GsSmarterPtr<GsTileClass>  CreateTileClass(const char* strName,GsSpatialReference* pSR,GsPyramid* pPyramid,const GsTileColumnInfo& oColumnInfo)
	{
		return 0;
	}

	/// \brief 打开栅格类对象
	///\param strName 栅格类的名称
	///\return 返回栅格类的指针或者NULL
	virtual Utility::GsSmarterPtr<GsRasterClass> OpenRasterClass(const char* strName)
	{
		return OpenDataRoom(strName,eRasterClass);
	}
	/// \brief 打开栅格类对象
	///\param strName 栅格类的名称
	///\param eFormat 创建栅格的格式
	///\param oColumnInfo 栅格类基本信息
	///\param pSR 空间参考
	///\param strOptions 参数选项
	///\return 返回栅格类的指针或者NULL
	virtual Utility::GsSmarterPtr<GsRasterClass> CreateRasterClass(const char* strName,GsRasterCreateableFormat eFormat,const GsRasterColumnInfo& oColumnInfo,GsSpatialReference* pSR,const char* strOptions = NULL)
	{
		return 0;
	}
	/// \brief 打开二维表对象
	///\param strName 二维表的名称
	///\return 返回二维表的指针或者NULL
	virtual Utility::GsSmarterPtr<GsRowClass> OpenRowClass(const char* strName)
	{
		return OpenDataRoom(strName,eRowClass);
	}
	/// \brief 创建一个二维表
	///\param strName 二维表的名称
	///\param fs 二维表的字段
	///\return 如果名称存在则返回已经存在的二维表，否则返回新建的二维表对象或者为空
	virtual Utility::GsSmarterPtr<GsRowClass> CreateRowClass(const char* strName, const GsFields&  fs)
	{
		return 0;
	}

	/// \brief 打开已经存在的数据集
	///\param strName 地物类的名称
	///\param eDRType 数据集的类型
	///\return 返回数据集对象
	virtual Utility::GsSmarterPtr<GsDataRoom> OpenDataRoom(const char* strName,GsDataRoomType eDRType)
	{
		switch(eDRType)
		{
		case eFeatureClass:
			return OpenFeatureClass(strName);
		case eTileClass:
			return OpenTileClass(strName);
		case eRasterClass:
			return OpenRasterClass(strName);
		case eRowClass:
			return OpenRowClass(strName);
		case eDataRoomFolder:
			return OpenDataRoomFolder(strName);
		}
		return 0;
	}

	/// \brief 创建数据集目录
	///\param strName 数据集目录的名称
	///\param pSR 数据集目录的空间参考
	///\return 如果名称存在则返回已经存在的数据集目录，否则返回新建的数据集目录对象或者为空
	virtual Utility::GsSmarterPtr<GsDataRoomFolder> CreateDataRoomFolder(const char* strName,GsSpatialReference* pSR)
	{
		return 0;
	}

	/// \brief 打开数据集目录
	///\param strName 数据集目录的名称
	///\return 返回数据集目录的指针或者NULL
	virtual Utility::GsSmarterPtr<GsDataRoomFolder> OpenDataRoomFolder(const char* strName)
	{
		return OpenDataRoom(strName, eDataRoomFolder);
	}
};


/// \brief 数据集目录
class GS_API GsDataRoomFolder:public GsDataRoomHouse<GsDataRoom>
{
protected:
public:
	virtual ~GsDataRoomFolder();
	 
	/// \brief 数据集类型
	///\return 返回数据集对象的类型
	virtual GsDataRoomType Type();
	
};

/// \brief GsDataRoomFolderPtr
GS_SMARTER_PTR(GsDataRoomFolder);

/// \brief 空间数据库对象
/// \details 代表一个能够存储GIS空间数据的数据库、文件（数据源）实体。
class GS_API GsGeoDatabase:public GsDataRoomHouse<Utility::GsRefObject>
{
protected:
	/// \brief 数据库的连接信息
	GsConnectProperty m_ConnectProperty; 

	/// \brief 来自工厂的配置项
	Utility::GsConfig  m_config;
	GsGeoDatabase();
public:
	virtual ~GsGeoDatabase(); 
	
	/// \brief 获取事务对象，如果不支持事务则返回NULL
	virtual Utility::Data::GsTransaction* Transaction();

	/// \brief 获取来自工厂的配置项
	Utility::GsConfig&  Config();

	/// \brief 数据库连接属性 
	///\return 返回数据库连接属性 
	virtual GsConnectProperty ConnectProperty();

	/// \brief 测试数据库能力
	///\param eCap 数据库能力类型
	///\return 能力类型的值，通常情况下1为支持能力，0为不支持。
	virtual long long TestCapbility(GsGeoDatabaseCapbility eCap);
};
/// \brief GsGeoDatabasePtr
GS_SMARTER_PTR(GsGeoDatabase);
 
/// \brief 空间数据库类厂
/// \details 通过空间数据库类厂创建空间数据库对象实例。
/// \code{.cpp}
/// GsOracleSpatialGeoDatabaseFactory fac;
/// GsGeoDatabasePtr ptrGDB = fac.Open(connProperty);
/// \endcode
class GS_API GsGeoDatabaseFactory:public Utility::GsRefObject
{
protected:
	GsGeoDatabaseFactory();
	/// \brief 针对数据库工厂的特殊配置项
	Utility::GsConfig m_FactoryConfig;
public:
	virtual ~GsGeoDatabaseFactory(); 

	/// \brief 获取配置项
	Utility::GsConfig Config();
	/// \brief 设置配置项
	void Config(const Utility::GsConfig& config);

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty) = 0;
};
/// \brief GsGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsGeoDatabaseFactory);
 
/// \brief 基于文件系统的数据库工厂基类
class GS_API GsFileSystemGeoDatabaseFactory :public GsGeoDatabaseFactory
{
protected:
	GsFileSystemGeoDatabaseFactory() {}
public:
	virtual ~GsFileSystemGeoDatabaseFactory() {}

	/// \brief 调用数据源DataRoomNames方法时检测到一个数据集类型的时候发生
	/// \details 参数说明，参数1，文件或目录的完整路径
	/// \details 参数2，数据集的类型
	/// \details 参数3，数据集的名称
	/// \details 参数4，该数据集辅助的文件名称（不包含完整路径）
	Utility::GsDelegate<void(const char*, GsDataRoomType,const char*, UTILITY_NAME::GsVector<Utility::GsString>&)> OnDetectDataRoom;
};
/// \brief GsFileSystemGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsFileSystemGeoDatabaseFactory);


/// \brief Sqlite数据库工厂
/// \details GeoStar文件格式
class GS_API GsSqliteGeoDatabaseFactory:public GsFileSystemGeoDatabaseFactory
{
public:
	GsSqliteGeoDatabaseFactory();
	virtual ~GsSqliteGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	/// \code{.cpp}
	/// GeoStar::Kernel::GsSqliteGeoDatabaseFactory vFac;
	/// GeoStar::Kernel::GsConnectProperty vConn;
	/// vConn.Server = GeoStar::Utility::GsFileSystem::Combine(CurrentFolder().c_str(),strpath)
	/// GeoStar::Kernel::GsGeoDatabasePtr ptrGDB = vFac.Open(vConn);
	/// \endcode
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty);
};
/// \brief GsSqliteGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsSqliteGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsSqliteGeoDatabaseFactory);


/// \brief ShapeFile数据库工厂
/// \details ESRI ShapeFile文件格式
class GS_API GsShpGeoDatabaseFactory:public GsFileSystemGeoDatabaseFactory
{ 
public:
	GsShpGeoDatabaseFactory();
	virtual ~GsShpGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty); 
};
/// \brief GsShpGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsShpGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsShpGeoDatabaseFactory);

/// \brief 文件类型数据库工厂
/// \details 包含Sqlite，SHP，GeoPackage以及文件栅格数据的类型识别
class GS_API GsFileGeoDatabaseFactory:public GsFileSystemGeoDatabaseFactory
{
public:
	GsFileGeoDatabaseFactory();
	virtual ~GsFileGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty); 
};
/// \brief GsFileGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsFileGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsFileGeoDatabaseFactory);



/// \brief OGC GeoPackage标准数据库工厂
/// \details 基于OGC GeoPackage标准文件格式http://www.geopackage.org/
class GS_API GsGeoPackageGeoDatabaseFactory:public GsFileSystemGeoDatabaseFactory
{ 
public:
	GsGeoPackageGeoDatabaseFactory();
	virtual ~GsGeoPackageGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty); 
};
/// \brief GsGeoPackageGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsGeoPackageGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsGeoPackageGeoDatabaseFactory);

/// \brief 以WebService为基础的数据库工厂
class GS_API GsWebGeoDatabaseFactory:public GsGeoDatabaseFactory
{ 
public:
	GsWebGeoDatabaseFactory();
	virtual ~GsWebGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty); 
};
/// \brief GsWebGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsWebGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsWebGeoDatabaseFactory);



/// \brief 以WebService为基础的数据库工厂
class GS_API GsMemoryGeoDatabaseFactory :public GsGeoDatabaseFactory
{
public:
	GsMemoryGeoDatabaseFactory();
	virtual ~GsMemoryGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty);
};
/// \brief GsMemoryGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsMemoryGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsMemoryGeoDatabaseFactory);


KERNEL_ENDNS
