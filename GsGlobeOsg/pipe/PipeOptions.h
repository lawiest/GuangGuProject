#ifndef GEOGLOBE_PIPE_PIPEOPTIONS
#define GEOGLOBE_PIPE_PIPEOPTIONS 1
#include <osgEarth/GeoData>
#include <Model/ModelOptions.h>
#include <Pipe/PipeFeature.h>
#include <osgDB/ReaderWriter>
#include <Tool/SQLiteOP.h>
#include <Tool/MemCache.h>
#include <osg/Referenced>
#include <Tool/GeoFCSAttributeCommon.h>
#include <osgEarthFeatures/Feature>
#include "Tool/CodeTransformHelp.h"

//#include <android/log.h>

using namespace GeoGlobe::Tool;
using namespace osgEarth::Features;

namespace GeoGlobe
{
	namespace Pipe
	{
		enum PipeDataSetType
			{
			    //本地管线
				LOCALPIPE,
				//服务管线
				SERVICEPIPE
			};


/*模型缓存，主要用于服务的缓存数据*/
		class PipeCache:public osg::Referenced
		{
		public:


/*下载的纹理数量*/
			int                       _texNum;
/*下载的几何数量*/
			int                       _geoNum;
/*下载的瓦片数量*/
			int                       _tileNum;

