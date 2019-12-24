#include "qt/GsGeospace3DQt.h"
#include <osg/ref_ptr>
#include <osgEarth/Layer>
#include <QGridLayout>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgQt/GraphicsWindowQt>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgDB/ReadFile>
#include <osgEarthDrivers/engine_mp/MPTerrainEngineOptions>
//#include <osgEarthDrivers/engine_rex/RexTerrainEngineOptions>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarth/MapNode>
#include <osgEarthUtil/Ephemeris>
#include <osgEarthUtil/Sky>
#include <osgEarth/ImageLayer>
#include <osgGA/StateSetManipulator>
#include "LayerBox.h"
#include "SceneBox.h"
#include "AnalysisBox.h"
#include "ElementBox.h"
#include "FlyBox.h"
#include <QDir>
#include <GGEarthManipulator.h>
#include "OsgCoreSingleton.h"
#include <osgEarthUtil/AutoClipPlaneHandler>
#include "ElementBox.h"
#include "LonAndLatElement.h"
#include "CompassElement.h"
#include <osg/Image>
#include <osgDB/WriteFile>
#include <QThread>
#include "OSGWidget.h"
#include <osgEarth/GLUtils>
#include <osgEarth/PhongLightingEffect>

GLOBE_NS

osgViewer::GraphicsWindowEmbedded* createGraphicsWindow1(int x, int y, int w, int h, const std::string& name, bool windowDecoration)
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowName = name;
	traits->windowDecoration = windowDecoration;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = 8/*ds->getNumMultiSamples()*/;

	return new osgViewer::GraphicsWindowEmbedded(traits.get());
}
osg::ref_ptr<osg::Image> texture1 = nullptr;

GsGeospace3DQt::GsGeospace3DQt(QWidget* parent,Qt::WindowFlags f) : QWidget(parent, f)
{
	//setThreadingModel(threadingModel);

	// disable the default setting of viewer.done() by pressing Escape.
	//setKeyEventSetsDone(0);

	QWidget* widget1 = addViewWidget(createGraphicsWindow(0, 0, 100, 100));
	QGridLayout* grid = new QGridLayout;
	grid->addWidget(widget1, 0, 0);
	//grid->addWidget(widget2, 0, 1);
	//grid->addWidget(widget3, 1, 0);
	//grid->addWidget(widget4, 1, 1);
	grid->setSpacing(0);
	grid->setContentsMargins(0, 0, 1, 0);
	setLayout(grid);
	connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
	_timer.start(17);
}

void GsGeospace3DQt::Test(std::string strUrl)
{

}

void GsGeospace3DQt::setSrsInfo(const std::string& strSrs, const KERNEL_NAME::GsRawPoint3D& originPoint)
{
	OsgCoreSingleton::Instance()->setSrsInfor(strSrs,osg::Vec3d(originPoint.X, originPoint.Y, originPoint.Z));
}

