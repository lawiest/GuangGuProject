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
#ifndef QTN_RIBBONBACKSTAGEPVIEWRIVATE_H
#define QTN_RIBBONBACKSTAGEPVIEWRIVATE_H

#include <QScrollBar>
#include <QToolButton>
#include <QHash>

#include "QtnRibbonBackstageView.h"

class QStyleOptionMenuItem;
class QWidgetAction;

QTITAN_BEGIN_NAMESPACE

/* RibbonBackstageCloseButton */
class RibbonBackstageCloseButton : public QToolButton
{
public:
    RibbonBackstageCloseButton(QWidget* parent);
public:
    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
private:
    Q_DISABLE_COPY(RibbonBackstageCloseButton)
};

class StyleOptionRibbonBackstage;
/* RibbonBackstageViewMenu */
class RibbonBackstageViewMenu : public QWidget
{
public:
    RibbonBackstageViewMenu(RibbonBackstageView* backstageView, RibbonBar* ribbon, QWidget* parent = Q_NULL);
public:
    void createBackstageCloseButton();
    RibbonBackstageCloseButton* closeButton() const { return m_backStageCloseButton; }
    QAction* actionAt(const QPoint& p) const;
    QRect actionRect(QAction* act) const;
    void resetItemsDirty();
    void setOffsetScroll(int offset);
    int totalHeight() const;
public:
    virtual QSize sizeHint() const;
protected:
    void initStyleOption(StyleOptionRibbonBackstage* opt) const;
    void initStyleOption(QStyleOptionMenuItem* option, const QAction* action) const;
    void updateActionRects() const;
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void leaveEvent(QEvent* event);
    virtual void showEvent(QShowEvent* event);
    virtual void hideEvent(QHideEvent* event);
private:
    RibbonBar* m_ribbon;
    RibbonBackstageView* m_backstageView;
    RibbonBackstageCloseButton* m_backStageCloseButton;
    int m_totalHeight;
    int m_offsetScroll;
    bool m_mouseDown;
    mutable bool m_itemsDirty;
    mutable int m_menuWidth;
    mutable int m_maxIconWidth;
    mutable QVector<QRect> m_actionRects;
private:
    Q_DISABLE_COPY(RibbonBackstageViewMenu)
};

class RibbonBar;
class HackFilter;
/* RibbonBackstageViewPrivate */
class RibbonBackstageViewPrivate : public QObject
{
public:
    Q_OBJECT
    QTN_DECLARE_PUBLIC(RibbonBackstageView)
public:
    explicit RibbonBackstageViewPrivate();
    virtual ~RibbonBackstageViewPrivate();

public:
    void init(RibbonBar* ribbonBar);
    void adjustLocations();
    void adjustScrollBars();
    void layoutBackstage();
    void updateLayout();
    void updateGeometryPage(QWidget* widget);

    void setCurrentAction(QAction *action);
    QAction* currentAction() const;
    QWidgetAction* getAction(QWidget* w) const;
public Q_SLOTS:
    void scrollVertTriggered(int action);
    void scrollHorzTriggered(int action);
public:
    bool m_closePrevented;
    int	m_scrollOffset;
    int	m_scrollOffsetHorz;
    int m_totalHeight;
    QMap<QWidget*, QSize> m_mapSize;
    RibbonBar::TabBarPosition m_tabBarPosition;
    RibbonBar* m_ribbon;
    QScrollBar* m_scrollBarVert;
    QScrollBar* m_scrollBarHorz;
    RibbonBackstageViewMenu* m_backstageMenu;
    QWidget* m_activePage;
    QAction* m_currentAction;
    QSize m_sizeRightPage;
    QRect m_rectRight;

    mutable QVector<QRect> m_actionRects;
    mutable QHash<QAction*, QWidget*> m_widgetItems;
private:
    Q_DISABLE_COPY(RibbonBackstageViewPrivate)
};


QTITAN_END_NAMESPACE

#endif // QTN_RIBBONBACKSTAGEPVIEWRIVATE_H

