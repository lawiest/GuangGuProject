#pragma once
#include "sencegraph.h"   
#include <QWidget>
#include <QMouseEvent>
GLOBE_NS 
template<class T,class W>
class GsQTSenceGraph:public T, public W
{
public:
	GsQTSenceGraph(QWidget* parent = Q_NULLPTR, 
		Qt::WindowFlags f = Qt::WindowFlags()):W(parent,f)
	{

#ifdef _WIN32
		setAttribute(Qt::WA_PaintOnScreen);
#endif
		setAttribute(Qt::WA_NoSystemBackground);
		setFocusPolicy(Qt::StrongFocus);
	}

	virtual void* WindowHandle()
	{
		return (void*)W::winId();
	}
public:

	virtual QPaintEngine *paintEngine() const
	{
		return NULL;
	}
	virtual void resizeEvent(QResizeEvent *event)
	{
		int width = size().width();
		int height = size().height();
	}
	// mouse events
	virtual void mousePressEvent(QMouseEvent *event)
	{
		int button = 0;
		if (event->button() & Qt::LeftButton)
			button |= eLeftButton;
		if (event->button() & Qt::RightButton)
			button |= eRightButton;
		if (event->button() & Qt::MidButton)
			button |= eMiddleButton;
		OnMouseDown((GsButton)button);

		W::mousePressEvent(event);
	}
	virtual void mouseReleaseEvent(QMouseEvent *event)
	{
		int button = 0;
		if (event->button() & Qt::LeftButton)
			button |= eLeftButton;
		if (event->button() & Qt::RightButton)
			button |= eRightButton;
		if (event->button() & Qt::MidButton)
			button |= eMiddleButton;
		OnMouseUp((GsButton)button);

		W::mouseReleaseEvent(event);
	}
	virtual void mouseDoubleClickEvent(QMouseEvent *event)
	{
		int button = 0;
		if (event->button() & Qt::LeftButton)
			button |= eLeftButton;
		if (event->button() & Qt::RightButton)
			button |= eRightButton;
		if (event->button() & Qt::MidButton)
			button |= eMiddleButton;
		OnMouseDoubleClick((GsButton)button);

		W::mouseDoubleClickEvent(event);
	}
	virtual void mouseMoveEvent(QMouseEvent *event)
	{
		QPoint pos = event->pos();
		OnMouseMove(pos.x(), pos.y());
		W::mouseMoveEvent(event);
	}
	virtual void wheelEvent(QWheelEvent *event)
	{
		OnMouseWheel(event->delta());
		W::wheelEvent(event);
	}

	// keyboard events
	virtual void keyPressEvent(QKeyEvent *event)
	{
		OnKeyDown((GsKey)event->key());
		W::keyPressEvent(event);
	}
	virtual void keyReleaseEvent(QKeyEvent *event)
	{
		OnKeyUp((GsKey)event->key());
		W::keyReleaseEvent(event);
	}

	virtual void focusInEvent(QFocusEvent *event)
	{
		OnFocus();
		W::focusInEvent(event);
	}
	virtual void focusOutEvent(QFocusEvent *event)
	{
		OnFocusOut();
		W::focusOutEvent(event);
	}

	virtual bool eventFilter(QObject *obj, QEvent *event)
	{
		Q_UNUSED(obj);

		if (event->type() == QEvent::MouseMove)
			mouseMoveEvent(static_cast<QMouseEvent *>(event));

		return false;
	}
};
GLOBE_ENDNS