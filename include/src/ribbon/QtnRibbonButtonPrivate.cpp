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
#include <QApplication>
#include <QPainter>

#include "QtnRibbonButtonPrivate.h"
#include "QtnRibbonGroup.h"
#include "QtnRibbonStyle.h"
#include "QtnStyleHelpers.h"

#ifdef DEBUG_MEMORY_ENABLED
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE


/*!
    \class Qtitan::RibbonButton
    \internal
*/
RibbonButton::RibbonButton(QWidget* parent)
    : QToolButton(parent)
    , m_wordWrap(true)
    , m_wrapIndex(-1)
    , m_saveButtonStyle(Qt::ToolButtonIconOnly)
    , m_saveArrowType(Qt::NoArrow)
{
    setAutoRaise(true);
}

RibbonButton::~RibbonButton()
{
}

bool RibbonButton::wordWrap() const
{
    return m_wordWrap;
}

void RibbonButton::setWordWrap(bool on)
{
    if (m_wordWrap != on)
    {
        m_wordWrap = on;
        m_sizeHint = QSize();
    }
}

QSize RibbonButton::sizeHint() const
{
    RibbonButton* thisButton = (RibbonButton*)this;

    if (m_saveButtonStyle != toolButtonStyle())
        thisButton->m_sizeHint = QSize();

    if (m_saveArrowType != arrowType())
        thisButton->m_sizeHint = QSize();

    if (m_sizeHint.isValid())
        return m_sizeHint;

    thisButton->m_saveButtonStyle = toolButtonStyle();
    thisButton->m_saveArrowType = arrowType();

    QSize sz = QToolButton::sizeHint();
    if (RibbonControl* control = qobject_cast<RibbonControl *>(parentWidget()))
    {
        if (RibbonBar* ribbonBar = control->parentGroup()->ribbonBar())
            sz.setHeight(ribbonBar->rowItemHeight());
    }

    int w = 0, h = 0;
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    QString textButton = text();

    if (!textButton.isEmpty() && (bool)style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonItemUpperCase, 0, this))
        textButton = textButton.toUpper();

    if (opt.toolButtonStyle != Qt::ToolButtonTextOnly)
    {
        w = opt.iconSize.width();
        h = opt.iconSize.height();
    }

    bool allowIndicator = false;
    QToolButton::ToolButtonPopupMode mode = QToolButton::popupMode();
    if (opt.toolButtonStyle != Qt::ToolButtonIconOnly)
    {
        if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon)
        {
            thisButton->resetWordWrap();
            thisButton->m_sizeWordWrap = thisButton->sizeWordWrap();

            h = sz.height();

            if (RibbonControl* control = qobject_cast<RibbonControl *>(parentWidget()))
            {
                if (RibbonBar* ribbonBar = control->parentGroup()->ribbonBar())
                    h = ribbonBar->rowItemHeight() * ribbonBar->rowItemCount();
            }

            // if the text is more than icon
            if (m_sizeWordWrap.width() > w)
                w = m_sizeWordWrap.width();
        } 
        else
        {
            allowIndicator = true;
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, textButton);
            textSize.setWidth(4 + textSize.width() + opt.fontMetrics.width(QLatin1Char(' ')) * 2);
            h = sz.height();

            if (opt.toolButtonStyle == Qt::ToolButtonTextBesideIcon) 
            {
                w += textSize.width() + 2;

                if (textSize.height() > h)
                   h = textSize.height();
            } 
            else 
                w = textSize.width() + 4;
        }
    }
    else 
    {
        allowIndicator = true;
        h = sz.height();
        QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, textButton.isEmpty() ? "XXX" : textButton);
        if (textSize.height() > h)
            h = textSize.height();

        w = h;
        if ((opt.features & QStyleOptionToolButton::HasMenu) || (opt.subControls & QStyle::SC_ToolButtonMenu))
            w += 1;
    }

    opt.rect.setSize(QSize(w, h)); // PM_MenuButtonIndicator depends on the height

    if ((mode == MenuButtonPopup || mode == QToolButton::InstantPopup) && allowIndicator)
        w += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);

    thisButton->m_sizeHint = QSize(w, h).expandedTo(QApplication::globalStrut());
    return m_sizeHint;
}