			CSQLiteOP                 _symCache;
/*纹理缓存*/
			CSQLiteOP                 _texCache;
/*几何缓存*/
			CSQLiteOP                 _geoCache;
/*瓦片缓存*/
			CSQLiteOP                 _tileCache;
/*下载的材质数量*/
			int                       _matNum;
//缓存名称
			std::string               _cacheName;
//缓存路径
			std::string               _cachePath;
//互斥锁
			Threading::ReadWriteMutex _cacheMutex;

/*初始化*/
			PipeCache(std::string cachePath,std::string cacheName)
			{
				_texNum=0;
				_geoNum=0;
				_tileNum=0;
				//_commitNum=0;
				_matNum=0;
				_cachePath=cachePath;
				_cacheName=cacheName;

				createTileCache(_cachePath+"/"+_cacheName+".tile");
				createGeoCache(_cachePath+"/"+_cacheName+".geo");
				createTexCache(_cachePath+"/"+_cacheName+".tex");
				createSymCache(_cachePath+"/"+_cacheName+".sym");
			}
			~PipeCache()
			{
				commitAll();
			}

/*解析名称，确定是什么数据*/
			std::string spliteFile(const std::string& fileName) const
			{

				std::string::size_type slash = fileName.find_last_of("/\\");

				return std::string(fileName.begin()+slash+1,fileName.end());
			}
/*将字符串转换为key*/
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
/*解析瓦片ID将其转换为瓦片级别，行列号*/
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
			void createTileCache(std::string tileCacheName)
			{
				if ( !osgDB::fileExists( osgDB::getFilePath(tileCacheName) ) )
					osgDB::makeDirectoryForFile(tileCacheName);
				_tileCache.OpenDataBase(tileCacheName.c_str());
				_tileCache.CreateTileValueTable("TileTable");
			}

/*构建几何体的缓存*/
			void createGeoCache(std::string geoCacheName)
			{

				if ( !osgDB::fileExists( osgDB::getFilePath(geoCacheName) ) )
					osgDB::makeDirectoryForFile(geoCacheName);
				_geoCache.OpenDataBase(geoCacheName.c_str());
				_geoCache.CreateKeyValueTable("ModelGeometry");
			}
/*构建纹理的缓存*/
			void createTexCache(std::string texCacheName)
			{	
				if ( !osgDB::fileExists( osgDB::getFilePath(texCacheName) ) )
					osgDB::makeDirectoryForFile(texCacheName);
				_texCache.OpenDataBase(texCacheName.c_str());
				_texCache.CreateKeyValueTable("ModelTexture");
			}
/*构建管段材质的缓存*/
			void createSymCache(std::string symCacheName)
			{	
				if ( !osgDB::fileExists( osgDB::getFilePath(symCacheName) ) )
					osgDB::makeDirectoryForFile(symCacheName);
				_symCache.OpenDataBase(symCacheName.c_str());
				_symCache.CreateKeyValueTable("ModelSymbol");
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
				else if (ext=="pipetile")
				{
					int lev,col,row;
					_tileNum++;
					parseTileName(fileName,lev,col,row);
					return _tileCache.InsertTileValueDataBatch("TileTable",lev,col,row,data,dataLen);

				}
				else if (ext=="mat")
				{
					ULONGLONG matKey;
					parseKeyName(fileName,matKey);
					_matNum++;
					return _symCache.InsertKeyValueDataBatch("ModelSymbol",matKey,data,dataLen);

				}
			}
//从缓存中读取数据
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
				else if (ext=="pipetile")
				{
					int lev,col,row;
					parseTileName(fileName,lev,col,row);

					return _tileCache.GetTileDataByTileKey(lev,col,row,data,dataLen);
				
					
				}
				else if (ext=="mat")
				{
					ULONGLONG matKey;
					parseKeyName(fileName,matKey);

					return _symCache.QuerySingleKeyValueTable("ModelSymbol",matKey,dataLen,data);
			

				}

				
				return false;
			}
//判断数据是否在缓存中
			bool isInCache(std::string fileName)
			{

				Threading::ScopedReadLock shared( _cacheMutex );
				std::string ext=osgDB::getFileExtension(fileName);
	

				if (ext=="pipetile")
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
			}
			//判断数据是否在缓存中
			bool commitCache(std::string cacheName)
			{
				if(_texNum>0)
				{
					Threading::ScopedWriteLock exclusive( _cacheMutex );
					_texNum=0;
					_texCache.CommitCache();
				}

				if (_geoNum>5)
				{
					Threading::ScopedWriteLock exclusive( _cacheMutex );
					_geoNum=0;
					_geoCache.CommitCache();
				}

				if (_tileNum>10)
				{
					Threading::ScopedWriteLock exclusive( _cacheMutex );
					_tileNum=0;
					_tileCache.CommitCache();
				}

				if (_matNum>0)
				{
					Threading::ScopedWriteLock exclusive( _cacheMutex );
					_matNum=0;
					_symCache.CommitCache();
				}
			

				return false;

			};
			//提交缓存数据
			bool commitAll()
			{
				Threading::ScopedWriteLock exclusive( _cacheMutex );
				_tileCache.CommitCache();
				_geoCache.CommitCache();
				_texCache.CommitCache();
				_symCache.CommitCache();
				return true;
			}

		};


/*管线数据库，主要读取本地数据*/
		class PipeDataBase:public osg::Referenced
		{
		public:
			PipeDataBase(){}
			osg::ref_ptr<CSQLiteOP>                     _texDB;
			osg::ref_ptr<CSQLiteOP>                     _geoDB;
			osg::ref_ptr<CSQLiteOP>                     _tileDB; 
			osg::ref_ptr<CSQLiteOP>                     _symDB;
		};

/*基本属性类*/
		class BasePipeOptions: public osgDB::ReaderWriter::Options
		{
		public:
			BasePipeOptions(){};

