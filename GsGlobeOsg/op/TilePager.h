#ifndef GEOGLOBE_OP_TILEPAGER
#define GEOGLOBE_OP_TILEPAGER 1
#include <osgDB/FileNameUtils>
#include <osgDB/ReaderWriter>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
using namespace osg;

namespace GeoGlobe
{
	namespace OP
	{
		//简化的瓦片四叉树，具体可参考osgEarth2.7的PageTest的例子
		class TilePager : public osg::Group
		{
		public:
			TilePager(const osgEarth::Profile* profile);

			void build();
			//additive表示加载子节点时不隐藏父节点内容
			bool getAdditive();
			void setAdditive(bool additive);
			//图层元信息的设置和读取
			unsigned int getMinLevel() const;
			void setMinLevel(unsigned int minLevel);

			unsigned int getMaxLevel() const;
			void setMaxLevel(unsigned int maxLevel) ;

			void setExtend(double west,double east,double north,double south);

			//计算瓦片的包围盒，如果计算错误会导致瓦片分裂出错，这里默认以无地形的地表为准，
			//如果真实地形高于地球半径或低于，会导致瓦片分裂出问题
			osg::BoundingSphered getBounds(const osgEarth::TileKey& key);

			//构建四叉树根节点
			osg::Node* buildRootNode();

			//构建指定Key值的瓦片，这里只是虚函数，通过重写这部分实现具体应用
			virtual osg::Node* createNode( const osgEarth::TileKey& key );
			//为构造的瓦片添加PageLOD属性，实现再分裂能力
			osg::PagedLOD* createPagedNode( const osgEarth::TileKey& key );
			//遍历场景
			virtual void traverse(NodeVisitor& nv);

			//生成场景四叉树
			osg::Node* loadKey( const osgEarth::TileKey& key );

			const osgEarth::Profile* getProfile();

			//为true表示遍历下级瓦片时上级瓦片不会被清除，渐进补充
			bool m_additive;
			double m_rangeFactor;

			//最大最小级别
			unsigned int m_minLevel;
			unsigned int m_maxLevel;
			//经纬度范围
			double m_west;
			double m_east;
			double m_north;
			double m_south;
			//中心点位置
			osg::Vec2d m_centerGeo;
			//是否可见
			bool m_visible;

			//调用虚拟地球的坐标系
			osg::ref_ptr< const osgEarth::Profile > m_profile;
			osg::ref_ptr< osgDB::Options > m_options;
		};

		/*伪插件，实现倾斜影像实时加载*/
		struct TilePagerPseudoLoader : public osgDB::ReaderWriter
		{
			TilePagerPseudoLoader();
			const char* className();

			/*伪插件入口函数*/
			virtual osgDB::ReaderWriter::ReadResult readNode(const std::string& uri, const Options* options) const;
		};
		
	}
}
#endif
