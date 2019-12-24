#pragma once
#include "pyramid.h"
#include "geometry.h"
#include "vectorhelp.h"
KERNEL_NS
class GsPlaceNameStyleExtensionData;
/// \brief Web服务地址解析类
class GS_API GsWebUriParser : public UTILITY_NAME::GsRefObject
{
	bool m_bSuccess;
protected:
	// 版本
	UTILITY_NAME::GsString m_strVersion;
	// URI地址
	UTILITY_NAME::GsString m_strUri;
	// capability字符串
	UTILITY_NAME::GsString m_strCapInfo;
	
	Utility::GsVector<UTILITY_NAME::GsString> m_vecNull;

	GsWebUriParser(const char* strUri = NULL);
public:
	
	virtual ~GsWebUriParser() {}

	/// \brief 创建格式化的URL字符串
	virtual UTILITY_NAME::GsString FormatUri() = 0;

	/// \brief 获取Uri字符串
	UTILITY_NAME::GsString UriString() { return m_strUri; }

	/// \brief 解析描述信息
	bool ParseCapability();

protected:
	/// \brief 描述信息URL地址
	virtual UTILITY_NAME::GsString CapabilityUri();

	/// \brief 解析描述字符串
	virtual bool ParseCapabilityString(const UTILITY_NAME::GsString& strXml);
	
};
GS_SMARTER_PTR(GsWebUriParser);

/// \brief 瓦片地图服务解析器
class GS_API GsTMSUriParser : public GsWebUriParser
{
protected:
	GsTMSUriParser(const char* strUri);
public:
	/// \brief 获取图层名
	virtual UTILITY_NAME::GsString LayerName() = 0;

	/// \brief 获取图层范围
	virtual GsBox LayerExtent() = 0;

	/// \brief 顶层瓦片级别
	virtual int TopLevel() = 0;

	/// \brief 底层瓦片级别
	virtual int BottomLevel() = 0;

	/// \brief 金字塔
	virtual GsPyramid* Pyramid() = 0;

	/// \brief 空间参考
	virtual GsSpatialReference* SpatialReference() = 0;
};
GS_SMARTER_PTR(GsTMSUriParser);

/// \brief 公众瓦片地图
enum GsWellknownWebTileService {
	/// \brief 天地图地理坐标系矢量瓦片WMTS服务
	eTiandituVectorGeographicWMTS,
	eTiandituImageGeographicWMTS,
	eTiandituVectorGeographicGeoTile,
	eTiandituImageGeographicGeoTile,

	eTiandituVectorWebMercatorWMTS,
	eTiandituImageWebMercatorWMTS,
	
	/// \brief 高德
	eAmapVectorWebMercator = 100,
	eAmapImageWebMercator,
	
	/// \brief 谷歌地图
	eGoogleVectorWebMercator = 200,
	eGoogleImageWebMercator,
	
	/// \brief 必应地图
	eBingVectorWebMercator = 300,
	eBingImageWebMercator,
	
	/// \brief OSM
	eOSMVectorWebMercator = 400,
	eOSMImageWebMercator,
	
	/// \brief Arcgis
	eArcgisOnlineVectorWebMercator = 500,
	eArcgisOnlineImageWebMercator,
};

/// \brief 公众瓦片地图解析
class GS_API GsWellknownTMSUriParser:public GsTMSUriParser
{
	UTILITY_NAME::GsString m_strFormat;
	UTILITY_NAME::GsString m_Name;
	GsBox m_Extent;
	int m_Top, m_Bottom;
	GsPyramidPtr m_ptrPyramid;

	GsSpatialReferencePtr m_ptrSR;
public:
	GsWellknownTMSUriParser(GsWellknownWebTileService web);

	/// \brief 创建格式化的URL字符串
	virtual UTILITY_NAME::GsString FormatUri();

	/// \brief 获取图层名
	virtual UTILITY_NAME::GsString LayerName();

	/// \brief 获取图层范围
	virtual GsBox LayerExtent();

	/// \brief 顶层瓦片级别
	virtual int TopLevel();

	/// \brief 底层瓦片级别
	virtual int BottomLevel();

	/// \brief 金字塔
	virtual GsPyramid* Pyramid();

	/// \brief 空间参考
	virtual GsSpatialReference* SpatialReference();
};

/// \brief WMTS服务解析类
class GS_API GsWMTSUriParser : public GsTMSUriParser
{
	/// 图层属性
	struct GsLayerProperty
	{
		// 图层名
		UTILITY_NAME::GsString strLayerName;
		// 图层样式
		UTILITY_NAME::GsVector<UTILITY_NAME::GsString> vecLayerStyle;
		// 瓦片矩阵集
		UTILITY_NAME::GsVector<UTILITY_NAME::GsString> vecTileMatrixSets;
		// 图片格式
		UTILITY_NAME::GsVector<UTILITY_NAME::GsString> vecImageFormat;

		GsBox extent;
	};

	/// 瓦片矩阵集
	struct GsTileMatrixSet
	{
		int nTopLevel;
		int nBottomLevel;
		GsPyramidPtr m_ptrPyramid;
		GsSpatialReferencePtr m_ptrRef;
	};

	UTILITY_NAME::GsVector<UTILITY_NAME::GsString> m_vecLayers;
	UTILITY_NAME::GsStlMap<UTILITY_NAME::GsString, GsLayerProperty> m_mapLayerProp;
	UTILITY_NAME::GsVector<UTILITY_NAME::GsString> m_vecTileMatrixSet;
	UTILITY_NAME::GsStlMap<UTILITY_NAME::GsString, GsTileMatrixSet> m_mapTileMatrixSet;

