#include "WFSLayer.h"
#include "innerimp.h"
#include "vector/GEOWFSFeatureOptions.h"
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#include <osgEarth/ModelLayer>
#include <OsgCoreSingleton.h>
#include <osgEarth/MapNode>
#include <osgEarth/XmlUtils>
#include "tool/WFSFeaturePicker.h"

#include <osgViewer/Viewer>
#include <osgEarth/Notify>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/ExampleResources>

// include for WFS feature driver:
#include <osgEarthDrivers/feature_wfs/WFSFeatureOptions>

#include <osgEarthSymbology/Style>
//#include <osgEarthUtil/FeatureQueryTool>
#include <osgEarthFeatures/FeatureIndex>
#include <osgEarth/Registry>
#include <osgEarthAnnotation/FeatureNode>


#define LC "[wfs example] "

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;

GLOBE_NS

//osg::ref_ptr<osgEarth::Util::FeatureQueryTool> g_ptrWFSTool;
////osg::ref_ptr<GeoGlobe::WFSFeaturePicker> g_WFSFeaturePicker;
//
//class ReadoutCallback : public FeatureQueryTool::Callback
//{
//public:
//	ReadoutCallback() : _lastFID(~0)
//	{
//	}
//
//	void onHit(ObjectID id)
//	{
//		FeatureIndex* index = Registry::objectIndex()->get<FeatureIndex>(id);
//		osgEarth::Features::Feature* feature = index ? index->getFeature(id) : 0L;
//
//		if (feature == NULL)
//			return;
//
//		//高亮 
//		osgEarth::Features::Feature* ptrBuildingFeature = new osgEarth::Features::Feature(*feature, osg::CopyOp::DEEP_COPY_ALL);
//
//		LineSymbol* line = ptrBuildingFeature->style()->getOrCreate<LineSymbol>();
//		line->stroke()->color() = Color(Color::Yellow,0.5);
//		line->stroke()->width() = 1.0f;
//		line->stroke()->widthUnits() = Units::PIXELS;
//		ptrBuildingFeature->style()->getOrCreateSymbol<ExtrusionSymbol>()->height() = 1000;
//		ptrBuildingFeature->style()->getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->enabled() = true;
//		ptrBuildingFeature->style()->getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->automatic() = true;
//		ptrBuildingFeature->style()->getOrCreate<osgEarth::RenderSymbol>()->depthTest() = true;
//		ptrBuildingFeature->style()->getOrCreate<PolygonSymbol>()->fill()->color() = Color(osgEarth::Symbology::Color::Cyan, 0.5);
//		//ptrBuildingFeature->style()->getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN /*AltitudeSymbol::CLAMP_ABSOLUTE*/;
//		//ptrBuildingFeature->style()->getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
//
//		osg::ref_ptr<osgEarth::Annotation::FeatureNode> polyNode = new osgEarth::Annotation::FeatureNode(OsgCoreSingleton::Instance()->MapNode(), ptrBuildingFeature);
//		OsgCoreSingleton::Instance()->MapNode()->addChild(polyNode);
//
//		if (feature && feature->getFID() != _lastFID)
//		{
//			_lastFID = feature->getFID();
//		}
//	}
//
//	void onMiss()
//	{
//		//_grid->setVisible(false);
//		_lastFID = 0u;
//	}
//
//	bool accept(const osgGA::GUIEventAdapter& ea, const osgGA::GUIActionAdapter& aa)
//	{
//		return ea.getEventType() == ea.RELEASE; // click
//	}
//
//	//Grid*     _grid;
//	FeatureID _lastFID;
//};

std::vector<std::string> WFSLayer::getTypeNameList(std::string strUrl)
{
	std::vector<std::string> vecTypeList;

	char sep = strUrl.find_first_of('?') == std::string::npos ? '?' : '&';

	std::string capUrl = strUrl;
	if (sep == '?')
	{
		capUrl = strUrl + sep +
			std::string("SERVICE=") + "WFS" +
			std::string("&VERSION=") + "1.0.0" +
			std::string("&REQUEST=GetCapabilities");
	}

	osgEarth::HTTPResponse response = osgEarth::HTTPClient::get(capUrl);
	std::string data = response.getPartAsString(0);

	std::istringstream in(data);
	osg::ref_ptr<GeoGlobe::GeoVector::GEOWFSCapabilities > _capabilities = GeoGlobe::GeoVector::GEOWFSCapabilitiesReader::read(in);

	if (_capabilities == NULL)
		return vecTypeList;

	GeoGlobe::GeoVector::GEOFeatureTypeList typeList = _capabilities->getFeatureTypes();
	for (GeoGlobe::GeoVector::GEOFeatureTypeList::iterator it = typeList.begin(); it != typeList.end(); it++)
	{
		std::string strTypeName = (*it)->getName();
		vecTypeList.push_back(strTypeName);
	}

	return vecTypeList;
}

