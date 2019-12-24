
#include "Cache.h"
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

GeoGlobe::Tool::Cache::Cache()
{
	//setThreadSafeReferenceCounting(true);
}

//////////////////////////////////////////////////////////
GeoGlobe::Tool::SQLiteCache::SQLiteCache(std::string cachePath)
{
	_texNum=0;
	_geoNum=0;
	_tileNum=0;
	_commitNum=0;
	_cachePath=cachePath;
			
	createTileCache(_cachePath+"/cache.tile");
	createGeoCache(_cachePath+"/cache.geo");
	createTexCache(_cachePath+"/cache.tex");
}
//////////////////////image and elevation////////////////////
GeoGlobe::Tool::SQLiteCache::SQLiteCache(std::string cachePath,SQLiteCacheType cacheType)
{
	_cachePath=cachePath;
	_cacheType=cacheType;
	if (cacheType== eImageCache)
	{
		_imageNum=0;
		_imageCache.OpenDataBase(_cachePath.c_str());
					
	}
	if (cacheType==eElevationCache)
	{
		_elevationNum=0;
		_elevationCache.OpenDataBase(_cachePath.c_str());
	}
	if(cacheType==ePlaceNameCache)
	{
		_placenameNum=0;
		_placenameCache.OpenDataBase(_cachePath.c_str());
	}
}
///////////////////////////////////////////////////////////

void GeoGlobe::Tool::SQLiteCache::createTileCache(std::string tileCacheName)
{
	if ( !osgDB::fileExists( osgDB::getFilePath(tileCacheName) ) )
		osgDB::makeDirectoryForFile(tileCacheName);
	_tileCache.OpenDataBase(tileCacheName.c_str());
	_tileCache.CreateTileValueTable("TileTable");
	

}

void GeoGlobe::Tool::SQLiteCache::createGeoCache(std::string geoCacheName)
{
			
	if ( !osgDB::fileExists( osgDB::getFilePath(geoCacheName) ) )
		osgDB::makeDirectoryForFile(geoCacheName);
	_geoCache.OpenDataBase(geoCacheName.c_str());
	_geoCache.CreateKeyValueTable("ModelGeometry");
}

void GeoGlobe::Tool::SQLiteCache::createTexCache(std::string texCacheName)
{	
	if ( !osgDB::fileExists( osgDB::getFilePath(texCacheName) ) )
		osgDB::makeDirectoryForFile(texCacheName);
	_texCache.OpenDataBase(texCacheName.c_str());
	_texCache.CreateKeyValueTable("ModelTexture");
}

///////////////////////////////image and elevation /////////////////////////
///////////////////////////////构建影像地形地名缓存数据库/////////////////////////
void GeoGlobe::Tool::SQLiteCache::createImageCache(/*std::string imageCacheName*/)
{
	//_imageCache.OpenDataBase(imageCacheName.c_str());
	_imageCache.CreateTileMetaTable("TileMeta");
	_imageCache.CreateTileValueTable("TileTable");
}

void GeoGlobe::Tool::SQLiteCache::createElevationCache(/*std::string elevationCacheName*/)
{
	//_elevationCache.OpenDataBase(elevationCacheName.c_str());
	_elevationCache.CreateTileMetaTable("TileMeta");
	_elevationCache.CreateTileValueTable("TileTable");
}

void GeoGlobe::Tool::SQLiteCache::createPlacenameCache(std::string tablename)
{
	/*_placenameCache.CreateTileMetaTable("TileMeta");*/
	_placenameCache.CreateTileValueTable(tablename.c_str());
}

void GeoGlobe::Tool::SQLiteCache::readTileMetafromCache(TileMetaData& metadata)
{
	if (_cacheType == eImageCache)
	{
		_imageCache.ReadTileMeta(metadata);

	}
	if (_cacheType == eElevationCache)
	{
		_elevationCache.ReadTileMeta(metadata);

	}
}
/////////////////////////////////////////////////////////////////////////


std::string GeoGlobe::Tool::SQLiteCache::spliteFile(const std::string& fileName) const
{
				
	std::string::size_type slash = fileName.find_last_of("/\\");
				
	return std::string(fileName.begin()+slash+1,fileName.end());
}

