#pragma once

#include "BaseTileSourceOptions.h"
#include <osgEarth/Registry>
#include <geodatabase.h> 

GLOBE_NS

enum QueryType
{
	eTileClass,
	eDefault
};

class ImageTileSource : public osgEarth::TileSource
{
public:
	//读取影像数据接口类的初始化，传入参数为BaseTileSourceOptions
	ImageTileSource(BaseTileSourceOptions options);
	ImageTileSource(KERNEL_NAME::GsTileClass* ptrTileClass);
	~ImageTileSource();

	//重写自基类的虚函数
	osgEarth::Status initialize(const osgDB::Options* dbOptions);
	//重写自基类的虚函数
	osg::Image* createImage( const osgEarth::TileKey& key, osgEarth::ProgressCallback* progress );
	//重写自基类的虚函数
	bool hasDataForFallback(const osgEarth::TileKey& key) const;
	//重写自基类的虚函数
	bool hasData(const osgEarth::TileKey& key) const;

	osg::Image* readImagefromSqlite(const osgEarth::TileKey& key,osgEarth::ProgressCallback* progress);
	osg::Image* QueryImageFromTileClass(const osgEarth::TileKey& key, osgEarth::ProgressCallback* progress);

	std::string createGeoGlobeURL(const osgEarth::TileKey& key);

	//创建天地图瓦片数据服务的请求url
	std::string createTiandituURL(const osgEarth::TileKey& key,std::string Url);

	//获取天地图wmts服务元信息
	void initializebyWMTS(const osgDB::Options* dbOptions,const osgEarth::Profile* overrideProfile);

	//截取字符串函数
	std::string stringRight(const std::string& input, const int num);

	osg::Image* readImagefromServiceOrCache( const osgEarth::TileKey& key,osgEarth::ProgressCallback* progress);

	std::string createImageTileFileName(int lev,int col,int row);

	osgEarth::DataExtentList& DataExtentList();

	KERNEL_NAME::GsTileEncodingType ImageTileSource::ImageType(const char* imageData);

protected:
	//元信息的存放类
	osgEarth::Util::WMTSCapabilities* _capWMTS;
	//天地图服务请求时的相关参数
	int m_layerindex;
	//所在图层
	int m_matrixsetindex;
	//图层下的数据块参数
	std::string _srsToUse;
	//空间参考系
	std::string _formatToUse;
	//数据格式
	std::string _wmtsType;
	//wmts服务类型
	std::string _protodataurl;
	BaseTileSourceOptions _options;
	osg::ref_ptr<GeoGlobe::Tool::SQLiteCache> _cache;
	//图层经纬度范围
	osgEarth::DataExtentList _dataExtents;
	//最大层数
	int _maxLevel;

	QueryType m_QueryType;
	KERNEL_NAME::GsTileClassPtr m_ptrTileClass;
};

GLOBE_ENDNS


