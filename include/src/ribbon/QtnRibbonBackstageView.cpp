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
#include <QStyle>
#include <QApplication>
#include <QStyleOption>
#include <QWidgetAction>
#include <QPainter>
#include <QStatusBar>
#include <QBitmap>
#include <QLayout>
#include <qevent.h>

#include "QtnStyleHelpers.h"
#include "QtnRibbonBackstageView.h"
#include "QtnRibbonBarPrivate.h"
#include "QtnRibbonBackstageViewPrivate.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonBar.h"
#ifdef DEBUG_MEMORY_ENABLED
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE

const int commandGap = 6;
const int textGap = 15;
const int imageMargin = 7;

/* RibbonBackstageCloseButton */
RibbonBackstageCloseButton::RibbonBackstageCloseButton(QWidget* parent)
    : QToolButton(parent)
{
}

QSize RibbonBackstageCloseButton::sizeHint() const
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    QList<QSize> sz = opt.icon.availableSizes();
    Q_ASSERT(sz.size() > 0);
    QSize size = sz[0];
    if (style()->property("DPIAware").toBool())
        size = QSize(static_cast<int>(DrawHelpers::dpiScaled((qreal)size.height())),
                     static_cast<int>(DrawHelpers::dpiScaled((qreal)size.width())));
    return size;
}

void RibbonBackstageCloseButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter p(this);

    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    QList<QSize> sz = opt.icon.availableSizes();
    opt.iconSize = sz.at(0);
    if (style()->property("DPIAware").toBool())
        opt.iconSize = QSize(static_cast<int>(DrawHelpers::dpiScaled((qreal)opt.iconSize.height())), 
                             static_cast<int>(DrawHelpers::dpiScaled((qreal)opt.iconSize.width())));
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonBackstageCloseButton, &opt, &p, this);
}

void RibbonBackstageCloseButton::mousePressEvent(QMouseEvent* event)
{
    if (testAttribute(Qt::WA_UnderMouse))
        QToolButton::mousePressEvent(event);
}


/* RibbonBackstageViewMenu */
RibbonBackstageViewMenu::RibbonBackstageViewMenu(RibbonBackstageView* backstageView, RibbonBar* ribbon, QWidget* parent)
    : QWidget(parent)
    , m_ribbon(ribbon)
    , m_backstageView(backstageView)
    , m_backStageCloseButton(Q_NULL)
    , m_totalHeight(0)
    , m_offsetScroll(0)
    , m_mouseDown(false)
    , m_itemsDirty(true)
    , m_menuWidth(132) 
    , m_maxIconWidth(0)
{
    setMouseTracking(true);
}

QSize RibbonBackstageViewMenu::sizeHint() const
{
    m_itemsDirty = true;
    updateActionRects();
    return QSize(m_menuWidth, m_backstageView->sizeHint().width());
}

void RibbonBackstageViewMenu::createBackstageCloseButton()
{
    if ((bool)style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs))
    {
        if (m_backStageCloseButton == Q_NULL)
        {
            m_backStageCloseButton = new RibbonBackstageCloseButton(this->parentWidget());
            QAction* actBackStageCloseButton = new QAction(this);
            actBackStageCloseButton->setIcon(QIcon(":/res/ribbonbackstageback_icon.png"));
            m_backStageCloseButton->setDefaultAction(actBackStageCloseButton);
            QSize sz = m_backStageCloseButton->sizeHint();
            m_backStageCloseButton->setGeometry(QRect(QPoint(sz.width()/2, sz.height()/2), sz));
            m_backStageCloseButton->setVisible(true);
            QObject::connect(m_backStageCloseButton, SIGNAL(pressed()), m_backstageView, SLOT(close()));
        }
    }
    else
    {
        delete m_backStageCloseButton;
        m_backStageCloseButton = Q_NULL;
    }
    m_itemsDirty = true;
}

QAction* RibbonBackstageViewMenu::actionAt(const QPoint& pt) const
{
    QList<QAction*> actions = m_backstageView->actions();
    for(int i = 0; i < m_actionRects.count(); ++i) 
    {
        if (m_actionRects.at(i).contains(pt))
            return actions.at(i);
    }
    return 0;
}

QRect RibbonBackstageViewMenu::actionRect(QAction* act) const
{
    QList<QAction*> actions = m_backstageView->actions();
    int index = actions.indexOf(act);
    if (index == -1)
        return QRect();

    updateActionRects();

    // we found the action
    return m_actionRects.at(index);
}

void RibbonBackstageViewMenu::resetItemsDirty()
{
    m_itemsDirty = true;
}

