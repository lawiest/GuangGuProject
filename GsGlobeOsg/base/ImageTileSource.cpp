//#include "stdafx.h"
#include "ImageTileSource.h"
#include "sqlite3.h"
#include <osgEarth/ImageUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

GLOBE_NS

ImageTileSource::ImageTileSource(BaseTileSourceOptions options) 
							: osgEarth::TileSource(options), _options(options)
{

	m_QueryType = eDefault;

	//创建读取以及缓存影像数据的接口类
	_cache = new GeoGlobe::Tool::SQLiteCache(*(options.cachePath()), options._cacheType);

	//若为本地数据则不需要创建缓存
	if (options._servicetype == GLOBE_NAME::eLocal)
	{
	}
	//若为天地图服务，创建影像数据的缓存
	else if (options._servicetype == GLOBE_NAME::eWMTS)
	{
		_cache->createImageCache();
	}
	//根据吉奥瓦片服务的类型创建影像或者地形缓存
	else if (options._servicetype == GLOBE_NAME::eGeoGlobeTile)
	{
		if (options._cacheType == GeoGlobe::Tool::eImageCache)
		{
			_cache->createImageCache();
		}
		else if (options._cacheType == GeoGlobe::Tool::eElevationCache)
		{
			_cache->createElevationCache();
		}
	}
}

ImageTileSource::ImageTileSource(KERNEL_NAME::GsTileClass* ptrTileClass) : osgEarth::TileSource(osgEarth::TileSourceOptions())
{
	m_ptrTileClass = ptrTileClass;
	m_QueryType = eTileClass;
}

osgEarth::DataExtentList& ImageTileSource::DataExtentList()
{
	return _dataExtents;
}

//截取字符串函数
std::string ImageTileSource::stringRight(const std::string& input, const int num)
{
	std::string output(input);
	int m_temnum = output.size() - num;
	return output.substr(m_temnum, num);
}

osgEarth::Status ImageTileSource::initialize(const osgDB::Options* dbOptions)
{
	if (!getProfile())
	{
		setProfile(osgEarth::Registry::instance()->getGlobalGeodeticProfile());
	}

	if (_options._servicetype == GLOBE_NAME::eWMTS)
	{
		//对于天地图服务，在此处获得服务的元信息，并判断是否调试成功
		initializebyWMTS(dbOptions, getProfile());
	}


	if (m_QueryType == eDefault)
	{
		_dataExtents.push_back(osgEarth::DataExtent(osgEarth::GeoExtent(this->getProfile()->getSRS(),
			_options._metadata.bbox.west,
			_options._metadata.bbox.south,
			_options._metadata.bbox.east,
			_options._metadata.bbox.north),
			_options._metadata.toplevel,
			_options._metadata.bottomlevel));

		_maxLevel = _options._metadata.bottomlevel;
	}

	return osgEarth::STATUS_OK;
}

ImageTileSource::~ImageTileSource()
{

}


//具体注释参照ImageTileSource.h文件
 std::string ImageTileSource::createImageTileFileName(int lev,int col,int row)
 {	 
	 std::stringstream ss;
	 ss<<lev<<"/"<<col<<"/"<<row<<".1.osgearth_engine_mp_tile_image";
	 
	 return ss.str();
 }

