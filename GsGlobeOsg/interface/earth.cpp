#include "earth.h"
#include <osgEarth/MapNode>
#include <osgEarth/MapNodeOptions>
#include <osgEarth/TerrainEngineNode>
#include <osgEarthDrivers/engine_mp/MPTerrainEngineOptions>
#include <osgEarth/Utils>
#include <osgEarthUtil/Sky>
#include <osgEarth/GeoTransform>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgViewer/ViewerEventHandlers>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgGA/StateSetManipulator>
#include "osg/Vec3d"
#include <osgEarthSymbology/Geometry>
#include <osgEarthSymbology/Style>
#include <osgEarthFeatures/Feature>
#include <osgEarthAnnotation/FeatureNode>
#include <osgViewer/ViewerBase>

#include <osgViewer/Viewer>
#include <osgUtil/Optimizer>

#include "Tool/GGEarthManipulator.h"

#include "WFSLayer.h"
#include "PlaceNameLayer.h"
#include "Analysis/LineTraker3DElement.h"
#include "HeightAnalysis.h"
#include "OsgCoreSingleton.h"

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Symbology;

GLOBE_NS

Earth::Earth(const GeoStar::Utility::GsConfig& config)
{
	m_Config = config;

	osg::Group* root = OsgCoreSingleton::Instance()->Root();
	m_Root = root;

	osgUtil::Optimizer optimizer;
	optimizer.optimize(root);

	osgEarth::MapOptions mapOptions;
	/// Justin
	mapOptions.elevationInterpolation() = osgEarth::ElevationInterpolation::INTERP_BILINEAR;///INTERP_GEO_BILINEAR;

	osgEarth::Map* map = new osgEarth::Map(mapOptions);

	osgEarth::Drivers::MPTerrainEngine::MPTerrainEngineOptions mpterEngineOption;

	mpterEngineOption.heightFieldSkirtRatio() = 0.5;
	//mpterop.heightFieldSampleRatio()=1.5;
	mpterEngineOption.minTileRangeFactor() = 3.5;
	mpterEngineOption.minLOD() = 10;
	osgEarth::MapNodeOptions mapNodeoptions;
	mapNodeoptions.setTerrainOptions(mpterEngineOption);
	osgEarth::MapNode* mapNode = new osgEarth::MapNode(map, mapNodeoptions);

	OsgCoreSingleton::Instance()->RegisteMapNode(mapNode);

	m_pMapNode = mapNode;
	//加载球皮
	GeoStar::Utility::GsConfig baseMap = m_Config["BaseMap"];
	std::string strValue = baseMap["DataType"].StringValue("Local");
	if (GeoStar::Utility::GsCRT::_stricmp(strValue.c_str(), "Local") == 0)
	{
		strValue = baseMap["Url"].StringValue("./world.tif").c_str();

		osgEarth::Drivers::GDALOptions basemapOpt;
		basemapOpt.url() = strValue;
		map->addLayer(new osgEarth::ImageLayer(osgEarth::ImageLayerOptions("basemap", basemapOpt)));
	}
	else
	{ 

	}

	//添加大气圈，星空背景、太阳

	osgEarth::Util::SkyNode *s_sky = osgEarth::Util::SkyNode::create(mapNode);
	//mapNode加入到sky的节点中，才可以融合出大气圈的效果。
	s_sky->addChild(mapNode);
	//sky节点放入root根节点中
	root->addChild(s_sky);
}

Earth::~Earth()
{

}

