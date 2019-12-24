#include "PlaceNameTileOptions.h"
#include "Tool/CodeTransformHelp.h"
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgEarth/MapNode>
#include <LayerBox.h>
#include "OsgCoreSingleton.h"

GLOBE_NS

//解析瓦片名称，得到瓦片级别行列号
void GeoFileLocationCallback::parseFileName(std::string uri, int& level, int& row, int& col) const
{
	std::vector<std::string> vecStr;

	std::string lessExtension;
	std::string ext;
	//vecStr->push_back(ext);
	lessExtension = osgDB::getNameLessExtension(uri);

	for (int i = 0; i < 3; i++)
	{
		ext = osgDB::getFileExtension(lessExtension);
		vecStr.push_back(ext);
		lessExtension = osgDB::getNameLessExtension(lessExtension);
	}

	level = atoi(std::string(vecStr[2]).c_str());
	row = atoi(std::string(vecStr[1]).c_str());
	col = atoi(std::string(vecStr[0]).c_str());
	//dataSetFileName=vecStr[3];
	//modelGeometrysFiles=vecStr[4];
}


//判断本地缓存中是否存在数据
osgDB::FileLocationCallback::Location GeoFileLocationCallback::fileLocation(const std::string& filename, const osgDB::Options* options)
{
	Location result = LOCAL_FILE;

	int level, row, col;
	parseFileName(filename, level, col, row);
	PlaceNameLayerOptions* pnLayerOptions = (PlaceNameLayerOptions*)options;

	if (level + 2 < pnLayerOptions->_minLevel)
	{
		return result;
	}

	const osgEarth::Profile* profile = pnLayerOptions->_mapNode->getMap()->getProfile();
	osgEarth::TileKey mapKey(level, col, row, profile);

	for (unsigned i = 0; i < 4; ++i)
	{
		osgEarth::TileKey childKey = mapKey.createChildKey(i);
		int lev = childKey.getLevelOfDetail() + 1;
		int col = childKey.getTileX();
		int row = childKey.getTileY();
		std::string pnTileFile = pnLayerOptions->createPlaceNameTileName(lev, col, row);
		if (!pnLayerOptions->_cache->isInCache(pnTileFile))
		{
			result = REMOTE_FILE;
			break;
		}
	}

	return result;
}



PlaceNameCache::PlaceNameCache(const std::string& cachePath)
{
	_tileNum = 0;
	_iconNum = 0;
	_cachePath = cachePath;

	createPlaceNameCache(_cachePath);
	//setThreadSafeReferenceCounting(true);
}

PlaceNameCache::~PlaceNameCache()
{
	commitAll();
}

/*解析数据名称*/
std::string PlaceNameCache::spliteFile(const std::string& fileName) const
{
	std::string::size_type slash = fileName.find_last_of("/\\");
	return std::string(fileName.begin() + slash + 1, fileName.end());
}

void PlaceNameCache::parseTileName(const std::string& uri,int& lev,int& col,int& row)
{
	std::vector<std::string> vecStr;
	std::string lessExtension = osgDB::getNameLessExtension(uri);

	for (int i = 0; i < 3; i++)
	{

		std::string str = spliteFile(lessExtension);
		vecStr.push_back(str);
		lessExtension = osgDB::getFilePath(lessExtension);
	}

	lev = atoi(vecStr[2].c_str());
	col = atoi(vecStr[1].c_str());
	row = atoi(vecStr[0].c_str());
}

void PlaceNameCache::parseIDName(const std::string& uri,int& id) const
{
	std::vector<std::string> vecStr;
	std::string lessExtension;

	lessExtension = osgDB::getNameLessExtension(uri);

	std::string str = spliteFile(lessExtension);
	std::stringstream tempss;
	tempss << str;
	tempss >> id;
}

