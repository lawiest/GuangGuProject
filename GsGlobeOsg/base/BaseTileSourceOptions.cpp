#pragma once

#include "Tool/CodeTransformHelp.h"
#include "BaseTileSourceOptions.h"
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgEarth/HTTPClient>

GLOBE_NS

BaseTileSourceOptions::BaseTileSourceOptions() :_wmtsVersion("1.0.0"),
												m_strToken("6c7643ee1518d566efd7c460a575f59d")
{
}
//对图层参数类的初始化，确定本地或者服务，影像或者地形数据
//本地图层参数类的初始化
BaseTileSourceOptions::BaseTileSourceOptions(std::string dbPath, GeoGlobe::Tool::SQLiteCacheType cachetype, StoreType servicetype) : _wmtsVersion("1.0.0")
{
	_metadata.toplevel = 0; //图层元信息设定，最顶层默认为0
	_metadata.bottomlevel = 23;//最底层默认为23
	_metadata.bbox.east = 180;//数据覆盖的经纬度范围默认东经为180
	_metadata.bbox.west = -180;//默认西经为-180
	_metadata.bbox.north = 90;//默认北纬为90
	_metadata.bbox.south = -90;//默认南纬为-90

	m_strDataSetPath = dbPath;
	m_strToken = "6c7643ee1518d566efd7c460a575f59d";

#ifdef _WIN32
	_cachePath = CodeHelp::String_To_UTF8(dbPath);//缓存路径有两种，一种是本地数据，另一种是服务数据的缓存，都可以作为数据库的路径处理
#endif

	_cacheType = cachetype;//缓存为影像或者地形数据
	if (servicetype == eLocal)//如果为本地数据，则直接读取本地文件的元信息
	{
		_servicetype = eLocal;
		readTileMetafromLocal(_cachePath.value(), _metadata);
	}
}

BaseTileSourceOptions::BaseTileSourceOptions(std::string dbPath,//缓存数据存放路径
	std::string url,//服务地址
	GeoGlobe::Tool::SQLiteCacheType cachetype,//缓存类型，影像或者地形数据
	StoreType servicetype)//确定服务类型，吉奥瓦片服务、天地图服务等
	:_wmtsVersion("1.0.0")
{
	_metadata.toplevel = 0; //图层元信息设定，最顶层默认为0
	_metadata.bottomlevel = 23;//最底层默认为23
	_metadata.bbox.east = 180;//数据覆盖的经纬度范围默认东经为180
	_metadata.bbox.west = -180;//默认西经为-180
	_metadata.bbox.north = 90;//默认北纬为90
	_metadata.bbox.south = -90;//默认南纬为-90

	m_strToken = "6c7643ee1518d566efd7c460a575f59d";

	m_strDataSetPath = url;
	_cachePath = dbPath;//缓存路径有两种，一种是本地数据，另一种是服务数据的缓存，都可以作为数据库的路径处理
	_url = url;//设置服务路径
	_cacheType = cachetype;//缓存为影像或者地形数据

	bool exist = false;
	if (osgDB::fileExists(dbPath))//如果缓存数据已经存在，在直接读取元信息
	{
		exist = true;
		readTileMetafromLocal(dbPath, _metadata);
	}

	if (servicetype == eWMTS)//设定服务类型为天地图服务
	{
		readMetaFromServer(url, _metadata);
		_servicetype = eWMTS;
	}

	if (servicetype == eGeoGlobeTile)//设定服务类型吉奥瓦片服务
	{
		_servicetype = eGeoGlobeTile;
		if (!exist)//若缓存路径不存在
		{
			if (!osgDB::fileExists(osgDB::getFilePath(dbPath)))
				osgDB::makeDirectoryForFile(dbPath);

			getCapbilitiesfromGeoGlobeTile(_metadata);
			writeTileMetatoCache(dbPath, _metadata);//将元信息写入缓存文件
		}
	}
	if (servicetype == eReadMap)
	{
		_servicetype = eReadMap;
	}
}

BaseTileSourceOptions::~BaseTileSourceOptions()
{

}

void BaseTileSourceOptions::readMetaFromServer(std::string dbPath, TileMetaData& metadata)
{
	char sep = dbPath.find_first_of('?') == std::string::npos ? '?' : '&';

	string capUrl = dbPath;
	if (sep == '?')
	{
		capUrl = dbPath + sep +
			std::string("SERVICE=") + "WMTS" +
			std::string("&VERSION=") + "1.0.0" +
			std::string("&REQUEST=GetCapabilities")+
			std::string("&tk=")+m_strToken;
	}

	osgEarth::HTTPResponse response = osgEarth::HTTPClient::get(capUrl);
	std::string data = response.getPartAsString(0);
	std::istringstream in(data);
	osgEarth::Util::WMTSCapabilities* _capWMTS = osgEarth::Util::WMTSCapabilitiesReader::read(in);

	//读取成功才做一些计算。zhangli yuzhou
	if (NULL != _capWMTS)
	{
		std::vector<TileMatrix> vecTileMatrixSet = _capWMTS->getMatrixSetAt(0).TileMatrixs;

		int nTop = 21;
		int nBottom = -1;
		for (std::vector<TileMatrix>::iterator it = vecTileMatrixSet.begin();it != vecTileMatrixSet.end();it++)
		{
			nTop = min(atoi((*it).ID.c_str()), nTop);
			nBottom = max(atoi((*it).ID.c_str()), nBottom);
		}

		metadata.strDataSetName = _capWMTS->getLayerAt(0).Title;
		metadata.toplevel = nTop;
		metadata.bottomlevel = nBottom;
		metadata.bbox.east = _capWMTS->getLayerAt(0).wgs84bound.east;
		metadata.bbox.south = _capWMTS->getLayerAt(0).wgs84bound.south;
		metadata.bbox.west = _capWMTS->getLayerAt(0).wgs84bound.west;
		metadata.bbox.north = _capWMTS->getLayerAt(0).wgs84bound.north;
	}

}

bool BaseTileSourceOptions::readTileMetafromLocal(std::string dbPath, TileMetaData& metadata)
{
	sqlite3* m_pSqliteDB;
	sqlite3_open(dbPath.c_str(), &m_pSqliteDB);

	if (!m_pSqliteDB)
		return false;

	std::string sql;
	sql = "select itemdata from TileMeta where itemname='ValidTopLevel'";//获取数据最顶层
	sqlite3_stmt* stmt;

	if (SQLITE_OK == sqlite3_prepare(m_pSqliteDB, sql.c_str(), -1, &stmt, 0))
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			metadata.toplevel = sqlite3_column_int(stmt, 0) - 1;
		}

		if (metadata.toplevel < 0)
		{
			metadata.toplevel = 0;
		}
	}

	sql = "select itemdata from TileMeta where itemname='ValidBottomLevel'";//数据最底层
	if (SQLITE_OK == sqlite3_prepare(m_pSqliteDB, sql.c_str(), -1, &stmt, 0))
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			metadata.bottomlevel = sqlite3_column_int(stmt, 0) - 1;
		}
		if (metadata.bottomlevel < 0)
		{
			metadata.bottomlevel = 0;
		}
	}

	sql = "select itemdata from TileMeta where itemname='West'";//西经
	if (SQLITE_OK == sqlite3_prepare(m_pSqliteDB, sql.c_str(), -1, &stmt, 0))
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			metadata.bbox.west = sqlite3_column_double(stmt, 0);
		}
	}


	sql = "select itemdata from TileMeta where itemname='East'";//东经
	if (SQLITE_OK == sqlite3_prepare(m_pSqliteDB, sql.c_str(), -1, &stmt, 0))
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			metadata.bbox.east = sqlite3_column_double(stmt, 0);
		}
	}


	sql = "select itemdata from TileMeta where itemname='South'";//南纬
	if (SQLITE_OK == sqlite3_prepare(m_pSqliteDB, sql.c_str(), -1, &stmt, 0))
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			metadata.bbox.south = sqlite3_column_double(stmt, 0);
		}
	}

	sql = "select itemdata from TileMeta where itemname='North'";//北纬
	if (SQLITE_OK == sqlite3_prepare(m_pSqliteDB, sql.c_str(), -1, &stmt, 0))
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			metadata.bbox.north = sqlite3_column_double(stmt, 0);
		}
	}

	sql = "select itemdata from TileMeta where itemname='DataSetType'";
	if (SQLITE_OK == sqlite3_prepare(m_pSqliteDB, sql.c_str(), -1, &stmt, 0))
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			metadata.nDataSetType = sqlite3_column_int(stmt, 0);
		}
	}

	sql = "select itemdata from TileMeta where itemname='DataSetName'";
	if (SQLITE_OK == sqlite3_prepare(m_pSqliteDB, sql.c_str(), -1, &stmt, 0))
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			metadata.strDataSetName = (char*)sqlite3_column_text(stmt, 0);
		}
	}

	sql = "select itemdata from TileMeta where itemname='DEMValueType'";
	if (SQLITE_OK == sqlite3_prepare(m_pSqliteDB, sql.c_str(), -1, &stmt, 0))
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			std::string temp;
			int size = sqlite3_column_bytes(stmt, 0);
			temp.assign((char*)sqlite3_column_text(stmt, 0), size);

			metadata.DEMDataType = temp;
		}
	}

	sqlite3_finalize(stmt);
	sqlite3_close(m_pSqliteDB);
	return true;
}