			virtual osg::Object* clone(const osg::CopyOp& copyop) const 
			{
				return (osg::Object*)(this);
			}

/*解析GX文件*/
			void parseGX(std::string gxFile)
			{
				_dataBase=new PipeDataBase();
				std::string datasetName=osgDB::getStrippedName(gxFile);
				_datasetName=datasetName;
				std::string directory = gxFile.substr(0,gxFile.find_last_of('/'));
				osgEarth::ReadResult result = URI(gxFile).readString();
				if (result.succeeded())
				{
					std::string tileDBName;
					std::string symbolDBName;
					std::string geoDBName;
					std::string texDBName;
					

					std::string str = result.getString();
					std::stringstream in( str.c_str()  );

					osg::ref_ptr< XmlDocument > doc = XmlDocument::load( in );
					if (!doc.valid()) return ;
					osg::ref_ptr<XmlElement>  modeldataset= doc->getSubElement( "modeldataset" );
				/*	osg::ref_ptr<XmlElement>  datasetfile = modeldataset->getSubElement( "datasetfile" );
					tileDBName=gxFile+"/."+datasetfile->getAttrs()["path"].c_str();*/

					//osg::ref_ptr<XmlElement>  modelgeometrysfiles = modeldataset->getSubElement( "pipetilefile" );
					osg::ref_ptr<XmlElement> modelgeometryfile=modeldataset->getSubElement("modelgeometryfile");
					geoDBName=directory+modelgeometryfile->getAttrs()["path"];
				//	__android_log_print(ANDROID_LOG_ERROR, "PIPELAYER", "dbpath:%s", geoDBName.c_str());
		


					osg::ref_ptr<XmlElement>  modeltexturesfile = modeldataset->getSubElement( "modeltexturefile" );
					texDBName=directory+modeltexturesfile->getAttrs()["path"];

					osg::ref_ptr<XmlElement>  pipetilefile = modeldataset->getSubElement( "pipetilefile" );
					tileDBName=directory+pipetilefile->getAttrs()["path"];

					osg::ref_ptr<XmlElement>  modelsymbolfile = modeldataset->getSubElement( "modelsymbolfile" );
					symbolDBName=directory+modelsymbolfile->getAttrs()["path"];

					osg::ref_ptr<XmlElement>  pointfeatureclassfile = modeldataset->getSubElement( "pointfeatureclassfile" );
					_pfcDBName=directory+pointfeatureclassfile->getAttrs()["path"];

					osg::ref_ptr<XmlElement>  linefeatureclassfile = modeldataset->getSubElement( "linefeatureclassfile" );
					_lfcDBName=directory+linefeatureclassfile->getAttrs()["path"];
					
					/*osgEarth::XmlNodeList modeltexturefile=modeltexturesfiles->getSubElements("modeltexturefile");
					for( XmlNodeList::const_iterator i = modeltexturefile.begin(); i != modeltexturefile.end(); i++ )
					{
					if ( i->get()->isElement() )
					{
					XmlElement* e = (XmlElement*)i->get();
					texDBNames.push_back(gxFile+"/."+e->getAttrs()["path"]);

					}

					}*/


					/*	osg::ref_ptr<XmlElement>  featureclassesfiles = modeldataset->getSubElement( "featureclassesfiles" );
					osgEarth::XmlNodeList featureclassfile=featureclassesfiles->getSubElements("featureclassfile");
					for( XmlNodeList::const_iterator i = featureclassfile.begin(); i != featureclassfile.end(); i++ )
					{
					if ( i->get()->isElement() )
					{
					XmlElement* e = (XmlElement*)i->get();
					fcDBNames.push_back(gxFile+"/."+e->getAttrs()["path"]);

					}

					}*/


					_dataBase->_tileDB=new CSQLiteOP;
					_dataBase->_tileDB->OpenDataBase(CodeHelp::String_To_UTF8(tileDBName.c_str()).c_str());
					_dataBase->_tileDB->DataBaseName(CodeHelp::String_To_UTF8(tileDBName.c_str()).c_str());

					_dataBase->_symDB=new CSQLiteOP;
					_dataBase->_symDB->OpenDataBase(CodeHelp::String_To_UTF8(symbolDBName.c_str()).c_str());
					_dataBase->_symDB->DataBaseName(CodeHelp::String_To_UTF8(symbolDBName.c_str()).c_str());
				
					_dataBase->_geoDB=new CSQLiteOP;
					_dataBase->_geoDB->OpenDataBase(CodeHelp::String_To_UTF8(geoDBName.c_str()).c_str());
				
					_dataBase->_texDB=new CSQLiteOP;
					_dataBase->_texDB->OpenDataBase(CodeHelp::String_To_UTF8(texDBName.c_str()).c_str());

		

	/*				for(int i=0;i<fcDBNames.size();i++)
					{
						CSQLiteOP* fcDB=new CSQLiteOP;
						fcDB->OpenDataBase(fcDBNames[i].c_str());
						_dataBase->_fcDBs.push_back(fcDB);
					}*/

				}

			}

/*生成和解析数据的唯一名称，必须有名称才能用osg自带的分页数据加载功能*/
			//生成管线ID
			std::string createPipeName(UINT64 fcKey,int lod)
			{
				return Stringify() <<_datasetName<<"/"<<_layerName<<"/Pipe/"<<fcKey<<".pipe";
			}
			//生成管点模型ID
			std::string createPipeModelName(UINT64 geoKey)
			{
				return Stringify() <<_datasetName<<"/"<<_layerName<<"/PipeModel/"<<geoKey<<".pipemodel";
			}
			//生成管点几何ID
			std::string createGeometryName(UINT64 geoKey)
			{
				return Stringify() <<_datasetName<<"/"<<_layerName<<"/Geometry/"<<geoKey<<".geo";
			}
			//生成管点纹理ID
			std::string createTextureName(UINT64 texKey)
			{
				return Stringify() <<_datasetName<<"/"<<_layerName<<"/Texture/"<<texKey<<".tex";
			}
			//生成材质ID
			std::string createMaterialName(UINT64 matKey)
			{
				return Stringify() <<_datasetName<<"/"<<_layerName<<"/Material/"<<matKey<<".mat";
			}
			//生成管线瓦片ID
			std::string createPipeTileName(int lev,int col,int row)
			{
				return Stringify() <<_datasetName<<"/"<<_layerName<<"/PipeTile/"<<lev<<"/"<<col<<"/"<<row<<".pipetile";
			}