void RibbonBackstageViewMenu::setOffsetScroll(int offset)
{
    if (m_offsetScroll != offset)
    {
        m_offsetScroll = offset;
        resetItemsDirty();
        updateActionRects();
        update();
    }
}

int RibbonBackstageViewMenu::totalHeight() const
{
    return m_totalHeight;
}

void RibbonBackstageViewMenu::initStyleOption(StyleOptionRibbonBackstage* opt) const
{
    opt->init(this);
    opt->menuWidth = m_menuWidth;
    opt->rect.adjust(0, 0, -1, -1);
}

void RibbonBackstageViewMenu::initStyleOption(QStyleOptionMenuItem* option, const QAction* action) const
{
    if (!option || !action)
        return;

    option->initFrom(this);
    option->palette = palette();
    option->state = QStyle::State_None;

    if (m_backstageView->window()->isActiveWindow())
        option->state |= QStyle::State_Active;
    if (m_backstageView->isEnabled() && action->isEnabled()
        && (!action->menu() || action->menu()->isEnabled()))
        option->state |= QStyle::State_Enabled;
    else
        option->palette.setCurrentColorGroup(QPalette::Disabled);

    option->font = action->font().resolve(font());
    option->fontMetrics = QFontMetrics(option->font);

    if (m_backstageView->qtn_d().m_currentAction && m_backstageView->qtn_d().m_currentAction == action && !m_backstageView->qtn_d().m_currentAction->isSeparator()) 
        option->state |= QStyle::State_Selected | (m_mouseDown ? QStyle::State_Sunken : QStyle::State_None);

    option->menuItemType = QStyleOptionMenuItem::Normal;
    if (const QWidgetAction* pageAct = qobject_cast<const QWidgetAction*>(action))
    {
        option->menuItemType = QStyleOptionMenuItem::SubMenu;
        if (pageAct->defaultWidget() == m_backstageView->activePage()/*m_activePage*/)
            option->state |= QStyle::State_HasFocus;
    }
    else if (action->isSeparator())
        option->menuItemType = QStyleOptionMenuItem::Separator;

    if (action->isIconVisibleInMenu())
        option->icon = action->icon();
    QString textAndAccel = action->text();

#ifndef QT_NO_SHORTCUT
    if (textAndAccel.indexOf(QLatin1Char('\t')) == -1) {
        QKeySequence seq = action->shortcut();
        if (!seq.isEmpty())
            textAndAccel += QLatin1Char('\t') + seq.toString();
    }
#endif
    option->text = textAndAccel;
    option->maxIconWidth = m_maxIconWidth;
    option->menuRect = rect();
}

void RibbonBackstageViewMenu::updateActionRects() const
{
    if (!m_itemsDirty)
        return;

    RibbonBackstageViewMenu* menuThis = (RibbonBackstageViewMenu*)this;
    menuThis->m_totalHeight = 0;

    ensurePolished();
    QList<QAction*> actions = m_backstageView->actions();
    //let's reinitialize the buffer
    m_actionRects.resize(actions.count());
    m_actionRects.fill(QRect());

    QStyle *style = ((RibbonBackstageViewMenu*)this)->style();
    QStyleOption opt;
    opt.init(this);

    const int icone = style->pixelMetric(QStyle::PM_SmallIconSize, &opt, this);

    QFontMetrics qfm = fontMetrics();

    int actionHeight = 26;
    int pageHeight = qMax(37, qfm.height() + 10);

    for (int i = 0, count = actions.count(); i < count; i++)
    {
        QAction *action = actions.at(i);
        if (action->isSeparator() || !action->isVisible() )
            continue;

        QIcon is = action->icon();
        if (!is.isNull())
            m_maxIconWidth = qMax<uint>(m_maxIconWidth, icone + 4);
    }

    for (int i = 0, count = actions.count(); i < count; i++)
    {
        QAction* action = actions.at(i);
        if (!action->isVisible() ||  action->isSeparator())
            continue; // we continue, this action will get an empty QRect

        //let the style modify the above size..
        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);

        const QFontMetrics& fm = opt.fontMetrics;

        QRect br(fm.boundingRect(action->text()));
        int width = br.width() + commandGap + textGap + commandGap + 6;

        actionHeight = qMax(actionHeight, qMax(fm.height() + fm.descent()*2, qfm.height() + qfm.descent()*2));
        width += m_maxIconWidth + imageMargin;
        m_menuWidth = qMax(m_menuWidth, width);
    }

    int top = m_backStageCloseButton ? m_backStageCloseButton->geometry().bottom() + 17 :  7;
    top -= m_offsetScroll;
    for (int i = 0, count = actions.count(); i < count; i++)
    {
        QAction* action = actions.at(i);
        if (!action->isVisible() /*||  action->isSeparator()*/)
            continue; // we continue, this action will get an empty QRect

        if (qobject_cast<QWidgetAction*>(action))
        {
            m_actionRects[i] = QRect(QPoint(0, top), QPoint(m_menuWidth, top + pageHeight));
            top += pageHeight + 2;
        }
        else if (action->isSeparator())
        {
            m_actionRects[i] = QRect(QPoint(-commandGap, top), QPoint(m_menuWidth - commandGap * 2, top + actionHeight - 3));
            top += actionHeight - 3;
        }
        else
        {
            m_actionRects[i] = QRect(QPoint(commandGap, top), QPoint(m_menuWidth - commandGap, top + actionHeight));
            top += actionHeight + 2;
        }
    }

    if (!m_backstageView->activePage())
    {
        for (int i = 0, count = actions.count(); i < count; i++)
        {
            if (QWidgetAction* action = qobject_cast<QWidgetAction*>(actions.at(i)))
            {
                m_backstageView->setActivePage(action->defaultWidget());
                break;
            }
        }
    }

    menuThis->m_totalHeight = top + m_offsetScroll;
    m_itemsDirty = false;
}

