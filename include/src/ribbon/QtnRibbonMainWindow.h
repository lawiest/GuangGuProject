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
#ifndef QTN_RIBBONMAINWINDOW_H
#define QTN_RIBBONMAINWINDOW_H

#include <QMainWindow>

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class StatusBar;
class RibbonBar;

class QTITAN_EXPORT RibbonMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit RibbonMainWindow(QWidget* parent = Q_NULL, Qt::WindowFlags flags = 0);
    ~RibbonMainWindow();

public:
    RibbonBar* ribbonBar() const;
    void setRibbonBar(RibbonBar* ribbonBar);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    using QMainWindow::setCentralWidget;
    void setCentralWidget(QStyle* style);
#endif
private:
    Q_DISABLE_COPY(RibbonMainWindow)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONMAINWINDOW_H
