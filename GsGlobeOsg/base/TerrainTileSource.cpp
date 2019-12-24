
#include "TerrainTileSource.h"
#include <osgEarth/ImageToHeightFieldConverter>
#include "zlib.h"

GLOBE_NS

//读取地形数据接口类的初始化，传入参数为BaseTileSourceOptions
ElevationTileSource::ElevationTileSource(GLOBE_NAME::BaseTileSourceOptions options)
										: osgEarth::TileSource(options), _options(options)
{
	//创建读取以及缓存地形数据的接口类
	_cache = new GeoGlobe::Tool::SQLiteCache(*(options.cachePath()), options._cacheType);

	if (options._servicetype == GLOBE_NAME::eLocal)////若为本地数据则不需要创建缓存
	{
	}
	if (options._servicetype == GLOBE_NAME::eGeoGlobeTile)//根据吉奥瓦片服务的类型创建影像或者地形缓存
	{
		if (options._cacheType == GeoGlobe::Tool::eImageCache)
		{
			_cache->createImageCache();
		}
		if (options._cacheType == GeoGlobe::Tool::eElevationCache)
		{
			_cache->createElevationCache();
		}
	}
	if (options._servicetype == GLOBE_NAME::eReadMap)
	{
		if (options._cacheType == GeoGlobe::Tool::eImageCache)
		{
			_cache->createImageCache();
		}
		if (options._cacheType == GeoGlobe::Tool::eElevationCache)
		{
			_cache->createElevationCache();
		}
	}

	_demType = options._metadata.DEMDataType;
	if (_demType == "")
	{
		_demType = "int16";
	}
}

ElevationTileSource::~ElevationTileSource()
{

}

//在该图层加载数据前，对其进行初始化，包括数据范围、类型、图层的确定
osgEarth::Status ElevationTileSource::initialize(const osgDB::Options* dbOptions)
{

	if (!getProfile())
	{
		setProfile(osgEarth::Registry::instance()->getGlobalGeodeticProfile());
	}
	_dataExtents.push_back(osgEarth::DataExtent(osgEarth::GeoExtent(this->getProfile()->getSRS(),
		_options._metadata.bbox.west,
		_options._metadata.bbox.south,
		_options._metadata.bbox.east,
		_options._metadata.bbox.north),
		_options._metadata.toplevel, _options._metadata.bottomlevel));

	_maxLevel = _options._metadata.bottomlevel;
	return osgEarth::STATUS_OK;
}

osgEarth::DataExtentList& ElevationTileSource::DataExtentList()
{
	return _dataExtents;
}

//具体注释参照TerrainTileSource.h文件
std::string ElevationTileSource::createElevationTileFileName(int lev,int col,int row)
{
	std::stringstream ss;
	ss<<lev<<"/"<<col<<"/"<<row<<".1.osgearth_engine_mp_tile_elevation";
	return ss.str();
}

