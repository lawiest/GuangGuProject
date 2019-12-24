#include "QuadPlaceNameTileDriver.h"
#include <osgEarth/Registry>

GLOBE_NS

QuadPlaceNameTileDriver::QuadPlaceNameTileDriver() { supportsExtension("quadplacenametile","user defined format");}

const char* QuadPlaceNameTileDriver::className()
{
	return "User Defined Tile Engine";
}

void QuadPlaceNameTileDriver::parseFileName(const std::string& uri,int& level,int& row,int& col) const
{
	std::vector<std::string> vecStr;
	std::string lessExtension;
	std::string ext;
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

	
/*将瓦片加入到PageLod中，实现四叉树分裂功能*/
osg::Node * QuadPlaceNameTileDriver::createTile(osg::Node* node, const osgEarth::TileKey& tileKey, GLOBE_NAME::PlaceNameLayerOptions* options) const
{
	//构建PageLOD以实现模型瓦片再分裂的能力
	//osg::ref_ptr<TilePagedLOD> tile=new TilePagedLOD(tileKey,options->_mapNode);
	osg::ref_ptr<osg::PagedLOD> tile=new osg::PagedLOD();

	int level;
	unsigned int row,col;
	level=tileKey.getLevelOfDetail();
	tileKey.getTileXY(col,row);

	osgEarth::GeoPoint gp=tileKey.getExtent().computeBoundingGeoCircle().getCenter();
	osgEarth::GeoExtent extent = tileKey.getExtent();
	osgEarth::GeoPoint lowerLeft(extent.getSRS(), extent.xMin(), extent.yMin(), 0.0, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
	osgEarth::GeoPoint upperRight(extent.getSRS(), extent.xMax(), extent.yMax(), 0.0, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
	osg::Vec3d ll, ur;
	lowerLeft.toWorld( ll );
	upperRight.toWorld( ur );
	double radius = (ur - ll).length() / 2.0;
	float minRange = (float)(radius*2 );
	double dbTemp = tileKey.getExtent().computeBoundingGeoCircle().getRadius();
	osg::Vec3d center;
	gp.toWorld(center);

	tile->setCenter(center);
	tile->setRadius(tileKey.getExtent().computeBoundingGeoCircle().getRadius());
	tile->setRangeMode( osg::LOD::DISTANCE_FROM_EYE_POINT );

	if (node!=NULL)
	{
        //PageLOD的第一级是地名瓦片数据
		PlaceNameTile* tileNode =(PlaceNameTile*) node;
		osg::Vec3d localPoint;
		osg::Vec3d worldPoint;
		double z;
		options->_mapNode->getMapSRS()->transformToWorld(osg::Vec3d(gp.x(),gp.y(),tileNode->_altitude),worldPoint);
		tile->setInitialBound(osg::BoundingSphere(worldPoint,radius));
		tile->addChild(node);
        //PageLOD的第二级指向下一级的QuadPlaceNameTile，也就是下级的4个子瓦片
		tile->setFileName( 1, osgEarth::Stringify() <<"PlaceName"<<"."<<level << "." << col <<"." <<row<<".quadplacenametile" );
		tile->setRange( 0, minRange, FLT_MAX );
		tile->setRange( 1, 0, minRange );	
	}
	//else if((level+1)<=options->_minLevel)
	//顶层瓦片没有模型数据但也需要遍历下去
	else// if((level+1<=options->_minLevel)/*||(options->_minLevel<3&&options->_minLevel>=0)*/)
	{
		tile->setFileName(0, osgEarth::Stringify() <<"PlaceName"<<"."<< level << "." << col <<"." <<row<<".quadplacenametile" );
		tile->setRange( 0, 0, minRange );
	}
	//else
	//{
	//	return NULL;
	//}
	tile->setDatabaseOptions(options);
	return tile.release();
}

std::string QuadPlaceNameTileDriver::createPlaceNameTileName(const std::string& gmdxFile,const osgEarth::TileKey& key) const
{
	return osgEarth::Stringify() <<gmdxFile<<"_"<<key.str() <<".PlaceNameTile";
}

/*生成4个子瓦片*/
osg::Node* QuadPlaceNameTileDriver::createNode(const osgEarth::TileKey& tileKey, GLOBE_NAME::PlaceNameLayerOptions* options) const
{
	osg::ref_ptr<osg::Group> quad = new osg::Group();
	if (tileKey.getLOD()+2>options->_maxLevel)
		return quad.release();
			
	osg::ref_ptr<osg::Node> node[4];
	if (tileKey.getLOD()+2 >= options->_minLevel )
	{
		//构建四个子瓦片
		for(unsigned q=0; q<4; ++q)
		{
			//得到child的子瓦片值
			osgEarth::TileKey child = tileKey.createChildKey(q);
			std::string pnTileName = options->createPlaceNameTileName(child.getLevelOfDetail()+1,child.getTileX(),child.getTileY());
			if (!osgEarth::Registry::instance()->isBlacklisted(pnTileName))
			{
				//构建地名瓦片		
				osg::ref_ptr<PlaceNameTile> pnTile=new PlaceNameTile(child,options);
				if (pnTile->hasRealData())
				{
					node[q]=pnTile;
				}
				else
					osgEarth::Registry::instance()->blacklist(pnTileName);
			}				
		}
	}

	//// If there's a minimum LOD set, and we haven't reached it yet, make the tile.)
	//bool makeTile ;
	//if ( tileKey.getLOD()+3 <= options->_minLevel )
	//{
	//	makeTile = true;
	//}
	//// Otherwise, only make the tile if at least one quadrant has REAL data
	//// (not fallback data).
	//else 
	//{
	//	makeTile = false;
	//	for(unsigned q=0; q<4; ++q)
	//	{
	//		if ( node[q].valid())
	//		{
	//			makeTile = true;
	//			break;
	//		}
	//	}
	//}
 //   //为子瓦片构建PageLOD，这样子瓦片才会继续分裂
	////上面是构建具体的瓦片数据，这里是使瓦片具有再分裂的能力，这里比较绕，主要是为了应用OSG内部的分页数据管理机制
	////实现四叉树动态加载，如果理解困难，需要先学习osg::PageLOD部分内容，在倾斜影像部分，我将这个步骤进行了简化
	////因为工作量的原因，模型和管线还是保持原样
	//if (!makeTile)
	//	return quad.release();

	for( unsigned q=0; q<4; ++q )
	{
		osg::ref_ptr<osg::Node> tile = createTile((node[q]),tileKey.createChildKey(q),options);
		if (tile)
		{
			quad->addChild( tile );
		}
	}

	return quad.release();
}

/*入口函数*/
osgDB::ReaderWriter::ReadResult QuadPlaceNameTileDriver::readNode(const std::string& uri, const Options* options) const
{
	std::string ext = osgDB::getFileExtension(uri);
	if ( acceptsExtension(ext) )
	{
		osg::ref_ptr<GLOBE_NAME::PlaceNameLayerOptions> pnLayerOptions = (GLOBE_NAME::PlaceNameLayerOptions*)(options) ;
		int level, row, col;
		parseFileName(uri,level, col, row);
		osgEarth::TileKey tileKey = osgEarth::TileKey(level,col,row,pnLayerOptions->_mapNode->getMap()->getProfile());

        //构建四个子瓦片，模仿osgEarth实现四叉树地形部分写得，具体代码参考MPTerrainEngineDriver
		osg::ref_ptr<osg::Node> node = createNode(tileKey,pnLayerOptions);
		node->setThreadSafeRefUnref(true);
		if (node.valid())
		{		
			return ReadResult( node.release(), ReadResult::FILE_LOADED );
		}
		else
		{					
			return ReadResult::FILE_NOT_FOUND;
		}
	}
	else
	{
		return ReadResult::FILE_NOT_HANDLED;
	}
}


REGISTER_OSGPLUGIN(QuadPlaceNameTileDriver, QuadPlaceNameTileDriver);

GLOBE_ENDNS
