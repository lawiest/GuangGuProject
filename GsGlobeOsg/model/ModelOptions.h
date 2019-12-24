#ifndef GEOGLOBE_MODEL_MODELOPTIONS
#define GEOGLOBE_MODEL_MODELOPTIONS 1

#include "PreConfig.h"

#include <osgDB/ReaderWriter>
#include <osgEarth/MapNode>
#include <Tool/SQLiteOP.h>
#include <Model/CFeature.h>
#include <osgEarth/XmlUtils>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osg/Referenced>
#include <sstream>
#include <iomanip>

//#include <Tool/FileSystemCache.h>
#include <Tool/Cache.h>
#include <osg/Referenced>
#include <Tool/GeoTileCapabilitiesReader.h>
#include <Tool/Download.h>
#include <osg/Texture2D>
#include <Tool/GeoFCSAttributeCommon.h>
//#include <time.h>
#include "Tool/CodeTransformHelp.h"

using namespace GeoGlobe::Tool;
using namespace std;

GG_MODEL_NS

enum ModelDataSetType
{
	//本地模型
	LOCALMODEL,
	//服务模型
	SERVICEMODEL
};

/*模型缓存，主要用于服务的缓存数据*/
class ModelCache:public osg::Referenced
{
public:

	/*下载的纹理数量*/
	int                       _texNum;
	/*下载的几何数量*/
	int                       _geoNum;
	/*下载的瓦片数量*/
	int                       _tileNum;
	/*纹理缓存*/
	CSQLiteOP                 _texCache;
	/*几何缓存*/
	CSQLiteOP                 _geoCache;
	/*瓦片缓存*/
	CSQLiteOP                 _tileCache;
	//缓存名称
	std::string               _cacheName;
	//缓存位置
	std::string               _cachePath;
	//互斥锁
	Threading::ReadWriteMutex _cacheMutex;

	/*初始化*/
	ModelCache(std::string cachePath,std::string cacheName)
	{
		_texNum=0;
		_geoNum=0;
		_tileNum=0;
		//_commitNum=0;

		_cachePath=cachePath;
		_cacheName=cacheName;

		createTileCache(_cachePath+"/"+_cacheName+".tile");
		createGeoCache(_cachePath+"/"+_cacheName+".geo");
		createTexCache(_cachePath+"/"+_cacheName+".tex");

	}
	~ModelCache()
	{
		commitAll();
	}

	std::string spliteFile(const std::string& fileName) const
	{
		std::string::size_type slash = fileName.find_last_of("/\\");
		return std::string(fileName.begin()+slash+1,fileName.end());
	}

	/*根据瓦片ID得到瓦片的key值*/
	void parseKeyName(std::string uri,unsigned long long& key) const
	{
		std::vector<std::string> vecStr;
		std::string lessExtension;

		lessExtension=osgDB::getNameLessExtension(uri);

		std::string str=spliteFile(lessExtension);
		std::stringstream tempss;

		tempss<<str;
		tempss>>key;
	}

	/*根据瓦片ID得到瓦片的Level col row*/
	void parseTileName(std::string uri,int& lev,int& col,int& row)
	{
		std::vector<std::string> vecStr;
		std::string lessExtension;

		lessExtension=osgDB::getNameLessExtension(uri);

		for (int i=0;i<3;i++)
		{
			std::string str=spliteFile(lessExtension);
			vecStr.push_back(str);
			lessExtension=osgDB::getFilePath(lessExtension);
		}

		lev= atoi(vecStr[2].c_str());
		col= atoi(vecStr[1].c_str());
		row= atoi(vecStr[0].c_str());
	}

	/*生成缓存数据库*/
	/*构造瓦片缓存*/
	void createTileCache(std::string tileCacheName)
	{
		if ( !osgDB::fileExists( osgDB::getFilePath(tileCacheName) ) )
			osgDB::makeDirectoryForFile(tileCacheName);
		_tileCache.OpenDataBase(tileCacheName.c_str());
		_tileCache.CreateTileValueTable("TileTable");
	}

