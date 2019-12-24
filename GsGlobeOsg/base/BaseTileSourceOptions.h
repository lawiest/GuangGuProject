#pragma once

#include <osgEarth/TileSource>
#include <Layer.h>

#include "Base/Service.h"
#include <Tool/Cache.h>
#include <osgEarth/URI>


GLOBE_NS

class BaseTileSourceOptions : public osgEarth::TileSourceOptions
{
public:

	//optional<GEOServiceType>& servicetype() { return _servicetype; }//判断服务类型
	//const optional<GEOServiceType>& servicetype() const { return _servicetype; }

	osgEarth::optional<std::string>& layer() { return _layer; }//需要使用的图层名
	const osgEarth::optional<std::string>& layer() const { return _layer; }

	osgEarth::optional<std::string>& style() { return _style; }//图层的类型
	const osgEarth::optional<std::string>& style() const { return _style; }

	osgEarth::optional<std::string>& format() { return _format; }//图层的格式
	const osgEarth::optional<std::string>& format() const { return _format; }

	osgEarth::optional<std::string>& wmtsFormat() { return _wmtsFormat; }//天地图wmts服务的格式，是天地图的服务还是吉奥的服务
	const osgEarth::optional<std::string>& wmtsFormat() const { return _wmtsFormat; }

	osgEarth::optional<std::string>& wmtsVersion() { return _wmtsVersion; }//天地图wmts服务的版本
	const osgEarth::optional<std::string>& wmtsVersion() const { return _wmtsVersion; }

	osgEarth::optional<std::string>& elevationUnit() { return _elevationUnit; }//高程单位，主要用于地形瓦片的加载
	const osgEarth::optional<std::string>& elevationUnit() const { return _elevationUnit; }

	osgEarth::optional<std::string>& srs() { return _srs; }//该图层所使用的空间参考系统
	const osgEarth::optional<std::string>& srs() const { return _srs; }

	osgEarth::optional<std::string>& cachePolicy() { return _cachePolicy; }//存储策略 tiles为碎瓦片，dat为单文件
	const osgEarth::optional<std::string>& cachePolicy() const { return _cachePolicy; }

	osgEarth::optional<osgEarth::URI>& url() { return _url; }//服务地址 "http://t0.tianditu.com/img_c/wmts"
	const osgEarth::optional<osgEarth::URI>& url() const { return _url; }

	osgEarth::optional<std::string>& cachePath() { return _cachePath; }//设置缓存路径
	const osgEarth::optional<std::string>& cachePath() const { return _cachePath; }

public:

	BaseTileSourceOptions();

	//对图层参数类的初始化，确定本地或者服务，影像或者地形数据
	//本地图层参数类的初始化
	BaseTileSourceOptions(std::string dbPath,GeoGlobe::Tool::SQLiteCacheType cachetype,StoreType servicetype = eLocal);

	BaseTileSourceOptions(std::string dbPath, std::string url, GeoGlobe::Tool::SQLiteCacheType cachetype, StoreType servicetype);

	~BaseTileSourceOptions();

	void readMetaFromServer(std::string dbPath, TileMetaData& metadata);
	bool readTileMetafromLocal(std::string dbPath, TileMetaData& metadata);

	void getCapbilitiesfromGeoGlobeTile(TileMetaData& metadata);

	//将获得元信息写入缓存文件，sqlite文件
	bool writeTileMetatoCache(std::string dbPath, TileMetaData& metadata);

private:

	osgEarth::optional<std::string>				_layer;//需要使用的图层名
	osgEarth::optional<std::string>				_style;//图层的类型
	osgEarth::optional<std::string>				_format;//图层的格式
	osgEarth::optional<std::string>				_wmtsFormat;//天地图wmts服务的格式，是天地图的服务还是吉奥的服务
	osgEarth::optional<std::string>				_wmtsVersion;//天地图wmts服务的版本
	osgEarth::optional<std::string>				_elevationUnit;//高程单位，主要用于地形瓦片的加载
	osgEarth::optional<std::string>				_srs;//该图层所使用的空间参考系统
	osgEarth::optional<std::string>				_cachePolicy;//存储策略 tiles为碎瓦片，dat为单文件

public:
	string m_strDataSetPath;
	osgEarth::optional<std::string> _cachePath;//本地或者缓存数据文件路径
	osgEarth::optional<osgEarth::URI>		_url;//服务地址
	StoreType               _servicetype;//服务类型
	GeoGlobe::Tool::SQLiteCacheType _cacheType;//缓存文件类型，影像或者地形

	std::string m_strToken;

	TileMetaData _metadata;//瓦片元信息结构体
	//注意：metaData的最大层最小层对应osgEarth的编码原则，与吉奥编码相差1，吉奥编码比osgEarth大一层，osgEarthLevel+1=jiaoLevel
};

GLOBE_ENDNS







