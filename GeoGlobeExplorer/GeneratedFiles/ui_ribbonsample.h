/********************************************************************************
** Form generated from reading UI file 'ribbonsample.ui'
**
** Created by: Qt User Interface Compiler version 5.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RIBBONSAMPLE_H
#define UI_RIBBONSAMPLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Styles
{
public:
    QSplitter *splitter;
    QToolButton *toolButton;
    QToolButton *toolButton_2;
    QToolButton *toolButton_3;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QListView *listView;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;

    void setupUi(QDialog *Styles)
    {
        if (Styles->objectName().isEmpty())
            Styles->setObjectName(QStringLiteral("Styles"));
        Styles->resize(223, 446);
        splitter = new QSplitter(Styles);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setGeometry(QRect(10, 420, 85, 19));
        splitter->setOrientation(Qt::Horizontal);
        toolButton = new QToolButton(splitter);
        toolButton->setObjectName(QStringLiteral("toolButton"));
        splitter->addWidget(toolButton);
        toolButton_2 = new QToolButton(splitter);
        toolButton_2->setObjectName(QStringLiteral("toolButton_2"));
        splitter->addWidget(toolButton_2);
        toolButton_3 = new QToolButton(splitter);
        toolButton_3->setObjectName(QStringLiteral("toolButton_3"));
        splitter->addWidget(toolButton_3);
        layoutWidget = new QWidget(Styles);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 221, 411));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(4, 4, 4, 4);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        listView = new QListView(layoutWidget);
        listView->setObjectName(QStringLiteral("listView"));

        verticalLayout->addWidget(listView);

        checkBox = new QCheckBox(layoutWidget);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        verticalLayout->addWidget(checkBox);

        checkBox_2 = new QCheckBox(layoutWidget);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));

        verticalLayout->addWidget(checkBox_2);


        retranslateUi(Styles);

        QMetaObject::connectSlotsByName(Styles);
    } // setupUi

    void retranslateUi(QDialog *Styles)
    {
        Styles->setWindowTitle(QApplication::translate("Styles", "Dialog", Q_NULLPTR));
        toolButton->setText(QApplication::translate("Styles", "...", Q_NULLPTR));
        toolButton_2->setText(QApplication::translate("Styles", "...", Q_NULLPTR));
        toolButton_3->setText(QApplication::translate("Styles", "...", Q_NULLPTR));
        checkBox->setText(QApplication::translate("Styles", "Show Preview", Q_NULLPTR));
        checkBox_2->setText(QApplication::translate("Styles", "Disable Linked Styles", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Styles: public Ui_Styles {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RIBBONSAMPLE_H