			std::string spliteFile(const std::string& fileName) const
			{
				//std::string::size_type dot = fileName.find_last_of('.');
				std::string::size_type slash = fileName.find_last_of("/\\");
				//	if (dot==std::string::npos || (slash!=std::string::npos && dot<slash)) return std::string("");
				return std::string(fileName.begin()+slash+1,fileName.end());
			}
			//生成管点模型ID
			void parsePipeModelName(std::string uri,unsigned long long& geoKey) const
			{
				std::vector<std::string> vecStr;

				std::string lessExtension;

				lessExtension=osgDB::getNameLessExtension(uri);

				for (int i=0;i<1;i++)
				{

					std::string str=spliteFile(lessExtension);
					vecStr.push_back(str);
					lessExtension=osgDB::getFilePath(lessExtension);
				}
				std::stringstream tempss;
				tempss<<vecStr[0].c_str();
				tempss>>geoKey;

			}
/*从本地数据库中读取数据*/
			//从数据库中得到瓦片数据
			bool getTileDataFromDB(int level,int col,int row,char ** ppTileBuff,int &nSize)
			{
		
					if( _dataBase->_tileDB->GetTileDataByTileKey(level,col,row,ppTileBuff,nSize))
						return true;
			
				return false;

			}
			//从数据库中得到几何数据
			bool GetGeoDataFromDB(UINT64 geoKey,int &dataLen,char** geoData)
			{
				return _dataBase->_geoDB->QuerySingleKeyValueTable("ModelGeometry",geoKey,dataLen,geoData);
			}
			//从数据库中得到纹理数据
			bool GetTexDataFromDB(UINT64 texKey,int &dataLen,char** texData)
			{
				return _dataBase->_texDB->QuerySingleKeyValueTable("ModelTexture",texKey,dataLen,texData);
			}

/*从本地数据库中查询管段属性信息*/
			bool queryPipeLineInfoFromLocal(LONGLONG fcKey,osgEarth::Features::AttributeTable &attr,osgEarth::Bounds& bds)
			{
				if(!osgDB::fileExists(_lfcDBName))
						return false;
				GeoGlobe::Tool::GeoFCSAttributeCommon gfcsAttriCom(_lfcDBName);
				int oid = gfcsAttriCom.getOIDfromFeatureID(fcKey);

				if(oid==-1)
					return false;
				//__android_log_print(ANDROID_LOG_ERROR, "queryPipeLineInfoFromLocal", "%d",oid);
				//__android_log_print(ANDROID_LOG_ERROR, "queryPipeLineInfoFromLocal", "%d",2);
				//获取包围盒

				gfcsAttriCom.getBoundBoxbyoidGeometry(oid,bds);
				gfcsAttriCom.getAttributeTablebyFID(fcKey,attr);
//				__android_log_print(ANDROID_LOG_ERROR, "queryPipeLineInfoFromLocal", "%d",attr.size());
				return true;
			}

/*从本地数据库中查询管点属性信息*/
			bool queryPipePointInfoFromLocal(LONGLONG fcKey,osgEarth::Features::AttributeTable &attr,osgEarth::Bounds& bds)
			{
				if(!osgDB::fileExists(_pfcDBName))
						return false;
				GeoGlobe::Tool::GeoFCSAttributeCommon gfcsAttriCom(_pfcDBName);
				int oid = gfcsAttriCom.getOIDfromFeatureID(fcKey);
				if(oid==-1)
						return false;
				//__android_log_print(ANDROID_LOG_ERROR, "queryPipePointInfoFromLocal", "%d",oid);
				//获取包围盒
				//osgEarth::Bounds bds;
				gfcsAttriCom.getBoundBoxbyoidGeometry(oid,bds);
				gfcsAttriCom.getAttributeTablebyFID(fcKey,attr);
				return true;
			}