void RibbonBackstageViewMenu::paintEvent(QPaintEvent* event)
{
    updateActionRects();

    QPainter p(this);

    int backstageViewTop = 0;
    if (m_backStageCloseButton && m_ribbon->isVisible() && static_cast<bool>(style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs)))
        backstageViewTop = m_ribbon->backstageViewTop();

    QRect rc = rect();
    rc.adjust(0, backstageViewTop, 0, 0);
    QRegion emptyArea = QRegion(rc);
    QRegion emptyArea1 = emptyArea;

    StyleOptionRibbonBackstage optBackstage;
    if (m_backStageCloseButton == Q_NULL)
    {
        optBackstage.init(m_backstageView);
        optBackstage.menuWidth = m_menuWidth;
    }
    else
        initStyleOption(&optBackstage);
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_Backstage, &optBackstage, &p, m_backstageView);

    QRect rectEvent = event->rect();
    QList<QAction*> acts = m_backstageView->actions();
    //draw the items that need updating..
    for (int i = 0, count = acts.count(); i < count; ++i) 
    {
        QAction* action = acts.at(i);
        QRect adjustedActionRect = m_actionRects.at(i);
        if (!rectEvent.intersects(adjustedActionRect))
            continue;

        //set the clip region to be extra safe (and adjust for the scrollers)
        QRegion adjustedActionReg(adjustedActionRect);
        emptyArea -= adjustedActionReg;
        p.setClipRegion(adjustedActionReg);

        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);
        opt.rect = adjustedActionRect;
        opt.text = action->text();

        if (QWidgetAction* pageAct = qobject_cast<QWidgetAction*>(action))
        {
            opt.menuItemType = QStyleOptionMenuItem::SubMenu;
            if (pageAct->defaultWidget() == m_backstageView->activePage())
                opt.state |= QStyle::State_HasFocus;
        }
        p.setClipRegion(emptyArea1);
        style()->drawControl(QStyle::CE_MenuItem, &opt, &p, m_backstageView);
    }
}

void RibbonBackstageViewMenu::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
    switch(event->key())
    {
        case Qt::Key_Escape :
                m_backstageView->close();
            break;
        case Qt::Key_Up :
            break;
        case Qt::Key_Down :
            break;
        default:
            break;
    }
}

void RibbonBackstageViewMenu::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;
    if (rect().contains(event->pos()))
    {
        if (QWidgetAction* act = qobject_cast<QWidgetAction*>(actionAt(event->pos())))
        {
            if (act && (act->isSeparator() || !act->isEnabled()))
                return;
            if (QWidget* w = act->defaultWidget())
            {
                m_backstageView->setActivePage(w);
            }
        }
    }
}

void RibbonBackstageViewMenu::mouseMoveEvent(QMouseEvent* event)
{
    if (rect().contains(event->pos()))
    {
        QAction* action = actionAt(event->pos());
        m_backstageView->qtn_d().setCurrentAction(action ? action : Q_NULL);
        setFocus();
    }
}

