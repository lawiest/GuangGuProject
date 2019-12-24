#ifndef GEOGLOBE_PIPE_QUADPIPETILEDRIVER
#define GEOGLOBE_PIPE_QUADPIPETILEDRIVER 1

#include <Pipe/PipeOptions.h>
#include <Pipe/PipeTile.h>
#include <Model/ModelTilePagedLOD.h>
#include <osgEarthAnnotation/PlaceNode>
#include <osg/ShapeDrawable>
#include <Base/TilePagedLOD.h>

using namespace osgEarth::Annotation;
using namespace GeoGlobe::Base;

namespace GeoGlobe{

	namespace Pipe{
//伪插件，通过伪插件才能将管线瓦片以DataBasePager(线程)的方式导入
//跟QuadModelTileDriver几乎一样，在此不重复注释
	class QuadPipeTileDriver : public osgDB::ReaderWriter
	{
	public:
		QuadPipeTileDriver() { supportsExtension("quadpipetile","user defined format");}

		virtual const char* className()
		{
			return "User Defined Tile Engine";
		}

		void parseFileName(std::string uri,int& level,int& row,int& col) const
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
			//dataSetFileName=vecStr[3];
			//modelGeometrysFiles=vecStr[4];
		}

		osg::Node * createTile(osg::Node* node,osgEarth::TileKey tileKey,PipeLayerOptions* options) const
		{

			osg::ref_ptr<ModelTilePagedLOD> tile=new ModelTilePagedLOD(options->_mapNode);

			tile->setName(tileKey.str());

			int level;
			unsigned int row,col;
			level=tileKey.getLevelOfDetail();
			tileKey.getTileXY(col,row);

			double radius;
			osg::Vec3d center;
			osgEarth::GeoPoint gp=tileKey.getExtent().getBoundingGeoCircle().getCenter();
			gp.toWorld(center);
			radius=tileKey.getExtent().getBoundingGeoCircle().getRadius();
			/*GeoExtent extent = tileKey.getExtent();
			osgEarth::GeoPoint lowerLeft(extent.getSRS(), extent.xMin(), extent.yMin(), 0.0, ALTMODE_ABSOLUTE);
			osgEarth::GeoPoint upperRight(extent.getSRS(), extent.xMax(), extent.yMax(), 0.0, ALTMODE_ABSOLUTE);
			osg::Vec3d ll, ur;
			lowerLeft.toWorld( ll );
			upperRight.toWorld( ur );
			radius = (ur - ll).length() / 2.0;*/
			float minRange = (float)(radius*1.5 );

			tile->setRangeMode( osg::LOD::DISTANCE_FROM_EYE_POINT );
			tile->setTileKey(tileKey);
			//tile->_mapNode=options->_mapNode;
			//tile->setInitialBound(osg::BoundingSphere(center,radius));

			tile->setCenter  ( center );
			tile->setRadius(minRange);
			tile->_setting=options->_setting;

			//tile->getBound();
			/*node->setRadius(radius);
			node->setCen*/
			//node->setInitialBound(osg::BoundingSphere(tile->computeBound()));
		//	tile->useAutoClamp();
			if (node!=NULL)
			{
				
				PipeTile* tileNode =(PipeTile*) node;
				

				/*tileNode->setNumChildrenWithCullingDisabled(0);*/

				/*			tileNode->setCenter(center);
				tileNode->setRadius(radius);*/
				osg::Vec3d localPoint;
				osg::Vec3d worldPoint;
				double z;
				//options->_mapNode->getMapSRS()->transformFromWorld(tileNode->getCenter(),localPoint,&z);
				options->_mapNode->getMapSRS()->transformToWorld(osg::Vec3d(gp.x(),gp.y(),tileNode->_altitude),worldPoint);
			//	gp.z()=tileNode->_altitude;
			//	tileNode->addChild(new PlaceNode(options->_mapNode,gp,tileKey.str()));
				tile->setInitialBound(osg::BoundingSphere(worldPoint,radius));
				//tile->getBound();
				tile->addChild   ( tileNode/*temp*/);
				//tile->getBound();
				tile->setFileName( 1, Stringify() <<"Pipe"<<"."<<level << "." << col <<"." <<row<<".quadpipetile" );
				/*		tile->setCenter(tileNode->getBound().center());
				tile->setRadius(tileNode->getBound().radius());*/

				//tile->setRange( 0, /*minRange*/1000, 1000*2/*FLT_MAX*/ );
				//tile->setRange( 1, 0, /*minRange*/1000 );
				tile->_normalTraverse=false;

				//	tile->_hasRealData=true;
				//tile->useAutoClamp();
				//options->_mapNode->getTerrain()->addTerrainCallback(new AutoClampCallback(tile));

			}
			else if((level+1)<=options->_minLevel)
			{
				//	tile->addChild   ( new PlaceNode(options->_mapNode,gp,"no"+tileKey.str()));

				tile->setFileName(0, Stringify() <<"Model"<<"."<< level << "." << col <<"." <<row<<".quadpipetile" );
				//  tile->setRange( 0, minRange, FLT_MAX );
				tile->setRange( 0, 0, minRange );
//				tile->setCenter(center);
				tile->setCullingActive(false);
				//	tile->_hasRealData=true;

				//	tile->setInitialBound(osg::BoundingSphere(center,radius));
			} 
			else
			{
				return NULL;
			}

			//options->setFileLocationCallback()
			tile->setDatabaseOptions(options);


			return tile.release();
		}

