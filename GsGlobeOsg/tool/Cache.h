#pragma once

#include <string>
#include <osg/Referenced>
#include "SQLiteOP.h"
#include <Base/Service.h>
#include "SqliteTileCache.h"

namespace GeoGlobe
{
	namespace Tool
	{
		class DataBase:public osg::Referenced
		{
		public:
			DataBase(){}
			std::vector<osg::ref_ptr<CSQLiteOP> >        _texDBs;
			std::vector<osg::ref_ptr<CSQLiteOP> >        _geoDBs;
			std::vector<osg::ref_ptr<CSQLiteOP> >        _fcDBs;
			osg::ref_ptr<CSQLiteOP>                     _tileDB; 
			osg::ref_ptr<CSQLiteOP>                     _symDB;
		};

        /*全局设置*/
		class Setting:public osg::Referenced
		{
		public:
			Setting()
			{
				_multTex=false;
				_visualRange=2000;
				_lodFactor=1.0f;
			}
			bool       _multTex;
			float      _visualRange;
			float      _lodFactor;
			std::multimap<std::string,bool> _heightLightMap;
		};

		class Shared:public osg::Referenced
		{
		public:
			Shared(){}

			OpenThreads::Mutex                      _texMutex;
			OpenThreads::Mutex                      _geoMutex;
		};
		
		class Cache:public osg::Referenced
		{
		public:
			std::string                _cachePath;
			Cache();
			//~Cache()
			//{
			//commitAll();
			//}

			virtual bool writeDataToCache(std::string fileName,int dataLen,char* data)=0;
			//////////////////////////////////datatype
			virtual bool writeDataToCache(std::string fileName,int dataLen,int dataType,char* data)=0;
			virtual bool writeDataToCache(std::string tablename,std::string fileName,int dataLen,int dataType,char* data)=0;
			virtual bool readDataFromCache(std::string fileName,char** data,int& dataLen,int& dataType)=0;
			virtual bool readDataFromCache(std::string tablename,std::string fileName,char** data,int& dataLen,int& dataType)=0;
			///////////////////////////////////////////////////
			virtual bool readDataFromCache(std::string fileName,int& dataLen,char** data)=0;
			virtual bool isInCache(std::string fileName)=0;
			virtual bool updateDataInCache(std::string fileName,int dataLen,char* data)=0;
			virtual bool commitCache(std::string cacheName)=0;
			virtual bool commitAll()=0;

		};
		enum SQLiteCacheType
		{
			eModelCache,
			eImageCache,
			eElevationCache,
			ePlaceNameCache
		};

		/*基于SQLite实现的缓存，读取影像、地形、地名的缓冲，主要是为了实现数据的批入库*/
		/*模型、管线的缓存则也是通过类似方式实现，详细可参照modelCache类等*/
		class SQLiteCache:public Cache
		{
		public:
			std::string                _cacheName;
			std::string                _cachePath;
			OpenThreads::Mutex         _cacheMutex;
			int                        _commitNum;
			////////////////////////model////////////////////
			CSQLiteOP                 _texCache;
			CSQLiteOP                 _geoCache;
			CSQLiteOP                 _tileCache;
			CSQLiteOP                 _modelCache;
			int                       _texNum;
			int                       _geoNum;
			int                       _tileNum;
			/////////////////image and elevation////////////////////////
             //影像缓存
			//CSQLiteOP  _imageCache;
			CSqliteTileCache _imageCache;
			//地形缓存
			CSQLiteOP  _elevationCache;
            //地名缓存
			CSQLiteOP  _placenameCache;
			int _imageNum;
			int _elevationNum;
			int _placenameNum;
			SQLiteCacheType _cacheType;
			SQLiteCache(){}

			//////////////////////////////////////////////////////////
			SQLiteCache(std::string cachePath);
			//////////////////////image and elevation////////////////////
			SQLiteCache(std::string cachePath,SQLiteCacheType cacheType);
			///////////////////////////////////////////////////////////

			void createTileCache(std::string tileCacheName);

			void createGeoCache(std::string geoCacheName);

			void createTexCache(std::string texCacheName);
			///////////////////////////////image and elevation /////////////////////////
			///////////////////////////////构建影像地形地名缓存数据库/////////////////////////
			void createImageCache(/*std::string imageCacheName*/);

			void createElevationCache(/*std::string elevationCacheName*/);

			void createPlacenameCache(std::string tablename);
			void readTileMetafromCache(TileMetaData& metadata);
			/////////////////////////////////////////////////////////////////////////


			std::string spliteFile(const std::string& fileName) const;

			void parseModelName(std::string uri,unsigned long long& geoKey,int& lod) const;

			void parseGeoTexName(std::string uri,unsigned long long& key) const;

			void parseTileName(std::string uri,int& lev,int& col,int& row);

			///////////////////image and Elevation/////////////////////////////
			void parseImageTileName(const std::string uri,int& lev,int& col,int& row);
			void parseElevationTileName(const std::string uri,int& lev,int& col,int& row);
			void parseQuadPlacenameTileName(const std::string uri,int& lev,int& col,int& row);
			                   //////dataType
			bool writeDataToCache(std::string fileName,int dataLen,int dataType,char* data);
			bool writeDataToCache(std::string tablename,std::string fileName,int dataLen,int dataType,char* data);
			bool readDataFromCache(std::string fileName,char** data,int& dataLen,int& dataType);
			bool readDataFromCache(std::string tablename,std::string fileName,char** data,int& dataLen,int& dataType);
			////////////////////////////////////////////////////////////////

			bool writeDataToCache(std::string fileName,int dataLen,char* data);

			bool readDataFromCache(std::string fileName,int &dataLen,char** data);

			bool isInCache(std::string fileName);

			bool updateDataInCache(std::string fileName,int dataLen,char* data);

			bool commitAll();
			/*void commitAllCache();*/

			bool commitCache(std::string cacheName);
		};
	}
}