           //数据集名称
			std::string                     _datasetName;
			//图层名称
			std::string                     _layerName;
			std::string                     m_strLayerURL;
//服务url
			std::string                     _serviceUrl;
			//缓存位置
			std::string                     _cachePath;
			//管段fc文件位置
			std::string                     _lfcDBName;
			//管点fc文件位置
			std::string                     _pfcDBName;

			//全局设置
			osg::ref_ptr<Setting>                                        _setting;
			//内存
			osg::ref_ptr<GeoGlobe::Tool::MemCache>                       _memCache;
			//服务缓冲
			osg::ref_ptr<GeoGlobe::Pipe::PipeCache>                       _cache;
			//本地数据库
			osg::ref_ptr<PipeDataBase>                                   _dataBase;
		};

/*管线图层属性*/
		class PipeLayerOptions: public BasePipeOptions
		{
		public:
			PipeLayerOptions(osgEarth::MapNode* mapNode,std::string url,std::string layerName="",std::string cachePath="")
			{
				_bInitialized=false;
				_setting=new Setting;
				_setting->_visualRange=600;

				m_strLayerURL = url;
				
				//_downloadTool=new DownLoadTool;
				_memCache=new  GeoGlobe::Tool::MemCache;
				_mapNode=mapNode;
				
				_layerName=layerName;
				//_dataName=url;
				if(cachePath=="")
				{
					parseGX(url);
					_bInitialized = readTileMetafromLocal( _dataBase->_tileDB->DataBaseName());
				}
				else
				{	
					_serviceUrl=url;
					_cachePath=cachePath;
					_cache=new PipeCache(cachePath,_layerName);
					readTileMetafromService(url);
					setFileLocationCallback(new FileLocationCallback);
				}

			}

			~PipeLayerOptions()
			{
 //              cout<<"modellayer release"<<endl; 
			};

/*用于确定数据是否已存在本地缓存中，从而确定采用网络线程还是本地线程加载数据*/
			class FileLocationCallback : public osgDB::FileLocationCallback
			{
			public:
				FileLocationCallback() { }

				/** dtor */
				virtual ~FileLocationCallback() { }

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

