/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2017 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#ifndef QTN_OFFICEFRAMEHELPER_WIN_H
#define QTN_OFFICEFRAMEHELPER_WIN_H

#include <shobjidl.h>

#include <QObject>
#include <QSize>
#include <QRect>
#include <QTimer>
#include <QMap>
#include <qt_windows.h>


#include "QtnOfficeFrameHelper.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE


class AdsManagerHook
{
public:
    AdsManagerHook();
    virtual ~AdsManagerHook();
public:
    virtual int onHookMessage(HWND hWnd, uint nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult) = 0;
public:
    bool m_autoDestroy;
};

class HookManager
{
public:
    static uint m_msgSinkRemoved;
    class HookSink;

protected:
    HookManager();
public:
    ~HookManager();
    static HookManager* qtnHookManager();
public:
    void setHook(HWND hWnd, AdsManagerHook* hook);
    void removeHook(HWND hWnd, AdsManagerHook* hook);
    void removeAll(HWND hWnd);
    void removeAll(AdsManagerHook* hook);
    HookSink* lookup(HWND hWnd );

public:
    LRESULT defaultProc();
    LRESULT defaultProc(WPARAM wParam, LPARAM lParam);
    LRESULT defaultProc(HWND hWnd, uint message, WPARAM wParam, LPARAM lParam);

private:
    static LRESULT CALLBACK hookWndProc(HWND hWnd, uint message, WPARAM wParam, LPARAM lParam);

private:
    void removeAll();
    void removeAll(HWND hWnd, bool bLastMessage);

private:
    QMap<HWND, LPVOID> m_mapHooks;
    MSG m_lastSentMsg;
private:
    friend class HookSink;
    Q_DISABLE_COPY(HookManager)
};


class StyleOptionFrame;
class RibbonBar;
class OfficeFrameHelperWin : public QObject,
                             public OfficeFrameHelper,
                             public AdsManagerHook
{
    Q_OBJECT
public:
    OfficeFrameHelperWin(QWidget* parent = Q_NULL);
    virtual ~OfficeFrameHelperWin();

    static UINT m_msgGetFrameHook;
public:
    virtual void enableOfficeFrame(QWidget* parent);
    void disableOfficeFrame();

    virtual bool isDwmEnabled() const;
    virtual bool isActive() const;
    virtual bool isMaximize() const;
    virtual bool canHideTitle() const;

    virtual int frameBorder() const;
    virtual int titleBarSize() const;
    enum VistaState { VistaAero, VistaBasic, Classic, Dirty };
    VistaState vistaState();
    static int titleBarSizeDp() { return OfficeFrameHelperWin::frameSizeDp() + OfficeFrameHelperWin::captionSizeDp(); }

    virtual int tabBarHeight() const;
    virtual bool clientMetrics(int& iCaptionWidth, int& iCaptionHeight) const;

    virtual void setHeaderRect(const QRect& rcHeader);
    virtual void setContextHeader(QList<ContextHeader*>* listContextHeaders);

    virtual QPixmap getFrameSmallIcon() const;
    virtual QString getSystemMenuString(uint item) const;
    virtual QString getWindowText() const;

    virtual void drawTitleBar(QPainter* painter, const StyleOptionTitleBar& opt);
    virtual void drawIcon(QPainter* painter, const QIcon& icon, const QRect& rect);
    virtual void fillSolidRect(QPainter* painter, const QRect& rect, QColor clr);
    virtual void drawDwmCaptionText(QPainter* painter, const QRect& rect, const QString& strWindowText, const QColor& colText, bool active, bool contextTab);
    bool getCaptionQtnFont(int dpi, QFont *result);
    virtual QSize sizeSystemIcon(const QIcon& icon, const QRect& rect) const;
    virtual HICON windowIcon(const QIcon& icon) const;
    virtual void resetWindowIcon();
    virtual void recalcFrameLayout();
    virtual void postRecalcFrameLayout();
    virtual void processClickedSubControl(QStyle::SubControl subControl);

public:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    void collapseTopFrame();
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    virtual bool winEvent(MSG* message, long* result);
    virtual int onHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

private:
    static int glowSize();

    static int frameSize() { return OfficeFrameHelperWin::frameSizeDp() / OfficeFrameHelperWin::m_devicePixelRatio; }
    static int frameSizeDp();
    int topOffset();

    virtual int iconSize() const;
    static int captionSize() { return captionSizeDp() / OfficeFrameHelperWin::m_devicePixelRatio; }
    static int captionSizeDp() { return GetSystemMetrics(SM_CYCAPTION); }

protected:
    DWORD getStyle(bool exStyle = false) const;
    void refreshFrameStyle(bool redraw = true);
    void updateFrameRegion();
    void updateFrameRegion(const QSize& szFrameRegion, bool bUpdate);
    void updateShellAutohideBars();
    int shellAutohideBars();

    bool isMDIMaximized() const;
    bool isTitleVisible() const;
    bool isTheme2013() const;
    bool isTheme2016() const;
    void initStyleOption(StyleOptionFrame* option);
    void redrawFrame();
    bool modifyStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags);
    bool isFrameHasStatusBar(int* statusHeight = Q_NULL) const;
    HRGN calcFrameRegion(QSize sz);
    LOGFONT getCaptionLogFont(HANDLE hTheme);
    HFONT getCaptionFont(HANDLE hTheme);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    HDC backingStoreDC(const QWidget* wd);
#endif
    bool hitTestContextHeaders(const QPoint& point) const;
    bool hitTestBackstageHeaders(const QPoint& point) const;

    bool isCompositionEnabled();
    bool isSmallSystemBorders();
    void enableWindowAero(HWND hwnd, bool enable);
    void enableWindowShadow(HWND hwnd, bool enable);

    bool setWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
    bool resolveSymbols();
    void setFrameHook();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    void saveMargins();
    void restoreMargins();
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

protected Q_SLOTS:
    void refreshFrameTimer();

protected:
    virtual bool event(QEvent* event);
    virtual bool eventFilter(QObject* obj, QEvent* event);

public:
    static bool m_allowDwm;
    static int m_devicePixelRatio;

    bool m_lockNCPaint;
    bool m_inUpdateFrame;
    bool m_inLayoutRequest;
    bool m_postReclalcLayout;
    bool m_active;

private:
    HICON m_hIcon;
    QSize m_szIcon;
    QWidget* m_frame;
    RibbonBar* m_ribbonBar;
    QList<ContextHeader*>* m_listContextHeaders;
    QSize m_szFrameRegion;
    QRect m_rcHeader;
    int m_frameBorder;
    int m_borderSizeBotton;
    HWND m_hwndFrame;
    QTimer m_refreshFrame;

    int m_oldMarginsleft;
    int m_oldMarginstop;
    int m_oldMarginsright;
    int m_oldMarginsbottom;

    bool m_dwmEnabled;
    bool m_skipNCPaint;
    bool m_officeFrameEnabled;
    bool m_isTransparentCaption;
    int  m_shellAutohideBars;
    bool m_shellAutohideBarsInitialized;

    bool m_wasFullScreen;
    bool m_closeWindow;
    bool m_isTheme2013;
    int m_changedSize;

private:
    class FrameHelperEventHook;
    Q_DISABLE_COPY(OfficeFrameHelperWin)
};

QTITAN_END_NAMESPACE

#endif // QTN_OFFICEFRAMEHELPER_WIN_H