	/*构造几何缓存*/
	void createGeoCache(std::string geoCacheName)
	{
		if ( !osgDB::fileExists( osgDB::getFilePath(geoCacheName) ) )
			osgDB::makeDirectoryForFile(geoCacheName);

		_geoCache.OpenDataBase(geoCacheName.c_str());
		_geoCache.CreateKeyValueTable("ModelGeometry");
	}

	/*构造纹理缓存*/
	void createTexCache(std::string texCacheName)
	{
		if ( !osgDB::fileExists( osgDB::getFilePath(texCacheName) ) )
			osgDB::makeDirectoryForFile(texCacheName);

		_texCache.OpenDataBase(texCacheName.c_str());
		_texCache.CreateKeyValueTable("ModelTexture");
	}

	/*将数据写入缓存中*/
	bool writeDataToCache(std::string fileName,int dataLen,char* data)
	{
		Threading::ScopedReadLock shared( _cacheMutex );
		std::string ext=osgDB::getFileExtension(fileName);

		if (ext=="geo")
		{
			ULONGLONG geoKey;
			parseKeyName(fileName,geoKey);
			_geoNum++;
			return _geoCache.InsertKeyValueDataBatch("ModelGeometry",geoKey,data,dataLen);
		}
		else if (ext=="tex")
		{
			ULONGLONG texKey;
			parseKeyName(fileName,texKey);
			_texNum++;

			return _texCache.InsertKeyValueDataBatch("ModelTexture",texKey,data,dataLen);
		}
		else if (ext=="modeltile")
		{
			int lev,col,row;
			_tileNum++;
			parseTileName(fileName,lev,col,row);
			return _tileCache.InsertTileValueDataBatch("TileTable",lev,col,row,data,dataLen);
		}
	}

	/*从缓存中读出数据*/
	bool readDataFromCache(std::string fileName,int &dataLen,char** data)
	{
		Threading::ScopedReadLock shared( _cacheMutex );
		std::string ext=osgDB::getFileExtension(fileName);
		if (ext=="geo")
		{
			ULONGLONG geoKey;
			parseKeyName(fileName,geoKey);
			return _geoCache.QuerySingleKeyValueTable("ModelGeometry",geoKey,dataLen,data);

		}
		else if (ext=="tex")
		{
			ULONGLONG texKey;
			parseKeyName(fileName,texKey);

			return _texCache.QuerySingleKeyValueTable("ModelTexture",texKey,dataLen,data);
		}
		else if (ext=="modeltile")
		{
			int lev,col,row;
			parseTileName(fileName,lev,col,row);

			return _tileCache.GetTileDataByTileKey(lev,col,row,data,dataLen);
		}

		return false;
	}
	/*判断数据是否在缓存里*/
	bool isInCache(std::string fileName)
	{
		Threading::ScopedReadLock shared( _cacheMutex );
		std::string ext=osgDB::getFileExtension(fileName);

		if (ext=="modeltile")
		{
			int lev,col,row;
			parseTileName(fileName,lev,col,row);
			return _tileCache.IsTileDataExist(lev,col,row);
			//return _tileCache.InsertTileValueData("TileTable",lev,col,row,data.c_str(),data.size());
		}
		else if (ext=="geo")
		{
			//int lod;
			ULONGLONG geoKey;
			parseKeyName(fileName,geoKey);
			return _geoCache.IsKeyValueExist("ModelGeometry",geoKey);
		}
		else if (ext=="tex")
		{
			//int lod;
			ULONGLONG texKey;
			parseKeyName(fileName,texKey);
			return _texCache.IsKeyValueExist("ModelTexture",texKey);
		}
		else if (ext=="model")
		{
			//int lod;
			ULONGLONG geoKey;
			parseKeyName(fileName,geoKey);
			return _geoCache.IsKeyValueExist("ModelGeometry",geoKey);
		}
	}