		//		osg::Node * createTile2(osg::Node* node,TileKey& tileKey,ModelLayerOptions* options) const
		//	{

		//		

		//			osg::ref_ptr<TilePagedLOD> tile=new TilePagedLOD(tileKey,options->_mapNode);
		//	

		//		int level;
		//		unsigned int row,col;
		//		level=tileKey.getLevelOfDetail();
		//		tileKey.getTileXY(col,row);
		//		
		//				
		//		double radius;
		//		osg::Vec3d center;
		//		osgEarth::GeoPoint gp=tileKey.getExtent().getBoundingGeoCircle().getCenter();
		//		gp.toWorld(center);

		//	 osgEarth::GeoExtent extent = tileKey.getExtent();
		//		osgEarth::GeoPoint lowerLeft(extent.getSRS(), extent.xMin(), extent.yMin(), 0.0, ALTMODE_ABSOLUTE);
		//		osgEarth::GeoPoint upperRight(extent.getSRS(), extent.xMax(), extent.yMax(), 0.0, ALTMODE_ABSOLUTE);
		//		osg::Vec3d ll, ur;
		//		lowerLeft.toWorld( ll );
		//		upperRight.toWorld( ur );
		//		radius = (ur - ll).length() / 2.0;
		//		float minRange = (float)(radius*6 );

		//		tile->setRangeMode( osg::LOD::DISTANCE_FROM_EYE_POINT );
		//		//tile->setTileKey(tileKey);
		//		//tile->_mapNode=options->_mapNode;
		//		//tile->setInitialBound(osg::BoundingSphere(center,radius));
		//		
		//		
		///*		tile->setCenter  ( center );
		//		tile->setRadius(minRange);*/

		//		
		//		//tile->getBound();
		//		/*node->setRadius(radius);
		//		node->setCen*/
		//		//node->setInitialBound(osg::BoundingSphere(tile->computeBound()));
		//		tile->useAutoClamp();
		//		if (node!=NULL)
		//		{

		//			tile->addChild   ( node/*temp*/);
		//			tile->setFileName( 1, Stringify() <<"PlaceName"<<"."<<level << "." << col <<"." <<row<<".quadmodeltile" );
		//	/*		tile->setCenter(tileNode->getBound().center());
		//			tile->setRadius(tileNode->getBound().radius());*/
		//			
		//			tile->setRange( 0, minRange, FLT_MAX );
		//			tile->setRange( 1, 0, minRange );
		//	/*		bool c=tile->isCullingActive();
		//			
		//			int a=tile->getNumChildrenWithCullingDisabled();
		//			int b=0;*/
		//	/*		tile->_normalTraverse=false;
		//			tile->_hasRealData=true;*/
		//	    	//tile->useAutoClamp();
		//			//options->_mapNode->getTerrain()->addTerrainCallback(new AutoClampCallback(tile));
		//			
		//		}
		//		else if((level+1)<=options->_minLevel)
		//		{
		//			//tile->addChild   ( new PlaceNode(options->_mapNode,gp,"no"+tileKey.str()));
		//		
		//			tile->setFileName(0, Stringify() <<"PlaceName"<<"."<< level << "." << col <<"." <<row<<".quadmodeltile" );
		//		  // tile->setRange( 0, minRange, FLT_MAX );
		//			tile->setRange( 0, 0, minRange );
		//		//	tile->setCenter(center);
		//			//tile->_hasRealData=true;
		//		
		//		//	tile->setInitialBound(osg::BoundingSphere(center,radius));
		//		}
		//		else 
		//			return NULL;
		//		tile->setDatabaseOptions(options);
		//		return tile.release();
		//	}