//UTILITY_NAME::GsDelegate<void(Feature*)>& WFSLayer::OnPicked()
//{
//	return g_WFSFeaturePicker->OnPicked;
//}

WFSLayer::WFSLayer(std::string strUrl, std::string strTypeName, double dbheight)
{
	m_bIsPick = false;
	ReadWFSMetaDataFromServer(strUrl, strTypeName);

	// Create the WFS driver:
	osgEarth::Drivers::WFSFeatureOptions wfs;
	wfs.url() = osgEarth::URI(strUrl);
	wfs.typeName() = strTypeName;
	//wfs.outputFormat() = "json";     // JSON or GML

	// Configure a rendering style:
	Style style;
	style.setName(strTypeName);

	std::string strGeomtryType = ReadFeatureType(strUrl, strTypeName);

	if (strGeomtryType == "gml:PointPropertyType")
	{
		PointSymbol* point = style.getOrCreate<PointSymbol>();
		point->size() = 5;
		point->fill()->color() = Color::Red;
	}
	else if (strGeomtryType == "gml:LineStringPropertyType")
	{
		LineSymbol* line = style.getOrCreate<LineSymbol>();
		line->stroke()->color() = Color::Yellow;
		line->stroke()->width() = 1.0f;
		line->stroke()->widthUnits() = Units::PIXELS;
		style.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = dbheight;

		//style.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
		//style.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
	}
	else if (strGeomtryType == "gml:PolygonPropertyType")
	{
		LineSymbol* line = style.getOrCreate<LineSymbol>();
		line->stroke()->color() = Color::Yellow;
		line->stroke()->width() = 1.0f;
		line->stroke()->widthUnits() = Units::PIXELS;
		style.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = dbheight;
		style.getOrCreateSymbol<PolygonSymbol>()->fill()->color() = Color(osgEarth::Symbology::Color::Cyan, 1.0);
		//style.getOrCreateSymbol<ExtrusionSymbol>()->height() = 100;
		//style.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
		//style.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
		//解决Z fighting
		style.getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->enabled() = true;
		style.getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->automatic() = true;
	}

	// Configure a model layer to render the features:
	osgEarth::Drivers::FeatureGeomModelOptions geom;
	geom.featureOptions() = wfs;
	geom.styles() = new StyleSheet();
	geom.styles()->addStyle(style);

	osgEarth::Features::FeatureSourceIndexOptions psindexOption;
	psindexOption.embedFeatures() = true;
	geom.featureIndexing() = psindexOption;

	// Make the new layer and add it to the map.
	ModelLayerOptions layerOptions(strTypeName, geom);
	osg::ref_ptr<osgEarth::ModelLayer> layer = new osgEarth::ModelLayer(layerOptions);
	m_ptrLayerHandle = new ReferenceImp <osgEarth::ModelLayer>(layer);
	m_StoreType = eWFS;
	m_LayerType = eVector;
}


