#ifndef GEOGLOBE_MODEL_TILEPAGER
#define GEOGLOBE_MODEL_TILEPAGER 1

#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgDB/FileNameUtils>
#include <osgDB/ReaderWriter>
#include "Model/ModelOptions.h"

using namespace osgEarth;
namespace GeoGlobe
{
	namespace Model
	{
		//简化的瓦片四叉树，具体可参考osgEarth2.7的PageTest的例子
		class ModelTilePager : public osg::Group
		{
		public:
			//ModelTilePager(const osgEarth::Profile* profile):
			//  _profile( profile ),
			//	  _rangeFactor( 1.5 ),
			//	  _additive(false),
			//	  _minLevel(0),
			//	  _maxLevel(30),
			//	  _west(-180),
			//	  _east(180),
			//	  _south(-90),
			//	  _north(90),
			//	  _visible(true)
			//  {
			//	  _options = new osgDB::Options;
			//	  _options->setUserData( this );

			//  }

			  ModelTilePager(const osgEarth::Profile* profile,ModelLayerOptions* options):
			      _profile( profile ),
				   _options( options ),
				  _rangeFactor( 1.0 ),
				  _additive(false),
				  _minLevel(0),
				  _maxLevel(30),
				  _west(-180),
				  _east(180),
				  _south(-90),
				  _north(90),
				  _visible(true)
			  {
				  _options->setUserData( this );
			  }

			  void build()
			  {
				  addChild( buildRootNode() );
			  }
			  //additive表示加载子节点时不隐藏父节点内容
			  bool getAdditive() { return _additive; }
			  void setAdditive(bool additive) { _additive = additive; }
			  
			  //图层元信息的设置和读取
			  unsigned int getMinLevel() const { return _minLevel; }
			  void setMinLevel(unsigned int minLevel) { _minLevel = minLevel; }

			  unsigned int getMaxLevel() const { return _maxLevel; }
			  void setMaxLevel(unsigned int maxLevel) { _maxLevel = maxLevel; }

			  void setExtend(double west,double east,double north,double south)
			  {
				  _west = west;
				  _east = east;
				  _north = north;
				  _south = south;
			  }

			  //计算瓦片的包围盒，如果计算错误会导致瓦片分裂出错，这里默认以无地形的地表为准，
			  //如果真实地形高于地球半径或低于，会导致瓦片分裂出问题
			  osg::BoundingSphered getBounds(const TileKey& key)
			  {
				  int samples = 6;

				  GeoExtent extent = key.getExtent();

				  double xSample = extent.width() / (double)samples;
				  double ySample = extent.height() / (double)samples;

				  osg::BoundingSphered bs;
				  for (int c = 0; c < samples+1; c++)
				  {
					  double x = extent.xMin() + (double)c * xSample;
					  for (int r = 0; r < samples+1; r++)
					  {
						  double y = extent.yMin() + (double)r * ySample;
						  osg::Vec3d world;

						  GeoPoint samplePoint(extent.getSRS(), x, y, 0, ALTMODE_ABSOLUTE);

						  GeoPoint wgs84 = samplePoint.transform(osgEarth::SpatialReference::create("epsg:4326"));
						  wgs84.toWorld(world);
						  bs.expandBy(world);
					  }
				  }
				  return bs;
			  }

              //构建四叉树根节点
			  osg::Node* buildRootNode()
			  {
				  //构建顶层瓦片
				  osg::Group* root = new osg::Group;
				  std::vector< TileKey > keys;
				  GeoExtent ext=GeoExtent(_profile->getSRS(),_west,_south,_east,_north);
				  _profile->getIntersectingTiles(ext,_minLevel,keys);

				  for (unsigned int i = 0; i < keys.size(); i++)
				  {
					  root->addChild( createPagedNode( keys[i] ) );
				  }
				  return root;
			  }