	/*提交数据，批提交*/
	bool commitCache(std::string cacheName)
	{
		//如果加载的纹理数据大于100则批量提交到纹理数据库中
		//if(_texNum>100)
		{
			Threading::ScopedWriteLock exclusive( _cacheMutex );
			_texNum=0;
			_texCache.CommitCache();
		}
		//如果加载的几何数据大于30则批量提交到几何数据库中
		//if (_geoNum>30)
		{
			Threading::ScopedWriteLock exclusive( _cacheMutex );
			_geoNum=0;
			_geoCache.CommitCache();
		}
		//如果加载的瓦片数据大于10则批量提交到瓦片数据库中
		//if (_tileNum>10)
		{
			Threading::ScopedWriteLock exclusive( _cacheMutex );
			_tileNum=0;
			_tileCache.CommitCache();
		}
		return false;
	};

	//提交所有缓存
	bool commitAll()
	{
		Threading::ScopedWriteLock exclusive( _cacheMutex );
		_tileCache.CommitCache();
		_geoCache.CommitCache();
		_texCache.CommitCache();

		return true;
	}
};

/*模型数据库，主要读取本地数据*/
class DataBase:public osg::Referenced
{
public:
	DataBase(){}
	//纹理数据库
	std::vector<osg::ref_ptr<CSQLiteOP> >        _texDBs;
	//几何数据库
	std::vector<osg::ref_ptr<CSQLiteOP> >        _geoDBs;
	//对象数据库
	std::vector<osg::ref_ptr<CSQLiteOP> >        _fcDBs;
	//瓦片数据库
	osg::ref_ptr<CSQLiteOP>                     _tileDB; 
};

class Model;

/*基本属性类*/
class BaseModelOptions : public osgDB::ReaderWriter::Options
{
public:
	BaseModelOptions();
	virtual ~BaseModelOptions();

	/*服务器URL地址*/
	std::string                     _serviceUrl;
	/*图层名称*/
	std::string                     _layerName;
	/*缓存路径*/
	std::string                     _cachePath;
	std::string                     m_strLayerURL;
	std::string                     m_strDatasetName;
	/*本地数据库*/
	osg::ref_ptr<DataBase>          _dataBase;
    /*全局设置*/
	osg::ref_ptr<Setting>           _setting;
	osg::ref_ptr<Shared>            _shared;

    /*本地featureclass数据库位置*/
	std::vector<std::string>          _fcDBNames;
	//osg::ref_ptr<DownLoadTool>      _downloadTool;
    /*服务缓存*/
	osg::ref_ptr<ModelCache> _cache;

	//是否注册子节点
	bool _bShaderGenerator;

    /*解析本地GMDX文件生成数据库*/
	bool parseGMDX(std::string gmdxFile);

	/*生成和解析数据的唯一名称，必须有名称才能用osg自带的分页数据加载功能*/
	/*构造纹理唯一ID*/
	std::string createTextureName(ULONGLONG texKey)
	{
		return Stringify() <<_layerName<<"/"<<"Texture/"<<texKey<<".tex";
	}
	/*构造几何唯一ID*/
	std::string createGeometryName(ULONGLONG geoKey)
	{
		return Stringify() <<_layerName<<"/"<<"Geometry/"<<geoKey<<".geo";
	}
	/*构造模型唯一ID*/
	std::string createModelName(ULONGLONG geoKey,int lod)
	{
		return Stringify() <<_layerName<<"/"<<"Model/"<<geoKey<<"/"<<lod<<".model";
	}
	/*构造Lod模型唯一ID*/
	std::string createLodModelName(ULONGLONG featureKey)
	{
		return Stringify() <<_layerName<<"/"<<"LodModel/"<<featureKey <<".lodmodel";

	};
	/*构造模型瓦片唯一ID*/
	std::string createModelTileName(int lev,int col,int row)
	{
		return Stringify() <<_layerName<<"/"<<"ModelTile/"<<lev<<"/"<<col<<"/"<<row<<".modeltile";
	}

	//根据“\\”将字符串分成多个小字符串
	std::string spliteFile(const std::string& fileName) const
	{
		std::string::size_type slash = fileName.find_last_of("/\\");
		return std::string(fileName.begin()+slash+1,fileName.end());
	}

