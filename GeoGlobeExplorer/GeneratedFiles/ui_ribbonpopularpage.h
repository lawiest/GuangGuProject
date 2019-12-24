/********************************************************************************
** Form generated from reading UI file 'ribbonpopularpage.ui'
**
** Created by: Qt User Interface Compiler version 5.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RIBBONPOPULARPAGE_H
#define UI_RIBBONPOPULARPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_customizePopularPage
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *labelTitle;
    QFrame *frame;
    QLabel *labelTheme;
    QComboBox *comboBoxTheme;
    QLabel *labelAccentColor;
    QPushButton *pushButtonAccentColor;
    QLabel *labelBackgroundImage;
    QComboBox *comboBoxBackground;
    QFrame *frameAccentColor;

    void setupUi(QWidget *customizePopularPage)
    {
        if (customizePopularPage->objectName().isEmpty())
            customizePopularPage->setObjectName(QStringLiteral("customizePopularPage"));
        customizePopularPage->resize(600, 380);
        customizePopularPage->setMinimumSize(QSize(600, 380));
        QIcon icon;
        icon.addFile(QStringLiteral(":/res/popularpage.png"), QSize(), QIcon::Normal, QIcon::Off);
        customizePopularPage->setWindowIcon(icon);
        horizontalLayout = new QHBoxLayout(customizePopularPage);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout->setContentsMargins(-1, 0, -1, 0);
        labelTitle = new QLabel(customizePopularPage);
        labelTitle->setObjectName(QStringLiteral("labelTitle"));
        labelTitle->setFrameShape(QFrame::StyledPanel);
        labelTitle->setFrameShadow(QFrame::Plain);
        labelTitle->setMargin(4);

        verticalLayout->addWidget(labelTitle);

        frame = new QFrame(customizePopularPage);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        labelTheme = new QLabel(frame);
        labelTheme->setObjectName(QStringLiteral("labelTheme"));
        labelTheme->setGeometry(QRect(40, 30, 81, 16));
        comboBoxTheme = new QComboBox(frame);
        comboBoxTheme->setObjectName(QStringLiteral("comboBoxTheme"));
        comboBoxTheme->setGeometry(QRect(150, 28, 219, 23));
        labelAccentColor = new QLabel(frame);
        labelAccentColor->setObjectName(QStringLiteral("labelAccentColor"));
        labelAccentColor->setGeometry(QRect(40, 63, 171, 16));
        pushButtonAccentColor = new QPushButton(frame);
        pushButtonAccentColor->setObjectName(QStringLiteral("pushButtonAccentColor"));
        pushButtonAccentColor->setGeometry(QRect(353, 61, 17, 23));
        labelBackgroundImage = new QLabel(frame);
        labelBackgroundImage->setObjectName(QStringLiteral("labelBackgroundImage"));
        labelBackgroundImage->setGeometry(QRect(40, 98, 191, 16));
        comboBoxBackground = new QComboBox(frame);
        comboBoxBackground->setObjectName(QStringLiteral("comboBoxBackground"));
        comboBoxBackground->setGeometry(QRect(246, 96, 124, 23));
        frameAccentColor = new QFrame(frame);
        frameAccentColor->setObjectName(QStringLiteral("frameAccentColor"));
        frameAccentColor->setGeometry(QRect(246, 61, 105, 23));
        frameAccentColor->setFrameShape(QFrame::StyledPanel);
        frameAccentColor->setFrameShadow(QFrame::Plain);

        verticalLayout->addWidget(frame);

        verticalLayout->setStretch(1, 1);

        horizontalLayout->addLayout(verticalLayout);

#ifndef QT_NO_SHORTCUT
        labelTheme->setBuddy(comboBoxTheme);
        labelAccentColor->setBuddy(pushButtonAccentColor);
        labelBackgroundImage->setBuddy(comboBoxBackground);
#endif // QT_NO_SHORTCUT

        retranslateUi(customizePopularPage);

        QMetaObject::connectSlotsByName(customizePopularPage);
    } // setupUi

    void retranslateUi(QWidget *customizePopularPage)
    {
        customizePopularPage->setWindowTitle(QApplication::translate("customizePopularPage", "Popular", Q_NULLPTR));
#ifndef QT_NO_STATUSTIP
        customizePopularPage->setStatusTip(QApplication::translate("customizePopularPage", "Change the most popular options", Q_NULLPTR));
#endif // QT_NO_STATUSTIP
        labelTitle->setText(QApplication::translate("customizePopularPage", "<html><head/><body><p><span style=\" font-weight:600;\">Top options for Working with Demo</span></p></body></html>", Q_NULLPTR));
        labelTheme->setText(QApplication::translate("customizePopularPage", "&Theme:", Q_NULLPTR));
        labelAccentColor->setText(QApplication::translate("customizePopularPage", "Accen&t Color (Office 2013 only):", Q_NULLPTR));
        pushButtonAccentColor->setText(QString());
        labelBackgroundImage->setText(QApplication::translate("customizePopularPage", "&Background Image (Office 2013 only):", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class customizePopularPage: public Ui_customizePopularPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RIBBONPOPULARPAGE_H
