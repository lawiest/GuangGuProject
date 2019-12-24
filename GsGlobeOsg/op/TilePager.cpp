#include "TilePager.h"

namespace GeoGlobe
{
	namespace OP
	{
		//简化的瓦片四叉树，具体可参考osgEarth2.7的PageTest的例子
		TilePager::TilePager(const osgEarth::Profile* profile):
			m_profile( profile ),
			m_rangeFactor( 1.5 ),
			m_additive(false),
			m_minLevel(0),
			m_maxLevel(30),
			m_west(-180),
			m_east(180),
			m_south(-90),
			m_north(90),
			m_visible(true)
		{
			m_options = new osgDB::Options;
			m_options->setUserData( this );

		}

		void TilePager::build()
		{
			addChild( buildRootNode() );
		}

		//additive表示加载子节点时不隐藏父节点内容
		bool TilePager::getAdditive() { return m_additive; }
		void TilePager::setAdditive(bool additive) { m_additive = additive; }
		//图层元信息的设置和读取
		unsigned int TilePager::getMinLevel() const { return m_minLevel; }
		void TilePager::setMinLevel(unsigned int minLevel) { m_minLevel = minLevel; }

		unsigned int TilePager::getMaxLevel() const { return m_maxLevel; }
		void TilePager::setMaxLevel(unsigned int maxLevel) { m_maxLevel = maxLevel; }

		void TilePager::setExtend(double west,double east,double north,double south)
		{
			m_west = west;
			m_east = east;
			m_north = north;
			m_south = south;
		}

		//计算瓦片的包围盒，如果计算错误会导致瓦片分裂出错，这里默认以无地形的地表为准，
		//如果真实地形高于地球半径或低于，会导致瓦片分裂出问题
		osg::BoundingSphered TilePager::getBounds(const osgEarth::TileKey& key)
		{
			int samples = 6;

		 osgEarth::GeoExtent extent = key.getExtent();

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

					osgEarth::GeoPoint samplePoint(extent.getSRS(), x, y, 0, osgEarth::ALTMODE_ABSOLUTE);

					osgEarth::GeoPoint wgs84 = samplePoint.transform(osgEarth::SpatialReference::create("epsg:4326"));
					wgs84.toWorld(world);
					bs.expandBy(world);
				}
			}
			return bs;
		}

		//构建四叉树根节点
		osg::Node* TilePager::buildRootNode()
		{
			//构建顶层瓦片
			osg::Group* root = new osg::Group;

			//std::vector<TileKey> keys;
			//_profile->getRootKeys( keys );
			
			std::vector< osgEarth::TileKey > keys;
			osgEarth::GeoExtent ext= osgEarth::GeoExtent(m_profile->getSRS(),m_west,m_south,m_east,m_north);
			/*int firstLevel=_minLevel-1;
			firstLevel=firstLevel>0?firstLevel:0;*/
			m_profile->getIntersectingTiles(ext,m_minLevel,keys);

			for (unsigned int i = 0; i < keys.size(); i++)
			{
				root->addChild( createPagedNode( keys[i] ) );
			}

			return root;
		}

		//构建指定Key值的瓦片，这里只是虚函数，通过重写这部分实现具体应用
		osg::Node* TilePager::createNode( const osgEarth::TileKey& key )
		{
			return NULL;
		}

		//为构造的瓦片添加PageLOD属性，实现再分裂能力
		osg::PagedLOD* TilePager::createPagedNode( const osgEarth::TileKey& key )
		{
			osg::BoundingSphered tileBounds = getBounds( key );

			osg::PagedLOD* plod = new osg::PagedLOD;
			plod->setCenter( tileBounds.center() );
			plod->setRadius( tileBounds.radius() );

			// Create the actual data for this tile.
			osg::Node* node = createNode( key );
			if (!node && key.getLevelOfDetail() < m_minLevel)
			{              
				// If we couldn't create any data, just create an empty group.  That's ok.
				node = new osg::Group;
			}
			if (!node) return 0;
			plod->addChild( node );

			if(key.getLevelOfDetail() >= m_maxLevel)
			{
				plod->setRange( 0, 0, FLT_MAX );
				return plod;
			}

			std::stringstream buf;
			buf << key.getLevelOfDetail() << "_" << key.getTileX() << "_" << key.getTileY() << ".tile_pager";

			std::string uri = buf.str();

			// Now setup a filename on the PagedLOD that will load all of the children of this node.
			plod->setFileName(1, uri);
			plod->setDatabaseOptions( m_options.get() );

			// Setup the min and max ranges.

			// This setups a replacement mode where the parent will be completely replaced by it's children.
			float minRange = (float)(tileBounds.radius() * m_rangeFactor);

			if (!m_additive)
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
		void TilePager::traverse(NodeVisitor& nv)
		{
			//如果设置不可见就不绘制
			if(!m_visible)
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
		osg::Node* TilePager::loadKey( const osgEarth::TileKey& key )
		{       
			osg::ref_ptr< osg::Group >  group = new osg::Group;
			//生成四个子节点
			for (unsigned int i = 0; i < 4; i++)
			{
				osgEarth::TileKey childKey = key.createChildKey( i );

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

		const osgEarth::Profile* TilePager::getProfile()
		{
			return m_profile.get();
		}

		/*伪插件，实现倾斜影像实时加载*/

		TilePagerPseudoLoader::TilePagerPseudoLoader()
		{
			supportsExtension( "tile_pager", "" );
		}

		const char* TilePagerPseudoLoader::className()
		{ // override
			return "Simple Pager";
		}

		/*伪插件入口函数*/
		osgDB::ReaderWriter::ReadResult TilePagerPseudoLoader::readNode(const std::string& uri, const Options* options) const
		{
			if ( !acceptsExtension( osgDB::getLowerCaseFileExtension(uri) ) )
				return ReadResult::FILE_NOT_HANDLED;

			unsigned lod, x, y;
			sscanf( uri.c_str(), "%d_%d_%d.%*s", &lod, &x, &y );

			TilePager* pager = dynamic_cast< TilePager*>(const_cast<Options*>(options)->getUserData());
			if (pager)
			{
				return pager->loadKey(osgEarth::TileKey( lod, x, y, pager->getProfile() ) );
			}

			return ReadResult::ERROR_IN_READING_FILE;
		}
		
		REGISTER_OSGPLUGIN(tile_pager, TilePagerPseudoLoader);
	}
}
