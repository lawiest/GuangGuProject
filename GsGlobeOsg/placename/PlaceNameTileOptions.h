#ifndef GEOGLOBE_PLACENAME_PLACENAMETILEOPTIONS
#define GEOGLOBE_PLACENAME_PLACENAMETILEOPTIONS 1

#include <Tool/Cache.h>
#include <osgEarthSymbology/Style>
#include <utility.h>

namespace osgEarth
{
	class MapNode;
}

GLOBE_NS

enum PlaceNameServiceType
{
	eLocalPlaceName,
	eGeoGlobeTilePlaceName
};

class PlaceNameCache:public osg::Referenced
{
public:

	CSQLiteOP                 _pnCache;
	int                       _iconNum;
	/*下载的瓦片数量*/
	int                       _tileNum;
	/*缓存路径*/
	std::string               _cachePath;
	/*多线程用的mutex*/
	osgEarth::Threading::ReadWriteMutex _cacheMutex;
	//std::string               _cacheName;

	PlaceNameCache(const std::string& cachePath);
	~PlaceNameCache();

	/*解析数据名称*/
	std::string spliteFile(const std::string& fileName) const;

	void parseTileName(const std::string& uri,int& lev,int& col,int& row);

	void parseIDName(const std::string& uri,int& id) const;
	/*构造地名缓存*/
	void createPlaceNameCache(const std::string& pnCacheName);

	bool writeDataToCache(const std::string& fileName,char* data,int dataLen);

	bool readDataFromCache(const std::string& fileName,char** data,int &dataLen,int& dataType);

	bool isInCache(const std::string& fileName);

	bool commitCache(const std::string& cacheName);

	bool commitAll();
};

/*地名图层的属性类*/
class PlaceNameLayerOptions : public osgDB::ReaderWriter::Options
{
public:

	/*从sqlite中读取图层元信息*/
	void readTileMetafromSqlite(const std::string& sqlitePath);
	/*确定默认的字体*/
	void createFontStyle(const std::string& fontPath);
	virtual osg::Object* clone(const osg::CopyOp& copyop) const;
	std::string createPlaceNameTileName(int lev,int col,int row);
	std::string createPlaceNameIconName(int id);


	//设置服务缓存地址
	void setUrlCachePath(std::string urlcachepath);
	//得到服务缓存地址
	std::string getUrlCachePath();
	/*从服务器中读取元信息*/
	void getCapbilitiesfromService(std::string url);
	//设置服务Http地址
	void setUrl(std::string url);
	//得到服务Http地址
	std::string getUrl();
	//override
	PlaceNameLayerOptions(const std::string& cachePath, const std::string& fontPath, const std::string& serviceURL = "");
	~PlaceNameLayerOptions();

	bool useFileCache()  const
	{
		return false;
	}

	/*图层元信息*/
	//地名数据库位置
	std::string _pnDBName;
	//地球节点
	osgEarth::MapNode * _mapNode;
	//地名图层的级别
	int                 _minLevel;
	int                 _maxLevel;
	//显示LOD
	float                 _minLOD;
	float                 _maxLOD;

	bool _bCoverAutoHide;

	//地名图层的范围
	double              _west;
	double              _east;
	double              _south;
	double              _north;
	string				m_LayerName;
	string              m_strLayerURL;
	/*字体类型*/
	osgEarth::Symbology::Style _chapin;

	//服务Http地址
	std::string _url;
	//服务缓存位置
	std::string _urlCachePath;
	//数据类型:服务还是本地
	PlaceNameServiceType _pnServiceType;
	PlaceNameCache* _cache;
};

/*判断使用网络线程还是本地线程加载数据*/
class GeoFileLocationCallback : public osgDB::FileLocationCallback
{
public:
	GeoFileLocationCallback() { }
	virtual ~GeoFileLocationCallback() { }
	virtual bool useFileCache() const { return true; }

	//解析瓦片名称，得到瓦片级别行列号
	void parseFileName(std::string uri, int& level, int& row, int& col) const;
	//判断本地缓存中是否存在数据
	osgDB::FileLocationCallback::Location fileLocation(const std::string& filename, const osgDB::Options* options);
};

GLOBE_ENDNS

#endif







