
#include <Tool/SQLiteOP.h>
#include <Tool/Download.h>
#include <osgEarthAnnotation/AnnotationUtils>
#include "Tool/CodeTransformHelp.h"
#include "Analysis/DistanceLoD.h"
#include <PlaceName/PlaceNameTile.h>
#include <osgEarthAnnotation/PlaceNode>
#include <osg/Depth>
#include <placename/GeoBillboard.h>

GLOBE_NS

PlaceNameTile::PlaceNameTile(const  osgEarth::TileKey& tileKey, GLOBE_NAME::PlaceNameLayerOptions* options):osg::Group()
{
	_tileKey=tileKey;	
	_options=options;
	createPlaceNameTile();
}

/*判断瓦片中是否有数据*/
bool PlaceNameTile::hasRealData()
{
	if ( _children.size()==0)
	{
		return false;
	}
	return true;
}

//将图标写入缓存中
bool  PlaceNameTile::writeIconToCache(int dataID,const char* pData,int dataType,int nDataLen)
{
	//构建sqlie数据库
	sqlite3 *m_pSqliteDB= _options->_cache->_pnCache.m_pSqliteDB;

	if ((! pData) || nDataLen <= 0 ||!m_pSqliteDB)
			return false;

    //插入的sql语句
	std::string TableName="WTFSICONLIB" ;
	std::string strSQL = "insert into " +TableName + " (ID, PIC1,TYPE) values (?, ?, ?)";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int(pStmt,1,dataID);
	sqlite3_bind_blob(pStmt,2,pData,nDataLen,NULL);
	sqlite3_bind_int(pStmt,3,dataType);
	//sqlite3_bind_int(pStmt,3,c);
	//sqlite3_bind_int(pStmt,4,101);
	//sqlite3_bind_int(pStmt,6,nDataLen);
	//sqlite3_bind_blob(pStmt,5,pcData,nDataLen,NULL);
	//sqlite3_step(stmt);
	if (SQLITE_DONE != sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	sqlite3_finalize(pStmt);

	return true;
}

//构造地名瓦片
void PlaceNameTile::createPlaceNameTile()
{
	int lev,row,col;
	lev=_tileKey.getLevelOfDetail()+1;
	col=_tileKey.getTileX();
	row=_tileKey.getTileY();

	double lon,lat;
	_tileKey.getExtent().getCentroid(lon,lat);

    //计算该经纬度的高程值
	osg::ref_ptr<osgEarth::MapNode> mapnode=_options->_mapNode;
	if (!_options->_mapNode->getTerrain()->getHeight(mapnode->getMapSRS(),lon,lat,&_altitude))
		_altitude=0;

    //设置瓦片的中心点
	osg::Vec3d center;
	osgEarth::GeoPoint gp=_tileKey.getExtent().computeBoundingGeoCircle().getCenter();
	gp.toWorld(center);
    double radius=_tileKey.getExtent().computeBoundingGeoCircle().getRadius();
	std::string tilefn=_options->createPlaceNameTileName(lev,col,row);

	CPBPOITile gpoitile;
	bool  bHasData=false;
	int nSize=0;
	int nTileType=0;
	char* pData=NULL;

     //从缓存中取数据
	if (_options->_pnServiceType == eLocalPlaceName)
	{
		_options->_cache->readDataFromCache(tilefn, &pData, nSize, nTileType);

		if (nTileType == 10)
		{
			//解析地名二进制块
			bHasData = gpoitile.From((unsigned char*)(pData), nSize);
		}

		if (pData !=NULL)
		{
			delete[] pData;
			pData =NULL;
		}
	}
	else if(_options->_pnServiceType == eGeoGlobeTilePlaceName)
	{
		//如果有URL地址则从服务中取数据
		std::string strRequesURL=_options->_url;
		std::stringstream buffer;
		buffer <<"/GetTiles?lxys="<<lev<<","<<col<<","<<row;

		if (lev == 16 && col == 53600 && row ==10840)
		{
			int i = 0;
			i++;
		}

		strRequesURL += buffer.str();
		GeoGlobe::Tool::Download downLoad;

		if(downLoad.downloadData(strRequesURL) && (downLoad.size >= 20))
			nSize =*(int*)(downLoad.data+15);

		//解析地名二进制块
		if (gpoitile.From((unsigned char*)(downLoad.data)+19, nSize))
		{
			bHasData =true;
			if (_options->_cache->writeDataToCache(tilefn,(char*)(downLoad.data+19), nSize))
			{
				_options->_cache->commitCache(tilefn);
			}
		}
	}

    //从gpoitile中获取地名信息
	if (bHasData)
	{
		int poisize=gpoitile.TilePtr()->pois_size();

		for(int i=0;i<poisize;i++)
		{
			CPBPOI poi(gpoitile.TilePtr()->mutable_pois(i),gpoitile.TilePtr());
			//coordinate
			int codsize=poi.POIPtr()->coordinates_size();
            //取地名经纬度坐标
			double x=poi.POIPtr()->coordinates(0);
			double y=poi.POIPtr()->coordinates(1);
			double z = 0;
			if (codsize == 3)
				z = poi.POIPtr()->coordinates(2);

			//placename

            //取地名名称并转码使其支持中文
			string na=poi.POIPtr()->name();
			//na = CodeHelp::String_To_UTF8(na);
			//设置地名的颜色和大小	
			int fontsize=poi.POIPtr()->fontsize();
			unsigned int fontcolor=poi.POIPtr()->fontcolor();

			int nA = fontcolor>>24;
			int nR = fontcolor<<8>>24;
			int nG = fontcolor<<16>>24;
			int nB = fontcolor<<24>>24;
			fontcolor = ((((nA)&0xff)<<24)|(((nB)&0xff)<<16)|(((nG)&0xff)<<8)|((nR)&0xff));

			osgEarth::Symbology::Style style =_options->_chapin;
			style.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
			style.getOrCreate<osgEarth::Symbology::TextSymbol>()->size()=fontsize*2.0;
			style.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color()=osgEarth::Symbology::Color(fontcolor,osgEarth::Symbology::Color::ABGR);
			//IconSymbol的设置
			style.getOrCreate<osgEarth::Symbology::IconSymbol>()->scale()=osgEarth::Symbology::NumericExpression(0.7);
			//style.getOrCreate<osgEarth::Symbology::IconSymbol>()->alignment() = osgEarth::Symbology::IconSymbol::ALIGN_LEFT_CENTER;
			style.getOrCreate<osgEarth::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_LEFT_CENTER;
			style.getOrCreate<osgEarth::Symbology::IconSymbol>()->declutter() = _options->_bCoverAutoHide;
			style.getOrCreate<osgEarth::Symbology::TextSymbol>()->declutter() = _options->_bCoverAutoHide;
			//chapin.getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset()->enabled() = true;
			//chapin.getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset()->automatic() = true;
			//chapin.getOrCreate<TextSymbol>()->halo() = Color("#5f5f5f");

             //构建图标
			int symbolID=poi.POIPtr()->symbolid();
			osg::ref_ptr<osg::Image> img=createIconImage(symbolID);
            
			//获取地名节点高度
			//double z = 0;
			//if (!_options->_mapNode->getTerrain()->getHeight(mapnode->getMapSRS(),x,y,&z))
			//	z=0;

			////构建地名节点
			//osg::ref_ptr<PlaceNode> pn = new PlaceNode(mapnode, osgEarth::GeoPoint(mapnode->getMapSRS(),x,y),img,na,chapin);
			//pn->setHorizonCulling(true);
			//pn->setName(na);
			//pn->getBound();
			//pn->setCullingActive(true);


			//Style pm;
			////pm.getOrCreate<IconSymbol>()->url()->setLiteral("F:/GsOSGEarthDev/dependency/osgearth/data/placemark32.png");
			//pm.getOrCreate<IconSymbol>()->declutter() = true;
			//pm.getOrCreate<TextSymbol>()->halo() = Color("#5f5f5f");

			// bunch of pins:
			//GeoGlobe::PlaceName::GeoBillboard* ptrPlaceNode = new GeoGlobe::PlaceName::GeoBillboard(osgEarth::GeoPoint(mapnode->getMapSRS(), x, y, z), na, style);
			osg::LOD* ptrLod = new osg::DistanceLOD();// new osg::LOD();
			//ptrPlaceNode->setIconImage(img);
			GeoGlobe::Annotation::PlaceNode* ptrPlaceNode = new GeoGlobe::Annotation::PlaceNode(osgEarth::GeoPoint(mapnode->getMapSRS(), x, y, z),img,na ,style);
			//osgEarth::Annotation::PlaceNode* ptrPlaceNode = new osgEarth::Annotation::PlaceNode(osgEarth::GeoPoint(mapnode->getMapSRS(), x, y, z), na, style, img);
			ptrPlaceNode->setCullingActive(false);
			ptrLod->addChild(ptrPlaceNode, _options->_minLOD, _options->_maxLOD);
			this->addChild(ptrLod); 

			//this->addChild(pn );
		}
	}
}

enum ImageType{  
	jpg,  
	bmp,  
	png,  
	nothing  
};  

ImageType image_type(const char* imageData)  
{  
	unsigned char png_type[9] = {0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A,'/0'};  
	unsigned char file_head[9];  

	for (int i=0;i<8;++i)  
	{  
		file_head[i] = imageData[i];  
	}  

	file_head[8] = '/0';  

	switch (file_head[0])  
	{  
	case 0xff:  
		if (file_head[1]==0xd8)  
			return jpg;//jpeg  
	case 0x42:  
		if (file_head[1]==0x4D)  
			return bmp;//bmp  
	case 0x89:  
		if (file_head[1]==png_type[1] && file_head[2]==png_type[2] && file_head[3]==png_type[3] && file_head[4]==png_type[4]&&  
			file_head[5]==png_type[5] && file_head[6]==png_type[6] && file_head[7]==png_type[7])  
			return png;//png  
	default:  
		return nothing;  
	}  
}

osg::ref_ptr<osg::Image> PlaceNameTile::createIconImage(int symbolID)
{
	std::string iconID=_options->createPlaceNameIconName(symbolID);
	osg::ref_ptr<osg::Image> img = (osg::Image*)(osgDB::Registry::instance()->getFromObjectCache(iconID));
	if (!img.valid())
	{
		int imagesize=0;
		int imagetype=0;
		char* imageData=NULL;

		if (_options->_cache->readDataFromCache(iconID,&imageData,imagesize,imagetype))
		{
			if (image_type(imageData) == ImageType::bmp)
			{
				////如果格式是bmp的，则读取本地资源对应的png图片，因为bmp格式透明度有问题
				osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension("png");
				if (reader.valid())
				{
					std::stringstream ss;
					ss<<"./Resources/PlaceNameIconSet/image/"<<symbolID<<".png";
					string strpath = ss.str();
					img = (reader->readImage(strpath)).getImage();
				}
			}
			else if(image_type(imageData) == ImageType::png)
			{
				std::string imgsrc;
				imgsrc.assign(imageData,imagesize);
				std::stringstream ss;
				ss<<imgsrc;
				osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension("png");
				if (reader.valid())
					img=(reader->readImage(ss)).getImage();
			}

			if (imageData!=NULL)
			{
				delete[] imageData;
				imageData=NULL;
			}
		}
		else if(_options->_url!="")
		{
			std::stringstream buf;
			buf<<_options->getUrl()<<"/GetIcon?id="<<symbolID;
			GeoGlobe::Tool::Download dl;

			if(dl.downloadData(buf.str()))
			{
				std::string imgsrc;
				imgsrc.assign(dl.data,dl.size);
				std::stringstream ss;
				ss<<imgsrc;
				osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension("png");
				img=reader->readImage(ss).getImage();
				writeIconToCache(symbolID,dl.data,2,dl.size);
			}
		}

		if (img.valid())
		{
			osgDB::Registry::instance()->addEntryToObjectCache(iconID,img);
		}
	}
	return img;
}

osg::ref_ptr<osg::Geometry> PlaceNameTile::createIcon(int symbolID)
{
	std::string iconID=_options->createPlaceNameIconName(symbolID);
	osg::ref_ptr<osg::Image> image;
	osg::ref_ptr<osg::Geometry> imageGeom=(osg::Geometry*)(osgDB::Registry::instance()->getFromObjectCache(iconID));
	if (!imageGeom.valid())
	{
		int imagesize=0;
	    int imagetype=0;
	    char* imageData=NULL;

		if (_options->_cache->readDataFromCache(iconID,&imageData,imagesize,imagetype))
		{
			if (image_type(imageData) == ImageType::bmp)
			{				
				////如果格式是bmp的，则读取本地资源对应的png图片，因为bmp格式透明度有问题
				osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension("png");
				if (reader.valid())
				{
					std::stringstream ss;
					ss<<"../resource/PlaceNameIconSet/image/"<<symbolID<<".png";
					string strpath = ss.str();
					image = (reader->readImage(strpath)).getImage();
				}
			}
			else if(image_type(imageData) == ImageType::png)
			{
				std::string imgsrc;
				imgsrc.assign(imageData,imagesize);
				std::stringstream ss;
				ss<<imgsrc;
				osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension("png");
				if (reader.valid())
					image=(reader->readImage(ss)).getImage();
			}

			if (imageData!=NULL)
			{
			   delete[] imageData;
			   imageData=NULL;
			}
		}
		else if(_options->_url!="")
		{
			std::stringstream buf;
			buf<<_options->getUrl()<<"/GetIcon?id="<<symbolID;
			GeoGlobe::Tool::Download dl;

			if(dl.downloadData(buf.str()))
			{
				std::string imgsrc;
				imgsrc.assign(dl.data,dl.size);
				std::stringstream ss;
				ss<<imgsrc;
				osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension("png");
				image=reader->readImage(ss).getImage();
				writeIconToCache(symbolID,dl.data,2,dl.size);
			}
		}

		if(image.valid())
		{
			double t = image->t();
			osg::Vec2s offset;
			offset.set(0.0, (t / 2.0));
			imageGeom = osgEarth::Annotation::AnnotationUtils::createImageGeometry( image.get(), offset, 0, 0, 0);
			osgDB::Registry::instance()->addEntryToObjectCache(iconID,imageGeom);
		}
	 }

	return imageGeom;
}

GLOBE_ENDNS

