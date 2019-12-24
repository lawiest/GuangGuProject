#include "ImageLayer.h"
#include "osgEarth/GeoData"
#include "base/ImageTileSource.h"
#include <osgEarth/ImageLayer>
#include "innerimp.h"
#include <any.h>

GLOBE_NS

ImageLayer::ImageLayer(std::string strUrl, StoreType eSerType)
{
	BaseTileSourceOptions imageop;

	if (eSerType == eLocal)
	{
		imageop = BaseTileSourceOptions(strUrl, GeoGlobe::Tool::eImageCache, (GLOBE_NAME::StoreType)eSerType);
	}
	else if (eSerType == eWMTS || eSerType == eGeoGlobeTile)
	{
		UTILITY_NAME::GsString gsHashString = UTILITY_NAME::GsStringHelp::ToString(UTILITY_NAME::GsHash::Hash64(strUrl.c_str()));

		string strCachPath = "./cache/" + std::string(gsHashString.c_str()) + ".tile";
		imageop = BaseTileSourceOptions(strCachPath,strUrl,GeoGlobe::Tool::eImageCache, eSerType);
	}

	osgEarth::ImageLayerOptions imagelayop = osgEarth::ImageLayerOptions("Image", imageop);
	imagelayop.minLevel() = imageop._metadata.toplevel;
	imagelayop.maxLevel() = imageop._metadata.bottomlevel;

	ImageTileSource* ptrImageTileSource = new ImageTileSource(imageop);
	ptrImageTileSource->open();
	
	osg::ref_ptr<osgEarth::ImageLayer> ptrImgLayer = new osgEarth::ImageLayer(imagelayop, ptrImageTileSource);

	m_ptrLayerHandle = new ReferenceImp <osgEarth::ImageLayer>(ptrImgLayer);

	osgEarth::DataExtentList dex = ptrImageTileSource->DataExtentList();
	Range(dex[0].west(), dex[0].east(), dex[0].south(), dex[0].north());

	m_LayerType = eImage/*(DataType)imageop._metadata.nDataSetType*/;
	m_StoreType = eSerType;

	m_strLayerUrl = strUrl;
	m_nTopLevel = imageop._metadata.toplevel;
	m_nBottomLevel = imageop._metadata.bottomlevel;
}



ImageLayer::ImageLayer(KERNEL_NAME::GsTileClass* ptrTileClass)
{
	osgEarth::ImageLayerOptions imageLayerOp = osgEarth::ImageLayerOptions("Image");
	ImageTileSource* ptrImageTileSource = new ImageTileSource(ptrTileClass);
	ptrImageTileSource->open();

	osg::ref_ptr<osgEarth::ImageLayer> ptrImgLayer = new osgEarth::ImageLayer(imageLayerOp, ptrImageTileSource);

	m_ptrLayerHandle = new ReferenceImp <osgEarth::ImageLayer>(ptrImgLayer);

	GsBox box = ptrTileClass->Extent();
	Range(box.XMin,box.XMax,box.YMin,box.YMax);

	m_LayerType = eImage;
}

int ImageLayer::UID()
{
	return m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::ImageLayer>>()->getUID();
}
 

ImageLayer::~ImageLayer()
{

}

void ImageLayer::Visible(bool bVisible)
{
	m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::ImageLayer>>()->setVisible(bVisible);
}

bool ImageLayer::Visible() const
{
	return m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::ImageLayer>>()->getVisible();
}

GLOBE_ENDNS