osg::HeightField* ElevationTileSource::readTerrianfromSqlite(const osgEarth::TileKey& key,osgEarth::ProgressCallback* progress)
{
	osg::ref_ptr<osg::HeightField> nhf=NULL;

	unsigned int lev,r,c;
	lev=key.getLevelOfDetail()+1;
	key.getTileXY(c,r);

	int tsize=0;
	int tiletype=0;
	char* tileData=NULL;

	std::string tilefn=createElevationTileFileName(lev,c,r);
	//从数据库文件中读取该瓦片数据，并判断地形数据的格式
	if (_cache->readDataFromCache(tilefn,&tileData,tsize,tiletype))
	{
		/////////////////////////////读取readmap地形数据或者tif类型的地形数据///////////////////////
		if(tiletype==8)//tif 
		{
			/////////////////////////////the way of read tif,like read image
			std::string imgstr;
			imgstr.assign(tileData,tsize);
			std::stringstream ss;
			ss.str(imgstr);
			osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension("tif");
			if (reader.valid())
			{
				osg::ref_ptr<osg::Image> image=(reader->readImage(ss)).getImage();
				//////////////////////////////////////////////////////
				//FILE* temptFile;
				//temptFile=fopen("/storage/emulated/0/osgEarth/DATA/temptFile.tif","wb");
				//fwrite(*tileData,1,tsize,temptFile);
				//fclose(temptFile);
				//osg::ref_ptr<osg::Image> image=osgDB::readImageFile("temptFile.tif");
				//FILE* temptFile;
				//std::stringstream ss;
				//ss<<"L"<<lev<<"C"<<c<<"R"<<r<<".tif";
				//std::string tflie=/*"/storage/emulated/0/osgEarth/DATA/"+*/ss.str();
				//temptFile=fopen(tflie.c_str(),"wb");
				//if (temptFile)
				//{
				//	fwrite(tileData,1,tsize,temptFile);
				//	fclose(temptFile);
				//}

				//osg::ref_ptr<osg::Image> image=osgDB::readImageFile(tflie);
				//remove(tflie.c_str());

				//if (!image.valid())
				//{
				//	return NULL;
				//}
				osgEarth::ImageToHeightFieldConverter conv;
				nhf = conv.convert( image.get() );
			}//reader valid
				
		}//if tiletype==8
		else if (tiletype==9)//zlib格式，吉奥自定义的压缩地形格式，解压后的大小固定为150*150
		{
			//若地形类型为int16
			if (_demType == "int16")
			{
				uLong tlen=tsize+1;
				uLong blen=45000;
				unsigned char * buf= new unsigned char [blen];
				//地形数据解压成功
				if (Z_OK==uncompress(buf,&blen,(unsigned char*)(tileData),tlen))//uncompress the terrain successfully
				{
					//创建新的高度场
					nhf=new osg::HeightField();//allocate the height field
					nhf->allocate( 150, 150 );
					osg::FloatArray* floats = nhf->getFloatArray();

					float invalidDem=0.9999;//设置异常地形高度值

					invalidDem = -1.0;
					//该for循环用于将吉奥地形数据转换为osgEarth格式下的地形数据，并对异常高度值处理
					for (int i=0;i<150;i++)
					{
						for (int j=0;j<150;j++)
						{
							//change the invalid value to -1.0,and use the way of geo_interprotation
							if (abs((float)((short*)buf)[(150-i-1)*150+j]-invalidDem)<=FLT_EPSILON)
							{
								floats->at( i*150+j )=0.9999;
							}
							else
							{
								floats->at( i*150+j )=((short*)buf)[(150-i-1)*150+j];
							}
						}//for j
					}// for i
				}//if uncompress successfully
				delete[] buf;
				buf=NULL;

			}//if int16
			//若地形类型为float32
			else if(_demType == "float32")
			{
				uLong tlen=tsize+1;
				uLong blen=90000;
				unsigned char * buf= new unsigned char [blen];

				if (Z_OK==uncompress(buf,&blen,(unsigned char*)(tileData),tlen))//uncompress the terrain successfully
				{
					nhf=new osg::HeightField();//allocate the height field
					nhf->allocate( 150, 150 );
					osg::FloatArray* floats = nhf->getFloatArray();

					float invalidDem=0.9999;//the invaildDem in heightutil
					invalidDem = 0.0;

					for (int i=0;i<150;i++)
					{
						for (int j=0;j<150;j++)
						{
							//change the invalid value to -1.0,and use the way of geo_interprotation
							if (abs(((float*)buf)[(150-i-1)*150+j]-invalidDem)<=FLT_EPSILON)
							{
								floats->at( i*150+j )=0.9999;
							}
							else
							{
								floats->at( i*150+j )=((float*)buf)[(150-i-1)*150+j];
							}
						}//for j
					}// for i
				}//if uncompress successfully
				delete[] buf;
				buf=NULL;
			}//if float32
		}//if tiletype==9
	}//if read successfully

	if (tileData!=NULL)
	{
		delete[] tileData;
		tileData=NULL;
	}

	return nhf.release();
}

