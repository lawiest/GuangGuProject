#include <osgEarth/Terrain>
#include <osgViewer/Viewer>
#include <osgEarth/MapNode>
#include <osg/MatrixTransform>
#include <osg/LOD>
#include <osgEarthAnnotation/PlaceNode>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <Model/CT3D_TileDriver.h>
#include <Model/ModelTile.h>
//#include <osgEarth/ge>

using namespace osgEarth::Annotation;

#ifndef EXTENSION_ADDMESHCALLBACK_H
#define EXTENSION_ADDMESHCALLBACK_H 1









#include <osgEarthAnnotation/LabelNode>
using namespace GeoGlobe::Model;

class AddMeshCallback : public TerrainCallback
{
public:
	AddMeshCallback(MapNode* mapNode):
_mapNode(mapNode){

}

std::string createModelTileName(std::string gmdxFile,const osgEarth::TileKey& key)
{
	return Stringify() <<gmdxFile<<"_"<<key.str() <<".modelTile"; 
}

void onTileAdded( const osgEarth::TileKey& key, osg::Node* tile, TerrainCallbackContext& context )
{
	
	std::string modelTileName=createModelTileName("C:/Users/Administrator/Desktop/JinYang/jinyang/jianzhu/DataSetjianhzu.gmdx",key);

	if (!osgEarth::Registry::instance()->isBlacklisted(modelTileName))
	{
		ModelTileOptions* options=new ModelTileOptions("C:/Users/Administrator/Desktop/JinYang/jinyang/jianzhu/DataSetjianhzu.gmdx");
		options->setMapNode(_mapNode);

		osg::ref_ptr<GeoGlobe::Model::ModelTile> modelTile=new GeoGlobe::Model::ModelTile(key,options);
		

			if (modelTile->hasRealData())
		{
			tile->asGroup()->addChild(modelTile);
	}
	else
		osgEarth::Registry::instance()->blacklist(modelTileName);
	}



	

}


MapNode* _mapNode;


};

#endif 