void Earth::Test(std::string strUrl)
{
	MAP_NODE(m_pMapNode);

	//osg::ref_ptr<PlaceNameLayerOptions> placeNameLayerOptions = new PlaceNameLayerOptions( strUrl, "simkai.ttf");
	//PlaceNameLayerPrivate * pPlaceNameLayer = new PlaceNameLayerPrivate(placeNameLayerOptions.get());
	////dynamic_cast<osg::Group*>(m_Root)->addChild(pPlaceNameLayer);
	//mapNode->addChild(pPlaceNameLayer);

	//Layer* ptrLayer = new PlaceNameLayer(strUrl,eLocal);
	//m_ptrLayerBox->AddLayer(ptrLayer);

	//osgEarth::HTTPResponse response = osgEarth::HTTPClient::get("https://www.baidu.com/");
	//std::string data = response.getPartAsString(0);
	//Layer* ptrLayer = new WFSLayer(strUrl, eWFS);
	//m_ptrLayerBox->AddLayer(ptrLayer);

	HeightAnalysis* heiget = new HeightAnalysis();
	m_ptrAnalysisBox->AddAnalysis(heiget);

	//GeoGlobe::LineTraker3DElement* ptrHeightAnalysis = new GeoGlobe::LineTraker3DElement(mapNode, GeoGlobe::LineTraker3DElement::DragMode::DRAGMODE_VERTICAL);
	//ptrHeightAnalysis->setCullingActive(false);
	//dynamic_cast<osg::Group*>(m_Root)->addChild(ptrHeightAnalysis);
}

osg::Node* Earth::RootNodePtr()
{
	return m_Root;
}

osg::Node* Earth::MapNodePtr()
{
	return m_pMapNode;
}

LayerBox* Earth::LayerBoxP()
{
	return m_ptrLayerBox.p;
}

SceneBox* Earth::SceneBoxP()
{
	return m_ptrSceneBox.p;
}

SymbolBox* Earth::SymbolBoxP()
{
	return m_ptrSymbolBox.p;
}

void Earth::Init(osgViewer::Viewer* pViewer)
{
	MAP_NODE(m_pMapNode);

	m_pViewer = OsgCoreSingleton::Instance()->OsgViewer();

	m_Manip = OsgCoreSingleton::Instance()->GGEarthManipulator();
	m_pViewer->setCameraManipulator(m_Manip);

	m_ptrSceneBox = new SceneBox();
	m_ptrLayerBox = new LayerBox();
	m_ptrSymbolBox = new SymbolBox();
	m_ptrAnalysisBox = new AnalysisBox();

	m_Root->setThreadSafeRefUnref(true);

	m_pViewer->setSceneData(m_Root);
	m_pViewer->getCamera()->addCullCallback(new AutoClipPlaneCullCallback(mapNode));

	osg::ref_ptr<osg::Light> light = new osg::Light;
	light->setLightNum(0);
	//light->setDirection(osg::Vec3(0,0,-1.0f));
	//light->setPosition(osg::Vec4(0,0,0.0f,1.0f));
	light->setAmbient(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
	//light->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	light->setDiffuse(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
	light->setSpecular(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));

	m_pViewer->setLight(light.get());
	m_pViewer->setLightingMode(osg::View::SKY_LIGHT);
	m_pViewer->getCamera()->setClearColor(osg::Vec4(0, 0, 0, 1));

	osg::ref_ptr<osgEarth::Util::Ephemeris> ep = new Ephemeris();

	osg::Vec3d sunPosition = ep->getSunPosition(osgEarth::DateTime(2017, 7, 10, 8)).geocentric;
	light->setPosition(osg::Vec4(sunPosition, 0.0f));

	m_pViewer->setKeyEventSetsDone(0);
	m_pViewer->addEventHandler(new osgViewer::StatsHandler());
	m_pViewer->addEventHandler(new osgViewer::WindowSizeHandler());
	m_pViewer->addEventHandler(new osgViewer::LODScaleHandler());
	m_pViewer->addEventHandler(new osgGA::StateSetManipulator(m_pViewer->getCamera()->getOrCreateStateSet()));
	m_pViewer->addEventHandler(new osgViewer::ThreadingHandler);
	m_pViewer->addEventHandler(new osgViewer::HelpHandler);
	

	m_pViewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

	m_pViewer->getDatabasePager()->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_MIN);
	m_pViewer->getDatabasePager()->setUpThreads(6, 3);
	m_pViewer->getDatabasePager()->setDatabasePagerThreadPause(false);
}

void Earth::PreFrameUpdate(void)
{

}
void Earth::PostFrameUpdate(void)
{

}

GLOBE_ENDNS