#include "OSGWidget.h"
//#include "PickHandler.h"
#include <QGuiApplication>
#include <osg/Camera>

#include <osg/DisplaySettings>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/StateSet>

#include <osgDB/WriteFile>

#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>

#include <cassert>

#include <stdexcept>
#include <vector>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>

namespace
{

#ifdef WITH_SELECTION_PROCESSING
	QRect makeRectangle(const QPoint& first, const QPoint& second)
	{
		// Relative to the first point, the second point may be in either one of the
		// four quadrants of an Euclidean coordinate system.
		//
		// We enumerate them in counter-clockwise order, starting from the lower-right
		// quadrant that corresponds to the default case:
		//
		//            |
		//       (3)  |  (4)
		//            |
		//     -------|-------
		//            |
		//       (2)  |  (1)
		//            |

		if (second.x() >= first.x() && second.y() >= first.y())
			return QRect(first, second);
		else if (second.x() < first.x() && second.y() >= first.y())
			return QRect(QPoint(second.x(), first.y()), QPoint(first.x(), second.y()));
		else if (second.x() < first.x() && second.y() < first.y())
			return QRect(second, first);
		else if (second.x() >= first.x() && second.y() < first.y())
			return QRect(QPoint(first.x(), second.y()), QPoint(second.x(), first.y()));

		// Should never reach that point...
		return QRect();
	}
#endif

}

namespace osgWidget
{
	void Viewer::setupThreading()
	{
		if (_threadingModel == SingleThreaded)
		{
			if (_threadsRunning)
				stopThreading();
		}
		else
		{
			if (!_threadsRunning)
				startThreading();
		}
	}
}

OSGWidget::OSGWidget(osgViewer::GraphicsWindowEmbedded* graphic, osgViewer::View* view, QWidget* parent,
	Qt::WindowFlags f)
	: QOpenGLWidget(parent,
		f)
	, graphicsWindow_(graphic)
	, viewer_(new osgWidget::Viewer)
	,mIsFirstRenderRun(true)
{
	viewer_->addView(view);
	viewer_->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
	viewer_->realize();

	// This ensures that the widget will receive keyboard events. This focus
	// policy is not set by default. The default, Qt::NoFocus, will result in
	// keyboard events that are ignored.
	this->setFocusPolicy(Qt::StrongFocus);
	this->setMinimumSize(100, 100);

	// Ensures that the widget receives mouse move events even though no
	// mouse button has been pressed. We require this in order to let the
	// graphics window switch viewports properly.
	this->setMouseTracking(true);
	//后台线程模式
	//connect(this, &QOpenGLWidget::aboutToCompose, this, &OSGWidget::onAboutToCompose);
	//connect(this, &QOpenGLWidget::frameSwapped, this, &OSGWidget::onFrameSwapped);
	//connect(this, &QOpenGLWidget::aboutToResize, this, &OSGWidget::onAboutToResize);
	//connect(this, &QOpenGLWidget::resized, this, &OSGWidget::onResized);

	//m_thread = new QThread;
	//m_renderer = new Renderer(this);
	//m_renderer->moveToThread(m_thread);
	//connect(m_thread, &QThread::finished, m_renderer, &QObject::deleteLater);

	//connect(this, &OSGWidget::renderRequested, m_renderer, &Renderer::render);
	//connect(m_renderer, &Renderer::contextWanted, this, &OSGWidget::grabContext);

	//m_thread->start();
}

OSGWidget::~OSGWidget()
{
	//m_renderer->prepareExit();
	//m_thread->quit();
	//m_thread->wait();
	//delete m_thread;
}

void OSGWidget::paintEvent(QPaintEvent* /* paintEvent */)
{
	this->makeCurrent();

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	this->paintGL();

#ifdef WITH_SELECTION_PROCESSING
	if (selectionActive_ && !selectionFinished_)
	{
		painter.setPen(Qt::black);
		painter.setBrush(Qt::transparent);
		painter.drawRect(makeRectangle(selectionStart_, selectionEnd_));
	}
#endif

	painter.end();

	this->doneCurrent();
}

void OSGWidget::paintGL()
{
	if (mIsFirstRenderRun)
	{
		// set set the default FBO-id. 
		// this id will be used when the rendering-backend is finished with RTT FBOs 
		GLuint defaultFboId = this->defaultFramebufferObject();
		//mpGraphicsContext->setDefaultFboId(defaultFboId);
		graphicsWindow_->setDefaultFboId(defaultFboId);
		mIsFirstRenderRun = false;
	}
	viewer_->frame();
	QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void OSGWidget::resizeGL(int width, int height)
{
	this->getEventQueue()->windowResize(this->x(), this->y(), width, height);
	graphicsWindow_->resized(this->x(), this->y(), width, height);

	this->onResize(width, height);
}

void OSGWidget::keyPressEvent(QKeyEvent* event)
{
	QString keyString = event->text();
	const char* keyData = keyString.toLocal8Bit().data();

	if (event->key() == Qt::Key_S)
	{
#ifdef WITH_SELECTION_PROCESSING
		selectionActive_ = !selectionActive_;
#endif

		// Further processing is required for the statistics handler here, so we do
		// not return right away.
	}
	else if (event->key() == Qt::Key_D)
	{
		return;
	}
	else if (event->key() == Qt::Key_H)
	{
		this->onHome();
		return;
	}

	this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KeySymbol(*keyData));
}