	//根据模型名称获取模型的几何Key和LOD级别
	void parseModelName(std::string uri,unsigned long long& geoKey,int& lod) const
	{
		std::vector<std::string> vecStr;

		std::string lessExtension;

		lessExtension=osgDB::getNameLessExtension(uri);
		//ext=osgDB::getFileExtension(lessExtension);

		for (int i=0;i<2;i++)
		{

			std::string str=spliteFile(lessExtension);
			vecStr.push_back(str);
			lessExtension=osgDB::getFilePath(lessExtension);
		}
		
		lod= atoi(vecStr[0].c_str());

		std::stringstream tempss;
		tempss<<vecStr[1].c_str();
		tempss>>geoKey;

	}

	//解析几何Key
	void parseKeyName(std::string uri,unsigned long long& key) const
	{
		std::vector<std::string> vecStr;

		std::string lessExtension;

		lessExtension=osgDB::getNameLessExtension(uri);
		std::string str=spliteFile(lessExtension);
		std::stringstream tempss;
		tempss<<str;
		tempss>>key;

	}

	/*从本地数据库中读取数据*/
	//瓦片数据
	bool getTileDataFromDB(int level,int col,int row,char ** ppTileBuff,int &nSize)
	{
		if( _dataBase->_tileDB->GetTileDataByTileKey(level,col,row,ppTileBuff,nSize))
		return true;
		return false;

	}

	//几何数据
	bool GetGeoDataFromDB(ULONGLONG geoKey,int &dataLen,char** geoData)
	{
		for (int i=0;i< _dataBase->_geoDBs.size();i++)
		{
			if( _dataBase->_geoDBs[i]->QuerySingleKeyValueTable("ModelGeometry",geoKey,dataLen,geoData))
				return true;
		}
		return false;
	}

	//纹理数据
	bool GetTexDataFromDB(ULONGLONG texKey,int &dataLen,char** texData)
	{
		for (int i=0;i< _dataBase->_texDBs.size();i++)
		{
			if( _dataBase->_texDBs[i]->QuerySingleKeyValueTable("ModelTexture",texKey,dataLen,texData))
				return true;
		}
		return false;

	}

	//feature数据
	CFeature* getFeatureFromDB(ULONGLONG fcKey)
	{
		for(int i=0;i< _dataBase->_fcDBs.size();i++)
		{
			Buffer buffer;
				_dataBase->_fcDBs[i]->GetLodByFeatureKey(fcKey,buffer);
			osg::ref_ptr<CFeature> feature=new CFeature;
			if(feature->readBuffer(buffer))
				return feature.release();
		}
	}

	/*从本地数据库中查询数据属性信息*/
	bool queryModelInfoFromLocal(LONGLONG fcKey,osgEarth::Features::AttributeTable &attr,osgEarth::Bounds& bds)
	{
		for(int i=0;i<_fcDBNames.size();i++)
		{
			if(!osgDB::fileExists(_fcDBNames[i]))
			continue;

			//fcKey=216307349680291840;
			//__android_log_print(ANDROID_LOG_ERROR, "queryPipeLineInfoFromLocal", "%d",1);
			GeoGlobe::Tool::GeoFCSAttributeCommon gfcsAttriCom(_fcDBNames[i]);
			int oid = gfcsAttriCom.getOIDfromFeatureID(fcKey);
			if(oid==-1)
					continue;
			//	__android_log_print(ANDROID_LOG_ERROR, "queryPipePointInfoFromLocal", "%d",oid);
			//获取包围盒
			//osgEarth::Bounds bds;
			gfcsAttriCom.getBoundBoxbyoidGeometry(oid,bds);
			gfcsAttriCom.getAttributeTablebyFID(fcKey,attr);
			//				__android_log_print(ANDROID_LOG_ERROR, "queryPipeLineInfoFromLocal", "%d",attr.size());
			return true;
		}
		return false;
	}
};

/*模型图层属性类*/
class ModelLayerOptions: public BaseModelOptions
{
public:
	ModelLayerOptions(osgEarth::MapNode* mapNode,std::string url,std::string layerName="",std::string cachePath="");
	virtual ~ModelLayerOptions();
	
public:
	/*地球节点，可以用于读取地球的投影信息*/
	osg::ref_ptr<osgEarth::MapNode> _mapNode;