void GeoGlobe::Tool::SQLiteCache::parseModelName(std::string uri,unsigned long long& geoKey,int& lod) const
{
	std::vector<std::string> vecStr;

	std::string lessExtension;

	lessExtension=osgDB::getNameLessExtension(uri);
				

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

void GeoGlobe::Tool::SQLiteCache::parseGeoTexName(std::string uri,unsigned long long& key) const
{
	std::vector<std::string> vecStr;

	std::string lessExtension;

	lessExtension=osgDB::getNameLessExtension(uri);
				

	std::string str=spliteFile(lessExtension);
					

	std::stringstream tempss;
	tempss<<str;
	tempss>>key;

}

void GeoGlobe::Tool::SQLiteCache::parseTileName(std::string uri,int& lev,int& col,int& row)
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

///////////////////image and Elevation/////////////////////////////
void GeoGlobe::Tool::SQLiteCache::parseImageTileName(const std::string uri,int& lev,int& col,int& row)
{
	//"1/0/0.1.osgearth_engine_mp_tile_image"
	sscanf(uri.c_str(),"%d/%d/%d",&lev,&col,&row);

}
void GeoGlobe::Tool::SQLiteCache::parseElevationTileName(const std::string uri,int& lev,int& col,int& row)
{
	//"1/0/0.1.osgearth_engine_mp_tile_elevation"
	sscanf(uri.c_str(),"%d/%d/%d",&lev,&col,&row);
}
void GeoGlobe::Tool::SQLiteCache::parseQuadPlacenameTileName(const std::string uri,int& lev,int& col,int& row)
{
	//"1/0/0.1.osgearth_engine_mp_tile_elevation"
	sscanf(uri.c_str(),"%d/%d/%d",&lev,&col,&row);
}
			        //////dataType
bool GeoGlobe::Tool::SQLiteCache::writeDataToCache(std::string fileName,int dataLen,int dataType,char* data)
{
	std::string ext=osgDB::getFileExtension(fileName);
	//////////////////////////////////image and elevation//////////////////////////
	if (ext=="osgearth_engine_mp_tile_image")
	{
		int lev,col,row;
		_imageNum++;
		parseImageTileName(fileName,lev,col,row);
		return _imageCache.InsertTileValueDataBatch("TileTable",lev,col,row,(unsigned char*)data,dataLen,dataType);

	}
	if (ext=="osgearth_engine_mp_tile_elevation")
	{
		int lev,col,row;
		_elevationNum++;
		parseElevationTileName(fileName,lev,col,row);
		return _elevationCache.InsertTileValueDataBatch("TileTable",lev,col,row,data,dataLen,dataType);
	}
	if (ext=="placenametile")
	{
		int lev,col,row;
		_placenameNum++;
		parseQuadPlacenameTileName(fileName,lev,col,row);
		return _placenameCache.InsertTileValueDataBatch("TileTable",lev,col,row,data,dataLen,dataType);

	}
	/////////////////////////////////////////////////////////////////////////////
	return false;
}
bool GeoGlobe::Tool::SQLiteCache::writeDataToCache(std::string tablename,std::string fileName,int dataLen,int dataType,char* data)
{
	std::string ext=osgDB::getFileExtension(fileName);
	//////////////////////////////////image and elevation//////////////////////////
	if (ext=="osgearth_engine_mp_tile_image")
	{
		int lev,col,row;
		_imageNum++;
		parseImageTileName(fileName,lev,col,row);
		return _imageCache.InsertTileValueDataBatch(tablename.c_str(),lev,col,row,(unsigned char*)data,dataLen,dataType);

	}
	if (ext=="osgearth_engine_mp_tile_elevation")
	{
		int lev,col,row;
		_elevationNum++;
		parseElevationTileName(fileName,lev,col,row);
		return _elevationCache.InsertTileValueDataBatch(tablename.c_str(),lev,col,row,data,dataLen,dataType);
	}
	if (ext=="placenametile")
	{
		int lev,col,row;
		_placenameNum++;
		parseQuadPlacenameTileName(fileName,lev,col,row);
		return _placenameCache.InsertTileValueDataBatch(tablename.c_str(),lev,col,row,data,dataLen,dataType);
					
	}
	/////////////////////////////////////////////////////////////////////////////
	return false;

}
bool GeoGlobe::Tool::SQLiteCache::readDataFromCache(std::string fileName,char** data,int& dataLen,int& dataType)//tiletable
{

	std::string ext=osgDB::getFileExtension(fileName);

	if (ext=="osgearth_engine_mp_tile_image")
	{
		int lev,col,row;
		parseImageTileName(fileName,lev,col,row);
		if(_imageCache.GetTileDataByTileKey(lev,col,row,data,dataLen,dataType))
		{
			return true;
		}
	}
	if (ext=="osgearth_engine_mp_tile_elevation")
	{
		int lev,col,row;
		parseElevationTileName(fileName,lev,col,row);
		if(_elevationCache.GetTileDataByTileKey(lev,col,row,data,dataLen,dataType))
		{
			return true;
		}
	}
	if (ext=="placenametile")
	{
		int lev,col,row;
		parseQuadPlacenameTileName(fileName,lev,col,row);
		if(_placenameCache.GetTileDataByTileKey(lev,col,row,data,dataLen,dataType))
		{
			return true;
		}
	}
	return false;
}
bool GeoGlobe::Tool::SQLiteCache::readDataFromCache(std::string tablename,std::string fileName,char** data,int& dataLen,int& dataType)
{

	std::string ext=osgDB::getFileExtension(fileName);
			
	if (ext=="osgearth_engine_mp_tile_image")
	{
		int lev,col,row;
		parseImageTileName(fileName,lev,col,row);
		if(_imageCache.GetTileDataByTileKey(tablename,lev,col,row,data,dataLen,dataType))
		{
			return true;
		}
	}
	if (ext=="osgearth_engine_mp_tile_elevation")
	{
		int lev,col,row;
		parseElevationTileName(fileName,lev,col,row);
		if(_elevationCache.GetTileDataByTileKey(tablename,lev,col,row,data,dataLen,dataType))
		{
			return true;
		}
	}
	if (ext=="placenametile")
	{
		int lev,col,row;
		parseQuadPlacenameTileName(fileName,lev,col,row);
		if(_placenameCache.GetTileDataByTileKey(tablename,lev,col,row,data,dataLen,dataType))
		{
			return true;
		}
	}
	return false;
}
////////////////////////////////////////////////////////////////

bool GeoGlobe::Tool::SQLiteCache::writeDataToCache(std::string fileName,int dataLen,char* data)
{
				
	std::string ext=osgDB::getFileExtension(fileName);
			
	if (ext=="geo")
	{
					
		ULONGLONG geoKey;
		parseGeoTexName(fileName,geoKey);
		_geoNum++;
		return _geoCache.InsertKeyValueDataBatch("ModelGeometry",geoKey,data,dataLen);
	}
	else if (ext=="tex")
	{
		ULONGLONG texKey;
		parseGeoTexName(fileName,texKey);
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
	else if (ext=="pipetile")
	{
		int lev,col,row;
		_tileNum++;
		parseTileName(fileName,lev,col,row);
		return _tileCache.InsertTileValueDataBatch("TileTable",lev,col,row,data,dataLen);

	}
}

bool GeoGlobe::Tool::SQLiteCache::readDataFromCache(std::string fileName,int &dataLen,char** data)
{

	std::string ext=osgDB::getFileExtension(fileName);
	if (ext=="geo")
	{
		ULONGLONG geoKey;
		parseGeoTexName(fileName,geoKey);
		if(_geoCache.QuerySingleKeyValueTable("ModelGeometry",geoKey,dataLen,data))
		{
			//data.assign(pGeoBuff,nSize);
			return true;
		}
	}
	else if (ext=="tex")
	{
		ULONGLONG texKey;
		parseGeoTexName(fileName,texKey);
		
		if(_texCache.QuerySingleKeyValueTable("ModelTexture",texKey,dataLen,data))
		{
		//	data.assign(pTexBuff,nSize);
			return true;
		}
					
	}
	else if (ext=="modeltile")
	{
		int lev,col,row;
		parseTileName(fileName,lev,col,row);

			if(_tileCache.GetTileDataByTileKey(lev,col,row,data,dataLen))
			{
				return true;
			}
					
	}
	else if (ext=="modeltile")
	{
		int lev,col,row;
		parseTileName(fileName,lev,col,row);

		if(_tileCache.GetTileDataByTileKey(lev,col,row,data,dataLen))
		{
			return true;
		}

	}
				
	return false;
}

bool GeoGlobe::Tool::SQLiteCache::isInCache(std::string fileName)
{
/*	fileName=_cachePath+"/"+fileName;
	if(osgDB::fileExists(fileName))
		return true;
	return false;*/

	std::string ext=osgDB::getFileExtension(fileName);
	
	if (ext=="model")
	{
		//int lod;
		ULONGLONG geoKey;
		parseGeoTexName(fileName,geoKey);
		return _modelCache.IsKeyValueExist("ModelGeometry",geoKey);
	}
	else if (ext=="modeltile")
	{
		int lev,col,row;
		parseTileName(fileName,lev,col,row);
		return _tileCache.IsTileDataExist(lev,col,row);
		//return _tileCache.InsertTileValueData("TileTable",lev,col,row,data.c_str(),data.size());
	}
	else if (ext=="pipetile")
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
		parseGeoTexName(fileName,geoKey);
		return _modelCache.IsKeyValueExist("ModelGeometry",geoKey);
	}
	else if (ext=="tex")
	{
		//int lod;
		ULONGLONG texKey;
		parseGeoTexName(fileName,texKey);
		return _modelCache.IsKeyValueExist("ModelTexture",texKey);
	}
	else if (ext=="osgearth_engine_mp_tile_image")
	{
		int lev,col,row;
		parseImageTileName(fileName,lev,col,row);
		return _imageCache.IsTileDataExist(lev,col,row);

	}
	else if (ext=="osgearth_engine_mp_tile_elevation")
	{
		int lev,col,row;
		parseElevationTileName(fileName,lev,col,row);
		return _elevationCache.IsTileDataExist(lev,col,row);

	}
	else if (ext=="placenametile")
	{
		int lev,col,row;
		parseQuadPlacenameTileName(fileName,lev,col,row);

		return _placenameCache.IsTileDataExist("WTFS_China3D_dmtile",lev,col,row);
	}
}

bool GeoGlobe::Tool::SQLiteCache::updateDataInCache(std::string fileName,int dataLen,char* data)
{
	fileName=_cachePath+"/"+fileName;
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);


	if ( !osgDB::fileExists( osgDB::getFilePath(fileName) ) )
		osgDB::makeDirectoryForFile(fileName );

	osgDB::ofstream fout;
	//std::ios::
	fout.open(fileName.c_str(), std::ios::out | std::ios::binary);
	fout.write(data,dataLen * sizeof(unsigned char));
	fout.close();
	return true;
}