void OSGWidget::keyReleaseEvent(QKeyEvent* event)
{
	QString keyString = event->text();
	const char* keyData = keyString.toLocal8Bit().data();

	this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*keyData));
}

void OSGWidget::mouseMoveEvent(QMouseEvent* event)
{
	// Note that we have to check the buttons mask in order to see whether the
	// left button has been pressed. A call to `button()` will only result in
	// `Qt::NoButton` for mouse move events.
	{
		auto pixelRatio = this->devicePixelRatio();

		this->getEventQueue()->mouseMotion(static_cast<float>(event->x() * pixelRatio),
			static_cast<float>(event->y() * pixelRatio));
	}
}

void OSGWidget::mousePressEvent(QMouseEvent* event)
{

	{
		// 1 = left mouse button
		// 2 = middle mouse button
		// 3 = right mouse button

		unsigned int button = 0;

		switch (event->button())
		{
		case Qt::LeftButton:
			button = 1;
			break;

		case Qt::MiddleButton:
			button = 2;
			break;

		case Qt::RightButton:
			button = 3;
			break;

		default:
			break;
		}

		auto pixelRatio = this->devicePixelRatio();

		this->getEventQueue()->mouseButtonPress(static_cast<float>(event->x() * pixelRatio),
			static_cast<float>(event->y() * pixelRatio),
			button);
	}
}

void OSGWidget::mouseReleaseEvent(QMouseEvent* event)
{

	{
		// 1 = left mouse button
		// 2 = middle mouse button
		// 3 = right mouse button

		unsigned int button = 0;

		switch (event->button())
		{
		case Qt::LeftButton:
			button = 1;
			break;

		case Qt::MiddleButton:
			button = 2;
			break;

		case Qt::RightButton:
			button = 3;
			break;

		default:
			break;
		}

		auto pixelRatio = this->devicePixelRatio();

		this->getEventQueue()->mouseButtonRelease(static_cast<float>(pixelRatio * event->x()),
			static_cast<float>(pixelRatio * event->y()),
			button);
	}
}
void OSGWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
	int button = 0;
	switch (event->button())
	{
	case Qt::LeftButton: button = 1; break;
	case Qt::MidButton: button = 2; break;
	case Qt::RightButton: button = 3; break;
	case Qt::NoButton: button = 0; break;
	default: button = 0; break;
	}
	int pixelRatio = this->devicePixelRatio();
	this->getEventQueue()->mouseDoubleButtonPress(event->x()*pixelRatio, event->y()*pixelRatio, button);
}
void OSGWidget::wheelEvent(QWheelEvent* event)
{
	// Ignore wheel events as long as the selection is active.

	event->accept();
	int delta = event->delta();

	osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ? osgGA::GUIEventAdapter::SCROLL_UP
		: osgGA::GUIEventAdapter::SCROLL_DOWN;

	this->getEventQueue()->mouseScroll(motion);
}

bool OSGWidget::event(QEvent* event)
{
	bool handled = QOpenGLWidget::event(event);

	// This ensures that the OSG widget is always going to be repainted after the
	// user performed some interaction. Doing this in the event handler ensures
	// that we don't forget about some event and prevents duplicate code.
	switch (event->type())
	{
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove:
	case QEvent::Wheel:
		this->update();
		break;

	default:
		break;
	}

	return handled;
}

void OSGWidget::onHome()
{
	osgViewer::ViewerBase::Views views;
	viewer_->getViews(views);

	for (std::size_t i = 0; i < views.size(); i++)
	{
		osgViewer::View* view = views.at(i);
		view->home();
	}
}

void OSGWidget::onResize(int width, int height)
{
	std::vector<osg::Camera*> cameras;
	viewer_->getCameras(cameras);

	assert(cameras.size() >= 1);

	auto pixelRatio = this->devicePixelRatio();

	cameras[0]->setViewport(0, 0, width  * pixelRatio, height * pixelRatio);
	//cameras[1]->setViewport(width / 2 * pixelRatio, 0, width / 2 * pixelRatio, height * pixelRatio);
}

osgGA::EventQueue* OSGWidget::getEventQueue() const
{
	osgGA::EventQueue* eventQueue = graphicsWindow_->getEventQueue();

	if (eventQueue)
		return eventQueue;
	else
		throw std::runtime_error("Unable to obtain valid event queue");
}