osg::HeightField* ElevationTileSource::readTerrianfromServiceOrCache(const osgEarth::TileKey& key ,osgEarth::ProgressCallback*     progress)
{
	osg::ref_ptr<osg::HeightField> nhf=NULL;

	unsigned int lev,r,c;
	lev=key.getLevelOfDetail()+1;
	key.getTileXY(c,r);

	nhf=readTerrianfromSqlite(key,progress);
	
	if(!nhf.valid())//缓存文件中不存在该瓦片数据则读取uri并存入缓存
	{
		//get the url and write the data to cache sqlite
		std::string tile_uri;
		//////////////////////////////////////////////////geoglobe地形瓦片服务处理//////////////////////////////////////////////
		if (_options._servicetype== GLOBE_NAME::eGeoGlobeTile)
		{
			tile_uri=creatGeoGlobeURL(key);//创建地形瓦片请求服务
		
			//////judge the url if exist and the data is valid//////
			osgEarth::ReadResult rr= osgEarth::URI(tile_uri).readString();

			/*Download dl;
			if (dl.downloadData(tile_uri))*/
			if (rr.succeeded())
			{
				std::string elesrc=rr.getString();
				long tlen=elesrc.size();
			
				if (_demType == "int16")
				{
					uLong blen=45000;
					unsigned char * buf=  new unsigned char [blen];
					//下载的地形数据解压成功
					if (Z_OK==uncompress(buf,&blen,(unsigned char*)elesrc.c_str(),elesrc.size()))//uncompress the terrain successfully
					{
						//if uncompress successfully, first write into cache sqlite,then create the nhf
						/////////////////write cache sqlite////////////
						std::string tilefn=createElevationTileFileName(lev,c,r);
						if (_cache->writeDataToCache(tilefn,tlen,9,(char*)(elesrc.c_str())))
						{
							_cache->commitCache(tilefn);
						}
						/////////////////////////////////////cache over////////////////////////////
					
						///////////////////////将吉奥地形格式转换为osgEarth地形格式/////////////////////////////
						nhf=new osg::HeightField();//allocate the height field
						nhf->allocate( 150, 150 );
						osg::FloatArray* floats = nhf->getFloatArray();

						float invalidDem=0.9999;//the invaildDem in heightutil

						invalidDem = -1.0;
						for (int i=0;i<150;i++)
						{
							for (int j=0;j<150;j++)
							{
								//change the invalid value to -1.0,and use the way of geo_interprotation
								if (abs((float)((short*)buf)[(150-i-1)*150+j]-invalidDem)<=FLT_EPSILON)
								{
									floats->at( i*150+j )=0.9999;
								}
								else
								{
									floats->at( i*150+j )=((short*)buf)[(150-i-1)*150+j];
								}
							}//for j
						}// for i
					}//if uncompress successfully
					delete[] buf;
					buf=NULL;

				}//if int16
				else if(_demType == "float32")
				{
					uLong blen=90000;
					unsigned char * buf=  new unsigned char [blen];

					if (Z_OK==uncompress(buf,&blen,(unsigned char*)elesrc.c_str(),elesrc.size()))//uncompress the terrain successfully
					{
						//if uncompress successfully, first write into cache sqlite,then create the nhf
						/////////////////write cache sqlite////////////
						std::string tilefn=createElevationTileFileName(lev,c,r);
						if (_cache->writeDataToCache(tilefn,tlen,9,(char*)(elesrc.c_str())))
						{
							_cache->commitCache(tilefn);
						}
						/////////////////////////////////////cache over////////////////////////////

						///////////////////////create the nhf/////////////////////////////
						nhf=new osg::HeightField();//allocate the height field
						nhf->allocate( 150, 150 );
						osg::FloatArray* floats = nhf->getFloatArray();

						float invalidDem=0.9999;//the invaildDem in heightutil

						invalidDem = 0.0;
						for (int i=0;i<150;i++)
						{
							for (int j=0;j<150;j++)
							{
								//change the invalid value to -1.0,and use the way of geo_interprotation
								if (abs(((float*)buf)[(150-i-1)*150+j]-invalidDem)<=FLT_EPSILON)
								{
									floats->at( i*150+j )=0.9999;
								}

								else
								{
									floats->at( i*150+j )=((float*)buf)[(150-i-1)*150+j];
								}
							}//for j
						}// for i
					}//if uncompress successfully
					delete[] buf;
					buf=NULL;
				}//if float32
			}//if rr.succeeded
		
		}//if servicetype==geoglobetile
		else if (_options._servicetype== GLOBE_NAME::eReadMap)
		{
			tile_uri=createReadMapURL(key);
			//////judge the url if exist and the data is valid//////
			osgEarth::ReadResult rr= osgEarth::URI(tile_uri).readString();

			if (rr.succeeded())//if (dl.downloadData(image_uri))
			{
				std::string imgsrc=rr.getString();
				long imglen=imgsrc.size();

				int tiletype=8;//tif
			
				std::stringstream ss;
				ss<<imgsrc;
				osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension("tif");

				if (reader.valid())
				{
					osg::ref_ptr<osg::Image>image=(reader->readImage(ss)).getImage();
					if (image.valid())///////////////////////download successfully
					{
						/////////////////////////////write into sqlite////////////////////////////////////////
						std::string tilefn=createElevationTileFileName(lev,c,r);
						if (_cache->writeDataToCache(tilefn,imgsrc.size(),tiletype,(char*)(imgsrc.c_str())))
						{
							_cache->commitCache(tilefn);
						}
						//////////////////////////////////write into sqlite///////////////////////////////////////////
						osgEarth::ImageToHeightFieldConverter conv;
						nhf = conv.convert( image.get() );
					}//if image valid
				}//if reader valid
			
			}//if connect
		}//if servicetype==readmap
		/////////////////////////////////readmap///////////////////////////////////
	}//if cache nhf is not valid 

	return nhf.release();
}

