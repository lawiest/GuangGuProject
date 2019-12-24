#include "TerrainLayer.h"
#include "osgEarth/GeoData"
#include "base/TerrainTileSource.h"
#include <osgEarth/ElevationLayer>
#include "innerimp.h"

GLOBE_NS

TerrainLayer::TerrainLayer(std::string strUrl, StoreType eSerType)
{
	BaseTileSourceOptions terop;

	if (eSerType == eLocal)
	{
		terop = BaseTileSourceOptions(strUrl, GeoGlobe::Tool::eElevationCache, eSerType);
	}
	else if (eSerType == eGeoGlobeTile || eSerType == eWMTS)
	{
		UTILITY_NAME::GsString gsHashString = UTILITY_NAME::GsStringHelp::ToString(UTILITY_NAME::GsHash::Hash64(strUrl.c_str()));
		string strCachPath = "./cache/" + std::string(gsHashString.c_str()) + ".tile";

		terop = BaseTileSourceOptions(strCachPath, strUrl, GeoGlobe::Tool::eElevationCache, eSerType);
	}


	osgEarth::ElevationLayerOptions elelayop = osgEarth::ElevationLayerOptions("Terrain", terop);
	elelayop.minLevel() = terop._metadata.toplevel;
	elelayop.maxLevel() = terop._metadata.bottomlevel;

	ElevationTileSource* ptrTileSource = new ElevationTileSource(terop);
	ptrTileSource->open();

	osg::ref_ptr<osgEarth::ElevationLayer> pElevationLayer = new osgEarth::ElevationLayer(elelayop, ptrTileSource);

	m_ptrLayerHandle = new ReferenceImp <osgEarth::ElevationLayer>(pElevationLayer);

	osgEarth::DataExtentList dex = ptrTileSource->DataExtentList();
	Range(dex[0].west(), dex[0].east(), dex[0].south(), dex[0].north());

	m_LayerType = (DataType)terop._metadata.nDataSetType;
	m_StoreType = eSerType;

	m_strLayerUrl = strUrl;
	m_nTopLevel = terop._metadata.toplevel;
	m_nBottomLevel = terop._metadata.bottomlevel;
}



TerrainLayer::TerrainLayer(KERNEL_NAME::GsTileClass* ptrTileClass)
{
	//osgEarth::ImageLayerOptions imageLayerOp = osgEarth::ImageLayerOptions("Image");
	//ImageTileSource* ptrImageTileSource = new ImageTileSource(ptrTileClass);
	//ptrImageTileSource->open();

	//osg::ref_ptr<osgEarth::ImageLayer> ptrImgLayer = new osgEarth::ImageLayer(imageLayerOp, ptrImageTileSource);

	//m_ptrLayerHandle = new ReferenceImp <osgEarth::ImageLayer>(ptrImgLayer);

	//GsBox box = ptrTileClass->Extent();
	//Range(box.XMin,box.XMax,box.YMin,box.YMax);

	//m_eunmLayerType = eImage;
}

int TerrainLayer::UID()
{
	return m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::ElevationLayer>>()->getUID();
}
 

TerrainLayer::~TerrainLayer()
{

}

void TerrainLayer::Visible(bool bVisible)
{
	m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::ElevationLayer>>()->setVisible(bVisible);
}

bool TerrainLayer::Visible() const
{
	return m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::ElevationLayer>>()->getVisible();
}

GLOBE_ENDNS