/*构造地名缓存*/
void PlaceNameCache::createPlaceNameCache(const std::string& pnCacheName)
{
	if ( !osgDB::fileExists( osgDB::getFilePath(pnCacheName) ) )
		osgDB::makeDirectoryForFile(pnCacheName);

	_pnCache.OpenDataBase(pnCacheName.c_str());
	_pnCache.CreateTileValueTable("WTFS_China3D_dmtile");

	sqlite3 * m_pSqliteDB=_pnCache.m_pSqliteDB;
				
	std::string strSQLHead = "create table ";
	std::string strSQLTail = " (ID INTEGER PRIMARY KEY, PIC1 BLOB, TYPE INTEGER)";
	std::string strSQL = strSQLHead + "WTFSICONLIB" + strSQLTail;
	if (SQLITE_OK == sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0))
	{
		strSQLHead = "create index keyindex on ";
		strSQLTail = " (ID)";
		strSQL = strSQLHead + "WTFSICONLIB" + strSQLTail;
		sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0);
	}
	//_pnCache.CreateKeyValueTable("WTFSICONLIB");
}

bool PlaceNameCache::writeDataToCache(const std::string& fileName,char* data,int dataLen)
{
	std::string ext=osgDB::getFileExtension(fileName);

	if (ext=="placenametile")
	{
		osgEarth::Threading::ScopedReadLock shared( _cacheMutex );

		int lev,col,row;
		_tileNum++;

		parseTileName(fileName,lev,col,row);

		return _pnCache.InsertTileValueDataBatch("WTFS_China3D_dmtile",lev,col,row,data,dataLen,10);
	}
	//else if (ext=="icon")
	//{
	//	Threading::ScopedWriteLock exclusive( _cacheMutex );
	//	int id;
	//	parseIDName(fileName,id);
	//	_iconNum++;
	//	//return _pnCache.in
	//}
	return false;
}

bool PlaceNameCache::readDataFromCache(const std::string& fileName,char** data,int &dataLen,int& dataType)
{
	//Threading::ScopedReadLock shared( _cacheMutex );

	std::string ext=osgDB::getFileExtension(fileName);
	if (ext=="placenametile")
	{
		int lev,col,row;
		parseTileName(fileName,lev,col,row);

		string strTileName;
		_pnCache.ReadTileName(strTileName);

		string strTileTableName = strTileName + "_dmtile";

		return _pnCache.GetTileDataByTileKey(strTileTableName,lev,col,row,data,dataLen,dataType);
	}
	else if (ext=="icon")
	{
		int id;
		parseIDName(fileName,id);

		return _pnCache.GetPlaceNameIconDataByID(id,data,dataType,dataLen);
	}

	return false;
}

bool PlaceNameCache::isInCache(const std::string& fileName)
{
	osgEarth::Threading::ScopedReadLock shared(_cacheMutex);
	int lev, col, row;

	parseTileName(fileName, lev, col, row);

	return _pnCache.IsTileDataExist("WTFS_China3D_dmtile", lev, col, row);
}

bool PlaceNameCache::commitCache(const std::string& cacheName)
{
	//if(cacheName=="tex")
	//{

	//Threading::ScopedWriteLock exclusive( _cacheMutex );

	if(_tileNum>30)
	{
		_tileNum=0;
		_pnCache.CommitCache();
	}

	return false;
};

bool PlaceNameCache::commitAll()
{
	osgEarth::Threading::ScopedWriteLock exclusive(_cacheMutex);
	_pnCache.CommitCache();

	return true;
}