std::string ElevationTileSource::creatGeoGlobeURL(const osgEarth::TileKey& key)
{
	//http://192.168.42.178:9010/YINGXIANG/services/tile/GetCapabilities 获取capabilities
	//http://192.168.42.178:9010/YINGXIANG/DataServer?l=17&x=222&y=33333 瓦片请求
	//http://192.168.42.37:7005/dem/services/tile/GetCapabilities
	//http://192.168.42.37:7005/dem/DataServer?l=17&x=4444&y=444444

	unsigned int lev,r,c;
	lev=key.getLevelOfDetail()+1;
	key.getTileXY(c,r);

	std::string strTile = _options.url()->full();
	int pos=strTile.find("/services/tile");
	int n=strTile.size()-pos+1;
	strTile.replace(pos,n,"");
	std::stringstream buf;
	/*buf<<"/DataServer?l="<<key_L<<"&x="<<key_Y<<"&y="<<key_X;*/
	buf<<"/DataServer?l="<<lev<<"&x="<<c<<"&y="<<r;
	/*buf<<"/DataServer?l="<<17<<"&x="<<104307<<"&y="<<23027;*/
	strTile+=buf.str();
	/*strTile="http://192.168.42.178:9010/YINGXIANG/DataServer?l=17&x=104307&y=23027";*/
	//__android_log_print(ANDROID_LOG_INFO, "TerrainTileSource", "URL: %s",strTile.c_str());
	return strTile;

}

std::string ElevationTileSource::createReadMapURL( const osgEarth::TileKey& key ) 
{
	unsigned int x, y;
	key.getTileXY(x, y);
	unsigned int numRows, numCols;
	key.getProfile()->getNumTiles(key.getLevelOfDetail(), numCols, numRows);
	y  = numRows - y - 1;
	std::stringstream ss;
	ss<<"http://readymap.org/readymap/tiles/1.0.0/9/" << key.getLevelOfDetail() << "/" << x << "/" << y << "." << "tif";
	std::string url=ss.str();
	return url;
}