bool GeoGlobe::Tool::SQLiteCache::commitAll()
{
	return true;
};
/*		void commitAllCache()
{
	_texCache.CommitCache();
	_geoCache.CommitCache();

}*/

bool GeoGlobe::Tool::SQLiteCache::commitCache(std::string cacheName)
{
/*	if(cacheName=="tex")
	{*/
	_commitNum++;
	bool commitAll=false;
	if (_commitNum>100)
	{
		commitAll=true;
		_commitNum=0;
	}
				
	if(_texNum>100||commitAll)
	{
		_texNum=0;
		_texCache.CommitCache();
	}
/*	}
	else if (cacheName=="geo")
	{*/
	if (_geoNum>30||commitAll)
	{
		_geoNum=0;
		_geoCache.CommitCache();
	}

	if (_tileNum>10||commitAll)
	{
		_tileNum=0;
		_tileCache.CommitCache();
	}
	//影像超过50时则批插入缓存
	if (_imageNum>50||commitAll)
	{
		_imageNum=0;
		//_imageCache.CommitCache();
	}
	if (_elevationNum>50||commitAll)
	{
		_elevationNum=0;
		_elevationCache.CommitCache();
	}

	if (_placenameNum>50||commitAll)
	{
					
					
		_placenameNum=0;
		_placenameCache.CommitCache();

	}
	/*}*/

	return false;

};