	/*最小级别*/
	int                 _minLevel;
	/*最大级别*/
	int                 _maxLevel;
	/*范围*/
	double              _west;
	double              _east;
	double              _south;
	double              _north;
	bool                _valid;
	/*是否初始化*/
	bool                _bInitialized;
	/*空间参考用于坐标转换*/
	osg::ref_ptr<osgEarth::SpatialReference>          _srs;

	/*从本地读取数据集元信息*/
	bool readTileMetafromLocal(std::string sqlitePath);

	/*确定哪些模型需要高亮*/
	void heightlightModel(LONGLONG fcKey,bool hl)
	{
		stringstream ss;
		ss<<fcKey;
		//_setting->_heightLightList.push_back(ss.str());
		_setting->_heightLightMap.insert(make_pair(ss.str(),hl));
	}
	
	/*从服务器读取数据集元信息*/
	bool readTileMetafromService(std::string url)
	{
		std::string capUrl = url + std::string("/GetCapabilities") ;

		osg::ref_ptr<GeoGlobe::Tool::GeoTileCapabilities> capabilities;

		if ( osgDB::containsServerAddress(capUrl ) )
		{
			HTTPResponse response = HTTPClient::get( capUrl );
			if ( response.isOK() && response.getNumParts() > 0 )
			{
				std::string data=response.getPartAsString(0);
				std::string metaName=_cachePath+"/"+"tilemeta";

				if ( !osgDB::fileExists( osgDB::getFilePath(metaName) ) )
					osgDB::makeDirectoryForFile(metaName );

				osgDB::ofstream fout;
				fout.open(metaName.c_str(), std::ios::out | std::ios::binary);
				fout.write(data.c_str(),data.size() * sizeof(unsigned char));
				fout.close();
				std::stringstream buf( data);
				capabilities = GeoTileCapabilitiesReader::read(buf);
			}
		}

		if (capabilities==NULL||!capabilities.valid()||capabilities->getTileData().empty())
		{
			std::string metaData;
			std::string fileName=_cachePath+"/"+"tilemeta";

			if(osgDB::fileExists(fileName))
			{
				std::ifstream input/*( fileName.c_str() )*/;
				input.open(fileName.c_str(), std::ios::in | std::ios::binary);

				if ( input.is_open() )
				{
					std::stringstream buf;
					buf << input.rdbuf();
					capabilities = GeoTileCapabilitiesReader::read(buf);
				}
			}
		}

		if (!capabilities.valid())
		{
			OE_WARN << "[osgEarth::WMTS] Unable to read GeoTileCapabilities." << std::endl;
			return false;
		}
		else
		{
			if(capabilities->getTileData().empty())
			{
				OE_WARN << "[osgEarth::WMTS] Unable to Get TileData in Capabilities." << std::endl;
				return false;
			}
			OE_INFO << "[osgEarth::WMTS] Got capabilities from " << capUrl << std::endl;
			for(int i=0;i<capabilities->getTileData().size();i++)
			{
				GeoGlobe::Tool::TileData tdata;
				tdata=capabilities->getTileDataAt(i);
				_minLevel=tdata.toplevel;
				_maxLevel=tdata.bottomlevel;
				_west=tdata.bbox.west;
				_east=tdata.bbox.east;
				_south=tdata.bbox.south;
				_north=tdata.bbox.north;
				m_strDatasetName = capabilities->getName();
			}

		}
		return true;
	}

	virtual osg::Object* clone(const osg::CopyOp& copyop) const 
	{
		return (osg::Object*)(this);
	}

public:
	class FileLocationCallback : public osgDB::FileLocationCallback
	{
	public:
		FileLocationCallback() { }