void RibbonBackstageViewMenu::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (rect().contains(event->pos()))
    {
        QAction* action = actionAt(event->pos());

        if (action && (action->isSeparator() || !action->isEnabled() || m_backstageView->qtn_d().m_widgetItems.contains(action)))
            return;

        if (action && action == m_backstageView->qtn_d().m_currentAction) 
        {
            if (!m_backstageView->isClosePrevented())
                m_backstageView->close();
            action->activate(QAction::Trigger);
        }
    }
}

void RibbonBackstageViewMenu::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);
    m_backstageView->qtn_d().setCurrentAction(Q_NULL);
}

void RibbonBackstageViewMenu::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (m_backStageCloseButton)
        m_backStageCloseButton->show();
}

void RibbonBackstageViewMenu::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    if (m_backStageCloseButton)
        m_backStageCloseButton->hide();
}


/* RibbonBackstageSeparator */
RibbonBackstageSeparator::RibbonBackstageSeparator(QWidget* parent)
    : QFrame(parent)
{
    setAttribute(Qt::WA_MouseNoMask); 
    setFrameStyle(HLine | Sunken);
}

RibbonBackstageSeparator::~RibbonBackstageSeparator()
{
}

void RibbonBackstageSeparator::setOrientation(Qt::Orientation orient)
{ 
    setFrameShape(orient == Qt::Horizontal ? HLine : VLine); 
}

Qt::Orientation RibbonBackstageSeparator::orientation() const
{ 
    return frameShape() == HLine ? Qt::Horizontal : Qt::Vertical; 
}

QTITAN_BEGIN_NAMESPACE

class RibbonBackstageButtonPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(RibbonBackstageButton)
public:
    explicit RibbonBackstageButtonPrivate();
    virtual ~RibbonBackstageButtonPrivate();
public:
    bool m_tabStyle;
    bool m_flatStyle;
};

QTITAN_END_NAMESPACE

RibbonBackstageButtonPrivate::RibbonBackstageButtonPrivate()
{
    m_tabStyle = false;
    m_flatStyle = false;
}

RibbonBackstageButtonPrivate::~RibbonBackstageButtonPrivate()
{
}

/* RibbonBackstageButton */
RibbonBackstageButton::RibbonBackstageButton(QWidget* parent)
    : QToolButton(parent)
{
    QTN_INIT_PRIVATE(RibbonBackstageButton);
}

RibbonBackstageButton::~RibbonBackstageButton()
{
    QTN_FINI_PRIVATE();
}

bool RibbonBackstageButton::tabStyle() const
{
    QTN_D(const RibbonBackstageButton);
    return d.m_tabStyle;
}

void RibbonBackstageButton::setTabStyle(bool style)
{
    QTN_D(RibbonBackstageButton);
    d.m_tabStyle = style;
    update();
}

bool RibbonBackstageButton::flatStyle()
{
    QTN_D(const RibbonBackstageButton);
    return d.m_flatStyle;
}

void RibbonBackstageButton::setFlatStyle(bool flat)
{
    QTN_D(RibbonBackstageButton);
    d.m_flatStyle = flat;
    update();
}

/*! \reimp */
void RibbonBackstageButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(RibbonBackstageButton);

    QPainter p(this);
    StyleOptionBackstageButton option;
    initStyleOption(&option);
    option.tabStyle = d.m_tabStyle;
    option.flatStyle = d.m_flatStyle;

    if (!option.text.isEmpty() && ((option.features & QStyleOptionToolButton::HasMenu) || (option.subControls & QStyle::SC_ToolButtonMenu)))
    {
        option.text += QChar(' '); 
        option.text += QChar(' ');
    }

    style()->drawComplexControl(QStyle::CC_ToolButton, &option, &p, this);
}


/* RibbonBackstagePage */
RibbonBackstagePage::RibbonBackstagePage(QWidget* parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Light);
}

RibbonBackstagePage::~RibbonBackstagePage()
{
}

/*! \reimp */
void RibbonBackstagePage::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    p.fillRect(opt.rect, opt.palette.color(QPalette::Light));
}

/*!
\class Qtitan::RibbonBackstageViewPrivate
\internal
*/
RibbonBackstageViewPrivate::RibbonBackstageViewPrivate()
    : m_closePrevented(false)
    , m_scrollOffset(0)
    , m_scrollOffsetHorz(0)
    , m_totalHeight(0)
    , m_tabBarPosition(RibbonBar::TabBarTopPosition)
    , m_ribbon(Q_NULL)
    , m_scrollBarVert(Q_NULL)
    , m_scrollBarHorz(Q_NULL)
    , m_backstageMenu(Q_NULL)
    , m_activePage(Q_NULL)
    , m_currentAction(Q_NULL)
    , m_sizeRightPage(QSize())
    , m_rectRight(QRect())
{
}

