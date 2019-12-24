#ifndef GEOGLOBE_PLACENAME_QUADMODELTILEDRIVER
#define GEOGLOBE_PLACENAME_QUADMODELTILEDRIVER 1

#include <osgDB/ReaderWriter>
#include <PlaceName/PlaceNameTileOptions.h>
#include <PlaceName/PlaceNameTile.h>

GLOBE_NS

/*伪插件，通过伪插件才能将地名瓦片以DataBasePager(线程)的方式导入*/
class QuadPlaceNameTileDriver : public osgDB::ReaderWriter
{
public:
	QuadPlaceNameTileDriver();

	virtual const char* className();

	void parseFileName(const std::string& uri,int& level,int& row,int& col) const;

	
	/*将瓦片加入到PageLod中，实现四叉树分裂功能*/
	osg::Node * createTile(osg::Node* node, const osgEarth::TileKey& tileKey, GLOBE_NAME::PlaceNameLayerOptions* options) const;

	std::string createPlaceNameTileName(const std::string& gmdxFile, const osgEarth::TileKey& key) const;

    /*生成4个子瓦片*/
	osg::Node* createNode(const osgEarth::TileKey& tileKey, GLOBE_NAME::PlaceNameLayerOptions* options) const;
    /*入口函数*/
	virtual ReadResult readNode(const std::string& uri, const Options* options) const;
};

GLOBE_ENDNS

#endif
