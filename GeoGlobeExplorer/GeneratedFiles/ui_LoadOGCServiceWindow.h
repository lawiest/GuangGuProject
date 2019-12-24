/********************************************************************************
** Form generated from reading UI file 'LoadOGCServiceWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOADOGCSERVICEWINDOW_H
#define UI_LOADOGCSERVICEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_loadOGCServiceWindow
{
public:
    QPushButton *prePushButton;
    QPushButton *nextPushButton;
    QPushButton *okPushButton;
    QWidget *OGCWidget;
    QLabel *label_4;
    QLabel *label_3;
    QLineEdit *urlLineEdit;
    QLabel *label_2;
    QLabel *label;
    QGroupBox *groupBox;
    QLabel *label_5;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_3;
    QLabel *label_6;
    QCheckBox *checkBox;
    QLabel *label_12;
    QComboBox *serverTypeCB;

    void setupUi(QWidget *loadOGCServiceWindow)
    {
        if (loadOGCServiceWindow->objectName().isEmpty())
            loadOGCServiceWindow->setObjectName(QStringLiteral("loadOGCServiceWindow"));
        loadOGCServiceWindow->resize(540, 387);
        loadOGCServiceWindow->setStyleSheet(QString::fromUtf8("QMenu::item:selected { /* when user selects item using mouse or keyboard */\n"