osg::Image*ImageTileSource::readImagefromSqlite(const osgEarth::TileKey& key, osgEarth::ProgressCallback* progress )
{
	osg::ref_ptr<osg::Image> image=NULL;

	unsigned int lev,r,c;
	lev=key.getLevelOfDetail()+1;
	key.getTileXY(c,r);

	int tsize=0;
	int tiletype=0;
	char* tileData=NULL;

	std::string tilefn=createImageTileFileName(lev,c,r);
	
	//从数据库文件中读取该瓦片数据，并判断影像图片的格式，返回影像
	if (_cache->readDataFromCache(tilefn,&tileData,tsize,tiletype))
	{
		std::string exname;
		switch(tiletype)
		{
		case 0:
			exname="jpg";
			break;
		case 3: 
			exname="png";
			break;
		case 8:
			exname="tiff";
			break;
		default:
			return NULL;
		}
		std::string imgsrc;
		imgsrc.assign(tileData,tsize);
		if (imgsrc.c_str()[1]=='P'&&imgsrc.c_str()[2]=='N'&&imgsrc.c_str()[3]=='G')//png
			//if (dl.data[1]=='P'&&dl.data[2]=='N'&&dl.data[3]=='G')
		{
			exname="png";
		}
		if((imgsrc.c_str()[6]=='J'&&imgsrc.c_str()[7]=='F'&&imgsrc.c_str()[8]=='I'&&imgsrc.c_str()[9]=='F')||(imgsrc.c_str()[0]==-1&&imgsrc.c_str()[1]==-40&&imgsrc.c_str()[2]==-1))
			//if (tileData[0]==-1&&tileData[1]==-40&&tileData[2]==-1)//jpg
			//if((dl.data[6]=='J'&&dl.data[7]=='F'&&dl.data[8]=='I'&&dl.data[9]=='F')||(dl.data[0]==-1&&dl.data[1]==-40&&dl.data[2]==-1))
		{
			exname="jpg";
		}
		std::stringstream ss;
		ss<<imgsrc;
		osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension(exname);
		if (reader.valid())
		{
			image=(reader->readImage(ss)).getImage();	
		}//if reader
		
	}//if read successfully
	 if (tileData!=NULL)
	 {
			delete[] tileData;
			tileData=NULL;
	 }
	return image.release();
}

osg::Image* ImageTileSource::readImagefromServiceOrCache(const osgEarth::TileKey& key,osgEarth::ProgressCallback* progress)
{
	osg::ref_ptr<osg::Image> image=NULL;
	
	image=readImagefromSqlite(key,progress);

	if(!image.valid())//缓存中不存在
	{
		//缓存中不存在则读取uri并存入缓存
		unsigned int lev,r,c;
		lev=key.getLevelOfDetail()+1;
		key.getTileXY(c,r);

		std::string image_uri;
		if (_options._servicetype== GLOBE_NAME::eWMTS)
		{
			if (lev>=20)//tianditu no data image
			{
				return NULL;
			}
			image_uri=createTiandituURL(key,_protodataurl);//创建天地图瓦片请求服务
		}
		if (_options._servicetype== GLOBE_NAME::eGeoGlobeTile)
		{
			image_uri=createGeoGlobeURL(key);
		}

		osgEarth::ReadResult rr= osgEarth::URI(image_uri).readString();
		if (rr.succeeded())
		//if (dl.downloadData(image_uri))
		{
			std::string imgsrc=rr.getString();
			long imglen=imgsrc.size();
			
			////////////////judge the format of image
			int tiletype=100;//unknown format
			std::string exname;
			if (imgsrc.c_str()[1]=='P'&&imgsrc.c_str()[2]=='N'&&imgsrc.c_str()[3]=='G')//png
			//if (dl.data[1]=='P'&&dl.data[2]=='N'&&dl.data[3]=='G')
			{
				tiletype=3;
				exname="png";
			}
			if((imgsrc.c_str()[6]=='J'&&imgsrc.c_str()[7]=='F'&&imgsrc.c_str()[8]=='I'&&imgsrc.c_str()[9]=='F')||(imgsrc.c_str()[0]==-1&&imgsrc.c_str()[1]==-40&&imgsrc.c_str()[2]==-1))
			//if (tileData[0]==-1&&tileData[1]==-40&&tileData[2]==-1)//jpg
			//if((dl.data[6]=='J'&&dl.data[7]=='F'&&dl.data[8]=='I'&&dl.data[9]=='F')||(dl.data[0]==-1&&dl.data[1]==-40&&dl.data[2]==-1))
			{
				tiletype=0;
				exname="jpg";
			}
			//////////////////////////
			//std::string imgsrc;
			//imgsrc.assign(dl.data, dl.size);
			std::stringstream ss;
			ss<<imgsrc;
			osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension(exname);
			if(reader.valid())
			{
				image=(reader->readImage(ss)).getImage();
				if (image.valid())///////////////////////download successfully
				{
					/////////////////////////////write into sqlite////////////////////////////////////////
					std::string tilefn=createImageTileFileName(lev,c,r);
					if (_cache->writeDataToCache(tilefn,imgsrc.size(),tiletype,(char*)(imgsrc.c_str())))
					{
						_cache->commitCache(tilefn);
					}
					//////////////////////////////////write into sqlite///////////////////////////////////////////
				}//if image valid
			}//if reader valid
		}//if connect
	}//if no cache
	return image.release();
}