void RibbonButton::resetWordWrap()
{
    m_wordIndexes.clear();

    int start = 0;
    QString strText = text();
    for (int offset = start;;)
    {
        int index = strText.indexOf(QChar(' '), offset);
        if (index >= 0)
        {
            Q_ASSERT(index != 0);
            m_wordIndexes.append(index);
            offset = index + 1;
        }
        else
            break;
    }
}

QSize RibbonButton::sizeWordWrap()
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon)
    {
        const int sz = style()->pixelMetric(QStyle::PM_LargeIconSize, &opt, this);
        opt.iconSize = QSize(sz, sz);
    }

    QString strText = opt.text;
    QSize sizeText = opt.fontMetrics.size(Qt::TextShowMnemonic, strText);
    const int space = opt.fontMetrics.width(QLatin1Char(' ')) * 2;
    const int textLineHeight = sizeText.height();
    const bool indicatorCenter = m_wordIndexes.size() == 0  && opt.features & QStyleOptionToolButton::HasMenu;
    sizeText.setWidth(sizeText.width() + space);

    const int menuArrowWidth = !indicatorCenter ? style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this) + 2 : 0;

    m_wrapIndex = -1;
    int textWidth = 0;

    if (m_wordIndexes.size() == 0 || !m_wordWrap)
    {
        textWidth = sizeText.width() + space;
    }
    else
    {
        textWidth = 32767;
        for (int i = 0; i < m_wordIndexes.size(); i++)
        {
            int index = m_wordIndexes[i];
            QString strLineOne = strText.left(index);
            const int cxLineOne = opt.fontMetrics.size(Qt::TextShowMnemonic, strLineOne).width() + space;

            QString strLineTwo = strText.mid(index + 1);
            const int cxLineTwo = opt.fontMetrics.size(Qt::TextShowMnemonic, strLineTwo).width() + menuArrowWidth + space;
            int width = qMax(cxLineOne, cxLineTwo);

            if (width < textWidth)
            {
                textWidth = width;
                m_wrapIndex = index;
            }
        }
    }

    if (textWidth % 2)
        textWidth--;

    QSize size(textWidth, textLineHeight * 2);
    return size;
}

bool RibbonButton::event(QEvent* event)
{
    bool result = QToolButton::event(event);
    QEvent::Type type = event->type();
    if (type == QEvent::LayoutRequest ||
        type == QEvent::FontChange    ||
        type == QEvent::StyleChange)
        m_sizeHint = QSize();
    return result;
}

void RibbonButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon)
    {
        const int sz = style()->pixelMetric(QStyle::PM_LargeIconSize, &opt, this);
        opt.iconSize = QSize(sz, sz);
    }

    if (property("wordWrap").toBool())
    {
        QToolButton::ToolButtonPopupMode mode = QToolButton::popupMode();
        if (mode == MenuButtonPopup || mode == QToolButton::InstantPopup)
        {
            const bool indicatorCenter = m_wordIndexes.size() == 0 && opt.features & QStyleOptionToolButton::HasMenu;
            const int menuArrowWidth = !indicatorCenter ? style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this) : 0;
            if (menuArrowWidth > 0)
            {
                opt.text += QChar(' ');
                opt.text += QChar(' ');
            }
        }
    }
    style()->drawComplexControl(QStyle::CC_ToolButton, &opt, &p, this);
}

void RibbonButton::actionEvent(QActionEvent* event)
{
    QToolButton::actionEvent(event);
    switch (event->type()) 
    {
        case QEvent::ActionChanged:
            m_sizeHint = QSize();
            break;
        case QEvent::ActionAdded:
            m_sizeHint = QSize();
            break;
        case QEvent::ActionRemoved:
            m_sizeHint = QSize();
            break;
        default:
            break;
    }
}

/* RibbonTitleButton */
RibbonTitleButton::RibbonTitleButton(QWidget* parent, QStyle::SubControl subControl)
    : QToolButton(parent)
{
    m_subControl = subControl;
    setAttribute(Qt::WA_Hover, true);
}

RibbonTitleButton::~RibbonTitleButton()
{
}

QStyle::SubControl RibbonTitleButton::subControl() const
{
    return m_subControl;
}

void RibbonTitleButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    StyleCaptionButton opt;
    initStyleOption(&opt);
    opt.activeSubControls = m_subControl;
    style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
}