"     background-color: #2dabf9;/*\350\277\231\344\270\200\345\217\245\346\230\257\350\256\276\347\275\256\350\217\234\345\215\225\351\241\271\351\274\240\346\240\207\347\273\217\350\277\207\351\200\211\344\270\255\347\232\204\346\240\267\345\274\217*/\n"
" }\n"
"\n"
"QGroupBox {\n"
"\n"
"    border: 1px solid gray;\n"
"    border-radius: 5px;\n"
"    margin-top: 1ex; /* leave space at the top for the title */\n"
"}\n"
""));
        prePushButton = new QPushButton(loadOGCServiceWindow);
        prePushButton->setObjectName(QStringLiteral("prePushButton"));
        prePushButton->setGeometry(QRect(220, 340, 75, 23));
        nextPushButton = new QPushButton(loadOGCServiceWindow);
        nextPushButton->setObjectName(QStringLiteral("nextPushButton"));
        nextPushButton->setGeometry(QRect(320, 340, 75, 23));
        okPushButton = new QPushButton(loadOGCServiceWindow);
        okPushButton->setObjectName(QStringLiteral("okPushButton"));
        okPushButton->setGeometry(QRect(420, 340, 75, 23));
        OGCWidget = new QWidget(loadOGCServiceWindow);
        OGCWidget->setObjectName(QStringLiteral("OGCWidget"));
        OGCWidget->setGeometry(QRect(30, 40, 481, 281));
        OGCWidget->setAutoFillBackground(false);
        OGCWidget->setStyleSheet(QStringLiteral("background-color: rgb(255, 255, 255);"));
        label_4 = new QLabel(OGCWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(90, 140, 361, 16));
        label_3 = new QLabel(OGCWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 110, 61, 16));
        urlLineEdit = new QLineEdit(OGCWidget);
        urlLineEdit->setObjectName(QStringLiteral("urlLineEdit"));
        urlLineEdit->setGeometry(QRect(90, 110, 381, 20));
        urlLineEdit->setFrame(true);
        label_2 = new QLabel(OGCWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 40, 311, 21));
        label = new QLabel(OGCWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 0, 151, 41));
        QFont font;
        font.setPointSize(13);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setLineWidth(2);
        groupBox = new QGroupBox(OGCWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(30, 170, 441, 111));
        groupBox->setStyleSheet(QStringLiteral(""));
        groupBox->setCheckable(true);
        groupBox->setChecked(false);
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 20, 54, 12));
        lineEdit_2 = new QLineEdit(groupBox);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setEnabled(false);
        lineEdit_2->setGeometry(QRect(60, 20, 371, 20));
        lineEdit_3 = new QLineEdit(groupBox);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(60, 50, 371, 20));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 50, 41, 16));
        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(60, 80, 131, 16));
        label_12 = new QLabel(OGCWidget);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(30, 70, 61, 16));
        serverTypeCB = new QComboBox(OGCWidget);
        serverTypeCB->setObjectName(QStringLiteral("serverTypeCB"));
        serverTypeCB->setGeometry(QRect(90, 70, 381, 22));

        retranslateUi(loadOGCServiceWindow);

        QMetaObject::connectSlotsByName(loadOGCServiceWindow);
    } // setupUi

    void retranslateUi(QWidget *loadOGCServiceWindow)
    {
        loadOGCServiceWindow->setWindowTitle(QApplication::translate("loadOGCServiceWindow", "Loadgeotileservicewindow", Q_NULLPTR));
        prePushButton->setText(QApplication::translate("loadOGCServiceWindow", "\344\270\212\344\270\200\346\255\245", Q_NULLPTR));
        nextPushButton->setText(QApplication::translate("loadOGCServiceWindow", "\344\270\213\344\270\200\346\255\245", Q_NULLPTR));
        okPushButton->setText(QApplication::translate("loadOGCServiceWindow", "\347\241\256\345\256\232", Q_NULLPTR));
        label_4->setText(QApplication::translate("loadOGCServiceWindow", "<html><head/><body><p>\347\244\272\344\276\213\357\274\232http://t0.tianditu.com/img_c/wmts</p></body></html>", Q_NULLPTR));
        label_3->setText(QApplication::translate("loadOGCServiceWindow", "\346\267\273\345\212\240\346\234\215\345\212\241\357\274\232", Q_NULLPTR));
        label_2->setText(QApplication::translate("loadOGCServiceWindow", "\351\200\211\346\213\251\346\234\215\345\212\241\347\261\273\345\236\213\345\271\266\346\214\207\345\256\232\345\217\257\345\234\250internet\344\270\212\345\224\257\344\270\200\346\240\207\347\244\272\346\234\215\345\212\241\345\214\272\347\232\204URL", Q_NULLPTR));
        label->setText(QApplication::translate("loadOGCServiceWindow", "\346\267\273\345\212\240\345\200\276\346\226\234\346\234\215\345\212\241", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("loadOGCServiceWindow", "\350\264\246\345\217\267(\345\217\257\351\200\211)", Q_NULLPTR));
        label_5->setText(QApplication::translate("loadOGCServiceWindow", "\347\224\250\346\210\267\345\220\215\357\274\232", Q_NULLPTR));
        label_6->setText(QApplication::translate("loadOGCServiceWindow", "\345\257\206  \347\240\201\357\274\232", Q_NULLPTR));
        checkBox->setText(QApplication::translate("loadOGCServiceWindow", "\344\277\235\345\255\230\347\224\250\346\210\267\345\220\215\345\222\214\345\257\206\347\240\201", Q_NULLPTR));
        label_12->setText(QApplication::translate("loadOGCServiceWindow", "\346\234\215\345\212\241\347\261\273\345\236\213\357\274\232", Q_NULLPTR));
        serverTypeCB->clear();
        serverTypeCB->insertItems(0, QStringList()
         << QApplication::translate("loadOGCServiceWindow", "OGC WMTS\346\234\215\345\212\241", Q_NULLPTR)
         << QApplication::translate("loadOGCServiceWindow", "OGC WMS\346\234\215\345\212\241", Q_NULLPTR)
         << QApplication::translate("loadOGCServiceWindow", "OGC WFS\346\234\215\345\212\241", Q_NULLPTR)
         << QApplication::translate("loadOGCServiceWindow", "OSGB \345\200\276\346\226\234\346\234\215\345\212\241", Q_NULLPTR)
         << QApplication::translate("loadOGCServiceWindow", "\345\220\211\345\245\245\347\223\246\347\211\207\346\234\215\345\212\241", Q_NULLPTR)
        );
    } // retranslateUi

};

namespace Ui {
    class loadOGCServiceWindow: public Ui_loadOGCServiceWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOADOGCSERVICEWINDOW_H
