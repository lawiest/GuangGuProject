#include "ModelOptions.h"

GG_MODEL_NS

#pragma region BaseModelOptions
BaseModelOptions::BaseModelOptions()
{
	_cache=NULL;
	_dataBase=NULL;
	_bShaderGenerator = false;
}
BaseModelOptions::~BaseModelOptions()
{
}
void ParseChildName(XmlElement *pRoot, const char *strParent, const char *strChild, std::string strDir, std::vector<std::string> &vtNames)
{
	osg::ref_ptr<XmlElement>  modelgeometrysfiles = pRoot->getSubElement(strParent);
	osgEarth::XmlNodeList modelgeometryfile=modelgeometrysfiles->getSubElements(strChild);

	for( XmlNodeList::const_iterator i = modelgeometryfile.begin(); i != modelgeometryfile.end(); i++ )
	{
		if ( i->get()->isElement() )
		{
			XmlElement* e = (XmlElement*)i->get();
			vtNames.push_back(strDir + e->getAttrs()["path"]);
		}
	}
}
/*解析本地GMDX文件生成数据库*/
bool BaseModelOptions::parseGMDX(std::string gmdxFile)
{
	_dataBase=new DataBase();
	_layerName=osgDB::getStrippedName(gmdxFile);

	std::string directory = gmdxFile.substr(0,gmdxFile.find_last_of('/'));
	
	osgEarth::ReadResult result = URI(gmdxFile).readString();

	if (!result.succeeded())
		return false;
		
	std::string tileDBName;
	std::vector<std::string> geoDBNames;
	std::vector<std::string> texDBNames;
	std::vector<std::string> fcDBNames;

	std::string str = result.getString();
	std::stringstream in( str.c_str()  );

	osg::ref_ptr< XmlDocument > doc = XmlDocument::load( in );
	if (!doc.valid()) 
		return false;

	osg::ref_ptr<XmlElement>  modeldataset= doc->getSubElement( "modeldataset" );
	osg::ref_ptr<XmlElement>  datasetfile = modeldataset->getSubElement( "datasetfile" );
	tileDBName=directory+datasetfile->getAttrs()["path"].c_str();

	ParseChildName(modeldataset.get(), "modelgeometrysfiles", "modelgeometryfile", directory, geoDBNames);
	ParseChildName(modeldataset.get(), "modeltexturesfiles", "modeltexturefile", directory, texDBNames);
	ParseChildName(modeldataset.get(), "featureclassesfiles", "featureclassfile", directory, fcDBNames);

	_dataBase->_tileDB = new CSQLiteOP;

	if(!_dataBase->_tileDB->OpenDataBase(CodeHelp::String_To_UTF8(tileDBName).c_str()))
		return false;

	_dataBase->_tileDB->DataBaseName(tileDBName.c_str());

	for(int i=0;i<geoDBNames.size();i++)
	{
		CSQLiteOP* geoDB=new CSQLiteOP;
		if(!geoDB->OpenDataBase(CodeHelp::String_To_UTF8(geoDBNames[i]).c_str())) 
			continue;

		_dataBase->_geoDBs.push_back(geoDB);
	}

	for(int i=0;i<texDBNames.size();i++)
	{
		CSQLiteOP* texDB=new CSQLiteOP;
		if(!texDB->OpenDataBase(CodeHelp::String_To_UTF8(texDBNames[i]).c_str())) 
			continue;

		_dataBase->_texDBs.push_back(texDB);
	}

	return true;
}
#pragma endregion