KERNEL_NAME::GsTileEncodingType ImageTileSource::ImageType(const char* imageData)
{
	unsigned char png_type[9] = { 0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A,'/0' };
	unsigned char file_head[9];

	for (int i = 0;i < 8;++i)
	{
		file_head[i] = imageData[i];
	}

	file_head[8] = '/0';

	switch (file_head[0])
	{
	case 0xff:
		if (file_head[1] == 0xd8)
			return KERNEL_NAME::eJpgType;//jpeg  
	case 0x49:
		if (file_head[1] == 0x49)
			return KERNEL_NAME::eTiffType;//tiff  
	case 0x47:
		if (file_head[1] == 0x49)
			return KERNEL_NAME::eGifType;//tiff  
	case 0x42:
		if (file_head[1] == 0x4D)
			return KERNEL_NAME::eBmpType;//bmp  
	case 0x89:
		if (file_head[1] == png_type[1] && file_head[2] == png_type[2] && file_head[3] == png_type[3] && file_head[4] == png_type[4] &&
			file_head[5] == png_type[5] && file_head[6] == png_type[6] && file_head[7] == png_type[7])
			return KERNEL_NAME::ePngType;//png  
	default:
		return KERNEL_NAME::eUnKnownType;
	}
}

osg::Image* ImageTileSource::QueryImageFromTileClass(const osgEarth::TileKey& key, osgEarth::ProgressCallback* progress)
{
	osg::ref_ptr<osg::Image> image = NULL;

	unsigned int lev, r, c;
	lev = key.getLevelOfDetail() + 1;
	key.getTileXY(c, r);

	if (NULL == m_ptrTileClass)
		return NULL;

	KERNEL_NAME::GsTilePtr ptrTile = m_ptrTileClass->Tile(lev, r, c);

	if (NULL == ptrTile)
		return NULL;

	char * ptrData = NULL;
	int nSize = 0;
	ptrData = (char*)ptrTile->TileDataPtr();
	nSize = ptrTile->TileDataLength();

	if (NULL == ptrData)
		return NULL;

	std::string strImageType = "";

	//switch (ptrTile->TileType())
	switch (ImageType(ptrData))
	{
	case KERNEL_NAME::eJpgType:
		strImageType = "jpg";
		break;
	case KERNEL_NAME::ePngType:
		strImageType = "png";
		break;
	case KERNEL_NAME::eTiffType:
		strImageType = "tiff";
		break;
	case KERNEL_NAME::eBmpType:
		strImageType = "bmp";
		break;
	case KERNEL_NAME::eGifType:
		strImageType = "jif";
		break;
	default:
		return NULL;
	}

	std::string imgsrc;
	imgsrc.assign(ptrData, nSize);

	std::stringstream ss;
	ss << imgsrc;
	osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension(strImageType);

	if (reader.valid())
		image = (reader->readImage(ss)).getImage();

	return image.release();
}

//由osgEarth调用，影像图层加载函数
osg::Image* ImageTileSource::createImage( const osgEarth::TileKey& key, osgEarth::ProgressCallback* progress )
{
	osg::ref_ptr<osg::Image> image = NULL;		
	
	if (m_QueryType == eTileClass)
	{
		return QueryImageFromTileClass(key, progress);
	}




	//若为本地数据则直接从本地文件读取
	if (_options._servicetype== GLOBE_NAME::eLocal)
	{
		image=readImagefromSqlite(key,progress);
	}
	//若为服务数据，则根据服务类型获取url并缓存数据到本地
	if (_options._servicetype== GLOBE_NAME::eWMTS&&_options.url()->full()!="")
	{
		image=readImagefromServiceOrCache( key,progress);
	}
	if (_options._servicetype== GLOBE_NAME::eGeoGlobeTile&&_options.url()->full()!="")
	{
		image=readImagefromServiceOrCache(key, progress);
	}
	return image.release();
}