/*从sqlite中读取图层元信息*/
void PlaceNameLayerOptions::readTileMetafromSqlite(const std::string& sqlitePath)
{
	m_strLayerURL = sqlitePath;

	/*std::string  spath=*(this->rootPath())+*(this->sqlitePath());*/
	//	std::string  spath=*(this->sqlitePath());
	sqlite3 *db;
	sqlite3_open(sqlitePath.c_str(),&db);


	//__int64 minlevel=0;
	//__int64 maxlevel=0;
	sqlite3_stmt* stmt;

	std::string sql;
	//sql="select itemdata from TileMeta where itemname='ValidTopLevel'";

	//if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
	//{
	//	while(sqlite3_step(stmt)==SQLITE_ROW)
	//	{
	//		_minLevel=sqlite3_column_int(stmt,0);
	//	}
	//}
	//sqlite3_finalize(stmt);


	//sql="select itemdata from TileMeta where itemname='ValidBottomLevel'";
	//if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
	//{
	//	while(sqlite3_step(stmt)==SQLITE_ROW)
	//	{
	//		_maxLevel=sqlite3_column_int(stmt,0);
	//	}
	//}
	//sqlite3_finalize(stmt);


	sql="select value from metatable where name='XMin'";
	if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			_west=sqlite3_column_double(stmt,0);
		}
	}
	sqlite3_finalize(stmt);


	sql="select value from metatable where name='XMax'";
	if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			_east=sqlite3_column_double(stmt,0);
		}
	}
	sqlite3_finalize(stmt);

	sql="select value from metatable where name='YMin'";
	if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			_south=sqlite3_column_double(stmt,0);
		}
	}
	sqlite3_finalize(stmt);

	sql="select value from metatable where name='YMax'";
	if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			_north=sqlite3_column_double(stmt,0);
		}
	}
	sqlite3_finalize(stmt);

	sql="select value from metatable where name='Name'";
	if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			m_LayerName=(char*)sqlite3_column_text(stmt,0);
		}
	}
	sqlite3_finalize(stmt);

	//_minLevel=(unsigned int)minlevel;
	//_maxLevel=(unsigned int)maxlevel;

	sql="select WTFS_LEVEL from WTFSMetaTable";

	if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			//text类型sqlite3_column_type(stmt,0);
			unsigned char* levelData=(unsigned char*)sqlite3_column_text(stmt,0);

			std::stringstream ss;
			ss<<levelData;
			osgEarth::Config doc;
			doc.fromXML(ss);
			osgEarth::Config levels=doc.child("levels");

			osgEarth::Config cfminlevel=*(levels.children().begin());
			std::string s;
			s=cfminlevel.value();

			_minLevel=atoi(s.c_str());

			osgEarth::Config cfmaxlevel=levels.children().back();
			s=cfmaxlevel.value();

			_maxLevel=atoi(s.c_str());

		}
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
}
/*确定默认的字体*/
void PlaceNameLayerOptions::createFontStyle(const std::string& fontPath)
{
	//_chapin.getOrCreate<osgEarth::Symbology::IconSymbol>()->url()->setLiteral("E:/osgEarthdata/placenamePicture/14.bmp");
	//_chapin.getOrCreate<osgEarth::Symbology::IconSymbol>()->scale()=osgEarth::Symbology::NumericExpression(0.2);
	_chapin.getOrCreate<osgEarth::Symbology::IconSymbol>()->alignment()=osgEarth::Symbology::IconSymbol::ALIGN_CENTER_TOP;
	_chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->alignment()=osgEarth::Symbology::TextSymbol::ALIGN_CENTER_BOTTOM;
	_chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->font()=fontPath;
	_chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->size()=12.0/*poi.FontSize()*/;
	_chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color()= osgEarth::Symbology::Color::Yellow;
	//_chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=Color::White;
	_chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding()=osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
}

osg::Object* PlaceNameLayerOptions::clone(const osg::CopyOp& copyop) const
{
	return (osg::Object*)(this);
}

std::string PlaceNameLayerOptions::createPlaceNameTileName(int lev,int col,int row)
{
	std::stringstream ss;
	ss << "PlaceName/" << lev << "/" << col << "/" << row << ".placenametile";

	return ss.str();
}

std::string PlaceNameLayerOptions::createPlaceNameIconName(int id)
{
	std::stringstream ss;
	ss << "PlaceName/" << id << ".icon";

	return ss.str();
}


//设置服务Http地址
void PlaceNameLayerOptions::setUrl(std::string url)
{
	if (url != "")
	{
		_url = url;
	}
	return;
}
//得到服务Http地址
std::string PlaceNameLayerOptions::getUrl()
{
	return _url;
}

//设置服务缓存地址
void PlaceNameLayerOptions::setUrlCachePath(std::string urlcachepath)
{
	if (urlcachepath != "")
	{
		_urlCachePath = urlcachepath;
	}
	return;
}