RibbonBackstageViewPrivate::~RibbonBackstageViewPrivate()
{
    QTN_P(RibbonBackstageView);
    QWidget* parent =  p.parentWidget();
    Q_ASSERT(parent != Q_NULL);
    parent->removeEventFilter(&p);
}

void RibbonBackstageViewPrivate::init(RibbonBar* ribbonBar)
{
    QTN_P(RibbonBackstageView);
    m_ribbon = ribbonBar;
    p.setAttribute(Qt::WA_NoMousePropagation);

    QWidget* parent = p.parentWidget();
    Q_ASSERT(parent != Q_NULL);
    parent->installEventFilter(&p);

    p.setFont(QApplication::font("QMenu"));
    p.setPalette(QApplication::palette("QMenu"));

    m_backstageMenu = new RibbonBackstageViewMenu(&p, m_ribbon, parent);
    m_backstageMenu->setAttribute(Qt::WA_NoMousePropagation);
    m_backstageMenu->setPalette(p.palette());
    m_backstageMenu->setFont(p.font());
    m_backstageMenu->setVisible(false);

    m_scrollBarVert = new QScrollBar(Qt::Vertical, parent);
    m_scrollBarVert->setVisible(false);
    m_scrollBarVert->setGeometry(QRect());
    connect(m_scrollBarVert, SIGNAL(actionTriggered(int)), this, SLOT(scrollVertTriggered(int)));
    m_scrollBarHorz = new QScrollBar(Qt::Horizontal, parent);
    m_scrollBarHorz->setVisible(false);
    m_scrollBarHorz->setGeometry(QRect());
    connect(m_scrollBarHorz, SIGNAL(actionTriggered(int)), this, SLOT(scrollHorzTriggered(int)));
    p.setVisible(false);
}

void RibbonBackstageViewPrivate::adjustLocations()
{
    m_backstageMenu->setOffsetScroll(m_scrollOffset);
    layoutBackstage();
    m_scrollBarVert->raise();
    m_scrollBarHorz->raise();
}

void RibbonBackstageViewPrivate::adjustScrollBars()
{
    QTN_P(RibbonBackstageView);
    int scrollOffsetOld = m_scrollOffset;
    int scrollOffsetHorzOld = m_scrollOffsetHorz;

    const int scrollWidth = p.style()->pixelMetric(QStyle::PM_ScrollBarExtent, 0, &p);
    const int scrollHeight = scrollWidth;

    QRect rect = p.parentWidget()->rect();
    QRect rectLoc = p.rect();
    rectLoc.setTopLeft(p.mapToParent(rectLoc.topLeft()));
    rect.setTop(rectLoc.top());

    m_rectRight.setRight(rect.right());
    m_rectRight.setBottom(rect.bottom());

    int totalWidth = m_sizeRightPage.width();
    totalWidth += m_backstageMenu->sizeHint().width();

    int totalHeight = qMax(m_totalHeight, m_sizeRightPage.height());

    bool hasVertScrollBar = false;
    bool hasHorzScrollBar = totalWidth > rect.width();

    if (totalHeight <= rect.height())
    {
        m_scrollBarVert->setVisible(false);
        m_scrollOffset = 0;
    }
    else
    {
        int vertScrollHeight = rect.height();
        if (hasHorzScrollBar)
            vertScrollHeight -= scrollHeight;

        m_scrollBarVert->setGeometry(QRect(QPoint((rect.right() - scrollWidth) + 1, rect.top()), QSize(scrollWidth, vertScrollHeight)));
        m_scrollBarVert->setVisible(true);
        m_scrollBarVert->raise();

        m_scrollBarVert->setMinimum(0);
        m_scrollBarVert->setMaximum(totalHeight - rect.height());
        m_scrollBarVert->setPageStep(rect.height());

        if (m_scrollOffset > totalHeight)
            m_scrollOffset = totalHeight;

        hasVertScrollBar = true;
    }

    if (totalWidth <= rect.width())
    {
        m_scrollBarHorz->setVisible(false);
        m_scrollOffsetHorz = 0;
    }
    else
    {
        int horzScrollWidth = rect.width();
        if (hasVertScrollBar)
            horzScrollWidth -= scrollWidth;

        m_scrollBarHorz->setGeometry(QRect(QPoint(rect.left(), (rect.bottom() - scrollHeight) + 1), QSize(horzScrollWidth, scrollHeight)));
        m_scrollBarHorz->setVisible(true);
        m_scrollBarHorz->raise();

        m_scrollBarHorz->setMinimum(0);
        m_scrollBarHorz->setMaximum(totalWidth - rect.width());
        m_scrollBarHorz->setPageStep(rect.width());

        if (m_scrollOffsetHorz > totalWidth)
            m_scrollOffsetHorz = totalWidth;
    }

    if (((scrollOffsetOld != m_scrollOffset) || (scrollOffsetHorzOld != m_scrollOffsetHorz)) && m_backstageMenu != NULL)
        adjustLocations();
}