void OSGWidget::processSelection()
{
#ifdef WITH_SELECTION_PROCESSING
	QRect selectionRectangle = makeRectangle(selectionStart_, selectionEnd_);
	auto widgetHeight = this->height();
	auto pixelRatio = this->devicePixelRatio();

	double xMin = selectionRectangle.left();
	double xMax = selectionRectangle.right();
	double yMin = widgetHeight - selectionRectangle.bottom();
	double yMax = widgetHeight - selectionRectangle.top();

	xMin *= pixelRatio;
	yMin *= pixelRatio;
	xMax *= pixelRatio;
	yMax *= pixelRatio;

	osgUtil::PolytopeIntersector* polytopeIntersector
		= new osgUtil::PolytopeIntersector(osgUtil::PolytopeIntersector::WINDOW,
			xMin, yMin,
			xMax, yMax);

	// This limits the amount of intersections that are reported by the
	// polytope intersector. Using this setting, a single drawable will
	// appear at most once while calculating intersections. This is the
	// preferred and expected behaviour.
	polytopeIntersector->setIntersectionLimit(osgUtil::Intersector::LIMIT_ONE_PER_DRAWABLE);

	osgUtil::IntersectionVisitor iv(polytopeIntersector);

	for (unsigned int viewIndex = 0; viewIndex < viewer_->getNumViews(); viewIndex++)
	{
		qDebug() << "View index:" << viewIndex;

		osgViewer::View* view = viewer_->getView(viewIndex);

		if (!view)
			throw std::runtime_error("Unable to obtain valid view for selection processing");

		osg::Camera* camera = view->getCamera();

		if (!camera)
			throw std::runtime_error("Unable to obtain valid camera for selection processing");

		camera->accept(iv);

		if (!polytopeIntersector->containsIntersections())
			continue;

		auto intersections = polytopeIntersector->getIntersections();

		for (auto&& intersection : intersections)
			qDebug() << "Selected a drawable:" << QString::fromStdString(intersection.drawable->getName());
	}
#endif
}

void OSGWidget::grabContext()
{
	m_renderer->lockRenderer();
	QMutexLocker lock(m_renderer->grabMutex());
	context()->moveToThread(m_thread);
	m_renderer->grabCond()->wakeAll();
	m_renderer->unlockRenderer();
}

void OSGWidget::onAboutToCompose()
{
	m_renderer->lockRenderer();
}

void OSGWidget::onFrameSwapped()
{
	m_renderer->unlockRenderer();
	// Assuming a blocking swap, our animation is driven purely by the
	// vsync in this example.
	emit renderRequested();
}

void OSGWidget::onAboutToResize()
{
	m_renderer->lockRenderer();
}

void OSGWidget::onResized()
{
	m_renderer->unlockRenderer();
}


Q_GLOBAL_STATIC(QMutex, initMutex)
Renderer::Renderer(OSGWidget *w) : m_inited(false),
m_glwidget(w),
m_exiting(false) {

}
void Renderer::render()
{
	if (m_exiting)
		return;

	QOpenGLContext *ctx = m_glwidget->context();

	if (!ctx) // QOpenGLWidget not yet initialized
		return;
	QSurfaceFormat format;
	format.setVersion(2, 1);
	format.setOption(QSurfaceFormat::DeprecatedFunctions, true);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setSamples(8);
	ctx->setFormat(format);
	// Grab the context.
	m_grabMutex.lock();
	emit contextWanted();
	m_grabCond.wait(&m_grabMutex);
	QMutexLocker lock(&m_renderMutex);
	m_grabMutex.unlock();

	if (m_exiting)
		return;

	Q_ASSERT(ctx->thread() == QThread::currentThread());

	// Make the context (and an offscreen surface) current for this thread. The
	// QOpenGLWidget's fbo is bound in the context.
	m_glwidget->makeCurrent();

	if (!m_inited) {
		m_inited = true;
		initializeOpenGLFunctions();

		QMutexLocker initLock(initMutex());
	

		m_elapsed.start();
		// set set the default FBO-id. 
		// this id will be used when the rendering-backend is finished with RTT FBOs 
		GLuint defaultFboId = m_glwidget->defaultFramebufferObject();
		//mpGraphicsContext->setDefaultFboId(defaultFboId);
		m_glwidget->graphicsWindow_->setDefaultFboId(defaultFboId);
	}
	m_glwidget->viewer_->frame();
	// Make no context current on this thread and move the QOpenGLWidget's
	// context back to the gui thread.
	m_glwidget->doneCurrent();
	ctx->moveToThread(qGuiApp->thread());

	// Schedule composition. Note that this will use QueuedConnection, meaning
	// that update() will be invoked on the gui thread.
	QMetaObject::invokeMethod(m_glwidget, "update");
}