				Location fileLocation(const std::string& filename, const osgDB::Options* options)
				{
					return  LOCAL_FILE;
					//Location result = LOCAL_FILE;

					//int level, row, col;
					//parseFileName(filename,level,col,row);
					//PipeLayerOptions* pipeLayerOptions=(PipeLayerOptions*) options;

					//if (level+2<pipeLayerOptions->_minLevel)
					//{
					//	return result;
					//}

					//const osgEarth::Profile* profile = pipeLayerOptions->_mapNode->getMap()->getProfile();
					//osgEarth::TileKey mapKey( level, col, row, profile );


					//for( unsigned i=0; i<4; ++i )
					//{
					//	TileKey childKey = mapKey.createChildKey( i );
					//	int lev=childKey.getLevelOfDetail()+1;
					//	int col=childKey.getTileX();
					//	int row=childKey.getTileY();
					//	std::string modelTileFile=pipeLayerOptions->createPipeTileName(lev,col,row);
					//	if ( !/*modelLayerOptions->_modelCache->isTileDataInCache(lev,col,row)*//*osgDB::fileExists(modelTileFile)*/pipeLayerOptions->_cache->isInCache(modelTileFile) )
					//	{
					//		result = REMOTE_FILE;
					//		break;
					//	}
					//	//else
					//	//{
					//	//	int a=0;
					//	//}
					//}
					////level++;





					///*			
					//if (modelLayerOptions->_modelCache->isTileDataInCache(level,col,row))
					//{
					//return LOCAL_FILE;
					//}
					//else
					//{
					//return REMOTE_FILE;
					//}*/

					///*sscanf(filename.c_str(), "%d/%d/%d.%d", &lod, &x, &y, &id);
					//*/


					////OE_NOTICE << (result?"REMOTE":"LOCAL") << " - " << filename << std::endl;
					///*		if (result ==LOCAL_FILE)
					//{
					//int a=0;
					//}*/


					//return result;
				}

				bool useFileCache() const { return false; }
			};
/*从本地读取数据集元信息*/
			bool readTileMetafromLocal(std::string sqlitePath)
			{
				if(!osgDB::fileExists(CodeHelp::UTF8_To_string(sqlitePath)))
					return false;
				/*std::string  spath=*(this->rootPath())+*(this->sqlitePath());*/
				//	std::string  spath=*(this->sqlitePath());
				sqlite3 *db;
				sqlite3_open(sqlitePath.c_str(),&db);


				long long minlevel=0;
				long long maxlevel=0;
				sqlite3_stmt* stmt;

				std::string sql;
				sql="select itemdata from TileMeta where itemname='ValidTopLevel'";

				if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						_minLevel=sqlite3_column_int(stmt,0);
					}
				}
				sqlite3_finalize(stmt);


				sql="select itemdata from TileMeta where itemname='ValidBottomLevel'";
				if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						_maxLevel=sqlite3_column_int(stmt,0);
					}
				}
				sqlite3_finalize(stmt);


				sql="select itemdata from TileMeta where itemname='West'";
				if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						_west=sqlite3_column_double(stmt,0);
						//__android_log_print(ANDROID_LOG_ERROR, "PIPELAYER", "west:%f", _west);
					}
				}
				sqlite3_finalize(stmt);


				sql="select itemdata from TileMeta where itemname='East'";
				if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						_east=sqlite3_column_double(stmt,0);
						//__android_log_print(ANDROID_LOG_ERROR, "PIPELAYER", "east:%f", _east);
					}
				}
				sqlite3_finalize(stmt);

				sql="select itemdata from TileMeta where itemname='South'";
				if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						_south=sqlite3_column_double(stmt,0);
						//__android_log_print(ANDROID_LOG_ERROR, "PIPELAYER", "south:%f", _south);
					}
				}
				sqlite3_finalize(stmt);

				sql="select itemdata from TileMeta where itemname='DataSetName'";
				if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						_layerName=(char*)sqlite3_column_text(stmt,0);
						//__android_log_print(ANDROID_LOG_ERROR, "PIPELAYER", "south:%f", _south);
					}
				}
				sqlite3_finalize(stmt);

				sql="select itemdata from TileMeta where itemname='North'";
				if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						_north=sqlite3_column_double(stmt,0);
				//		__android_log_print(ANDROID_LOG_ERROR, "PIPELAYER", "north:%f", _north);
					}
				}
				sqlite3_finalize(stmt);



				//_minLevel=(unsigned int)minlevel;
				//_maxLevel=(unsigned int)maxlevel;

				sqlite3_close(db);
				return true;
			}