//由osgEarth调用，地形图层加载函数
osg::HeightField*ElevationTileSource::createHeightField(const osgEarth::TileKey& key,osgEarth::ProgressCallback*     progress)
{
	
	osg::ref_ptr<osg::HeightField> nhf=NULL;
	if (_options._servicetype== GLOBE_NAME::eLocal)
	{
		nhf=readTerrianfromSqlite(key,progress);
	}
	
	if (_options._servicetype== GLOBE_NAME::eGeoGlobeTile&&_options.url()->full()!="")
	{
		nhf=readTerrianfromServiceOrCache(key,progress);
	}
	if (_options._servicetype== GLOBE_NAME::eReadMap)
	{
		nhf=readTerrianfromServiceOrCache(key,progress);
	}
	return nhf.release();
}
//该函数不调用
osg::HeightField* ElevationTileSource::createNewHeightField(const osgEarth::TileKey& key,std::string source,int surhfsize)
{
	//对读取的地形数据进行预处理	
	unsigned char* tileData=(unsigned char*)source.c_str();
	uLong tlen=source.size();
	uLong blen=45000;
	unsigned char * buf=  new unsigned char [blen];
	
	uncompress(buf,&blen,tileData,tlen);//解压地形数据

	short  * tempt=new short [surhfsize*surhfsize];
	tempt=(short*)buf;
	short * rightbuf=new short [surhfsize*surhfsize];
	//处理解压数据,重置，行列编号起始不同
	for (int i=0;i<150;i++)
	{
		for (int j=0;j<150;j++)
		{

			if (tempt[(150-i)*150-(150-j)]<=-1)
			{
				rightbuf[i*150+j]=INVALIDDATA;

			}
			else 
				rightbuf[i*150+j]=tempt[(150-i)*150-(150-j)];
		}
	}
	//利用原始地形数据进行线性内插，从150*150变为33*33，设置高度场nhf
	osg::ref_ptr<osg::HeightField> nhf=new osg::HeightField;
	nhf->allocate( 33, 33 );
	unsigned numColumns = nhf->getNumColumns();
	unsigned numRows    = nhf->getNumRows();
	osgEarth::GeoExtent kextent=key.getExtent();
	double   xmin       = kextent.xMin();
	double   ymin       = kextent.yMin();
	double   dx         = kextent.width() / (double)(numColumns-1);
	double   sourcedx   = kextent.width()/(double)(surhfsize-1);
	double   dy         = kextent.height() / (double)(numRows-1);
	double   sourcedy   = kextent.height()/(double)(surhfsize-1);

	for (unsigned nc = 0; nc < numColumns; ++nc)
	{
		double x = xmin + (dx * (double)nc);
		for (unsigned nr = 0; nr < numRows; ++nr)
		{
			double y = ymin + (dy * (double)nr);
			double sc = osg::clampBetween( (x - xmin) / sourcedx, 0.0, (double)(surhfsize-1) );
			double sr = osg::clampBetween( (y - ymin) / sourcedy, 0.0, (double)(surhfsize-1) );
			//--------------------------------------------//
			int rowMin = osg::maximum((int)floor(sr), 0);
			int rowMax = osg::maximum(osg::minimum((int)ceil(sr), (int)(surhfsize-1)), 0);
			int colMin = osg::maximum((int)floor(sc), 0);
			int colMax = osg::maximum(osg::minimum((int)ceil(sc), (int)(surhfsize-1)), 0);

			if (rowMin > rowMax) rowMin = rowMax;
			if (colMin > colMax) colMin = colMax;

			float urHeight = rightbuf[colMax+rowMax*surhfsize];
			float llHeight = rightbuf[colMin+rowMin*surhfsize];
			float ulHeight = rightbuf[colMin+rowMax*surhfsize];
			float lrHeight = rightbuf[colMax+rowMin*surhfsize];

			if (urHeight == INVALIDDATA || llHeight == INVALIDDATA || ulHeight == INVALIDDATA || lrHeight == INVALIDDATA)
			{
				nhf->setHeight(nc,nr,NO_DATA_VALUE);
			}
			else
			{
				if ((colMax == colMin) && (rowMax == rowMin))
				{
					//OE_NOTICE << "Exact value" << std::endl;
					nhf->setHeight(nc,nr,rightbuf[(int)sc+(int)sr*surhfsize]);
				}
				else if (colMax == colMin)
				{
					//OE_NOTICE << "Vertically" << std::endl;
					//Linear interpolate vertically
					nhf->setHeight(nc,nr, ((double)rowMax - sr) * llHeight + (sr - (double)rowMin) * ulHeight);
				}
				else if (rowMax == rowMin)
				{
					//OE_NOTICE << "Horizontally" << std::endl;
					//Linear interpolate horizontally
					nhf->setHeight(nc,nr, ((double)colMax - sc) * llHeight + (sc - (double)colMin) * lrHeight);
				}
				else
				{
					//OE_NOTICE << "Bilinear" << std::endl;
					//Bilinear interpolate
					float r1 = ((double)colMax - sc) * llHeight + (sc - (double)colMin) * lrHeight;
					float r2 = ((double)colMax - sc) * ulHeight + (sc - (double)colMin) * urHeight;

					//OE_INFO << "r1, r2 = " << r1 << " , " << r2 << std::endl;

					nhf->setHeight(nc,nr,  ((double)rowMax - sr) * r1 + (sr - (double)rowMin) * r2);
				}
			}//对非异常高度内插

		}//nr遍历结束
	}//nc遍历结束

	//若为tif格式数据，即地形数据

	return nhf.release();
}


