#ifndef GEOGLOBE_MODEL_QUADMODELTILEDRIVER
#define GEOGLOBE_MODEL_QUADMODELTILEDRIVER 1

#include <Model/ModelOptions.h>
#include <Model/ModelTile.h>
#include <Model/ModelTilePagedLOD.h>
#include <osgEarthAnnotation/PlaceNode>
#include <osg/ShapeDrawable>
#include <Base/TilePagedLOD.h>

using namespace osgEarth::Annotation;
using namespace GeoGlobe::Base;

namespace GeoGlobe{ namespace Model{


	/*伪插件，通过伪插件才能将模型瓦片以DataBasePager(线程)的方式导入*/
	class QuadModelTileDriver : public osgDB::ReaderWriter
	{
	public:
		QuadModelTileDriver() { supportsExtension("quadmodeltile","user defined format");}

		virtual const char* className()
		{
			return "User Defined Tile Engine";
		}

		//从传入的url中解析瓦片的级别，X，Y。
		void parseFileName(std::string uri,int& level,int& row,int& col) const
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

		//构建四个子瓦片，构建4叉树使其能够再次分裂
		osg::Node * createTile(osg::Node* node,TileKey tileKey,ModelLayerOptions* options) const
		{
			//构建PageLOD以实现模型瓦片再分裂的能力
			osg::ref_ptr<ModelTilePagedLOD> tile=new ModelTilePagedLOD(options->_mapNode);
			tile->setName(tileKey.str());

			int level;
			unsigned int row,col;
			level=tileKey.getLevelOfDetail();
			tileKey.getTileXY(col,row);

			//设置PageLOD的中心点及可视半径，强烈建议先看osg关于pagelod的教程
			//再来研究程序代码，本程序几乎所有核心功能都是围绕pagelod展开的
			//包括伪插件，动态加载删除，LOD切换等在PagedLod中都有体现
			double radius;
			osg::Vec3d center;
			osgEarth::GeoPoint gp=tileKey.getExtent().getBoundingGeoCircle().getCenter();
			gp.toWorld(center);
			radius=tileKey.getExtent().getBoundingGeoCircle().getRadius();
			float minRange = (float)(radius*6 );
			tile->setRangeMode( osg::LOD::DISTANCE_FROM_EYE_POINT );
			tile->setTileKey(tileKey);
			tile->setCenter  ( center );
			tile->setRadius(minRange);
			tile->_setting=options->_setting;
			tile->_spliteFactor=2.0f;

			if (node!=NULL)
			{
				ModelTile* tileNode =(ModelTile*) node;

				osg::Vec3d localPoint;
				osg::Vec3d worldPoint;
				double z;
				//options->_mapNode->getMapSRS()->transformFromWorld(tileNode->getCenter(),localPoint,&z);
				options->_mapNode->getMapSRS()->transformToWorld(osg::Vec3d(gp.x(),gp.y(),tileNode->_elevation),worldPoint);
				//tile->setInitialBound(osg::BoundingSphere(worldPoint,radius));
				//PageLOD的第一级是模型瓦片数据
				tile->addChild   ( tileNode);
				tile->setRange( 0, 0, 8000 );
				//PageLOD的第二级指向下一级的QuadModelTile，也就是下级的4个子瓦片
				//tile->setFileName( 1, Stringify() <<"Model"<<"."<<level << "." << col <<"." <<row<<".quadmodeltile" );
				//tile->setRange( 1, 0, minRange );
				if (level == options->_minLevel - 2)
				{
					tile->setFileName( 1, Stringify() <<"Model"<<"."<<level << "." << col <<"." <<row<<".quadmodeltile" );
					tile->setRange( 1, 0, minRange );
				}
				tile->_normalTraverse=true;
			}
			//顶层瓦片没有模型数据但也需要遍历下去
			//else if((level+1)<=options->_minLevel)
			else if(level == options->_minLevel - 2)
			{
				tile->setFileName(0, Stringify() <<"Model"<<"."<< level << "." << col <<"." <<row<<".quadmodeltile" );
				tile->setRange( 0, 0, minRange );
				tile->_normalTraverse=true;
				//tile->setCullingActive(false);
			} 
			else
			{
				//return NULL;
				return tile.release();
			}

			tile->setDatabaseOptions(options);
			return tile.release();
		}