WFSLayer::WFSLayer(std::string strUrl, std::string strTypeName, GsRawPoint3D originPoint)
{	
	m_bIsPick = false;
	ReadWFSMetaDataFromServer(strUrl, strTypeName);

	Style style;
	style.setName(strTypeName);

	std::string strGeomtryType = ReadFeatureType(strUrl, strTypeName);

	GeoGlobe::GeoVector::GEOWFSFeatureOptions geoFeaOpt;
	geoFeaOpt.url() = strUrl;
	geoFeaOpt.cachePath() = "./cache";
	geoFeaOpt.typeName() = strTypeName;
	geoFeaOpt.init();
	geoFeaOpt.OriginX() = originPoint.X;
	geoFeaOpt.OriginY() = originPoint.Y;
	geoFeaOpt.OriginZ() = originPoint.Z;

	osgEarth::Drivers::FeatureGeomModelOptions geomOptions;
	geomOptions.featureOptions() = geoFeaOpt;

	if (strGeomtryType == "gml:PointPropertyType")
	{
		PointSymbol* point = style.getOrCreate<PointSymbol>();
		point->size() = 5;
		point->fill()->color() = Color::Red;
	}
	else if (strGeomtryType == "gml:LineStringPropertyType")
	{
		LineSymbol* line = style.getOrCreate<LineSymbol>();
		line->stroke()->color() = Color::Yellow;
		line->stroke()->width() = 1.0f;
		line->stroke()->widthUnits() = Units::PIXELS;
		//style.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = dbheight;

		//style.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
		//style.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
	}
	else if (strGeomtryType == "gml:PolygonPropertyType")
	{
		LineSymbol* line = style.getOrCreate<LineSymbol>();
		line->stroke()->color() = Color::Yellow;
		line->stroke()->width() = 1.0f;
		line->stroke()->widthUnits() = Units::PIXELS;
		//style.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = dbheight;
		style.getOrCreateSymbol<PolygonSymbol>()->fill()->color() = Color(osgEarth::Symbology::Color::Cyan, 1.0);
		//style.getOrCreateSymbol<ExtrusionSymbol>()->height() = 100;
		//style.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
		//style.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
		//解决Z fighting
		style.getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->enabled() = true;
		style.getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->automatic() = true;
	}

	geomOptions.styles() = new osgEarth::StyleSheet();
	geomOptions.styles()->addStyle(style);

	osgEarth::ModelLayerOptions layerOptions(strTypeName, geomOptions);
	osg::ref_ptr<osgEarth::ModelLayer> ptrVectorLayer = new osgEarth::ModelLayer(layerOptions);
	m_ptrLayerHandle = new ReferenceImp <osgEarth::ModelLayer>(ptrVectorLayer);

	m_LayerType = eVector;
	m_StoreType = eWFS;	
}

WFSLayer::WFSLayer(std::string strUrl, std::string strTypeName, RoadTextStyle* roadTextStyle)
{
	m_bIsPick = false;
	ReadWFSMetaDataFromServer(strUrl, strTypeName);

	Style style;
	style.setName(strTypeName);

	std::string strGeomtryType = ReadFeatureType(strUrl, strTypeName);

	GeoGlobe::GeoVector::GEOWFSFeatureOptions geoFeaOpt;
	geoFeaOpt.url() = strUrl;
	geoFeaOpt.cachePath() = "./cache";
	geoFeaOpt.typeName() = strTypeName;
	geoFeaOpt.init();
	geoFeaOpt.Size() = roadTextStyle->m_nsize;
	geoFeaOpt.ColorA() = roadTextStyle->m_color.A;
	geoFeaOpt.ColorR() = roadTextStyle->m_color.R;
	geoFeaOpt.ColorG() = roadTextStyle->m_color.G;
	geoFeaOpt.ColorB() = roadTextStyle->m_color.B;
	geoFeaOpt.FontFile() = roadTextStyle->m_strFontFile;
	geoFeaOpt.ShaowColorA() = roadTextStyle->m_shadowColor.A;
	geoFeaOpt.ShaowColorR() = roadTextStyle->m_shadowColor.R;
	geoFeaOpt.ShaowColorG() = roadTextStyle->m_shadowColor.G;
	geoFeaOpt.ShaowColorB() = roadTextStyle->m_shadowColor.B;
	geoFeaOpt.ShaowOffset() = roadTextStyle->m_dbOffset;
	geoFeaOpt.ShadowPos() = roadTextStyle->m_eShadowPos;

	osgEarth::Drivers::FeatureGeomModelOptions geomOptions;
	geomOptions.featureOptions() = geoFeaOpt;
	geomOptions.styles() = new osgEarth::StyleSheet();
	geomOptions.styles()->addStyle(style);

	osgEarth::ModelLayerOptions layerOptions(strTypeName, geomOptions);
	osg::ref_ptr<osgEarth::ModelLayer> ptrVectorLayer = new osgEarth::ModelLayer(layerOptions);
	m_ptrLayerHandle = new ReferenceImp <osgEarth::ModelLayer>(ptrVectorLayer);

	m_LayerType = eVector;
	m_StoreType = eWFS;
}

void WFSLayer::StartPick()
{
	if (!m_bIsPick)
	{
		////g_WFSFeaturePicker = new GeoGlobe::WFSFeaturePicker();
		//g_ptrWFSTool = new osgEarth::Util::FeatureQueryTool();
		//g_ptrWFSTool->addChild(OsgCoreSingleton::Instance()->MapNode());

		//g_ptrWFSTool->setDefaultCallback(new ReadoutCallback());
		//OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(g_ptrWFSTool);
		////OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(g_WFSFeaturePicker);
		m_bIsPick = true;
	}
}