void RibbonBackstageViewPrivate::layoutBackstage()
{
    QTN_P(RibbonBackstageView);
    if (!m_ribbon)
        return;

    int backstageViewTop = 0;
    m_backstageMenu->createBackstageCloseButton();

    if (m_ribbon->isVisible() && !static_cast<bool>(p.style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs)))
        backstageViewTop = m_ribbon->backstageViewTop();

    QWidget* parent = p.parentWidget();
    QRect rect = parent->rect();
    rect.setTop(rect.top() + backstageViewTop);
    QRect rc = rect;
    rc.setLeft(rc.left() - m_scrollOffsetHorz);
    QSize sz = m_backstageMenu->sizeHint();
    m_backstageMenu->setGeometry(QRect(QPoint(rc.topLeft()), QPoint(rc.left() + sz.width(), rc.bottom())));
    m_backstageMenu->raise();
    m_backstageMenu->closeButton()->raise();

    if (static_cast<bool>(p.style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs)))
    {
        backstageViewTop = m_ribbon->backstageViewTop();
        rect.setTop(rect.top() + backstageViewTop);
    }
    rect.setLeft(sz.width() + 1);
    rect.setLeft(rect.left() - m_scrollOffsetHorz);

    p.setGeometry(rect);
    p.raise();
    updateGeometryPage(m_activePage);
    m_totalHeight = qMax(m_totalHeight, m_backstageMenu->totalHeight());
}

void RibbonBackstageViewPrivate::updateLayout()
{
    m_backstageMenu->resetItemsDirty();
    layoutBackstage();
    adjustScrollBars();
}

void RibbonBackstageViewPrivate::updateGeometryPage(QWidget* widget)
{
    if (widget == Q_NULL)
        return;

    QTN_P(const RibbonBackstageView);
    m_totalHeight = 0;
    QSize minimumSize = widget->minimumSize();
    int top = static_cast<bool>(p.style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs)) ? 0 : 2;
    QRect rcContent(QPoint(0, top - m_scrollOffset), QPoint(p.rect().right(), p.rect().bottom()));
    rcContent.setRight(qMax(rcContent.right(), rcContent.left() + minimumSize.width()));
    rcContent.setBottom(qMax(rcContent.bottom(), rcContent.top() + minimumSize.height()));
    widget->setGeometry(rcContent);
    m_sizeRightPage = m_mapSize[widget];
}

#if 0
static inline bool isAncestor(const QObject* obj, const QObject* child)
{
    while (child) 
    {
        if (child == obj)
            return true;
        child = child->parent();
    }
    return false;
}
#endif

void RibbonBackstageViewPrivate::setCurrentAction(QAction* action)
{
    if (m_currentAction)
        m_backstageMenu->update(m_backstageMenu->actionRect(m_currentAction));

    m_currentAction = action;

    if (action) 
    {
        if (!action->isSeparator()) 
            m_backstageMenu->update(m_backstageMenu->actionRect(action));
    }
}

QAction* RibbonBackstageViewPrivate::currentAction() const
{
    return m_currentAction;
}

QWidgetAction* RibbonBackstageViewPrivate::getAction(QWidget* w) const
{
    QTN_P(const RibbonBackstageView);
    QList<QAction*> actions = p.actions();
    for (int i = 0, count = actions.count(); i < count; i++)
    {
        QWidgetAction* action = dynamic_cast<QWidgetAction*>(actions.at(i));
        if (action && action->defaultWidget() == w)
            return action;
    }
    return Q_NULL;
}

void RibbonBackstageViewPrivate::scrollVertTriggered(int action)
{
    int iOffset = m_scrollOffset;
    int pageStep = m_scrollBarVert->pageStep();
    int totalHeight = qMax(m_totalHeight, m_sizeRightPage.height());

    if (totalHeight - pageStep <= 1)
        return;

    switch (action)
    {
        case QAbstractSlider::SliderMove:
            iOffset = m_scrollBarVert->sliderPosition();
            break;
        case QAbstractSlider::SliderSingleStepSub:
            iOffset -= pageStep;
            break;
        case QAbstractSlider::SliderSingleStepAdd:
            iOffset += pageStep;
            break;
        default:
            break;
    }

    iOffset = qMin(qMax(0, iOffset), totalHeight - pageStep);

    if (iOffset == m_scrollOffset)
        return;

    m_scrollOffset = iOffset;
    adjustLocations();
    m_scrollBarVert->setSliderPosition(iOffset);
}