//获取天地图服务的元信息
void  ImageTileSource::initializebyWMTS(const osgDB::Options* dbOptions,const osgEarth::Profile* overrideProfile)
{
	osg::ref_ptr<const osgEarth::Profile> result;
	char sep=_options.url()->full().find_first_of('?')==std::string::npos?'?':'&';

	osgEarth::URI capUrl=_options.url().value();
	if (sep=='?')
	{
		capUrl = osgEarth::URI(
			_options.url()->full() + 
			sep + 
			std::string("SERVICE=WMTS") +
			std::string("&VERSION=") + _options.wmtsVersion().value() +
			std::string("&REQUEST=GetCapabilities")+
			std::string("&tk=") + _options.m_strToken);
	}
	if (_options.cachePath().isSet())
	{
		std::string capCachePath = osgDB::getNameLessAllExtensions(_options.cachePath().value())+"cap.xml";
		_capWMTS = osgEarth::Util::WMTSCapabilitiesReader::read( capUrl.full(),capCachePath,dbOptions);
	}
	else
	{
		_capWMTS = osgEarth::Util::WMTSCapabilitiesReader::read( capUrl.full(),dbOptions);
	}
	//_capWMTS = osgEarth::Util::WMTSCapabilitiesReader::read( capUrl.full(), 0L ); //getOptions() );
	int i = 0,j=0;
	if ( !_capWMTS )
	{
		//OE_WARN << "[osgEarth::WMTS] Unable to read WMTS GetCapabilities." << std::endl;
		//__android_log_print(ANDROID_LOG_ERROR, "[osgEarth::WMTS]", "Unable to read WMTS GetCapabilities");
		//return;
		_srsToUse = "wgs84";
		result = osgEarth::Registry::instance()->getGlobalGeodeticProfile();
		if (overrideProfile)
		{
		   result = overrideProfile;
		}

		setProfile( result.get() );
		return;
	}
	else
	{
		if(_capWMTS->getLayer().empty()||_capWMTS->getMatrixSet().empty())
		{
			//OE_WARN << "[osgEarth::WMTS] Unable to establish profile." << std::endl;
			///__android_log_print(ANDROID_LOG_ERROR, "[osgEarth::WMTS]", "Unable to establish profile");
			_srsToUse = "wgs84";
			result = osgEarth::Registry::instance()->getGlobalGeodeticProfile();
			setProfile( result.get() );
		}
		//OE_INFO << "[osgEarth::WMTS] Got capabilities from " << capUrl.full() << std::endl;
		//__android_log_print(ANDROID_LOG_ERROR, "[osgEarth::WMTS]", "Got capabilities: %s", (capUrl.full()).c_str());
		for(;i<_capWMTS->getLayer().size();i++)
		{
			if(_options.layer() == _capWMTS->getLayerAt(i).ID)
			{
				_formatToUse = _capWMTS->getLayerAt(i).format;
				std::string link = _capWMTS->getLayerAt(i).TileMatrixSetLink;
				for(;j<_capWMTS->getMatrixSet().size();j++)
					if(link==_capWMTS->getMatrixSetAt(j).ID)
					{
						_srsToUse = _capWMTS->getMatrixSetAt(j).SupportedCRS;
						break;
					}
					break;
			}
			//if(i == _capWMTS->getLayer().size())
			//OE_INFO<<"[osgEarth::WMTS]The layer required doesn't exit"<<std::endl;
			//__android_log_print(ANDROID_LOG_ERROR, "[osgEarth::WMTS]", "The layer required doesn't exit");
		}
	}

	if ( _formatToUse.empty() )
		_formatToUse = "tiles";
	if ( _srsToUse.empty() )
		_srsToUse = "wgs84";

	std::string wmtsFormatToUse = _options.wmtsFormat().value();

	//Initialize the WMS request prototype
	std::stringstream buf;

	// first the mandatory keys:
	if(i ==_capWMTS->getLayer().size())
		i = 0;
	buf
		<< std::fixed << _options.url()->full() << sep
		<< "SERVICE=WMTS"
		<< "&VERSION=" << _options.wmtsVersion().value()
		<< "&REQUEST=GetTile"
		<< "&LAYER=" << _capWMTS->getLayerAt(i).ID
		<< "&TILEMATRIXSET=" << _capWMTS->getMatrixSetAt(j).ID
		<< "&STYLE=" << _capWMTS->getLayerAt(i).style
		//<< "&FORMAT=" << ( wmtsFormatToUse.empty() ?  _formatToUse : wmtsFormatToUse ); 
		<< "&FORMAT=" << _capWMTS->getLayerAt(i).format; 
	_protodataurl = buf.str();
	m_layerindex = i;
	m_matrixsetindex = j;
	int pos=_srsToUse.find(":EPSG",0);
	_srsToUse=stringRight(_srsToUse,(_srsToUse.size()-pos-1));
	_srsToUse = osgDB::convertToLowerCase(_srsToUse);

	_srsToUse = "wgs84";
	osg::ref_ptr<osgEarth::SpatialReference> wmts_srs = osgEarth::SpatialReference::create( _srsToUse );

	// check for spherical mercator:
	if ( wmts_srs.valid() && wmts_srs->isEquivalentTo( osgEarth::Registry::instance()->getGlobalMercatorProfile()->getSRS() ) )
	{
		result = osgEarth::Registry::instance()->getGlobalMercatorProfile();
	}
	else if (wmts_srs.valid() && wmts_srs->isEquivalentTo( osgEarth::Registry::instance()->getGlobalGeodeticProfile()->getSRS()))
	{
		result = osgEarth::Registry::instance()->getGlobalGeodeticProfile();
	}

	if ( !result.valid() && wmts_srs->isGeographic())
	{
		result = osgEarth::Registry::instance()->getGlobalGeodeticProfile();
	}

	//Use the override profile if one is passed in.
	if (overrideProfile)
	{
		result = overrideProfile;
	}

	// TODO

	setProfile( result.get() );
	//OE_NOTICE << "[osgEarth::WMTS] Profile=" << result->toString() << std::endl;

}

