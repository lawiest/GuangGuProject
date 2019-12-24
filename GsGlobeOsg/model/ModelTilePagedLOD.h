
#ifndef GEOGLOBE_MODEL_MODELTILEPAGEDLOD
#define GEOGLOBE_MODEL_MODELTILEPAGEDLOD 1

#include <osgEarth/Common>
#include <osg/PagedLOD>
#include <osgEarth/ThreadingUtils>
#include <osgEarth/Progress>
#include <osgEarth/TileKey>
#include <osgEarth/Terrain>
#include <osgEarth/mapNode>
#include <osg/Node>
#include <Model/ModelOptions.h>


using namespace osg;
using namespace GeoGlobe::Tool;

 namespace GeoGlobe
{
    /**
     * TilePagedLOD is an extension to osg::PagedLOD that supports the tile registry.
     */
	namespace Model
	{
		/*用于判断瓦片是否进行四叉树分裂的功能类*/
		class ModelTilePagedLOD : public osg::PagedLOD
		{
		public:
			ModelTilePagedLOD(
			   osgEarth::MapNode* mapNode);
			//瓦片Key值
			TileKey                  _tileKey;
			//是否初始化成果
			bool                     _hasRealData;
	
			/*全局设置*/
			osg::ref_ptr<GeoGlobe::Tool::Setting> _setting;

			public: // osg::Group
			/*正常遍历即地形瓦片的遍历方式*/
			bool  _normalTraverse;
			/*分裂因子*/
			double _spliteFactor;

			/*继承osg::PagedLod,实现瓦片动态调度*/
			void traverse(osg::NodeVisitor& nv);

			public: 
			/*瓦片分裂*/
			bool splite(osg::NodeVisitor& nv,double timeStamp,double frameNumber,double range);

			/*以下都是老代码，实现瓦片自动调整高度，因效率不高故不使用*/
			void reclamp( const osgEarth::TileKey& key, osg::Node* tile, const osgEarth::Terrain* terrain );
			bool updateTransforms(  GeoPoint& mappos, osg::Node* patch =0L );
			void setTileKey(  TileKey& tikeKey );
			//virtual bool removeExpiredChildren(double expiryTime, unsigned int expiryFrame, NodeList& removedChildren);
			osg::ref_ptr<osgEarth::MapNode> _mapNode;

			//osg::Vec3d _tileCenter;
			//double     _tileRadius;
			/*	struct AutoClampCallback : public TerrainCallback
			{
				AutoClampCallback( TilePagedLOD* tilePagedLOD):
			_tilePagedLOD( tilePagedLOD )
			{
			}

			void onTileAdded( const osgEarth::TileKey& key, osg::Node* tile, TerrainCallbackContext& context )
			{
				_tilePagedLOD->reclamp( key, tile, context.getTerrain() );
			}

			TilePagedLOD* _tilePagedLOD;
			};*/

			/*osg::ref_ptr<AutoClampCallback> _autoClampCallback;
		
			void useAutoClamp();*/
			//struct ComputeTileBoundingSphereCallback : public osg::Node::ComputeBoundingSphereCallback
			//{
			//	ComputeTileBoundingSphereCallback() {}

			//	ComputeTileBoundingSphereCallback(const ComputeTileBoundingSphereCallback&,const CopyOp&) {}

			//	//META_Object(osg,ComputeTileBoundingSphereCallback);

			//	virtual BoundingSphere computeBound(const osg::Node&) const { return BoundingSphere(); }
			//};
			// osg::PagedLOD
			//BoundingSphere computeBound() const;
			/** override to manage the tile node registries. */
			//bool removeExpiredChildren(double expiryTime, unsigned expiryFrame, osg::NodeList& removedChildren);


			struct AutoClampCallback : public TerrainCallback
			{
				AutoClampCallback( ModelTilePagedLOD* tilePagedLOD):
				_tilePagedLOD( tilePagedLOD )
			{
			}

			void onTileAdded( const osgEarth::TileKey& key, osg::Node* tile, TerrainCallbackContext& context )
			{
				_tilePagedLOD->reclamp( key, tile, context.getTerrain() );
			}

			ModelTilePagedLOD* _tilePagedLOD;
			};
			osg::ref_ptr<AutoClampCallback> _autoClampCallback;
			void useAutoClamp();

		/*以上老代码，暂不使用*/
		protected:
			virtual ~ModelTilePagedLOD();

		};

	}  // namespace osgEarth::Drivers::MPTerrainEngine
}
#endif // OSGEARTH_DRIVERS_MP_TERRAIN_ENGINE_TILE_PAGED_LOD