/*从服务器读取数据集元信息*/
			bool readTileMetafromService(std::string url)
			{
				std::string capUrl = 
					url + 
					std::string("?SERVICE=PLS&VERSION=1.0.0&REQUEST=GetCapabilities") ;

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
						//std::ios::
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

				if ( !capabilities.valid() )
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


						if (tdata.layerName!=_layerName)
						continue;
						
						_minLevel=tdata.toplevel;
						_maxLevel=tdata.bottomlevel;
						_west=tdata.bbox.west;
						_east=tdata.bbox.east;
						_south=tdata.bbox.south;
						_north=tdata.bbox.north;
						
					}

				}

				_datasetName=capabilities->getName();

				return true;
			}

/*确定哪些模型需要高亮*/
			void heightlightModel(LONGLONG fcKey,bool hl)
			{
				stringstream ss;
				ss<<fcKey;
				//_setting->_heightLightList.push_back(ss.str());
				_setting->_heightLightMap.insert(make_pair(ss.str(),hl));
			}

			virtual osg::Object* clone(const osg::CopyOp& copyop) const 
			{
				return (osg::Object*)(this);
			}

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
//数据是否初始化成果
			bool                _bInitialized;


		};

/*管线瓦片属性*/
		class PipeTileOptions : public BasePipeOptions
		{
		public:
			PipeTileOptions(){}
			PipeTileOptions(osgEarth::TileKey& tileKey,PipeLayerOptions* options)
			{
				_tileKey=tileKey;
				_mapNode=options->_mapNode;
			
				_dataBase=options->_dataBase;
				_memCache=options->_memCache;
				_setting=options->_setting;
			
				_layerName=options->_layerName;
				_cache=options->_cache;
				_serviceUrl=options->_serviceUrl;

				_lfcDBName=options->_lfcDBName;
				_pfcDBName=options->_pfcDBName;
			}
			PipeTileOptions(TileKey& tileKey,osgEarth::MapNode*  mapNode)
			{
				_tileKey=tileKey;
				_mapNode=mapNode;
				
			}
//瓦片Key
				TileKey _tileKey;
//地球节点
				osgEarth::MapNode*   _mapNode;
		};

/*管线模型属性*/
		class PipeModelOptions : public BasePipeOptions
		{
		public:
			PipeModelOptions(){}

/*以下都是GeoGlobe中的管线模型属性*/
			enumPipeFeatureType    _pipeFeatureType;
			UINT64			_modelGeometryID; //管点几何体ID
			UINT64			_featureID;	//管线featureID
			osg::Vec3d		_modelCart;//管线的地心坐标
			osg::Vec3d		_modelGeo;//管线的地理坐标

			osg::Matrix		_featureMatrxi;//旋转矩阵
			osg::Matrix		_curWorldMatrix;//��ǰģ�͵��������
			osg::Matrix		_curProjMatrix;//ͶӰ����
			osg::Matrix		_curViewMatrix;//�۲����

			//地球节点
			osgEarth::MapNode*   _mapNode;
		};

/*管点模型属性*/
		class PipePointModelOptions : public PipeModelOptions
		{
		public:
			PipePointModelOptions(CPipePointFeature* pipePointFeature,PipeTileOptions* options)
			{
				_setting=options->_setting;
				_dataBase=options->_dataBase;
				
				_mapNode=options->_mapNode;
				_memCache=options->_memCache;
				_pipeFeatureType=pipePointFeature->PipeFeatureType();

			    _cache=options->_cache;
				_featureID = pipePointFeature->FeatureID();
				_modelGeometryID = /*134567566508044*/pipePointFeature->SymbolID();

				//{m_eFeatureType=ePipeFeaturePoint m_nFeatureID=216307349680291949 m_nSymbolID=134567566508044 }
			
				_featureMatrxi = pipePointFeature->GetMatrix();

				_modelGeo = pipePointFeature->GeoPoint();
			//	_modelGeo=_modelGeo;
				_mapNode->getMapSRS()->transformToWorld(_modelGeo,_modelCart);
				_layerName=options->_layerName;
				_serviceUrl=options->_serviceUrl;
				
				_lfcDBName=options->_lfcDBName;
				_pfcDBName=options->_pfcDBName;

			}

			UINT64	    _symbolID;	//材质ID
			osg::Matrix _featureMatrxi;//旋转矩阵

		};