void BaseTileSourceOptions::getCapbilitiesfromGeoGlobeTile(TileMetaData& metadata)//从服务器端获取数据元信息
{
	//http://192.168.42.178:9010/YINGXIANG/services/tile/GetCapabilities 获取capabilities
	//http://192.168.42.178:9010/YINGXIANG/DataServer?l=17&x=222&y=33333 瓦片请求
	//http://192.168.42.37:7005/dem/services/tile/GetCapabilities
	//http://192.168.42.37:7005/dem/DataServer?l=17&x=4444&y=444444

	osgEarth::URI capUrl = this->url().value();

	capUrl = osgEarth::URI(this->url()->full() + std::string("/GetCapabilities"));//拼写获取元信息的请求
	osg::ref_ptr<osgEarth::Util::GeoGlobeTileCapabilities> capabilities;//声明存放元信息的类

	//读取获得的capability文件，并存储
	if (cachePath().isSet())
	{
		std::string capCachePath = osgDB::getNameLessAllExtensions(cachePath().value()) + "cap.xml";
		capabilities = osgEarth::Util::GeoGlobeTileCapabilitiesReader::read(capUrl.full(), capCachePath, 0L);
	}
	else
	{
		capabilities = osgEarth::Util::GeoGlobeTileCapabilitiesReader::read(capUrl.full(), 0L); //getOptions() );
	}

	if (!capabilities.valid() || capabilities->getTileMetaData().empty())
		return;

	for (int i = 0;i < capabilities->getTileMetaData().size();i++)
	{
		TileMetaData tdata;
		tdata = capabilities->getTileMetaDataAt(i);
		metadata.toplevel = tdata.toplevel - 1;
		metadata.bottomlevel = tdata.bottomlevel - 1;
		metadata.bbox.west = tdata.bbox.west;
		metadata.bbox.east = tdata.bbox.east;
		metadata.bbox.north = tdata.bbox.north;
		metadata.bbox.south = tdata.bbox.south;
		metadata.DEMDataType = tdata.DEMDataType;
		metadata.strDataSetName = capabilities->getName();

		if (tdata.tile == "model")
			metadata.nDataSetType = 2;
		else if (tdata.tile == "terrain")
			metadata.nDataSetType = 1;
		else if (tdata.tile == "image")
			metadata.nDataSetType = 0;
	}

	return;
}