void RibbonBackstageViewPrivate::scrollHorzTriggered(int action)
{
    int iOffset = m_scrollOffsetHorz;
    int pageStep = m_scrollBarVert->pageStep();
    int totalWidth = m_sizeRightPage.width();
    totalWidth += m_backstageMenu->sizeHint().width();

    if (totalWidth - pageStep <= 1)
        return;

    switch (action)
    {
        case QAbstractSlider::SliderMove:
            iOffset = m_scrollBarHorz->sliderPosition();
            break;
        case QAbstractSlider::SliderSingleStepSub:
            iOffset -= pageStep;
            break;
        case QAbstractSlider::SliderSingleStepAdd:
            iOffset += pageStep;
            break;
        default:
            break;
    }

    iOffset = qMin(qMax(0, iOffset), totalWidth - pageStep);

    if (iOffset == m_scrollOffsetHorz)
        return;

    m_scrollOffsetHorz = iOffset;
    adjustLocations();
    m_scrollBarHorz->setSliderPosition(iOffset);
}


/* RibbonBackstageView */
RibbonBackstageView::RibbonBackstageView(RibbonBar* ribbonBar)
    : QWidget(ribbonBar->parentWidget())
{
    QTN_INIT_PRIVATE(RibbonBackstageView);
    QTN_D(RibbonBackstageView);
    d.init(ribbonBar);
}

RibbonBackstageView::~RibbonBackstageView()
{
    QTN_FINI_PRIVATE();
}

void RibbonBackstageView::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    QTN_D(RibbonBackstageView);
    d.m_tabBarPosition = d.m_ribbon->tabBarPosition();
    d.m_ribbon->setTabBarPosition(RibbonBar::TabBarTopPosition);
    d.updateLayout();
    d.m_backstageMenu->setVisible(true);
    d.m_ribbon->qtn_d().setVisibleBackstage(true);
    setFocus();
    emit aboutToShow();
}

void RibbonBackstageView::hideEvent(QHideEvent * event)
{
    Q_UNUSED(event);
    QTN_D(RibbonBackstageView);
    d.m_ribbon->setTabBarPosition(d.m_tabBarPosition);
    d.m_backstageMenu->setVisible(false);
    d.m_ribbon->qtn_d().setVisibleBackstage(false);
    emit aboutToHide();
}

#define QTN_WHEEL_DELTA  120      // Default value for rolling one notch
void RibbonBackstageView::wheelEvent(QWheelEvent* event)
{
    QTN_D(RibbonBackstageView);
    if (!d.m_scrollBarVert->isVisible())
        return;
    if (QApplication::activePopupWidget() != Q_NULL)
        return;

    const int steps = abs(event->delta()) / QTN_WHEEL_DELTA;
    for (int i = 0; i < steps; i++)
        d.scrollVertTriggered(event->delta() < 0 ? QAbstractSlider::SliderSingleStepAdd : QAbstractSlider::SliderSingleStepSub);
}

bool RibbonBackstageView::isClosePrevented() const
{
    QTN_D(const RibbonBackstageView);
    return d.m_closePrevented;
}

void RibbonBackstageView::setClosePrevented(bool prevent)
{
    QTN_D(RibbonBackstageView);
    d.m_closePrevented = prevent;
}

QAction* RibbonBackstageView::addAction(const QIcon& icon, const QString& text)
{
    QAction* action = new QAction(icon, text, this);
    addAction(action);
    return action;
}

QAction* RibbonBackstageView::addSeparator()
{
    QAction* action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
    return action;
}

QAction* RibbonBackstageView::addPage(QWidget* widget)
{
    QTN_D(RibbonBackstageView);
    QWidgetAction* action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    widget->setFont(font());
    action->setText(widget->windowTitle());
    d.m_mapSize[widget] = widget->sizeHint();
    addAction(action);
    return action;
}

QWidget* RibbonBackstageView::activePage() const
{
    QTN_D(const RibbonBackstageView);
    return d.m_activePage;
}