/*管段模型属性*/
		class PipeLineModelOptions : public PipeModelOptions
		{
		public:
			PipeLineModelOptions(){}
			PipeLineModelOptions(CPipeLineFeature* pipeLineFeature,PipeTileOptions* options)
			{
				_setting=options->_setting;
				_layerName=options->_layerName;
				_memCache=options->_memCache;
				_dataBase=options->_dataBase;
				_pipeFeatureType=pipeLineFeature->PipeFeatureType();
				_mapNode=options->_mapNode;
				_sectionType = pipeLineFeature->CrossSectionType();
				_featureID = pipeLineFeature->FeatureID();
				_modelGeometryID = pipeLineFeature->SymbolID();
				
			//	pipeLineFeature->
				osg::Vec3d  startGeo = pipeLineFeature->StartPoint();
				osg::Vec3d  endGeo = pipeLineFeature->EndPoint();

			/*	_modelGeo.x() = (startGeo.x()+endGeo.x())/2;
				_modelGeo.y() = (startGeo.y()+endGeo.y())/2;
				_modelGeo.z() = (startGeo.z()+endGeo.z())/2;*/
				_modelGeo=(startGeo+endGeo)/2;
				//_modelGeo=_modelGeo+osg::Vec3(0,0,20);
				_mapNode->getMapSRS()->transformToWorld(startGeo,_startCart);
				_mapNode->getMapSRS()->transformToWorld(endGeo,_endCart);
				_mapNode->getMapSRS()->transformToWorld(_modelGeo,_modelCart);
				
				_cache=options->_cache;
				_aabbBox=computerAABBox(_startCart,_endCart);

				_diameter=pipeLineFeature->Diameter();
				_width=pipeLineFeature->Width();
				_height=pipeLineFeature->Height();
				//_mapNode->getMapSRS()->tow
				_serviceUrl=options->_serviceUrl;
			//	_startCart = CMathHelper::Geo2Cart(ptStartGeo) ;
				//m_ptEndCart = CMathHelper::Geo2Cart(ptEndGeo);
				//m_ptModelCart = CMathHelper::Geo2Cart(m_ptModelGeo);

				//m_AABBox = ComputerAABBox(m_ptStartCart,m_ptEndCart);
				_lfcDBName=options->_lfcDBName;
				_pfcDBName=options->_pfcDBName;
			}

//计算模型AABB包围盒
			osg::BoundingBox computerAABBox(osg::Vec3d startPoint,osg::Vec3d endPoint)
			{
				osg::Vec3d minPos;
				minPos.x() = (startPoint.x()<endPoint.x())?startPoint.x():endPoint.x();
				minPos.y() = (startPoint.y()<endPoint.y())?startPoint.y():endPoint.y();
				minPos.z() = (startPoint.z()<endPoint.z())?startPoint.z():endPoint.z();

				osg::Vec3d maxPos;
				maxPos.x() = (startPoint.x()>endPoint.x())?startPoint.x():endPoint.x();
				maxPos.y() = (startPoint.y()>endPoint.y())?startPoint.y():endPoint.y();
				maxPos.z() = (startPoint.z()>endPoint.z())?startPoint.z():endPoint.z();

				return osg::BoundingBox(minPos,maxPos);
			}

/*以下都是GeoGlobe中的管段模型属性*/
			//管段类型：圆管还是方管
			enumCrossSectionType _sectionType;
			//起始点笛卡尔坐标
			osg::Vec3d  _startCart;
			//终止点笛卡尔坐标
			osg::Vec3d  _endCart;
			//aabb包围盒
			osg::BoundingBox _aabbBox;

//圆管半径
			double   _diameter;
//高度
			double   _height;
//宽度
			double   _width;
			

		};
	}
}


#endif