             //构建指定Key值的瓦片，这里只是虚函数，通过重写这部分实现具体应用
			  virtual osg::Node* createNode( const TileKey& key )
			  {
				  return NULL;
			  }
              //为构造的瓦片添加PageLOD属性，实现再分裂能力
			  osg::PagedLOD* createPagedNode( const TileKey& key )
			  {
				  osg::BoundingSphered tileBounds = getBounds( key );

				  osg::PagedLOD* plod = new osg::PagedLOD;
				  plod->setCenter( tileBounds.center() );
				  plod->setRadius( tileBounds.radius() );

				  // Create the actual data for this tile.
				  osg::Node* node = createNode( key );
				  if (!node && key.getLevelOfDetail() < _minLevel)
				  {              
					  // If we couldn't create any data, just create an empty group.  That's ok.
					  node = new osg::Group;
				  }
				  if (!node) return 0;
				  plod->addChild( node );

				  if(key.getLevelOfDetail() >= _maxLevel)
				  {
					  plod->setRange( 0, 0, FLT_MAX );
					  return plod;
				  }

				  std::stringstream buf;
				  buf << key.getLevelOfDetail() << "_" << key.getTileX() << "_" << key.getTileY() << ".modeltile_pager";

				  std::string uri = buf.str();

				  // Now setup a filename on the PagedLOD that will load all of the children of this node.
				  plod->setFileName(1, uri);
				  plod->setDatabaseOptions( _options.get() );

				  // Setup the min and max ranges.

				  // This setups a replacement mode where the parent will be completely replaced by it's children.
				  float minRange = (float)(tileBounds.radius() * _rangeFactor);

				  if (!_additive)
				  {
					  // Replace mode, the parent is replaced by it's children.
					  plod->setRange( 0, minRange, FLT_MAX );
					  plod->setRange( 1, 0, minRange );
				  }
				  else
				  {
					  // Additive, the parent remains and new data is added
					  plod->setRange( 0, 0, FLT_MAX );
					  plod->setRange( 1, 0, minRange );
				  }

				  return plod;
			  }

			//遍历场景
			virtual void traverse(NodeVisitor& nv)
			{
				//如果设置不可见就不绘制
				if(!_visible)
					return;

				//遍历子节点
				for(NodeList::iterator itr=_children.begin();
					itr!=_children.end();
					++itr)
				{
					(*itr)->accept(nv);
				}
			}

              //生成场景四叉树
			  osg::Node* loadKey( const TileKey& key )
			  {       
				  osg::ref_ptr< osg::Group >  group = new osg::Group;
				  //生成四个子节点
				  for (unsigned int i = 0; i < 4; i++)
				  {
					  TileKey childKey = key.createChildKey( i );

					  osg::PagedLOD* plod = createPagedNode( childKey );
					  if (plod)
					  {
						  group->addChild( plod );
					  }
				  }
				  if (group->getNumChildren() > 0)
				  {
					  return group.release();
				  }
				  return 0;
			  }

			  const osgEarth::Profile* getProfile()
			  {
				  return _profile.get();
			  }

			  //为true表示遍历下级瓦片时上级瓦片不会被清除，渐进补充
			  bool _additive;
			  double _rangeFactor;

			  //最大最小级别
			  unsigned int _minLevel;
			  unsigned int _maxLevel;
			  //经纬度范围
			  double _west;
			  double _east;
			  double _north;
			  double _south;
			  //中心点位置
			  osg::Vec2d _centerGeo;
			  //是否可见
			  bool _visible;

		public:
			  //调用虚拟地球的坐标系
			  osg::ref_ptr< const osgEarth::Profile > _profile;
			  osg::ref_ptr< ModelLayerOptions > _options;
		};

        /*伪插件，实现模型实时加载*/
		struct ModelTilePagerPseudoLoader : public osgDB::ReaderWriter
		{
			ModelTilePagerPseudoLoader()
			{
				supportsExtension( "modeltile_pager", "" );
			}

			const char* className()
			{ // override
				return "Simple Pager";
			}

            /*伪插件入口函数*/
			ReadResult readNode(const std::string& uri, const Options* options) const
			{
				if ( !acceptsExtension( osgDB::getLowerCaseFileExtension(uri) ) )
					return ReadResult::FILE_NOT_HANDLED;

				unsigned lod, x, y;
				sscanf( uri.c_str(), "%d_%d_%d.%*s", &lod, &x, &y );

				ModelTilePager* pager = dynamic_cast< ModelTilePager*>(const_cast<Options*>(options)->getUserData());
				if (pager)
				{
					return pager->loadKey( TileKey( lod, x, y, pager->getProfile() ) );
				}

				return ReadResult::ERROR_IN_READING_FILE;
			}
		};

		REGISTER_OSGPLUGIN(modeltile_pager, ModelTilePagerPseudoLoader);
	}
}
#endif
