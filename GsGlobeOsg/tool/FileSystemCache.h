#ifndef GEOGLOBE_TOOL_FILESYSTEMCACHE
#define GEOGLOBE_TOOL_FILESYSTEMCACHE 1
#include <string>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osg/Referenced>
#include <Tool/SQLiteOP.h>
namespace GeoGlobe
{
	namespace Tool
	{

		class Cache:public osg::Referenced
		{
		public:
			std::string                _cachePath;
			Cache(){}

			virtual bool writeDataToCache(std::string fileName,std::string& data)=0;
			virtual bool readDataFromCache(std::string fileName,std::string& data)=0;
			virtual bool isInCache(std::string fileName)=0;
			virtual bool updateDataInCache(std::string fileName,std::string& data)=0;
			virtual bool commitCache(std::string cacheName)=0;

		};

		class FileSystemCache:public Cache
		{
		public:
			std::string                _cacheName;
			std::string                _cachePath;
			OpenThreads::Mutex                          _cacheMutex;
			FileSystemCache(std::string cachePath)
			{
				//_cacheName=cacheName;
				_cachePath=cachePath;
			}

			virtual bool writeDataToCache(std::string fileName,std::string& data)
			{
				//	return false;
				fileName=_cachePath+"/"+fileName;
				OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
				if(osgDB::fileExists(fileName))
					return false;

				if ( !osgDB::fileExists( osgDB::getFilePath(fileName) ) )
					osgDB::makeDirectoryForFile(fileName );

				osgDB::ofstream fout;
				//std::ios::
				fout.open(fileName.c_str(), std::ios::out | std::ios::binary);
				fout.write(data.c_str(),data.size() * sizeof(unsigned char));
				fout.close();
				return true;
			}


			virtual bool readDataFromCache(std::string fileName,std::string& data)
			{
					//return false;
				fileName=_cachePath+"/"+fileName;
				OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
				//std::string cachePath=getCachePath(type);
				if(osgDB::fileExists(fileName))

				{
					std::ifstream input/*( fileName.c_str() )*/;
					input.open(fileName.c_str(), std::ios::in | std::ios::binary);
					if ( input.is_open() )
					{
					//	input >> std::noskipws;
						std::stringstream buf;
					/*	long l=in.tellg();
						int */
						buf << input.rdbuf();
						data=buf.str();

						//std::string bufStr;
						return true;
					}
				}
				return false;
			}

			virtual bool isInCache(std::string fileName)
			{
				fileName=_cachePath+"/"+fileName;
				if(osgDB::fileExists(fileName))
					return true;
				return false;

			}

			virtual bool updateDataInCache(std::string fileName,std::string& data)
			{
				fileName=_cachePath+"/"+fileName;
				OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);


				if ( !osgDB::fileExists( osgDB::getFilePath(fileName) ) )
					osgDB::makeDirectoryForFile(fileName );

				osgDB::ofstream fout;
				//std::ios::
				fout.open(fileName.c_str(), std::ios::out | std::ios::binary);
				fout.write(data.c_str(),data.size() * sizeof(unsigned char));
				fout.close();
				return true;
			}

			virtual bool commitCache(std::string cacheName){};

		};

		class SQLiteCache:public Cache
		{
		public:
			std::string                _cacheName;
			std::string                _cachePath;
			OpenThreads::Mutex                          _cacheMutex;

			CSQLiteOP                 _texCache;
			CSQLiteOP                 _geoCache;
			CSQLiteOP                 _tileCache;
			CSQLiteOP                 _modelCache;
			int                       _texNum;
			int                       _geoNum;
			int                       _tileNum;

			SQLiteCache(std::string cachePath)
			{
				_texNum=0;
				_geoNum=0;
				_tileNum=0;
				//_cacheName=cacheName;
				_cachePath=cachePath;
				/*			std::string cacheName=_cachePath+"/cache.geo";
				_texCache.OpenDataBase(cacheName.c_str());*/
				createTileCache(_cachePath+"/cache.tile");
				createGeoCache(_cachePath+"/cache.geo");
				createTexCache(_cachePath+"/cache.tex");
				//createLodModelCache(_cachePath+"/cache.fc");
			}



			void createTileCache(std::string tileCacheName)
			{
				//CSQLiteOP* tileCache=new CSQLiteOP;
				_tileCache.OpenDataBase(tileCacheName.c_str());
				_tileCache.CreateTileValueTable("TileTable");
	

			}