void WFSLayer::EndPick()
{
	if (m_bIsPick)
	{
		//OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(g_ptrWFSTool);
		////g_WFSFeaturePicker->ClearPreHighLight();
		////OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(g_WFSFeaturePicker);
		//g_ptrWFSTool = NULL;
		////g_WFSFeaturePicker = NULL;
		m_bIsPick = false;
	}
}

std::string WFSLayer::ReadFeatureType(std::string strUrl, std::string strTypeName)
{
	char sep = strUrl.find_first_of('?') == std::string::npos ? '?' : '&';

	std::string strTypeNameSeq = std::string("&TYPENAME=") + strTypeName;

	std::string strCapUrl = strUrl;
	if (sep == '?')
	{
		strCapUrl = strUrl + sep +
			std::string("SERVICE=") + "WFS" +
			std::string("&VERSION=") + "1.0.0" +
			std::string("&REQUEST=DescribeFeatureType") + 
			strTypeNameSeq;
	}

	osgEarth::HTTPResponse response = osgEarth::HTTPClient::get(strCapUrl);
	std::string strXmlData = response.getPartAsString(0);

	std::istringstream stream(strXmlData);
	osg::ref_ptr<osgEarth::XmlDocument> ptrDocXml = osgEarth::XmlDocument::load(stream);

	if (!ptrDocXml.valid())
		return "load xml file fail!";

	osg::ref_ptr<const osgEarth::XmlElement> ptrSequeElem = ptrDocXml->findElement("sequence");

	if (ptrSequeElem == NULL)
		return "request error!";

	osgEarth::XmlNodeList ptrNodeList = ptrSequeElem->getChildren();

	for (osgEarth::XmlNodeList::iterator it = ptrNodeList.begin(); it != ptrNodeList.end(); it++)
	{
		std::string strGeometryName = dynamic_cast<osgEarth::XmlElement*>((*it).get())->getAttr("name");

		if ("GEOMETRY" == strGeometryName)
			return dynamic_cast<osgEarth::XmlElement*>((*it).get())->getAttr("type");
	}

	return "not found";
}

void WFSLayer::ReadWFSMetaDataFromServer(std::string strUrl, std::string strTypeName)
{
	char sep = strUrl.find_first_of('?') == std::string::npos ? '?' : '&';

	std::string capUrl = strUrl;
	if (sep == '?')
	{
		capUrl = strUrl + sep +
			std::string("SERVICE=") + "WFS" +
			std::string("&VERSION=") + "1.0.0" +
			std::string("&REQUEST=GetCapabilities");
	}

	osgEarth::HTTPResponse response = osgEarth::HTTPClient::get(capUrl);
	std::string data = response.getPartAsString(0);

	std::istringstream in(data);
	osg::ref_ptr<GeoGlobe::GeoVector::GEOWFSCapabilities > _capabilities = GeoGlobe::GeoVector::GEOWFSCapabilitiesReader::read(in);

	GeoGlobe::GeoVector::GEOFeatureTypeList typeList = _capabilities->getFeatureTypes();
	osg::ref_ptr<GeoGlobe::GeoVector::GEOWFSFeatureType > ptrFeatureType;
	for (GeoGlobe::GeoVector::GEOFeatureTypeList::iterator it = typeList.begin(); it != typeList.end(); it++)
	{
		if (strTypeName == (*it)->getName())
		{
			ptrFeatureType = (*it);

			Range(ptrFeatureType->getExtent().east(), ptrFeatureType->getExtent().west(), ptrFeatureType->getExtent().north(), ptrFeatureType->getExtent().south());

			m_strLayerUrl = strUrl;
			m_nTopLevel = ptrFeatureType->getFirstLevel();
			m_nBottomLevel = ptrFeatureType->getMaxLevel();
		}
	}
}

WFSLayer::WFSLayer(KERNEL_NAME::GsTileClass* ptrTileClass)
{

}

int WFSLayer::UID()
{
	return 0;
}


WFSLayer::~WFSLayer()
{
}

void WFSLayer::Visible(bool bVisible)
{
	m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::ModelLayer>>()->setVisible(bVisible);
}

bool WFSLayer::Visible() const
{
 	return m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::ModelLayer>>()->getVisible();
}

GLOBE_ENDNS