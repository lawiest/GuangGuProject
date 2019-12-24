#pragma once
#include "../utility/preconfig.h"
#include "map.h" 
KERNEL_NS

/// \brief 地图定义文件读写类
class GS_API GsMapDefine
{
	/// \brief 地图定义文件的文件路径
	Utility::GsString m_strFile;
	GsMapPtr m_ptrMap;
public:
	GsMapDefine();
	~GsMapDefine();

	/// \brief 从地图定义文件打开
	GsMapDefine(const char* strFilePath);
	
	/// \brief 设置地图定义文件的路径
	void FilePath(const char* strFilePath);
	
	/// \brief 获取地图定义文件的路径
	Utility::GsString FilePath();

	/// \brief 解析地图定义文件为地图
	bool ParserMap(GsMap* pMap);

	/// \brief 解析地图定义文件为地图
	bool ParserMap(const char* strFilePath,GsMap* pMap);

	/// \brief 解析地图定义文件为地图
	bool ParserMap(const char* strGmdXML,const char* strSymXML,GsMap* pMap);

	/// \brief 保存到文件
	bool SaveMap(GsMap* pMap, const char* strFilePath);

	/// \brief 打开地物类
	Utility::GsDelegate<GsFeatureClassPtr(const GsConnectProperty&, const char*)> OnOpenFeatureClass;
	/// \brief 打开瓦片数据集
	Utility::GsDelegate<GsTileClassPtr(const GsConnectProperty&, const char*)> OnOpenTileClass;
	/// \Brief 打开栅格类
	Utility::GsDelegate<GsRasterClassPtr(const GsConnectProperty&, const char*)> OnOpenRasterClass;

	/// \Brief  设置与地图定义文件相同样式的layer
	bool ApplyStyleLayer(GsLayer* pLayer);

	/// \Brief  找到与class同源的层
	Utility::GsVector<GsLayerPtr> FindLayers(GsGeoDataRoom* pClass);
private:
	bool ApplyLayerByLayer(GsLayer* pLayer, GsLayer* pApplyLayer);
	void FindLayerByClass(GsLayer* pLayer, GsGeoDataRoom* pClass, Utility::GsVector<GsLayerPtr>& vecLayers);
};

KERNEL_ENDNS