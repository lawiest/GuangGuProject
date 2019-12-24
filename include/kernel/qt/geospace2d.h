#pragma once
#include <QtCore/qglobal.h>
#include <QWidget>
#include <QPainter>
#include <map.h>
#include <QMouseEvent>
#include <QWindow>
#include "../../utility/preconfig.h"
#include <QtGui>
#include <QSharedPointer>
#include <QImage>
KERNEL_NS
namespace QT{

class GS_API GsMapMouseEvent:public QMouseEvent
{
    double m_dblMapX,m_dblMapY;
public:
    GsMapMouseEvent(Type type, const QPointF &localPos, Qt::MouseButton button,
                Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers,double mapx,double mapy);
    double MapX()const;
    double MapY()const;
};

class GS_API GsScreenBox:public Utility::GsRefObject
{
protected:
    GsScreenBox();
public:
    virtual ~GsScreenBox();

    /// \brief 当前Tracker
    virtual GsSymbolTracker* CurrentTracker() = 0;

    /// \brief 当前Tracker
    virtual void  CurrentTracker(GsSymbolTracker* t) = 0;
    /// \brief 开始Tracker
    virtual GsSymbolTracker* PushTracker(GsSymbolTracker* t) = 0;
    /// \brief 弹出Tracker
   virtual  GsSymbolTrackerPtr PopTracker() = 0;

};

/// \brief GsScreenBoxPtr
GS_SMARTER_PTR(GsScreenBox);

class GS_API GsGeoSpace2D:public QWindow
{
    Q_OBJECT
    GsScreenBoxPtr m_ptrScreenBox;
    QBackingStore m_backingStore;
    GsMapPtr m_ptrMap;
    bool m_bInner;

    GsSymbolTracker::GsTrackerFeedBack OnQueryByEnvelope(GsGeometry* geo);
    GsSymbolTracker::GsTrackerFeedBack OnSelectByEnvelope(GsGeometry* geo);
    GsSymbolTracker::GsTrackerFeedBack OnZoomIn(GsGeometry* geo);
    GsSymbolTracker::GsTrackerFeedBack OnZoomOut(GsGeometry* geo);
    void OnZoomPan(const GsBox& box);

    QWidget* m_Widget;
public:
    GsGeoSpace2D(QWindow* parent = 0);
    ~GsGeoSpace2D();
    QWidget* Widget();
    void Widget(QWidget* w);
public:

    GsScreenBox* ScreenBox();
public:

    GsMap* Map();
    void Update();
    void QueryByEnvelope();
    void SelectByEnvelope();
    void ZoomInEnv();
    void ZoomOutEnv();
    void ZoomPan();
    void AddLayer(GsLayer* pLyr);
    void RemoveLayer(GsLayer* pLyr);
    void ClearLayers();
    void ViewExtent(const GsBox& box);
    void DirtyMap();
protected:
    virtual bool event(QEvent *event) Q_DECL_OVERRIDE;

    virtual void resizeEvent(QResizeEvent *resizeEvent) Q_DECL_OVERRIDE;
    virtual void exposeEvent(QExposeEvent *event) Q_DECL_OVERRIDE;

    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void touchEvent(QTouchEvent *);
signals:
   void MouseDown(const GsMapMouseEvent& event);
   void MouseMove(const GsMapMouseEvent& event);
   void MouseUp(const GsMapMouseEvent& event);
   void QueryByGeometry(GsGeometry* geo);
   void LayerChanged();

};

/// \brief Lite map
class GS_API GsGeoSpaceLite2D:public QWidget
{
    GeoStar::Kernel::GsMapPtr m_ptrMap;
    QSharedPointer<QImage> m_ptrCanvas;
    GeoStar::Kernel::GsPaintDevicePtr m_ptrPaintDevice;
private:
    void OnAfterFlush(GsDisplay* disp, GsCanvas* canvas);
public:
    GsGeoSpaceLite2D(QWidget* parent);
    ~GsGeoSpaceLite2D();
    GsMap* MainMap();

protected:
    virtual void resizeEvent(QResizeEvent *resizeEvent) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

}

KERNEL_ENDNS