//
//
//QWidget* GsGeospace3DQt::addViewWidget(osgQt::GraphicsWindowQt* gw)
//{
//	osgViewer::GraphicsWindowEmbedded* graphicwindow = createGraphicsWindow1(0, 0, this->width(), this->height(), "", false);
//	osgEarth::MapOptions mapOptions;
//	mapOptions.elevationInterpolation() = osgEarth::ElevationInterpolation::INTERP_BILINEAR;///INTERP_GEO_BILINEAR;
//																							//mapOptions.coordSysType() = MapOptions::CSTYPE_PROJECTED;
//																							//mapOptions.profile() = ProfileOptions("plate-carre");
//	osgEarth::Map* map = new osgEarth::Map(mapOptions);
//
//	osgEarth::Drivers::MPTerrainEngine::MPTerrainEngineOptions mpterEngineOption;
//
//	mpterEngineOption.heightFieldSkirtRatio() = 0.5;
//	//mpterop.heightFieldSampleRatio()=1.5;
//	mpterEngineOption.minTileRangeFactor() = 3.5;
//	mpterEngineOption.minLOD() = 10;
//	osgEarth::MapNodeOptions mapNodeoptions;
//	mapNodeoptions.setTerrainOptions(mpterEngineOption);
//
//	m_ptrMapNode = new osgEarth::MapNode(map, mapNodeoptions);
//
//	OsgCoreSingleton::Instance()->RegisteMapNode(m_ptrMapNode);
//
//	QString pstrTiffPath;
//	pstrTiffPath = QDir::currentPath() + "/" + "world.tif";
//
//	osgEarth::Drivers::GDALOptions basemapOpt;
//	basemapOpt.url() = pstrTiffPath.toLocal8Bit().toStdString();
//	map->addImageLayer(new osgEarth::ImageLayer(osgEarth::ImageLayerOptions("basemap", basemapOpt)));
//	m_ptrManip = OsgCoreSingleton::Instance()->GGEarthManipulator();
//
//	m_ptrViewer = OsgCoreSingleton::Instance()->OsgViewer();
//	//addView(m_ptrViewer);
//
//	osg::ref_ptr<osg::Light> light = new osg::Light;
//	light->setLightNum(0);
//	//light->setDirection(osg::Vec3(0,0,-1.0f));
//	//light->setPosition(osg::Vec4(0,0,0.0f,1.0f));
//	light->setAmbient(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
//	//light->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
//	light->setDiffuse(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
//	light->setSpecular(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
//
//	m_ptrViewer->setLight(light.get());
//	m_ptrViewer->setLightingMode(osg::View::SKY_LIGHT);
//	m_ptrViewer->getCamera()->setClearColor(osg::Vec4(0, 0, 0, 1));
//
//	osg::ref_ptr<osgEarth::Util::Ephemeris> ep = new osgEarth::Util::Ephemeris();
//
//	osg::Vec3d sunPosition = ep->getSunPositionECEF(osgEarth::DateTime(2017, 7, 10, 8));
//	light->setPosition(osg::Vec4(sunPosition, 0.0f));
//
//	m_ptrViewer->setThreadingModel(osgViewer::ViewerBase::ThreadingModel::SingleThreaded);
//	//m_ptrViewer->setThreadingModel(osgViewer::ViewerBase::ThreadingModel::DrawThreadPerContext);
//	//m_ptrViewer->startThreading();
//	m_ptrViewer->setCameraManipulator(m_ptrManip);
//	m_ptrViewer->setKeyEventSetsDone(0);
//	m_ptrViewer->addEventHandler(new osgViewer::StatsHandler());
//
//	//double a = m_ptrViewer->getEventHandlers().front();
//	m_ptrViewer->addEventHandler(new osgViewer::StatsHandler());
//	m_ptrViewer->addEventHandler(new osgViewer::WindowSizeHandler());
//	m_ptrViewer->addEventHandler(new osgViewer::LODScaleHandler());
//	m_ptrViewer->addEventHandler(new osgGA::StateSetManipulator(m_ptrViewer->getCamera()->getOrCreateStateSet()));
//	m_ptrViewer->addEventHandler(new osgViewer::ThreadingHandler);
//	m_ptrViewer->addEventHandler(new osgViewer::HelpHandler);
//	m_ptrViewer->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//	m_ptrViewer->getCamera()->setClearColor(osg::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
//	//view->getCamera()->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(this->width()) / static_cast<double>(this->height()), 1.0, 1000.0);
//
//	osg::Group* ptrRoot = OsgCoreSingleton::Instance()->Root();
//	//提供给RTT拾取
//	m_ptrMapNode->addChild(OsgCoreSingleton::Instance()->RTTPickGroup());
//	osgEarth::Util::SkyNode *s_sky = osgEarth::Util::SkyNode::create(m_ptrMapNode);
//	s_sky->addChild(m_ptrMapNode);
//	ptrRoot->addChild(s_sky);
//	ptrRoot->setThreadSafeRefUnref(true);
//	m_ptrViewer->setSceneData(ptrRoot);
//
//	m_ptrViewer->getCamera()->addCullCallback(new osgEarth::Util::AutoClipPlaneCullCallback(m_ptrMapNode));
//
//	osg::Camera* camera = m_ptrViewer->getCamera();
//	camera->setGraphicsContext(graphicwindow);
//
//	const osg::GraphicsContext::Traits* traits = graphicwindow->getTraits();
//
//	//camera->setClearColor(osg::Vec4(0.2, 0.2, 0.6, 1.0));
//	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
//	camera->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0f, 10000.0f);
//
//	GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
//	camera->setDrawBuffer(buffer);
//	camera->setReadBuffer(buffer);
//
//	texture1 = new osg::Image;
//	camera->attach(osg::Camera::COLOR_BUFFER, texture1);
//
//	m_ptrLayerBox = new GLOBE_NAME::LayerBox();
//	m_ptrScenceBox = new GLOBE_NAME::SceneBox();
//	m_ptrAnalysisBox = new GLOBE_NAME::AnalysisBox();
//	m_ptrFlyBox = new GLOBE_NAME::FlyBox();
//	m_ptrElementBox = new GLOBE_NAME::ElementBox();
//	OsgCoreSingleton::Instance()->RegisteElement(m_ptrElementBox);
//	OsgCoreSingleton::Instance()->RegisteLayerBox(m_ptrLayerBox);
//
//	LonAndLatElementPtr ptrLonAndLat = new LonAndLatElement();
//	m_ptrElementBox->AddElement(ptrLonAndLat);
//	OsgCoreSingleton::Instance()->RegisteLonAndLatElement(ptrLonAndLat);
//	CompassElementPtr ptrCompass = new CompassElement();
//	m_ptrElementBox->AddElement(ptrCompass);
//	//
//	OSGWidget* osgWidget = new OSGWidget(graphicwindow, m_ptrViewer, this);
//
//	return osgWidget;
//}