void RibbonBackstageView::setActivePage(QWidget* widget)
{
    QTN_D(RibbonBackstageView);

    bool find = false;
    QList<QAction*> acts = actions();
    for (int i = 0, count = acts.count(); i < count && !find; i++)
    {
        if (QWidgetAction* action = dynamic_cast<QWidgetAction*>(acts.at(i)))
        {
            if (action->defaultWidget() == widget)
            {
                if (!action->isVisible())
                {
                    Q_ASSERT(false);
                    return;
                }
                find = true;
            }
        }
    }

    if ( !find )
    {
        Q_ASSERT(false);
        return;
    }

    if (d.m_activePage)
    {
        if (QWidgetAction* act = d.getAction(d.m_activePage))
            d.m_backstageMenu->update(d.m_backstageMenu->actionRect(act));
    }

    for (int i = 0, count = acts.count(); i < count; i++)
    {
        if (QWidgetAction* action = dynamic_cast<QWidgetAction*>(acts.at(i)))
        {
            QWidget* w = action->defaultWidget();
            if (w == widget)
            {
                d.m_activePage = widget;
                d.updateGeometryPage(widget);
                d.adjustScrollBars();
                w->setVisible(true);
            }
            else
                w->setVisible(false);
        }
    }

    if (d.m_activePage)
    {
        if (QWidgetAction* act = d.getAction(d.m_activePage))
            d.m_backstageMenu->update(d.m_backstageMenu->actionRect(act));
    }
}

QRect RibbonBackstageView::actionGeometry(QAction* act) const
{
    QTN_D(const RibbonBackstageView);
    return d.m_backstageMenu->actionRect(act);
}

QRect RibbonBackstageView::menuGeometry() const
{
    QTN_D(const RibbonBackstageView);
    return d.m_backstageMenu->geometry();
}

void RibbonBackstageView::open()
{
    ensurePolished(); // Get the right font
    setVisible(true);
}

bool RibbonBackstageView::event(QEvent* event)
{
    switch (event->type())
    {
        case QEvent::ParentChange : 
            {
                if (QWidget* parent = parentWidget())
                    parent->installEventFilter(this);
            }
            break;
        case QEvent::ParentAboutToChange : 
            {
                if (QWidget* parent = parentWidget())
                    parent->removeEventFilter(this);
            }
            break;
        default:
            break;
    }
    return QWidget::event(event);
}

void RibbonBackstageView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(RibbonBackstageView);
    QPainter p(this);
    StyleOptionRibbonBackstage opt;
    opt.init(this);
    opt.menuWidth = d.m_backstageMenu->sizeHint().width();
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_Backstage, &opt, &p, this);
}

bool RibbonBackstageView::eventFilter(QObject* object, QEvent* event)
{
    QTN_D(RibbonBackstageView);
    if (parentWidget() == object && isVisible())
    {
        switch (event->type())
        {
            case QEvent::Resize : 
                    d.updateLayout();
                break;
            default:
                break;
        }
    }
    return QWidget::eventFilter(object, event);
}

void RibbonBackstageView::actionEvent(QActionEvent* event)
{
    QWidget::actionEvent(event);

    QTN_D(RibbonBackstageView);
    if (event->type() == QEvent::ActionAdded)
    {
        if (QWidgetAction* wa = qobject_cast<QWidgetAction*>(event->action())) 
        {
            QWidget *widget = wa->requestWidget(this);
            if (widget)
                d.m_widgetItems.insert(wa, widget);
        }
    } 
    else if (event->type() == QEvent::ActionRemoved)
    {
        if (QWidgetAction* wa = qobject_cast<QWidgetAction*>(event->action())) 
        {
            if (QWidget *widget = d.m_widgetItems.value(wa))
                wa->releaseWidget(widget);
        }
        d.m_widgetItems.remove(event->action());
    } 
    else if (event->type() == QEvent::ActionChanged)
    {
        if (QWidgetAction* action = dynamic_cast<QWidgetAction*>(event->action()))
        {
            if (!action->isVisible() && action->defaultWidget())
            {
                action->defaultWidget()->setVisible(false);

                if (activePage() == action->defaultWidget())
                {
                    bool activatePage = false;
                    QList<QAction*> acts = actions();
                    for (int i = 0, count = acts.count(); i < count && !activatePage; i++)
                    {
                        if (QWidgetAction* action = dynamic_cast<QWidgetAction*>(acts.at(i)))
                        {
                            if (action->isVisible())
                            {
                                if (QWidget* widget = action->defaultWidget())
                                {
                                    setActivePage(widget);
                                    activatePage = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        d.m_backstageMenu->resetItemsDirty();
    }
}

