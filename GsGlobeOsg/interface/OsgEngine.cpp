#include "OSGEngine.h"
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgViewer/Viewer>

GLOBE_NS

Gs3DEngine::~Gs3DEngine()
{

}

osgViewer::Viewer* OsgEngine::Viewer()
{
	return m_Viewer;
}

void OsgEngine::InitOSG()
{
	m_ptrEarth = new Earth(m_Config);

	// Create the viewer for this window
	m_Viewer = new osgViewer::Viewer();
	// Add a Stats Handler to the viewer
	//m_Viewer->addEventHandler(new osgViewer::StatsHandler);
	// Init the GraphicsContext Traits
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

#ifdef _WIN32
	// Local Variable to hold window size data
	RECT rect;
	// Get the current window size
	::GetWindowRect(m_hWnd, &rect);
	// Init the Windata Variable that holds the handle for the Window to display OSG in.
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

#endif
	// Setup the traits parameters
	traits->x = 0;
	traits->y = 0;
	traits->width = rect.right - rect.left;
	traits->height = rect.bottom - rect.top;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	traits->inheritedWindowData = windata;
	traits->samples = 8;
	// Create the Graphics Context
	osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	// Init Master Camera for this View
	osg::ref_ptr<osg::Camera> camera = m_Viewer->getCamera();

	// Assign Graphics Context to the Camera
	camera->setGraphicsContext(gc);
	/**/
	// Set the viewport for the Camera
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));

	// Set projection matrix and camera attribtues
	camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	camera->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));
	//fovy改成45度，和explorer保持一致，效果相同方便比较，也利于以后调试。
	camera->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0, 1000.0);
	// Add the Camera to the Viewer
	m_Viewer->setCamera(camera.get());

	// Add the Camera Manipulator to the Viewer
	//m_Viewer->setCameraManipulator(m_ptrKeyswitchManipulator.get());
	m_ptrEarth->Init(m_Viewer);
}

void OsgEngine::PreFrameUpdate(void)
{
	m_ptrEarth->PreFrameUpdate();
}
void OsgEngine::PostFrameUpdate(void)
{
	m_ptrEarth->PostFrameUpdate();
}

OsgEngine::OsgEngine(void* handle, const Utility::GsConfig& config)
{
	m_Config = config;
	m_Done = false;
#ifdef _WIN32
	m_hWnd = (HWND)handle;
#endif	
	InitOSG();
	Frame();
}

EarthPtr OsgEngine::GetEarth()
{
	return m_ptrEarth.p;
}

void OsgEngine::Close()
{
	//m_Done = true;
	//if (isRunning())
	//{
	//	cancel();
	//	join();
	//}
}

void OsgEngine::run()
{
	//do
	//{
	//	PreFrameUpdate();
	//	m_Viewer->run();
	//	PostFrameUpdate();
	//} while (!testCancel() && !m_Viewer->done() && !m_Done);
}

void OsgEngine::Frame()
{
	m_Viewer->frame();
}

OsgEngine::~OsgEngine()
{

}

GLOBE_ENDNS