		osg::Node * createTile2(osg::Node* node,TileKey& tileKey,ModelLayerOptions* options) const
		{
			osg::ref_ptr<TilePagedLOD> tile=new TilePagedLOD(tileKey,options->_mapNode);
			
			int level;
			unsigned int row,col;
			level=tileKey.getLevelOfDetail();
			tileKey.getTileXY(col,row);

			double radius;
			osg::Vec3d center;
			osgEarth::GeoPoint gp=tileKey.getExtent().getBoundingGeoCircle().getCenter();
			gp.toWorld(center);

		 osgEarth::GeoExtent extent = tileKey.getExtent();
			osgEarth::GeoPoint lowerLeft(extent.getSRS(), extent.xMin(), extent.yMin(), 0.0, ALTMODE_ABSOLUTE);
			osgEarth::GeoPoint upperRight(extent.getSRS(), extent.xMax(), extent.yMax(), 0.0, ALTMODE_ABSOLUTE);
			osg::Vec3d ll, ur;
			lowerLeft.toWorld( ll );
			upperRight.toWorld( ur );
			radius = (ur - ll).length() / 2.0;
			float minRange = (float)(radius*6 );

			tile->setRangeMode( osg::LOD::DISTANCE_FROM_EYE_POINT );
			//tile->setTileKey(tileKey);
			//tile->_mapNode=options->_mapNode;
			//tile->setInitialBound(osg::BoundingSphere(center,radius));
			/*tile->setCenter  ( center );
			tile->setRadius(minRange);*/
			//tile->getBound();
			/*node->setRadius(radius);
			node->setCen*/
			//node->setInitialBound(osg::BoundingSphere(tile->computeBound()));
			//tile->useAutoClamp();

			if (node!=NULL)
			{
				tile->addChild   ( node/*temp*/);
				tile->setFileName( 1, Stringify() <<"PlaceName"<<"."<<level << "." << col <<"." <<row<<".quadmodeltile" );
				/*tile->setCenter(tileNode->getBound().center());
				tile->setRadius(tileNode->getBound().radius());*/
					
				tile->setRange( 0, minRange, FLT_MAX );
				tile->setRange( 1, 0, minRange );
				/*bool c=tile->isCullingActive();
					
				int a=tile->getNumChildrenWithCullingDisabled();
				int b=0;*/
				/*tile->_normalTraverse=false;
				tile->_hasRealData=true;*/
			    //tile->useAutoClamp();
				//options->_mapNode->getTerrain()->addTerrainCallback(new AutoClampCallback(tile));
			}
			else if((level+1)<=options->_minLevel)
			{
				//tile->addChild   ( new PlaceNode(options->_mapNode,gp,"no"+tileKey.str()));
				
				tile->setFileName(0, Stringify() <<"PlaceName"<<"."<< level << "." << col <<"." <<row<<".quadmodeltile" );
				//tile->setRange( 0, minRange, FLT_MAX );
				tile->setRange( 0, 0, minRange );
				//tile->setCenter(center);
				//tile->_hasRealData=true;
				//tile->setInitialBound(osg::BoundingSphere(center,radius));
			}
			else 
				return NULL;

			tile->setDatabaseOptions(options);

			return tile.release();
		}

		std::string createModelTileName(std::string gmdxFile, TileKey& key) const
		{
			return Stringify() <<gmdxFile<<"_"<<key.str() <<".modelTile"; 
		}

        //构建四个子瓦片，模仿osgEarth实现四叉树地形部分写得，具体代码参考MPTerrainEngineDriver
		osg::ref_ptr<osg::Node> createNode(TileKey& tileKey,ModelLayerOptions* options) const
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
					//获取子瓦片的Key值
					TileKey child = tileKey.createChildKey(q);
					std::string modelTileName=options->createModelTileName(child.getLevelOfDetail()+1,child.getTileX(),child.getTileY());
					if (!osgEarth::Registry::instance()->isBlacklisted(modelTileName))
					{
						//构建ModelTile子瓦片
						osg::ref_ptr<ModelTileOptions> modelTileOptions=new ModelTileOptions(child,options);
						osg::ref_ptr<GeoGlobe::Model::ModelTile> modelTile=new GeoGlobe::Model::ModelTile(modelTileOptions);
						if (modelTile->hasRealData())
						{
							node[q]=modelTile;
						}
						else
						{
							osgEarth::Registry::instance()->blacklist(modelTileName);
						}
					}
				}

			}

			bool makeTile ;

			// If there's a minimum LOD set, and we haven't reached it yet, make the tile.
			if ( tileKey.getLOD()+2 < options->_minLevel )
			{
				makeTile = true;
			}

			// Otherwise, only make the tile if at least one quadrant has REAL data
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

			//为子瓦片构建PageLOD，这样子瓦片才会继续分裂
			//上面是构建具体的瓦片数据，这里是使瓦片具有再分裂的能力，这里比较绕，主要是为了应用OSG内部的分页数据管理机制
			//实现四叉树动态加载，如果理解困难，需要先学习osg::PageLOD部分内容，在倾斜影像部分，我将这个步骤进行了简化
			//因为工作量的原因，模型和管线还是保持原样
			if ( makeTile )
			{
				for( unsigned q=0; q<4; ++q )
				{
					//为每个子瓦片构建ModelTilePagedLod
					osg::ref_ptr<osg::Node> tile=createTile((node[q]),tileKey.createChildKey(q),options);
					//如果构建成功就将他插入的瓦片集中
					if (tile)
					{
						quad->addChild( tile );
					}
				}
			}
			return quad.release();
		}

		//插件的入口函数
		virtual ReadResult readNode(const std::string& uri, const Options* options) const
		{
			std::string ext = osgDB::getFileExtension(uri);

			if (acceptsExtension(ext))
			{
				osg::ref_ptr<ModelLayerOptions> modelLayerOptions=(ModelLayerOptions*)(options) ;

				//通过解析传入的URL获取瓦片集的级别,x,y坐标
				int level, row, col;
				parseFileName(uri,level, col, row);

				TileKey tileKey=TileKey(level,col,row,modelLayerOptions->_mapNode->getMap()->getProfile());

				//构建四个子瓦片，模仿osgEarth实现四叉树地形部分写得，具体代码参考MPTerrainEngineDriver
				osg::ref_ptr<osg::Node> node=createNode(tileKey,modelLayerOptions);

				if (node.valid())
				{
					//return ReadResult( node.get(), ReadResult::FILE_LOADED );
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
	};

	REGISTER_OSGPLUGIN(QuadModelTileDriver, QuadModelTileDriver);

}}

#endif
