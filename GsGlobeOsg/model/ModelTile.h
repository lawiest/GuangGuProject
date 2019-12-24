
#ifndef GEOGLOBE_MODEL_MODELTILE
#define GEOGLOBE_MODEL_MODELTILE 1

#include <osgEarth/Common>
#include <osg/Group>
#include <osgEarth/ThreadingUtils>
#include <osgEarth/Progress>
#include <Model/CFeature.h>
#include <Model/Buffer.h>
#include <Model/Material.h>
#include <Model/ModelOptions.h>
#include <Model/ModelBaseResource.h>



namespace GeoGlobe
{
	namespace Model
	{
		typedef unsigned long long  ULONGLONG;
		typedef std::map<ULONGLONG,osg::ref_ptr<CFeature> > FEATURE_MAP;//feature构成的map容器

		/*模型瓦片，每张瓦片包含了瓦片范围内的所有LOD模型*/
		class ModelTile : public /*osg::LOD*/osg::Group
		{
		public:
			/*模型瓦片的构造函数，根据Key值读取瓦片数据并生成LOD模型*/
			ModelTile(ModelTileOptions* options);
			~ModelTile()
			{	
			}

			void Buffer2Features(FEATURE_MAP &vtFeatures,  char *pData, int nSize) ;

			bool getFeaturesFromTile(int level,int col,int row);
			/*从服务中读取瓦片数据*/
			void createFeaturesFromService(int& level,int& col,int& row);
			/*从本地数据库中读取瓦片数据*/
			void createFeaturesFromLocal(int& level,int& col,int& row);

			osgDB::Options* getOrCreateDBOptions();

			//»ñÈ¡Ä£ÐÍÍßÆ¬ÖÐÄ£ÐÍµÄÊý¾ÝÀàÐÍ£¬G3D£¬ KMZ£¬ ÊµÑéÊÒÄ£ÐÍ(GMDL,05 00)
			GeoGlobe::enumModelVersion GetModelTileDataType(char *pTileBuf, int nSize);

		public: // osg::Group

			//std::string createLodModelName(ULONGLONG featureKey)
			//{
			//	return Stringify() <<dbName<<"_"<<featureKey <<".lodmodel";
			//};
			/*判断瓦片是否有数据*/
			bool hasRealData();
			/** called by the OSG DatabasePager when a paging result is ready. */
			/*继承自osg::Lod*/	
			//bool addChild( osg::Node* node );
			void writeTileDataToCache(int lev,int c,int r,std::string tileData);

			/*std::string createModelTileName(int lev,int c,int r)
			{
				return Stringify() <<lev<<"/"<<c<<"/"<<r <<".modeltile";
			};*/
			//virtual bool insertChild( unsigned int index, Node *child );

			void traverse(osg::NodeVisitor& nv);

		public: // osg::PagedLOD
			/*拼写瓦片服务的请求地址*/
			std::string creatModelTileURL(int level,int col,int row);
			int         _level;
			int         _row;
			int         _col;
			/*瓦片内的所有feature*/
			FEATURE_MAP _features;
			std::string _dataSetFileName;
			std::string _modelGeometrysFiles;
			std::string _tileDBName;
			/*空间参考由于局部坐标转化到球心坐标*/
			const osgEarth::SpatialReference* _srs;
			/*其他通用的属性信息*/
			osg::ref_ptr<ModelTileOptions>            _options;
			/*模型瓦片的平均高度*/
			double                                    _elevation;

			TileKey          _tileKey;

			float            _minRange;
			float            _maxRange;
			osg::Vec3d       _center;       

		protected:
			//  virtual ~ModelTile();

		private:
			Threading::Mutex  _updateMutex;

		};

	}  // namespace osgEarth::Drivers::MPTerrainEngine
}

#endif // OSGEARTH_DRIVERS_MP_TERRAIN_ENGINE_TILE_PAGED_LOD