	UTILITY_NAME::GsString m_strCurLayerName;
	UTILITY_NAME::GsString m_strCurTileMatrixSet;
	
protected:
	/// \brief 描述信息URL地址
	virtual UTILITY_NAME::GsString CapabilityUri();

	/// \brief 解析描述字符串
	virtual bool ParseCapabilityString(const UTILITY_NAME::GsString& strXml);

	/// \biref 解析图层节点
	bool ParseLayerNode(tinyxml2::XMLElement* pNode);

	/// \brief 解析矩阵集节点
	bool ParseMatrixSetNode(tinyxml2::XMLElement* pNode);
	 
public:
	GsWMTSUriParser(const char* strUri);
	virtual ~GsWMTSUriParser();

	/// \brief 创建格式化的URL字符串
	virtual UTILITY_NAME::GsString FormatUri();

	/// \brief 获取图层名
	virtual UTILITY_NAME::GsString LayerName();

	/// \brief 获取图层范围
	virtual GsBox LayerExtent();

	/// \brief 顶层瓦片级别
	virtual int TopLevel();

	/// \brief 底层瓦片级别
	virtual int BottomLevel();

	/// \brief 金字塔
	virtual GsPyramid* Pyramid();

	/// \brief 空间参考
	virtual	GsSpatialReference* SpatialReference();

public:
	/// \brief 设置当前图层名
	bool LayerName(const char* strLayerName);

	/// \brief 获取图层名
	const UTILITY_NAME::GsVector<UTILITY_NAME::GsString>& AllLayerName() { return m_vecLayers; }

	/// \brief 获取样式类型
	const UTILITY_NAME::GsVector<UTILITY_NAME::GsString>& Style();

	/// \brief 获取图片格式
	const UTILITY_NAME::GsVector<UTILITY_NAME::GsString>& ImageFormat();

	/// \brief 获取瓦片矩阵集名称
	const UTILITY_NAME::GsVector<UTILITY_NAME::GsString>& TileMatrixSet() { return m_vecTileMatrixSet; }
	/// \brief 设置当前瓦片矩阵集
	bool CurrentTileMatrixSet(const char* strTileMatrixSet);
	/// \brief 获取当前瓦片矩阵集
	UTILITY_NAME::GsString CurrentTileMatrixSet();

	
};
GS_SMARTER_PTR(GsWMTSUriParser);

/// \brief 吉奥瓦片服务解析类
class GS_API GsGeoTileUriParser : public GsTMSUriParser
{
protected:
	// 瓦片数据类型
	UTILITY_NAME::GsString m_strTileDataType;
	// 图层名
	UTILITY_NAME::GsString m_strLayerName;
	// 图层范围
	GsBox m_LayerExtent;
	
	// 顶层瓦片级别
	int m_nTopLevel;
	// 底层瓦片级别
	int m_nBottomLevel;
	// 金字塔对象
	GsPyramidPtr m_ptrPyramid;
	// 空间参考对象
	GsSpatialReferencePtr m_ptrRef;

	// 瓦片格式
	Utility::GsVector<UTILITY_NAME::GsString> m_vecTileFormat;

	// 节点名称
	UTILITY_NAME::GsString m_strNodeTileFormat;
	// 节点名称
	UTILITY_NAME::GsString m_strNodeData;
protected:
	/// \brief 描述信息URI地址
	virtual UTILITY_NAME::GsString CapabilityUri();

	/// \brief 解析描述字符串
	virtual bool ParseCapabilityString(const UTILITY_NAME::GsString& strXml);

	/// \brief 解析TileData节点
	virtual bool ParseTileData(tinyxml2::XMLElement* pTileData);

public:
	GsGeoTileUriParser(const char* strUri);
	virtual ~GsGeoTileUriParser();

	/// \brief 创建格式化的URL字符串
	virtual UTILITY_NAME::GsString FormatUri();

public:
	/// \brief 图层名
	virtual UTILITY_NAME::GsString LayerName() { return m_strLayerName; }

	/// \brief 获取图层范围
	virtual GsBox LayerExtent() { return m_LayerExtent; }

	/// \brief 顶层瓦片级别
	virtual int TopLevel() { return m_nTopLevel; }

	/// \brief 底层瓦片级别
	virtual int BottomLevel() { return m_nBottomLevel; }

	/// \brief 金字塔
	virtual GsPyramid* Pyramid() { return m_ptrPyramid.p; }

	/// \brief 空间参考
	virtual GsSpatialReference* SpatialReference() { return m_ptrRef.p; }

	/// \brief 瓦片数据类型
	/// \reutrn image-影像数据、terrain-地形数据、wtfs-地名数据
	UTILITY_NAME::GsString TileDataType() { return m_strTileDataType; }
};
GS_SMARTER_PTR(GsGeoTileUriParser);

/// \brief 地名服务解析类
class GS_API GsWTFSUriParser : public GsGeoTileUriParser
{
public:
	GsWTFSUriParser(const char* strUri);
	virtual ~GsWTFSUriParser();

	/// \brief 创建格式化的URL字符串
	virtual UTILITY_NAME::GsString FormatUri();

	/// \brief 创建格式化的请求ICON字符串
	UTILITY_NAME::GsString FormatIconUri();

	/// \brief 请求图标辅助数据对象
	Utility::GsSmarterPtr<GsPlaceNameStyleExtensionData> IconExtensionData();
protected:
	/// \brief 解析TileData节点
	virtual bool ParseTileData(tinyxml2::XMLElement* pTileData);
};
KERNEL_ENDNS