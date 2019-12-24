

#ifndef GEOGLOBE_BASE_TILEPAGEDLOD
#define GEOGLOBE_BASE_TILEPAGEDLOD 1

#include <osgEarth/Common>

#include <osg/PagedLOD>
#include <osgEarth/ThreadingUtils>
#include <osgEarth/Progress>
#include <osgEarth/TileKey>
#include <osgEarth/Terrain>
#include <osgEarth/mapNode>
#include <osg/Node>


using namespace osg;
 namespace GeoGlobe
{
	namespace Base
	{	
	/*实现基于瓦片的分页LOD功能，主要用于地名瓦片的四叉树分裂*/
    class TilePagedLOD : public osg::PagedLOD
    {
    public:
        TilePagedLOD(osgEarth::TileKey &                 tileKey,
           osgEarth::MapNode* mapNode);
		
        /*以下都是老代码，主要为了实现地名瓦片与地形的实时匹配，后发现移动端效率太低，故不再使用*/
		osgEarth::TileKey                  _tileKey;
		bool                     _hasRealData;
		osgEarth::GeoPoint                 _mapPosition;

    public: 
		void reclamp( const osgEarth::TileKey& key, osg::Node* tile, const osgEarth::Terrain* terrain );

		osgEarth::MapNode* _mapNode;

		virtual void traverse(NodeVisitor& nv);

		struct AutoClampCallback : public osgEarth::TerrainCallback
		{
			AutoClampCallback( TilePagedLOD* tilePagedLOD):
		_tilePagedLOD( tilePagedLOD )
		{
		}

		void onTileAdded( const osgEarth::TileKey& key, osg::Node* tile, osgEarth::TerrainCallbackContext& context )
		{
			_tilePagedLOD->reclamp( key, tile, context.getTerrain() );
		}

		TilePagedLOD* _tilePagedLOD;
		};

		osg::ref_ptr<AutoClampCallback> _autoClampCallback;
		
		void useAutoClamp();


    protected:
        virtual ~TilePagedLOD();


    };
	}

}  // namespace osgEarth::Drivers::MPTerrainEngine

#endif // OSGEARTH_DRIVERS_MP_TERRAIN_ENGINE_TILE_PAGED_LOD