//得到服务缓存地址
std::string PlaceNameLayerOptions::getUrlCachePath()
{
	return _urlCachePath;
}

/*从服务器中读取元信息*/
void PlaceNameLayerOptions::getCapbilitiesfromService(std::string url)
{
	//http://192.168.40.133:9010/SANWEI/wtfs/GetCapabilities
	//http://192.168.40.133:9010/SANWEI/wtfs

	osgEarth::URI capUrl;
	url+="/GetCapabilities";
	capUrl= osgEarth::URI(url);
	osgEarth::HTTPResponse response= osgEarth::HTTPClient::get(capUrl.full());
	std::stringstream ss;
	std::string metaName=osgDB::getFilePath(_urlCachePath)+"/"+"pntilemeta";

	if (response.isOK() && response.getNumParts()>0)
	{
		std::string data=capUrl.getString();
		ss<<data;

		if ( !osgDB::fileExists( osgDB::getFilePath(metaName) ) )
			osgDB::makeDirectoryForFile(metaName );

		osgDB::ofstream fout;
		//std::ios::
		fout.open(metaName.c_str(), std::ios::out | std::ios::binary);
		fout.write(data.c_str(),data.size() * sizeof(unsigned char));
		fout.close();
	}
	else if(osgDB::fileExists(metaName))
	{
		std::ifstream input/*( fileName.c_str() )*/;
		input.open(metaName.c_str(), std::ios::in | std::ios::binary);
		if ( input.is_open() )
		{
			ss << input.rdbuf();
		}
	}
	
	osgEarth::Config doc;
	doc.fromXML(ss);

	if (doc.empty() || doc.children().empty())
		return;

	const osgEarth::Config wtfsData = doc.child("getcapabilitiesresponse").child("servicecapabilities").child("data").child("wtfsdata");
	bool findBoundBox = false;
	bool findLevel = false;
	for (osgEarth::ConfigSet::const_iterator i = wtfsData.children().begin(); i != wtfsData.children().end(); i++)
	{
		if (i->key() == "boundbox")
		{
			osgEarth::Config boundBox = *i;
			_west = atof(boundBox.child("minx").value().c_str());
			_east = atof(boundBox.child("maxx").value().c_str());
			_south = atof(boundBox.child("miny").value().c_str());
			_north = atof(boundBox.child("maxy").value().c_str());
			findBoundBox = true;
		}
		if (i->key() == "wtfslevels")
		{
			osgEarth::Config wtfsLevels = *i;
			_minLevel = atoi(wtfsLevels.children().begin()->value().c_str());
			_maxLevel = atoi(wtfsLevels.children().back().value().c_str());
			findLevel = true;
		}
		if (findBoundBox&&findLevel)
		{
			break;
		}
	}
}

//override
PlaceNameLayerOptions::PlaceNameLayerOptions(const std::string& cachePath,const std::string&  fontPath, const std::string&  serviceURL)
{
	_mapNode = OsgCoreSingleton::Instance()->MapNode();
	_minLevel = 0;
	_maxLevel = 23;
	_west = -180.0;
	_east = 180.0;
	_north = 90.0;
	_south = -90.0;
	_url = serviceURL;
	_bCoverAutoHide = true;
	createFontStyle(fontPath);

	if (_url == "")
	{
		_pnDBName = cachePath;

#ifdef _WIN32
		_pnDBName = CodeHelp::String_To_UTF8(cachePath);
#endif
		readTileMetafromSqlite(_pnDBName);
		_cache = new PlaceNameCache(_pnDBName);
		_pnServiceType = eLocalPlaceName;
	}
	else
	{
		setUrlCachePath(cachePath);
		getCapbilitiesfromService(_url);

		_cache = new PlaceNameCache(_pnDBName);
		setFileLocationCallback(new GeoFileLocationCallback());
		_pnServiceType = eGeoGlobeTilePlaceName;
	}
}

PlaceNameLayerOptions::~PlaceNameLayerOptions()
{
	if (_cache)
	{
		delete _cache;
		_cache = NULL;
	}
}

GLOBE_ENDNS