QWidget* GsGeospace3DQt::addViewWidget(osgQt::GraphicsWindowQt* gw)
{
	osgEarth::MapOptions mapOptions;
	mapOptions.elevationInterpolation() = osgEarth::ElevationInterpolation::INTERP_BILINEAR;///INTERP_GEO_BILINEAR;
	//mapOptions.coordSysType() = MapOptions::CSTYPE_PROJECTED;
	//mapOptions.profile() = ProfileOptions("plate-carre");
	osgEarth::Map* map = new osgEarth::Map();

	//目前先不换rex地形引擎，因为其自带的裁剪器对改引擎生成的地形支持不好，会裁减异常
	osgEarth::Drivers::MPTerrainEngine::MPTerrainEngineOptions mpterEngineOption;

	mpterEngineOption.heightFieldSkirtRatio() = 0.5;
	//mpterop.heightFieldSampleRatio()=1.5;
	mpterEngineOption.minTileRangeFactor() = 3.5;
	mpterEngineOption.minLOD() = 10;
	osgEarth::MapNodeOptions mapNodeoptions;
	mapNodeoptions.setTerrainOptions(mpterEngineOption);

	m_ptrMapNode = new osgEarth::MapNode(map, mapNodeoptions);

	OsgCoreSingleton::Instance()->RegisteMapNode(m_ptrMapNode);

	QString pstrTiffPath;
	pstrTiffPath = QDir::currentPath() + "/" + "world.tif";

	osgEarth::Drivers::GDALOptions basemapOpt;
	basemapOpt.url() = pstrTiffPath.toLocal8Bit().toStdString();
	map->addLayer(new osgEarth::ImageLayer(osgEarth::ImageLayerOptions("basemap", basemapOpt)));
	m_ptrManip = OsgCoreSingleton::Instance()->GGEarthManipulator();

	m_ptrViewer = OsgCoreSingleton::Instance()->OsgViewer();

	GLUtils::setGlobalDefaults(m_ptrViewer->getCamera()->getOrCreateStateSet());
	m_ptrViewer->setThreadingModel(osgViewer::ViewerBase::ThreadingModel::SingleThreaded);
	//m_ptrViewer->setThreadingModel(osgViewer::ViewerBase::ThreadingModel::DrawThreadPerContext);
	//m_ptrViewer->startThreading();
	m_ptrViewer->setCameraManipulator(m_ptrManip);
	m_ptrViewer->setKeyEventSetsDone(0);
	m_ptrViewer->addEventHandler(new osgViewer::StatsHandler());
	
	//double a = m_ptrViewer->getEventHandlers().front();
	m_ptrViewer->addEventHandler(new osgViewer::StatsHandler());
	m_ptrViewer->addEventHandler(new osgViewer::WindowSizeHandler());
	m_ptrViewer->addEventHandler(new osgViewer::LODScaleHandler());
	m_ptrViewer->addEventHandler(new osgGA::StateSetManipulator(m_ptrViewer->getCamera()->getOrCreateStateSet()));
	m_ptrViewer->addEventHandler(new osgViewer::ThreadingHandler);
	m_ptrViewer->addEventHandler(new osgViewer::HelpHandler);
	m_ptrViewer->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	m_ptrViewer->getCamera()->setClearColor(osg::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	//view->getCamera()->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(this->width()) / static_cast<double>(this->height()), 1.0, 1000.0);

	osg::Group* ptrRoot = OsgCoreSingleton::Instance()->Root();
	//提供给RTT拾取
	m_ptrMapNode->addChild(OsgCoreSingleton::Instance()->RTTPickGroup());
	osgEarth::Util::SkyNode *s_sky = osgEarth::Util::SkyNode::create(m_ptrMapNode);

	osg::ref_ptr<osgEarth::Util::Ephemeris> ep = new osgEarth::Util::Ephemeris();
	osg::Vec3d sunPosition = ep->getSunPosition(osgEarth::DateTime(2019, 7, 10, 7)).geocentric;
	s_sky->getSunLight()->setPosition(osg::Vec4(sunPosition, 0.0f));
	s_sky->addChild(m_ptrMapNode);
	ptrRoot->addChild(s_sky);
	ptrRoot->setThreadSafeRefUnref(true);

	//PhongLightingEffect* phong = new PhongLightingEffect();
	//phong->attach(m_ptrMapNode->getOrCreateStateSet());

	m_ptrViewer->setSceneData(ptrRoot);
	m_ptrViewer->getCamera()->addCullCallback(new osgEarth::Util::AutoClipPlaneCullCallback(m_ptrMapNode));
	m_ptrViewer->getCamera()->setSmallFeatureCullingPixelSize(-1.0f);

	osg::Camera* camera = m_ptrViewer->getCamera();
	camera->setGraphicsContext(gw);

	const osg::GraphicsContext::Traits* traits = gw->getTraits();

	//camera->setClearColor(osg::Vec4(0.2, 0.2, 0.6, 1.0));
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0f, 10000.0f);

	//GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	//camera->setDrawBuffer(buffer);
	//camera->setReadBuffer(buffer);

	//texture1 = new osg::Image;
	//camera->attach(osg::Camera::COLOR_BUFFER, texture1);

	m_ptrLayerBox = new GLOBE_NAME::LayerBox();
	m_ptrScenceBox = new GLOBE_NAME::SceneBox();
	m_ptrAnalysisBox = new GLOBE_NAME::AnalysisBox();
	m_ptrFlyBox = new GLOBE_NAME::FlyBox();
	m_ptrElementBox = new GLOBE_NAME::ElementBox();
	OsgCoreSingleton::Instance()->RegisteElement(m_ptrElementBox);
	OsgCoreSingleton::Instance()->RegisteLayerBox(m_ptrLayerBox);

	LonAndLatElementPtr ptrLonAndLat = new LonAndLatElement();
	m_ptrElementBox->AddElement(ptrLonAndLat);
	OsgCoreSingleton::Instance()->RegisteLonAndLatElement(ptrLonAndLat);
	CompassElementPtr ptrCompass = new CompassElement();
	m_ptrElementBox->AddElement(ptrCompass);

	gw->setTouchEventsEnabled(true);
	return gw->getGLWidget();
}