			void createGeoCache(std::string geoCacheName)
			{
			
				_geoCache.OpenDataBase(geoCacheName.c_str());
				_geoCache.CreateKeyValueTable("ModelGeometry");
			}

			void createTexCache(std::string texCacheName)
			{	
				_texCache.OpenDataBase(texCacheName.c_str());
				_texCache.CreateKeyValueTable("ModelTexture");
			}

			//void createLodModelCache(std::string modelCacheName)
			//{
			//	_modelCache.OpenDataBase(modelCacheName.c_str());
			//	_modelCache.CreateKeyValueTable("Model");
			//}

			std::string spliteFile(const std::string& fileName) const
			{
				//std::string::size_type dot = fileName.find_last_of('.');
				std::string::size_type slash = fileName.find_last_of("/\\");
				//	if (dot==std::string::npos || (slash!=std::string::npos && dot<slash)) return std::string("");
				return std::string(fileName.begin()+slash+1,fileName.end());
			}

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

			void parseGeoTexName(std::string uri,unsigned long long& key) const
			{
				std::vector<std::string> vecStr;

				std::string lessExtension;

				lessExtension=osgDB::getNameLessExtension(uri);
				//ext=osgDB::getFileExtension(lessExtension);

		/*		for (int i=0;i<2;i++)
				{*/

					std::string str=spliteFile(lessExtension);
					//vecStr.push_back(str);
					//lessExtension=osgDB::getFilePath(lessExtension);
				/*}*/

			//	lod= atoi(vecStr[0].c_str());

				std::stringstream tempss;
				tempss<<str;
				tempss>>key;

			}

			void parseTileName(std::string uri,int& lev,int& col,int& row)
			{
				std::vector<std::string> vecStr;

				std::string lessExtension;

				lessExtension=osgDB::getNameLessExtension(uri);
				//ext=osgDB::getFileExtension(lessExtension);

				for (int i=0;i<3;i++)
				{

					std::string str=spliteFile(lessExtension);
					vecStr.push_back(str);
					lessExtension=osgDB::getFilePath(lessExtension);
				}

				lev= atoi(vecStr[2].c_str());
				col= atoi(vecStr[1].c_str());
				row= atoi(vecStr[0].c_str());

	/*			std::stringstream tempss;
				tempss<<vecStr[1].c_str();
				tempss>>geoKey;*/
			}

			bool writeDataToCache(std::string fileName,std::string& data)
			{
				//commitCache("");
				std::string ext=osgDB::getFileExtension(fileName);
				//if (ext=="model")
				//{
				//	return false;
				//}
				
				
				if (ext=="geo")
				{
					
					ULONGLONG geoKey;
					parseGeoTexName(fileName,geoKey);
					_geoNum++;
					// return _geoCache.InsertKeyValueData("ModelGeometry",geoKey,data.c_str(),data.size());
				   return _geoCache.InsertKeyValueDataBatch("ModelGeometry",geoKey,data.c_str(),data.size());
				}
				else if (ext=="tex")
				{
					//return false;
					ULONGLONG texKey;
					parseGeoTexName(fileName,texKey);
					_texNum++;
					//return _texCache.InsertKeyValueData("ModelTexture",texKey,data.c_str(),data.size());
					return _texCache.InsertKeyValueDataBatch("ModelTexture",texKey,data.c_str(),data.size());
				}
				else if (ext=="modeltile")
				{
					int lev,col,row;
					_tileNum++;
					parseTileName(fileName,lev,col,row);
					return _tileCache.InsertTileValueDataBatch("TileTable",lev,col,row,data.c_str(),data.size());
					//return _tileCache.InsertTileValueData("TileTable",lev,col,row,data.c_str(),data.size());
				}
				//else if (ext=="lodmodel")
				//{
				//	int lod;
				//	ULONGLONG geoKey;
				//	parseModelName(fileName,geoKey,lod);
				//	return _modelCache.InsertKeyLodData("Model",geoKey,lod);
				//}



				//	return false;
				//fileName=_cachePath+"/"+fileName;
				//OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
				//if(osgDB::fileExists(fileName))
				//	return false;

				//if ( !osgDB::fileExists( osgDB::getFilePath(fileName) ) )
				//	osgDB::makeDirectoryForFile(fileName );

				//osgDB::ofstream fout;
				////std::ios::
				//fout.open(fileName.c_str(), std::ios::out | std::ios::binary);
				//fout.write(data.c_str(),data.size() * sizeof(unsigned char));
				//fout.close();
				//return true;
			}