		//std::string createModelTileName(std::string gmdxFile, TileKey& key) const
		//{
		//	return Stringify() <<gmdxFile<<"_"<<key.str() <<".modelTile"; 
		//}

		osg::Node* createNode(TileKey& tileKey,PipeLayerOptions* options) const
		{
			osg::ref_ptr<osg::Group> quad;
			quad = new osg::Group();
			if (tileKey.getLOD()+2>options->_maxLevel)
				return quad.release();

			osg::ref_ptr<osg::Node> node[4];

			if (tileKey.getLOD()+2 >= options->_minLevel )
			{

				for(unsigned q=0; q<4; ++q)
				{
					//continue;
					TileKey child = tileKey.createChildKey(q);


					std::string modelTileName=options->createPipeTileName(child.getLevelOfDetail()+1,child.getTileX(),child.getTileY());
					if (!osgEarth::Registry::instance()->isBlacklisted(modelTileName))
					{
						osg::ref_ptr<PipeTileOptions> pipeTileOptions=new PipeTileOptions(child,options);
						osg::ref_ptr<GeoGlobe::Pipe::PipeTile> pipeTile=new GeoGlobe::Pipe::PipeTile(pipeTileOptions);
						if (pipeTile->hasRealData())
						{
							node[q]=pipeTile;
						}
					}
					else
						osgEarth::Registry::instance()->blacklist(modelTileName);
				}

			}

			bool makeTile ;
			//if ( key.getLOD() == 0 || (key.getLOD()-1) == _options.firstLOD().value() )
			//{
			//	makeTile = true;
			//}

			// If there's a minimum LOD set, and we haven't reached it yet, make the tile.
			if ( tileKey.getLOD()+3 <=options->_minLevel )
			{
				makeTile = true;
			}

			// Otherwise, only make the tile if at least one quadrant has REAL data
			// (not fallback data).
			else
			{
				makeTile = false;
				for(unsigned q=0; q<4; ++q)
				{
					if ( node[q] )
					{
						makeTile = true;
						break;
					}
				}
			}

			//osg::ref_ptr<osg::Group> quad;
			if ( makeTile )
			{
				for( unsigned q=0; q<4; ++q )
				{

					/*				osg::ref_ptr<ModelTilePagedLOD> tile=(ModelTilePagedLOD*)createTile((node[q]),tileKey.createChildKey(q),options);
					if (tile->_hasRealData)
					{
					quad->addChild( tile );
					}*/

					osg::ref_ptr<osg::Node> tile=createTile((node[q]),tileKey.createChildKey(q),options);
				//	tile->setName()
					/*		if(tile->asGroup()->getNumChildren()==0)
					int a=0;*/
					if (tile)
					{
						quad->addChild( tile );
					}
				}
			}

			return quad.release();
		}

		virtual ReadResult readNode(const std::string& uri, const Options* options) const
		{
			std::string ext = osgDB::getFileExtension(uri);

			if ( acceptsExtension(ext) )
			{
				//return ReadResult::FILE_NOT_FOUND;
				osg::ref_ptr<PipeLayerOptions> pipeLayerOptions=(PipeLayerOptions*)(options) ;

				int level, row, col;
				parseFileName(uri,level, col, row);

				TileKey tileKey=TileKey(level,col,row,pipeLayerOptions->_mapNode->getMap()->getProfile());
				osg::ref_ptr<osg::Node> node=createNode(tileKey,pipeLayerOptions);

				if (node.valid())
				{
					return ReadResult( node.get(), ReadResult::FILE_LOADED );
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
	};

	REGISTER_OSGPLUGIN(QuadPipeTileDriver, QuadPipeTileDriver);

}}

#endif