std::string ImageTileSource::createTiandituURL(const osgEarth::TileKey& key,std::string Url)
{
	int key_X=key.getTileX();
	int key_Y=key.getTileY();
	int key_L=key.getLevelOfDetail();

	std::ostringstream buf;
	if(key_L<18&&_capWMTS!=NULL)
	{
		//int level = std::atoi( _capWMTS->getMatrixSetAt(m_matrixsetindex).getTileMatrixAt(key_L).ID.c_str());
		//level -= 2;

		int level = key_L + 1;

		buf<<"&TILEMATRIX="<< level <<"&TILEROW="<<key_Y<<"&TILECOL="<<key_X;
	}
	Url+=buf.str(); 
	Url += std::string("&tk=") + _options.m_strToken;
	return Url;
}

std::string ImageTileSource::createGeoGlobeURL(const osgEarth::TileKey& key)
{
	//http://192.168.42.178:9010/YINGXIANG/services/tile/GetCapabilities 获取capabilities
	//http://192.168.42.178:9010/YINGXIANG/DataServer?l=17&x=222&y=33333 瓦片请求
	//http://192.168.42.37:7005/dem/services/tile/GetCapabilities
	//http://192.168.42.37:7005/dem/DataServer?l=17&x=4444&y=444444
	//__android_log_print(ANDROID_LOG_ERROR, "osgEarth::GeoGlobeTile", "create URL");
	unsigned int lev,r,c;
	lev=key.getLevelOfDetail()+1;
	key.getTileXY(c,r);

	std::string strTile = _options.url()->full();
	int pos=strTile.find("/services/tile");
	if (pos == std::string::npos)
		return "";

	int n=strTile.size()-pos+1;
	strTile.replace(pos,n,"");
	std::stringstream buf;
	/*buf<<"/DataServer?l="<<key_L<<"&x="<<key_Y<<"&y="<<key_X;*/
	buf<<"/DataServer?l="<<lev<<"&x="<<c<<"&y="<<r;
	/*buf<<"/DataServer?l="<<17<<"&x="<<104307<<"&y="<<23027;*/
	strTile+=buf.str();
	/*strTile="http://192.168.42.178:9010/YINGXIANG/DataServer?l=17&x=104307&y=23027";*/
	return strTile;

}

bool ImageTileSource::hasDataForFallback(const osgEarth::TileKey& key) const
{
	//matics: might have data.
	unsigned int lev,r,c;
	lev=key.getLevelOfDetail()+1;
	key.getTileXY(c,r);
	std::stringstream ss;
	ss<<lev<<"/"<<c<<"/"<<r<<".1.osgearth_engine_mp_tile_image";
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

bool ImageTileSource::hasData(const osgEarth::TileKey& key) const
{
	//sematics: might have data.

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