			bool readDataFromCache(std::string fileName,std::string& data)
			{

				std::string ext=osgDB::getFileExtension(fileName);
				if (ext=="geo")
				{
					ULONGLONG geoKey;
					char* pGeoBuff;
					int nSize;
					parseGeoTexName(fileName,geoKey);
					if(_geoCache.QuerySingleKeyValueTable("ModelGeometry",geoKey,nSize,&pGeoBuff))
					{
						data.assign(pGeoBuff,nSize);
						return true;
					}
				}
				else if (ext=="tex")
				{
					ULONGLONG texKey;
					parseGeoTexName(fileName,texKey);
					char* pTexBuff;
					int nSize;
					if(_texCache.QuerySingleKeyValueTable("ModelTexture",texKey,nSize,&pTexBuff))
					{
						data.assign(pTexBuff,nSize);
						return true;
					}
					//return _tileCache->GetTileDataByTileKey(level,col,row,ppTileBuff,nSize)
					//return _texCache.InsertKeyValueData("ModelTexture",texKey,data.c_str(),data.size());
				}
				else if (ext=="modeltile")
				{
					int lev,col,row;
					parseTileName(fileName,lev,col,row);
					char* pTileBuff;
					int nSize;
					 if(_tileCache.GetTileDataByTileKey(lev,col,row,&pTileBuff,nSize))
					 {
						
						 data.assign(pTileBuff,nSize);
						 return true;
					 }
					//return _tileCache.InsertTileValueData("TileTable",lev,col,row,data.c_str(),data.size());
				}
				else if (fileName=="tilemeta")
				{
					fileName=_cachePath+"/"+fileName;
					if(osgDB::fileExists(fileName))

				{
					std::ifstream input/*( fileName.c_str() )*/;
					input.open(fileName.c_str(), std::ios::in | std::ios::binary);
					if ( input.is_open() )
					{
					//	input >> std::noskipws;
						std::stringstream buf;
					/*	long l=in.tellg();
						int */
						buf << input.rdbuf();
						data=buf.str();

						//std::string bufStr;
						return true;
					}
				}
					//return _tileCache.InsertTileValueData("TileTable",lev,col,row,data.c_str(),data.size());
				}
				return false;
	/*			else if (ext=="model")
				{
					int lod;
					ULONGLONG geoKey;
					parseModelName(fileName,geoKey,lod);

					if (_modelCache.)
					{
					}

					return _modelCache.InsertKeyLodData("Model",geoKey,lod);
				}*/

					//return false;
				//fileName=_cachePath+"/"+fileName;
				//OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
				////std::string cachePath=getCachePath(type);
				//if(osgDB::fileExists(fileName))

				//{
				//	std::ifstream input/*( fileName.c_str() )*/;
				//	input.open(fileName.c_str(), std::ios::in | std::ios::binary);
				//	if ( input.is_open() )
				//	{
				//	//	input >> std::noskipws;
				//		std::stringstream buf;
				//	/*	long l=in.tellg();
				//		int */
				//		buf << input.rdbuf();
				//		data=buf.str();

				//		//std::string bufStr;
				//		return true;
				//	}
				//}
				//return false;
			}

			bool isInCache(std::string fileName)
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

	

			}

			bool updateDataInCache(std::string fileName,std::string& data)
			{
				fileName=_cachePath+"/"+fileName;
				OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);


				if ( !osgDB::fileExists( osgDB::getFilePath(fileName) ) )
					osgDB::makeDirectoryForFile(fileName );

				osgDB::ofstream fout;
				//std::ios::
				fout.open(fileName.c_str(), std::ios::out | std::ios::binary);
				fout.write(data.c_str(),data.size() * sizeof(unsigned char));
				fout.close();
				return true;
			}

			bool commitCache(std::string cacheName)
			{
			/*	if(cacheName=="tex")
				{*/
				if(_texNum>100)
				{
					_texNum=0;
					_texCache.CommitCache();
				}
			/*	}
				else if (cacheName=="geo")
				{*/
				if (_geoNum>30)
				{
					_geoNum=0;
					_geoCache.CommitCache();
				}

				if (_tileNum>10)
				{
					_tileNum=0;
					_tileCache.CommitCache();
				}
				
					
				/*}*/

				return false;

			};

		};
	}
}

#endif