void GsGeospace3DQt::CaptureScreen(std::string strPicture)
{
	if (!texture1)
		return;

	std::string strExt = osgDB::getFileExtension(strPicture);
	if (strExt == "jpg" || strExt == "jpeg")
		texture1->setPixelFormat(GL_RGB);
	else
		texture1->setPixelFormat(GL_RGBA);

	osgDB::writeImageFile(*(texture1.get()), strPicture);
}

osgQt::GraphicsWindowQt* GsGeospace3DQt::createGraphicsWindow(int x, int y, int w, int h, const std::string& name, bool windowDecoration)
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowName = name;
	traits->windowDecoration = windowDecoration;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = 8/*ds->getNumMultiSamples()*/;

	return new osgQt::GraphicsWindowQt(traits.get());
}

LayerBox* GsGeospace3DQt::LayerBox()
{
	return m_ptrLayerBox;
}

SceneBox* GsGeospace3DQt::SceneBox()
{
	return m_ptrScenceBox;
}

AnalysisBox* GsGeospace3DQt::AnalysisBox()
{
	return m_ptrAnalysisBox;
}

FlyBox* GsGeospace3DQt::FlyBox()
{
	return m_ptrFlyBox;
}
ElementBox* GsGeospace3DQt::ElementBox()
{
	return m_ptrElementBox;
}

SymbolBox * GsGeospace3DQt::SymbolBox()
{
	return m_ptrSymbolBox;
}

void GsGeospace3DQt::paintEvent(QPaintEvent* event)
{
	m_ptrViewer->frame();
}



GLOBE_ENDNS