bool ElevationTileSource::hasDataForFallback(const osgEarth::TileKey& key) const
{
	//matics: might have data.
	unsigned int lev,r,c;
	lev=key.getLevelOfDetail()+1;
	key.getTileXY(c,r);
	std::stringstream ss;
	ss<<lev<<"/"<<c<<"/"<<r<<".1.osgearth_engine_mp_tile_elevation";
	std::string tilefn=ss.str();
	bool iscached=_cache->isInCache(tilefn);
	//If no data extents are provided, just see if cache
	if (this->getDataExtents().size() == 0&&iscached)
	{
		return true;
	}
	const osgEarth::GeoExtent& keyExtent = key.getExtent();
	bool intersectsData = false;

	for (osgEarth::DataExtentList::const_iterator itr = this->getDataExtents().begin(); itr != this->getDataExtents().end(); ++itr)
	{
		if ((keyExtent.intersects( *itr )) &&
			(!itr->minLevel().isSet() || itr->minLevel() <= key.getLOD()))
		{
			intersectsData = true;
			break;
		}
	}
	if (intersectsData&&iscached)
	{
		return true;
	}

	return false;
}
//#include <android/log.h>
bool ElevationTileSource::hasData(const osgEarth::TileKey& key) const
{
	//sematics: might have data.
	//__android_log_print(ANDROID_LOG_INFO, "ElevationTileSource::hasData", "%d",_dataExtents[0].);
	//If no data extents are provided, just return true
	if (_dataExtents.size() == 0)
		return true;

	unsigned int lod = key.getLevelOfDetail();

	// Remap the lod to an appropriate lod if it's not in the same SRS
	if (!key.getProfile()->isHorizEquivalentTo( getProfile() ) )
	{
		lod = getProfile()->getEquivalentLOD( key.getProfile(), key.getLevelOfDetail() );
	}

	// Check the explicit max data override:
	if (lod > _maxLevel)
		return false;

	bool intersectsData = false;
	const osgEarth::GeoExtent& keyExtent = key.getExtent();

	for (osgEarth::DataExtentList::const_iterator itr = _dataExtents.begin(); itr != _dataExtents.end(); ++itr)
	{
		if ((keyExtent.intersects( *itr )) &&
			(!itr->minLevel().isSet() || itr->minLevel() <= lod ) &&
			(!itr->maxLevel().isSet() || itr->maxLevel() >= lod ))
		{
			intersectsData = true;
			break;
		}
	}

	return intersectsData;
}

GLOBE_ENDNS