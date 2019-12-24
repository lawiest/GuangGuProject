#pragma once

 #include <osgEarth/Registry>
#include "BaseTileSourceOptions.h"

#define INVALIDDATA -SHRT_MAX

GLOBE_NS

class ElevationTileSource : public osgEarth::TileSource
{
    public:
	
		//读取地形数据接口类的初始化，传入参数为BaseTileSourceOptions
		ElevationTileSource(GLOBE_NAME::BaseTileSourceOptions options);
		~ElevationTileSource();

		//在该图层加载数据前，对其进行初始化，包括数据范围、类型、图层的确定
		osgEarth::Status initialize(const osgDB::Options* dbOptions);

	    osg::HeightField* readTerrianfromSqlite(const osgEarth::TileKey& key,osgEarth::ProgressCallback* progress);

	    //获取吉奥瓦片服务数据的url地址
		std::string creatGeoGlobeURL(const osgEarth::TileKey& key);
		//获取readmap服务数据的url地址
		std::string createReadMapURL( const osgEarth::TileKey& key );

		//对服务数据从缓存或者服务器读取数据，并对未写入缓存的数据写入缓存
		osg::HeightField* readTerrianfromServiceOrCache(const osgEarth::TileKey& key ,osgEarth::ProgressCallback* progress);

		//根据原始数据可以创建新的高度场，改变地形，测试函数
		osg::HeightField* createNewHeightField(const osgEarth::TileKey& key,std::string source,int surhfsize);

		//确定该地形瓦片的大小，一般为32*32
		int getPixelsPerTile() const
		{
			return 32;
		}

		//重写，加载地形数据接口函数，由osgEarth调用
		osg::HeightField* createHeightField(const osgEarth::TileKey& key,osgEarth::ProgressCallback* progress);

		//创建该地形瓦片的名字，以便写入缓存或从缓存中读取
		std::string createElevationTileFileName(int lev,int col,int row);

		//图层参数类
	    const GLOBE_NAME::BaseTileSourceOptions _options;

	    //读写数据库文件操作类
	    osg::ref_ptr<GeoGlobe::Tool::SQLiteCache> _cache;

	    //地形的类型
	    std::string _demType;

	    //重写函数，由osgEarth调用，
		//若该图层在当前范围内并且在osgEarth的内存缓存列表中，
		//则直接从内存中读取，不用重新加载
	    bool hasDataForFallback(const osgEarth::TileKey& key) const;

	    //图层经纬度范围
		osgEarth::DataExtentList _dataExtents;
	    int _maxLevel;//最大层数
	    //重写函数，由osgEarth调用，判断该图层是否在当前范围内
	    bool hasData(const osgEarth::TileKey& key) const;
		osgEarth::DataExtentList& DataExtentList(); 

    };

GLOBE_ENDNS