		/** dtor */
		virtual ~FileLocationCallback() { }
		//解析数据名
		void parseFileName(std::string uri,int& level,int& row,int& col)
		{
			std::vector<std::string> vecStr;

			std::string lessExtension;
			std::string ext;
			//vecStr->push_back(ext);
			lessExtension=osgDB::getNameLessExtension(uri);

			for (int i=0;i<3;i++)
			{
				ext=osgDB::getFileExtension(lessExtension);
				vecStr.push_back(ext);
				lessExtension=osgDB::getNameLessExtension(lessExtension);
			}


			level= atoi(std::string(vecStr[2]).c_str());
			row= atoi(std::string(vecStr[1]).c_str());
			col= atoi(std::string(vecStr[0]).c_str());
		}
		//判断数据是否存在本地
		Location fileLocation(const std::string& filename, const osgDB::Options* options)
		{
			//	return  LOCAL_FILE;
			Location result = LOCAL_FILE;

			int level, row, col;
			parseFileName(filename,level,col,row);
			ModelLayerOptions* modelLayerOptions=(ModelLayerOptions*) options;

			if (level+2<modelLayerOptions->_minLevel)
			{
				return result;
			}

			const osgEarth::Profile* profile = modelLayerOptions->_mapNode->getMap()->getProfile();
			osgEarth::TileKey mapKey( level, col, row, profile );


			for( unsigned i=0; i<4; ++i )
			{
				TileKey childKey = mapKey.createChildKey( i );
				int lev=childKey.getLevelOfDetail()+1;
				int col=childKey.getTileX();
				int row=childKey.getTileY();
				std::string modelTileFile=modelLayerOptions->createModelTileName(lev,col,row);
				if ( !modelLayerOptions->_cache->isInCache(modelTileFile) )
				{
					result = REMOTE_FILE;
					break;
				}
			}
			return result;
		}

		bool useFileCache() const { return false; }
	};

};

/*模型瓦片属性类*/
class ModelTileOptions : public BaseModelOptions
{
public:
	ModelTileOptions()
	{};
	ModelTileOptions(TileKey tileKey,ModelLayerOptions* options)
	{
	//				_downloadTool=options->_downloadTool;
		_shared=options->_shared;
		_setting=options->_setting;
		_dataBase=options->_dataBase;
		_mapNode=options->_mapNode;
		_tileKey=tileKey;
		_layerName=options->_layerName;
		_serviceUrl=options->_serviceUrl;

        _cache=options->_cache;
        _srs=options->_srs;
        _fcDBNames=options->_fcDBNames;
        //setThreadSafeRefUnref(true);

	}
	virtual ~ModelTileOptions()
	{

	}
	virtual osg::Object* clone(const osg::CopyOp& copyop) const 
	{
		return (osg::Object*)(this);
	}

	/*地球节点，可以用于读取地球的投影信息*/
	osgEarth::MapNode*  _mapNode;
	/*模型瓦片Key值*/
	TileKey             _tileKey;
	/*空间参考用于坐标转换*/
	osgEarth::SpatialReference*          _srs;
};

/*Lod模型属性类*/
class LodModelOptions : public BaseModelOptions
{
public:
			
	osg::ref_ptr<Shared>            _lodModelShared;
	LodModelOptions(osgEarth::MapNode* mapNode,std::string url,ULONGLONG fcKey/*,DataType dataType=LOCAL*/)
	{
				
		_lodModelShared=new Shared;
		_shared=new Shared;
		_setting=new Setting;

		_mapNode=mapNode;
		parseGMDX(url);

		_feature=getFeatureFromDB(fcKey);
		computeCenterPos();
		//osg::Vec3d local;
		//_mapNode->getMapSRS()->transformFromWorld(_centerPos,local);
		//_mapNode->getMapSRS()->transformToWorld(osg::Vec3(local.x(),local.y(),local.z()+20),_centerPos2);
	}

	LodModelOptions(ModelTileOptions* options,CFeature* feature)
	{
	//				_downloadTool=options->_downloadTool;
		_lodModelShared=new Shared;
		_shared=options->_shared;
	    _setting=options->_setting;
		_dataBase=options->_dataBase;
		_mapNode=options->_mapNode;
		_feature=feature;
		_srs=options->_srs;
			_fcDBNames=options->_fcDBNames;

			//计算模型中心
		computeCenterPos();
		_layerName=options->_layerName;
		_serviceUrl=options->_serviceUrl;

		_cache=options->_cache;
		_cachePath=options->_cachePath;
		if (_serviceUrl!="")
		{
			setFileLocationCallback(new lodModelFileLocationCallback);
		}
		//_centerPos=pos;
		//setThreadSafeRefUnref(true);
	}
	//计算模型的中心点
	void computeCenterPos()
	{

		osg::Vec3d localPos=osg::Vec3d(_feature->getLocation().dLongitude,_feature->getLocation().dLatitude,/*_feature->m_MaxVertex.z()*/_feature->getLocation().dElevation);
		_mapNode->getMapSRS()->transformToWorld(localPos,_centerPos);
	}

