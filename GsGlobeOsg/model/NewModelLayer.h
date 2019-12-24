#ifndef GEOGLOBE_MODEL_NewModelLayer
#define GEOGLOBE_MODEL_NewModelLayer 1

#include "Model/ModelTilePager.h"
#include "Tool/SQLiteOP.h"
#include "Model/ModelTile.h"

namespace GeoGlobe
{
	namespace Model
	{
		class NewModelLayer : public ModelTilePager
		{
		public:
			/*是否初始化*/
			bool _bInitialized;

		   /*读取吉奥格式的数据集*/
		   NewModelLayer(const osgEarth::Profile* profile,ModelLayerOptions* modelLayerOptions):
			ModelTilePager( profile,modelLayerOptions)
			{
				//读取吉奥自定义格式的倾斜影像数据
				_bInitialized  = false;
				_bInitialized = init();
				build();
			}

		     /*初始化*/
			 bool init()
			 {
				 if (NULL == _options)
					 return false;
				 
				 _minLevel = _options->_minLevel - 1;
				 _maxLevel = _options->_maxLevel - 1;

				 _west = _options->_west;
				 _east = _options->_east;
				 _north = _options->_north;
				 _south = _options->_south;

				 _centerGeo = osg::Vec2d(0.5*(_east+_west), 0.5*(_north+_south));
				 return true;
			 }

		    /*继承TilePager 根据key值生成瓦片*/
			virtual osg::Node* createNode( const TileKey& key )
			{        
				//std::cout<<key.getLOD()+1<<" "<<key.getTileX()<<" "<<key.getTileY()<<std::endl;
				osg::ref_ptr<GeoGlobe::Model::ModelTile> tile = createModelTile(key);
				if(tile.valid()&&tile->getNumChildren()>0)
					return tile.release();
				else
					return NULL;
			}

			 /*构建瓦片数据*/
			osg::ref_ptr<GeoGlobe::Model::ModelTile> createModelTile(const TileKey& key)
			{
				if (key.getLOD()+1 > _options->_maxLevel)
					return NULL;

				std::string modelTileName=_options->createModelTileName(key.getLevelOfDetail() + 1,key.getTileX(),key.getTileY());		
				//构建ModelTile子瓦片
				osg::ref_ptr<ModelTileOptions> modelTileOptions=new ModelTileOptions(key,_options);
				osg::ref_ptr<GeoGlobe::Model::ModelTile> modelTile=new GeoGlobe::Model::ModelTile(modelTileOptions);
				if (modelTile->hasRealData())
				{
					return modelTile;
				}
				else
					return NULL;
			}
		};
	}
}

#endif
