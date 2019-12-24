#pragma once
#include <QPoint>
#include <QOpenGLWidget>

#include <osg/ref_ptr>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QMatrix4x4>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>
namespace osgWidget
{
	//! The subclass of osgViewer::CompositeViewer we use
	/*!
	* This subclassing allows us to remove the annoying automatic
	* setting of the CPU affinity to core 0 by osgViewer::ViewerBase,
	* osgViewer::CompositeViewer's base class.
	*/
	class Viewer : public osgViewer::CompositeViewer
	{
	public:
		virtual void setupThreading();
	};
}
class OSGWidget;
class Renderer : public QObject, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	Renderer(OSGWidget *w);
	void lockRenderer() { m_renderMutex.lock(); }
	void unlockRenderer() { m_renderMutex.unlock(); }
	QMutex *grabMutex() { return &m_grabMutex; }
	QWaitCondition *grabCond() { return &m_grabCond; }
	void prepareExit() { m_exiting = true; m_grabCond.wakeAll(); }

signals:
	void contextWanted();
public :
	void render();

private:

	bool m_inited;
	qreal m_fAngle;
	qreal m_fScale;
	QVector<QVector3D> vertices;
	QVector<QVector3D> normals;
	int vertexAttr;
	int normalAttr;
	int matrixUniform;
	OSGWidget *m_glwidget;
	QMutex m_renderMutex;
	QElapsedTimer m_elapsed;
	QMutex m_grabMutex;
	QWaitCondition m_grabCond;
	bool m_exiting;
};
// openg 组件信息过程
class OSGWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	OSGWidget(osgViewer::GraphicsWindowEmbedded* graphic,osgViewer::View* view,QWidget* parent = 0,
		Qt::WindowFlags f = 0);

	virtual ~OSGWidget();

protected:

	virtual void paintEvent(QPaintEvent* paintEvent);
	virtual void paintGL();
	virtual void resizeGL(int width, int height);

	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);

	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);
	virtual void wheelEvent(QWheelEvent* event);

	virtual bool event(QEvent* event);

public:

	virtual void onHome();
	virtual void onResize(int width, int height);

	osgGA::EventQueue* getEventQueue() const;

	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow_;
	osg::ref_ptr<osgWidget::Viewer> viewer_;


	void processSelection();
	//
	//
signals:
	void renderRequested();

	public slots:
	void grabContext();

	private slots:
	void onAboutToCompose();
	void onFrameSwapped();
	void onAboutToResize();
	void onResized();
private:
	QThread *m_thread;
	Renderer *m_renderer;
	bool mIsFirstRenderRun;
};