	~LodModelOptions()
	{
	}

	virtual osg::Object* clone(const osg::CopyOp& copyop) const 
	{
		return (osg::Object*)(this);
	}

	/*用于确定数据是否已存在本地缓存中，从而确定采用网络线程还是本地线程加载数据*/
	class lodModelFileLocationCallback : public osgDB::FileLocationCallback
	{
	public:
		lodModelFileLocationCallback() { }

		/** dtor */
		virtual ~lodModelFileLocationCallback() { }

		void parseFileName(std::string uri,unsigned long long& geoKey,int& lod) const
		{
			std::vector<std::string> vecStr;

			std::string lessExtension;
			lessExtension=osgDB::getNameLessExtension(uri);
			//ext=osgDB::getFileExtension(lessExtension);

			for (int i=0;i<2;i++)
			{
				std::string ext=osgDB::getFileExtension(lessExtension);
				vecStr.push_back(ext);
				lessExtension=osgDB::getNameLessExtension(lessExtension);
			}

			lod= atoi(vecStr[0].c_str());

			std::stringstream tempss;
			tempss<<vecStr[1].c_str();
			tempss>>geoKey;

		}

		Location fileLocation(const std::string& filename, const osgDB::Options* options)
		{
			unsigned long long geoKey;
			int lod=-1;
			//parseFileName(filename,geoKey,lod);
			LodModelOptions* lodModelOptions=(LodModelOptions*) options;
			lodModelOptions->parseModelName(filename,geoKey,lod);
			std::string geoName=lodModelOptions->createGeometryName(geoKey);

			//判断数据是否在缓存中
			if(lodModelOptions->_cache->isInCache(geoName))
				return LOCAL_FILE;
			else
				return REMOTE_FILE;
		}

		bool useFileCache() const { return false; }
	};

	/*模型的feature*/
	osg::ref_ptr<CFeature> _feature;
	/*地球节点*/
	osgEarth::MapNode* _mapNode;
	/*参考坐标*/
	osgEarth::SpatialReference*          _srs;
	/*Lod模型中心位置*/
	osg::Vec3d           _centerPos;
};

/*模型属性类*/
class ModelOptions : public BaseModelOptions
{
public:

	ModelOptions(LodModelOptions* options,ULONGLONG geoKey,int lod)
	{
		//				_downloadTool=options->_downloadTool;
		_shared=options->_shared;
		_setting=options->_setting;
		_dataBase=options->_dataBase;
		_mapNode=options->_mapNode;
		_centerPos=options->_centerPos;
		//		_centerPos2=options->_centerPos2;
		_srs=options->_srs;
		_serviceUrl=options->_serviceUrl;
		_geoKey=geoKey;
		_lod=lod;

		_layerName=options->_layerName;
		_cache=options->_cache;

		_feature = options->_feature;

		_bShaderGenerator = options->_bShaderGenerator;
		//setThreadSafeRefUnref(true);
				
	}
	~ModelOptions()
	{

	}

	virtual osg::Object* clone(const osg::CopyOp& copyop) const 
	{
		return (osg::Object*)(this);
	}

	/*地球节点，可以用于读取地球的投影信息*/
	osgEarth::MapNode* _mapNode;
	/*当前模型的中心位置*/
	osg::Vec3           _centerPos;//lat,lon,elevation
	/*当前模型的几何Key*/
	ULONGLONG _geoKey;
	/*当前模型的LOD级别*/
	int _lod;
	/*空间参考用于坐标转换*/
	osgEarth::SpatialReference*          _srs;

	osg::ref_ptr<CFeature> _feature;
};

GG_MODEL_ENDNS

#endif //GEOGLOBE_MODEL_MODELOPTIONS