#pragma region ModelLayerOptions  
ModelLayerOptions::ModelLayerOptions(osgEarth::MapNode* mapNode,std::string url,std::string layerName,std::string cachePath)
{
	_bInitialized = false;
	_setting=new Setting;
	_setting->_lodFactor=1.0f;
	_shared=new Shared;
	//_srs=mapNode->getMapSRS()->get();
	_srs=SpatialReference::create(mapNode->getMapSRS()->getHorizInitString(),mapNode->getMapSRS()->getVertInitString());

	//_downloadTool=new DownLoadTool;

	_mapNode=mapNode;
    _layerName=layerName;

    //判断是本地数据还是服务数据
	if(cachePath=="")
	{
		m_strLayerURL = url;

		//_gmdxFile=url;
		//从本地数据库获取元信息
		_bInitialized = parseGMDX(url);
		//__android_log_print(ANDROID_LOG_ERROR, "ModelOption", "_bInitialized: %d", (int)_bInitialized);

		//_modelCache=new ModelCache(url,dataType);
		readTileMetafromLocal( _dataBase->_tileDB->DataBaseName());
	}
	else
	{
		m_strLayerURL = url;
		//从服务器获取元信息
		_serviceUrl=url;

		_cachePath=cachePath;
		//构建本地缓存
		_cache=new ModelCache(cachePath,_layerName);
		_bInitialized = readTileMetafromService(url);
		if(!_bInitialized)
		{
			time_t timel;
			time(&timel);
			std::string logtxt = "/storage/emulated/0/osgEarth/DATA/ERROR/log.txt";

			if ( !osgDB::fileExists( osgDB::getFilePath(logtxt) ) )
				osgDB::makeDirectoryForFile(logtxt);

			std::ofstream fout(logtxt.c_str());
			fout<< asctime(localtime(&timel))<<" "<<"模型:"<<_layerName.c_str()<<" URL:"<<url.c_str();
			fout.close();
		}

		setFileLocationCallback(new FileLocationCallback);
	}
}

ModelLayerOptions::~ModelLayerOptions()
{
}

bool ModelLayerOptions::readTileMetafromLocal(std::string sqlitePath)
{
	if(!osgDB::fileExists(sqlitePath))
		return false;

	sqlite3 *db;
	int iErrorCode = sqlite3_open(CodeHelp::String_To_UTF8(sqlitePath).c_str(),&db);

	if (SQLITE_OK != iErrorCode)
		return false;

	long long  minlevel=0;
	long long  maxlevel=0;
	sqlite3_stmt* stmt;

	std::string sql;
	sql="select itemdata from TileMeta where itemname = ?";
	if (SQLITE_OK!=sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
		return false;

	std::string strFieldName = "ValidTopLevel";
	sqlite3_bind_text(stmt, 1, strFieldName.c_str(), strFieldName.size(), 0);
	if (sqlite3_step(stmt) == SQLITE_ROW)
		_minLevel = sqlite3_column_int(stmt,0);
	sqlite3_reset(stmt);

	strFieldName = "ValidBottomLevel";
	sqlite3_bind_text(stmt, 1, strFieldName.c_str(), strFieldName.size(), 0);
	if (sqlite3_step(stmt) == SQLITE_ROW)
		_maxLevel = sqlite3_column_int(stmt,0);

	sqlite3_reset(stmt);
	strFieldName = "West";
	sqlite3_bind_text(stmt, 1, strFieldName.c_str(), strFieldName.size(), 0);
	if (sqlite3_step(stmt) == SQLITE_ROW)
		_west = sqlite3_column_double(stmt,0);

	sqlite3_reset(stmt);
	strFieldName = "East";
	sqlite3_bind_text(stmt, 1, strFieldName.c_str(), strFieldName.size(), 0);
	if (sqlite3_step(stmt) == SQLITE_ROW)
		_east = sqlite3_column_double(stmt,0);

	sqlite3_reset(stmt);
	strFieldName = "South";
	sqlite3_bind_text(stmt, 1, strFieldName.c_str(), strFieldName.size(), 0);
	if (sqlite3_step(stmt) == SQLITE_ROW)
		_south = sqlite3_column_double(stmt,0);

	sqlite3_reset(stmt);
	strFieldName = "North";
	sqlite3_bind_text(stmt, 1, strFieldName.c_str(), strFieldName.size(), 0);
	if (sqlite3_step(stmt) == SQLITE_ROW)
		_north = sqlite3_column_double(stmt,0);

	sqlite3_reset(stmt);
	strFieldName = "DataSetName";
	sqlite3_bind_text(stmt, 1, strFieldName.c_str(), strFieldName.size(), 0);
	if (sqlite3_step(stmt) == SQLITE_ROW)
		m_strDatasetName = (char*)sqlite3_column_text(stmt,0);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return true;
}
#pragma endregion


GG_MODEL_ENDNS