//将获得元信息写入缓存文件，sqlite文件
bool BaseTileSourceOptions::writeTileMetatoCache(std::string dbPath, TileMetaData& metadata)
{
	sqlite3* m_pSqliteDB;
	sqlite3_open(dbPath.c_str(), &m_pSqliteDB);
	if (!m_pSqliteDB) return false;

	std::string strSQL = "create table TileMeta(itemname text,itemdata blob,itemtype integer,itemid integer,primary key(itemname))";
	if (SQLITE_OK == sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0))
	{
		sqlite3_stmt *pStmt = 0;
		int rv;
		std::string sql;
		sql = "insert into TileMeta values(?,?,?,?)";
		if (SQLITE_OK == sqlite3_prepare(m_pSqliteDB, sql.c_str(), -1, &pStmt, 0))
		{
			sqlite3_bind_text(pStmt, 1, "ValidTopLevel", strlen("ValidTopLevel"), SQLITE_STATIC);
			sqlite3_bind_int(pStmt, 2, _metadata.toplevel + 1);
			sqlite3_bind_int(pStmt, 3, 3);
			sqlite3_bind_int(pStmt, 4, 12);
			rv = sqlite3_step(pStmt);
			if (SQLITE_DONE != rv && SQLITE_OK != rv && SQLITE_ROW != rv)
			{
				sqlite3_finalize(pStmt);
				sqlite3_close(m_pSqliteDB);
				remove(dbPath.c_str());
				return false;
			}


			sqlite3_reset(pStmt);
			sqlite3_bind_text(pStmt, 1, "ValidBottomLevel", strlen("ValidBottomLevel"), 0);
			sqlite3_bind_int(pStmt, 2, _metadata.bottomlevel + 1);
			sqlite3_bind_int(pStmt, 3, 3);
			sqlite3_bind_int(pStmt, 4, 13);
			rv = sqlite3_step(pStmt);
			if (SQLITE_DONE != rv && SQLITE_OK != rv && SQLITE_ROW != rv)
			{
				sqlite3_finalize(pStmt);
				sqlite3_close(m_pSqliteDB);
				remove(dbPath.c_str());
				return false;
			}

			sqlite3_reset(pStmt);
			sqlite3_bind_text(pStmt, 1, "West", strlen("West"), 0);
			sqlite3_bind_double(pStmt, 2, _metadata.bbox.west);
			sqlite3_bind_int(pStmt, 3, 9);
			sqlite3_bind_int(pStmt, 4, 3);
			rv = sqlite3_step(pStmt);
			if (SQLITE_DONE != rv && SQLITE_OK != rv && SQLITE_ROW != rv)
			{
				sqlite3_finalize(pStmt);
				sqlite3_close(m_pSqliteDB);
				remove(dbPath.c_str());
				return false;
			}

			sqlite3_reset(pStmt);
			sqlite3_bind_text(pStmt, 1, "East", strlen("East"), 0);
			sqlite3_bind_double(pStmt, 2, _metadata.bbox.east);
			sqlite3_bind_int(pStmt, 3, 9);
			sqlite3_bind_int(pStmt, 4, 4);
			rv = sqlite3_step(pStmt);
			if (SQLITE_DONE != rv && SQLITE_OK != rv && SQLITE_ROW != rv)
			{
				sqlite3_finalize(pStmt);
				sqlite3_close(m_pSqliteDB);
				remove(dbPath.c_str());
				return false;
			}

			sqlite3_reset(pStmt);
			sqlite3_bind_text(pStmt, 1, "South", strlen("South"), 0);
			sqlite3_bind_double(pStmt, 2, _metadata.bbox.south);
			sqlite3_bind_int(pStmt, 3, 9);
			sqlite3_bind_int(pStmt, 4, 5);
			rv = sqlite3_step(pStmt);
			if (SQLITE_DONE != rv && SQLITE_OK != rv && SQLITE_ROW != rv)
			{
				sqlite3_finalize(pStmt);
				sqlite3_close(m_pSqliteDB);
				remove(dbPath.c_str());
				return false;
			}

			sqlite3_reset(pStmt);
			sqlite3_bind_text(pStmt, 1, "North", strlen("North"), 0);
			sqlite3_bind_double(pStmt, 2, _metadata.bbox.north);
			sqlite3_bind_int(pStmt, 3, 9);
			sqlite3_bind_int(pStmt, 4, 6);
			rv = sqlite3_step(pStmt);
			if (SQLITE_DONE != rv && SQLITE_OK != rv && SQLITE_ROW != rv)
			{
				sqlite3_finalize(pStmt);
				sqlite3_close(m_pSqliteDB);
				remove(dbPath.c_str());
				return false;
			}

			sqlite3_reset(pStmt);
			sqlite3_bind_text(pStmt, 1, "DEMValueType", strlen("DEMValueType"), 0);
			sqlite3_bind_text(pStmt, 2, _metadata.DEMDataType.c_str(), _metadata.DEMDataType.size(), 0);
			sqlite3_bind_int(pStmt, 3, 10);
			sqlite3_bind_int(pStmt, 4, 103);
			rv = sqlite3_step(pStmt);
			if (SQLITE_DONE != rv && SQLITE_OK != rv && SQLITE_ROW != rv)
			{
				sqlite3_finalize(pStmt);
				sqlite3_close(m_pSqliteDB);
				remove(dbPath.c_str());
				return false;
			}

			sqlite3_reset(pStmt);
			sqlite3_bind_text(pStmt, 1, "DataSetType", strlen("DataSetType"), SQLITE_STATIC);
			sqlite3_bind_int(pStmt, 2, _metadata.nDataSetType);
			sqlite3_bind_int(pStmt, 3, 3);
			sqlite3_bind_int(pStmt, 4, 12);
			rv = sqlite3_step(pStmt);
			if (SQLITE_DONE != rv && SQLITE_OK != rv && SQLITE_ROW != rv)
			{
				sqlite3_finalize(pStmt);
				sqlite3_close(m_pSqliteDB);
				remove(dbPath.c_str());
				return false;
			}

			sqlite3_reset(pStmt);
			sqlite3_bind_text(pStmt, 1, "DataSetName", strlen("DataSetName"), 0);
			sqlite3_bind_text(pStmt, 2, _metadata.strDataSetName.c_str(), _metadata.strDataSetName.size(), 0);
			sqlite3_bind_int(pStmt, 3, 11);
			sqlite3_bind_int(pStmt, 4, 104);
			rv = sqlite3_step(pStmt);
			if (SQLITE_DONE != rv && SQLITE_OK != rv && SQLITE_ROW != rv)
			{
				sqlite3_finalize(pStmt);
				sqlite3_close(m_pSqliteDB);
				remove(dbPath.c_str());
				return false;
			}

			sqlite3_finalize(pStmt);
			sqlite3_close(m_pSqliteDB);
			return true;
		}//insert

	}//create
	sqlite3_close(m_pSqliteDB);
	remove(dbPath.c_str());
	return false;
}

GLOBE_ENDNS








