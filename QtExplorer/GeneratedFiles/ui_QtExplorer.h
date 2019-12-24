/********************************************************************************
** Form generated from reading UI file 'QtExplorer.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTEXPLORER_H
#define UI_QTEXPLORER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtExplorerClass
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *toolButton;
    QToolButton *saveWSToolButton;
    QToolButton *openWSToolButton;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label;
    QSpacerItem *horizontalSpacer_3;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QTabWidget *mainTabWidget;
    QWidget *mainpage;
    QHBoxLayout *horizontalLayout_3;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *menuFilePushButton;
    QToolButton *loadLocalDataToolButton;
    QToolButton *loadServerToolButton;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_6;
    QToolButton *selectToolButton;
    QToolButton *polygonQueryToolButton;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_7;
    QToolButton *addFlyPointToolButton;
    QToolButton *startFlyToolButton;
    QToolButton *stopFlyToolButton;
    QToolButton *saveFlyToolButton;
    QToolButton *loadFlyToolButton;
    QGroupBox *groupBox_6;
    QHBoxLayout *horizontalLayout_8;
    QToolButton *snapShootToolButton;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_2;
    QCheckBox *layerManageCheckBox;
    QCheckBox *birdsEyecheckBox;
    QCheckBox *lonLatInfoCheckBox;
    QCheckBox *compassCheckBox;
    QCheckBox *atmosphereCheckBox;
    QSpacerItem *horizontalSpacer;
    QWidget *analysispage;
    QHBoxLayout *horizontalLayout_9;
    QGroupBox *groupBox_7;
    QHBoxLayout *horizontalLayout_10;
    QToolButton *observerPointsInRangeAnalysisToolButton;
    QToolButton *lineOfSightAnalysisToolButton;
    QGroupBox *groupBox_15;
    QHBoxLayout *horizontalLayout_19;
    QToolButton *areaAnalysisToolButton;
    QToolButton *heightAnalysisToolButton;
    QGroupBox *groupBox_8;
    QHBoxLayout *horizontalLayout_11;
    QToolButton *clearAnalysisToolButton;
    QSpacerItem *horizontalSpacer_4;
    QWidget *sunAnalysisTab;
    QHBoxLayout *horizontalLayout_22;
    QGroupBox *groupBox_12;
    QHBoxLayout *horizontalLayout_16;
    QToolButton *emuToolButton;
    QToolButton *coverToolButton;
    QToolButton *coneAnalysisToolButton;
    QToolButton *regionToolButton;
    QToolButton *sunConfigToolButton;
    QSpacerItem *horizontalSpacer_8;
    QWidget *DemolitionAnalysisTab;
    QHBoxLayout *horizontalLayout_23;
    QGroupBox *groupBox_11;
    QHBoxLayout *horizontalLayout_15;
    QToolButton *relocationToolButton;
    QToolButton *relocationResultToolButton;
    QToolButton *revokeToolButton;
    QSpacerItem *horizontalSpacer_9;
    QWidget *compareAnalysisTab;
    QHBoxLayout *horizontalLayout_24;
    QGroupBox *groupBox_13;
    QHBoxLayout *horizontalLayout_17;
    QToolButton *CompareCfgToolButton;
    QToolButton *splitScreenToolButton;
    QSpacerItem *horizontalSpacer_10;
    QWidget *asisPage;
    QHBoxLayout *horizontalLayout_21;
    QGroupBox *groupBox_14;
    QHBoxLayout *horizontalLayout_18;
    QToolButton *rectSeleteToolButton;
    QToolButton *AddCModelButton;
    QToolButton *deleteCModeltoolButton;
    QToolButton *seletBuildToolButton;
    QToolButton *modifyTexToolButton;
    QToolButton *transToolButton;
    QToolButton *rotateToolButton;
    QToolButton *stretchToolButton;
    QToolButton *analysisToolButton;
    QToolButton *sunEmuToolButton;
    QToolButton *canceltoolButton;
    QSpacerItem *horizontalSpacer_7;
    QWidget *particlePage;
    QHBoxLayout *horizontalLayout_20;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_4;
    QCheckBox *smokeCheckBox;
    QCheckBox *fireCheckBox;
    QGroupBox *groupBox_151;
    QHBoxLayout *horizontalLayout_191;
    QRadioButton *smallRadioBtn;
    QRadioButton *middleRadioBtn;
    QRadioButton *heavyRadioBtn;
    QCheckBox *rainCheckBox;
    QCheckBox *snowCheckBox;
    QGroupBox *groupBox_9;
    QHBoxLayout *horizontalLayout_12;
    QToolButton *playMusic;
    QSpacerItem *horizontalSpacer_6;

    void setupUi(QWidget *QtExplorerClass)
    {
        if (QtExplorerClass->objectName().isEmpty())
            QtExplorerClass->setObjectName(QStringLiteral("QtExplorerClass"));
        QtExplorerClass->setEnabled(true);
        QtExplorerClass->resize(1344, 177);
        QtExplorerClass->setStyleSheet(QString::fromUtf8("/*QMenu {\n"
"	background-image: url(:/QMainWindow/Resources/defaultbkg.jpg);\n"
"    background-color: #FCFCFC;\n"
"    border: 1px solid #8492A6;\n"
"}*/\n"
"\n"
"QMenu {\n"
"	background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                stop: 0 #FFFFFF, stop: 0.5 #DCE6F4,\n"
"                                stop: 0.95 #DCE6F4, stop: 1.0 #E5F0FB);\n"
"}\n"
"\n"
"MenuItemWidget,QMenu::item {\n"
"     /* sets background of menu item. set this to something non-transparent\n"
"         if you want menu color and menu item color to be different */\n"
"     background-color: transparent;\n"
"     padding:8px 35px;/*\350\256\276\347\275\256\350\217\234\345\215\225\351\241\271\346\226\207\345\255\227\344\270\212\344\270\213\345\222\214\345\267\246\345\217\263\347\232\204\345\206\205\350\276\271\350\267\235\357\274\214\346\225\210\346\236\234\345\260\261\346\230\257\350\217\234\345\215\225\344\270\255\347\232\204\346\235\241\347\233\256\345\267\246\345\217\263\344\270\212\344\270\213\346"
                        "\234\211\344\272\206\351\227\264\351\232\224*/\n"
"     margin:0px 4px;/*\350\256\276\347\275\256\350\217\234\345\215\225\351\241\271\347\232\204\345\244\226\350\276\271\350\267\235*/\n"
"     border-bottom:1px solid #DBDBDB;/*\344\270\272\350\217\234\345\215\225\351\241\271\344\271\213\351\227\264\346\267\273\345\212\240\346\250\252\347\272\277\351\227\264\351\232\224*/\n"
" }\n"
"  \n"
" MenuItemWidget::hover,QMenu::item:selected { /* when user selects item using mouse or keyboard */\n"
"     background-color: #2dabf9;/*\350\277\231\344\270\200\345\217\245\346\230\257\350\256\276\347\275\256\350\217\234\345\215\225\351\241\271\351\274\240\346\240\207\347\273\217\350\277\207\351\200\211\344\270\255\347\232\204\346\240\267\345\274\217*/\n"
" }\n"
"\n"
"/*MenuItemWidget::hover{\n"
"	border: 1px solid #FFB700;\n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FEF9F4, stop:0.38 #FDE0BD,stop:0.39 #FFCE69, stop:1 #FFFFE7);\n"
"}*/\n"
"\n"
"/*QMenu::item:selected { \n"
"    background"
                        "-color: rgba(220, 224, 231,100);\n"
"}*/\n"
"\n"
"#QMainWindowClass{\n"
"	background-color:rgb(255, 255, 255);\n"
"}\n"
"\n"
"#menuFilePushButton::menu-indicator{\n"
"	image:None;\n"
"}\n"
"\n"
"QWidget{\n"
"	font-family: \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"}\n"
""));
        gridLayout = new QGridLayout(QtExplorerClass);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(2, 2, 1, 2);
        widget_2 = new QWidget(QtExplorerClass);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy);
        horizontalLayout_4 = new QHBoxLayout(widget_2);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 90, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        toolButton = new QToolButton(widget_2);
        toolButton->setObjectName(QStringLiteral("toolButton"));
        toolButton->setEnabled(true);
        toolButton->setStyleSheet(QStringLiteral("background: transparent;"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/QMainWindow/Resources/icon/main.ico"), QSize(), QIcon::Normal, QIcon::On);
        icon.addFile(QStringLiteral(":/QMainWindow/Resources/main.ico"), QSize(), QIcon::Disabled, QIcon::Off);
        icon.addFile(QStringLiteral(":/QMainWindow/Resources/main.ico"), QSize(), QIcon::Disabled, QIcon::On);
        toolButton->setIcon(icon);
        toolButton->setCheckable(false);

        horizontalLayout_2->addWidget(toolButton);

        saveWSToolButton = new QToolButton(widget_2);
        saveWSToolButton->setObjectName(QStringLiteral("saveWSToolButton"));
        saveWSToolButton->setStyleSheet(QLatin1String("QToolButton{background: transparent;}\n"
"\n"
"QToolButton::hover{\n"
"	background-color:rgba(220, 224, 231,150);\n"
"	border:0px;\n"
"}"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/QMainWindow/Resources/icon/save.png"), QSize(), QIcon::Normal, QIcon::On);
        icon1.addFile(QStringLiteral(":/QMainWindow/Resources/save.png"), QSize(), QIcon::Disabled, QIcon::Off);
        icon1.addFile(QStringLiteral(":/QMainWindow/Resources/save.png"), QSize(), QIcon::Disabled, QIcon::On);
        saveWSToolButton->setIcon(icon1);

        horizontalLayout_2->addWidget(saveWSToolButton);

        openWSToolButton = new QToolButton(widget_2);
        openWSToolButton->setObjectName(QStringLiteral("openWSToolButton"));
        openWSToolButton->setStyleSheet(QLatin1String("QToolButton{background: transparent;}\n"
"QToolButton::hover{background-color:rgba(220, 224, 231,150);border:0px;}"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/QMainWindow/Resources/icon/open.png"), QSize(), QIcon::Normal, QIcon::On);
        icon2.addFile(QStringLiteral(":/QMainWindow/Resources/open.png"), QSize(), QIcon::Disabled, QIcon::Off);
        icon2.addFile(QStringLiteral(":/QMainWindow/Resources/open.png"), QSize(), QIcon::Disabled, QIcon::On);
        icon2.addFile(QStringLiteral(":/QMainWindow/Resources/open.png"), QSize(), QIcon::Selected, QIcon::Off);
        icon2.addFile(QStringLiteral(":/QMainWindow/Resources/open.png"), QSize(), QIcon::Selected, QIcon::On);
        openWSToolButton->setIcon(icon2);

        horizontalLayout_2->addWidget(openWSToolButton);


        horizontalLayout_4->addLayout(horizontalLayout_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        label = new QLabel(widget_2);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(label);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        label->raise();

        gridLayout->addWidget(widget_2, 0, 0, 1, 1);

        widget = new QWidget(QtExplorerClass);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        mainTabWidget = new QTabWidget(widget);
        mainTabWidget->setObjectName(QStringLiteral("mainTabWidget"));
        mainTabWidget->setEnabled(true);
        sizePolicy.setHeightForWidth(mainTabWidget->sizePolicy().hasHeightForWidth());
        mainTabWidget->setSizePolicy(sizePolicy);
        mainTabWidget->setStyleSheet(QString::fromUtf8("#mainTabWidget::tab-bar {\n"
"    left: 52px;\n"
"}\n"
"#mainTabWidget QTabBar::tab{\n"
"	min-width:60px;\n"
"	max-width:60px;\n"
"	min-height:22px;\n"
"	max-height:22px;\n"
"}\n"
"#mainTabWidget QTabBar::tab {\n"
"	background: transparent;\n"
"	margin-left: 4px;\n"
"	margin-right: 4px;\n"
"}\n"
"#mainTabWidget QTabBar::tab:selected, QTabBar::tab:hover {\n"
"    border-top-left-radius: 2px;\n"
"    border-top-right-radius: 2px;\n"
"}\n"
"#mainTabWidget::tab-bar {\n"
" 	border: 1px solid #BAC9DB;\n"
"}\n"
"#mainTabWidget QTabBar::tab:selected{\n"
"	border: 1px solid #BAC9DB;\n"
"	background: white;\n"
"    border-bottom-color: #FFFFFF;\n"
"}\n"
"#mainTabWidget QTabBar::tab:hover:!selected{\n"
"	border: 1px solid #ECBC3D;\n"
"}\n"
"#mainTabWidget QTabBar::tab:!selected {\n"
"    margin-top: 1px; \n"
"}\n"
"\n"
"#mainTabWidget QGroupBox{\n"
"	border-left: 0px solid gray;\n"
"	border-top: 0px solid gray;\n"
"    margin:4px 0px 4px 0;/*\350\256\276\347\275\256\350\217\234\345\215\225\351\241\271\347\232\204\345\244"
                        "\226\350\276\271\350\267\235*/\n"
"	border-right:1px solid #D4D4D4;/*\344\270\272\350\217\234\345\215\225\351\241\271\344\271\213\351\227\264\346\267\273\345\212\240\346\250\252\347\272\277\351\227\264\351\232\224*/\n"
"}\n"
"\n"
"#mainTabWidget QToolButton{\n"
"	background-color:transparent;\n"
"	min-width:30px;\n"
"	min-height:64px;\n"
"}\n"
"\n"
"#mainTabWidget QToolButton:hover{\n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, \n"
"		stop:0 #ddf4ff, stop:0.5 #9de0ff,stop:0.51 #87d8ff, stop:1 #c6edff);\n"
"	border:0px;\n"
"	border-radius:5px;\n"
"}\n"
"\n"
"#mainTabWidget QToolButton:checked{\n"
"		background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, \n"
"		stop:0 #ddf4ff, stop:0.5 #9de0ff,stop:0.51 #87d8ff, stop:1 #c6edff);\n"
"\n"
"	border-radius:5px;\n"
"}  \n"
"\n"
"#mainTabWidget QToolButton::menu-button:hover {\n"
"    border-left: 1px solid #444444;\n"
"    background: transparent;\n"
"    width: 16px;\n"
"}\n"
"\n"
"#mainTabWidget QToolButton::menu-indicato"
                        "r {\n"
"    subcontrol-position: bottom center;\n"
"	position: relative;\n"
"    bottom : 0px;\n"
"}\n"
"\n"
"#mainTabWidget QGroupBox::title {\n"
"	color: #738399;\n"
"    subcontrol-position: bottom center; \n"
"}\n"
"\n"
"#mainTabWidget::pane{\n"
"	border: 1px solid #BAC9DB;\n"
"	background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                stop: 0 #FFFFFF, stop: 0.5 #DCE6F4,\n"
"                                stop: 0.95 #DCE6F4, stop: 1.0 #E5F0FB);\n"
"	top: -1px;\n"
"}"));
        mainpage = new QWidget();
        mainpage->setObjectName(QStringLiteral("mainpage"));
        horizontalLayout_3 = new QHBoxLayout(mainpage);
        horizontalLayout_3->setSpacing(4);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        groupBox = new QGroupBox(mainpage);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy2);
        horizontalLayout_5 = new QHBoxLayout(groupBox);
        horizontalLayout_5->setSpacing(3);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 3, 13);
        menuFilePushButton = new QPushButton(groupBox);
        menuFilePushButton->setObjectName(QStringLiteral("menuFilePushButton"));
        menuFilePushButton->setStyleSheet(QLatin1String("#menuFilePushButton{\n"
"	min-width:50px;\n"
"	max-width:50px;\n"
"	min-height:22px;\n"
"	max-height:22px;\n"
"}\n"
"#menuFilePushButton{\n"
"	color:white;\n"
"	border: 1px solid #416ABD;\n"
"    border-top-left-radius: 2px;\n"
"    border-top-right-radius: 2px;\n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, \n"
"		stop:0 #467FBD, stop:0.5 #2A5FAC,stop:0.51 #1A4088, stop:1 #419ACF);\n"
"}\n"
"#menuFilePushButton::hover{\n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, \n"
"		stop:0 #7BB2EB, stop:0.5 #477ECD,stop:0.51 #114ECF, stop:1 #80E1FF);\n"
"}\n"
"#menuFilePushButton::pressed{\n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, \n"
"		stop:0 #467BBB, stop:0.5 #2F63AE,stop:0.51 #1C438A, stop:1 #358BC9);\n"
"}"));

        horizontalLayout_5->addWidget(menuFilePushButton);

        loadLocalDataToolButton = new QToolButton(groupBox);
        loadLocalDataToolButton->setObjectName(QStringLiteral("loadLocalDataToolButton"));
        loadLocalDataToolButton->setMinimumSize(QSize(32, 66));
        loadLocalDataToolButton->setStyleSheet(QStringLiteral(""));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/QMainWindow/Resources/icon/localData.png"), QSize(), QIcon::Normal, QIcon::On);
        loadLocalDataToolButton->setIcon(icon3);
        loadLocalDataToolButton->setIconSize(QSize(32, 32));
        loadLocalDataToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_5->addWidget(loadLocalDataToolButton);

        loadServerToolButton = new QToolButton(groupBox);
        loadServerToolButton->setObjectName(QStringLiteral("loadServerToolButton"));
        loadServerToolButton->setMinimumSize(QSize(32, 66));
        loadServerToolButton->setStyleSheet(QStringLiteral(""));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/QMainWindow/Resources/icon/Service.png"), QSize(), QIcon::Normal, QIcon::On);
        loadServerToolButton->setIcon(icon4);
        loadServerToolButton->setIconSize(QSize(32, 32));
        loadServerToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_5->addWidget(loadServerToolButton);


        horizontalLayout_3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(mainpage);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        horizontalLayout_6 = new QHBoxLayout(groupBox_2);
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 3, 13);
        selectToolButton = new QToolButton(groupBox_2);
        selectToolButton->setObjectName(QStringLiteral("selectToolButton"));
        selectToolButton->setMinimumSize(QSize(32, 66));
        selectToolButton->setStyleSheet(QStringLiteral(""));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/QMainWindow/Resources/icon/Select.png"), QSize(), QIcon::Normal, QIcon::On);
        selectToolButton->setIcon(icon5);
        selectToolButton->setIconSize(QSize(32, 32));
        selectToolButton->setCheckable(true);
        selectToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_6->addWidget(selectToolButton);

        polygonQueryToolButton = new QToolButton(groupBox_2);
        polygonQueryToolButton->setObjectName(QStringLiteral("polygonQueryToolButton"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/QMainWindow/Resources/icon/PolygonQuery.png"), QSize(), QIcon::Normal, QIcon::On);
        polygonQueryToolButton->setIcon(icon6);
        polygonQueryToolButton->setIconSize(QSize(32, 32));
        polygonQueryToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_6->addWidget(polygonQueryToolButton);


        horizontalLayout_3->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(mainpage);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        horizontalLayout_7 = new QHBoxLayout(groupBox_3);
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 3, 9);
        addFlyPointToolButton = new QToolButton(groupBox_3);
        addFlyPointToolButton->setObjectName(QStringLiteral("addFlyPointToolButton"));
        addFlyPointToolButton->setMinimumSize(QSize(32, 66));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/QMainWindow/Resources/icon/NewFlyNode.png"), QSize(), QIcon::Normal, QIcon::On);
        addFlyPointToolButton->setIcon(icon7);
        addFlyPointToolButton->setIconSize(QSize(32, 32));
        addFlyPointToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_7->addWidget(addFlyPointToolButton);

        startFlyToolButton = new QToolButton(groupBox_3);
        startFlyToolButton->setObjectName(QStringLiteral("startFlyToolButton"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/QMainWindow/Resources/icon/Flying.png"), QSize(), QIcon::Normal, QIcon::On);
        startFlyToolButton->setIcon(icon8);
        startFlyToolButton->setIconSize(QSize(32, 32));
        startFlyToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_7->addWidget(startFlyToolButton);

        stopFlyToolButton = new QToolButton(groupBox_3);
        stopFlyToolButton->setObjectName(QStringLiteral("stopFlyToolButton"));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/QMainWindow/Resources/icon/stopfly.png"), QSize(), QIcon::Normal, QIcon::On);
        stopFlyToolButton->setIcon(icon9);
        stopFlyToolButton->setIconSize(QSize(32, 32));
        stopFlyToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_7->addWidget(stopFlyToolButton);

        saveFlyToolButton = new QToolButton(groupBox_3);
        saveFlyToolButton->setObjectName(QStringLiteral("saveFlyToolButton"));
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/QMainWindow/Resources/icon/savefly.png"), QSize(), QIcon::Normal, QIcon::On);
        saveFlyToolButton->setIcon(icon10);
        saveFlyToolButton->setIconSize(QSize(32, 32));
        saveFlyToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_7->addWidget(saveFlyToolButton);

        loadFlyToolButton = new QToolButton(groupBox_3);
        loadFlyToolButton->setObjectName(QStringLiteral("loadFlyToolButton"));
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/QMainWindow/Resources/icon/loadfly.png"), QSize(), QIcon::Normal, QIcon::On);
        loadFlyToolButton->setIcon(icon11);
        loadFlyToolButton->setIconSize(QSize(32, 32));
        loadFlyToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_7->addWidget(loadFlyToolButton);


        horizontalLayout_3->addWidget(groupBox_3);

        groupBox_6 = new QGroupBox(mainpage);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        horizontalLayout_8 = new QHBoxLayout(groupBox_6);
        horizontalLayout_8->setSpacing(0);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(0, 0, 3, 9);
        snapShootToolButton = new QToolButton(groupBox_6);
        snapShootToolButton->setObjectName(QStringLiteral("snapShootToolButton"));
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/QMainWindow/Resources/icon/Snapshot.png"), QSize(), QIcon::Normal, QIcon::On);
        snapShootToolButton->setIcon(icon12);
        snapShootToolButton->setIconSize(QSize(32, 32));
        snapShootToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_8->addWidget(snapShootToolButton);


        horizontalLayout_3->addWidget(groupBox_6);

        groupBox_4 = new QGroupBox(mainpage);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_2 = new QGridLayout(groupBox_4);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 3, 13);
        layerManageCheckBox = new QCheckBox(groupBox_4);
        layerManageCheckBox->setObjectName(QStringLiteral("layerManageCheckBox"));

        gridLayout_2->addWidget(layerManageCheckBox, 1, 0, 1, 1);

        birdsEyecheckBox = new QCheckBox(groupBox_4);
        birdsEyecheckBox->setObjectName(QStringLiteral("birdsEyecheckBox"));

        gridLayout_2->addWidget(birdsEyecheckBox, 0, 0, 1, 1);

        lonLatInfoCheckBox = new QCheckBox(groupBox_4);
        lonLatInfoCheckBox->setObjectName(QStringLiteral("lonLatInfoCheckBox"));
        lonLatInfoCheckBox->setChecked(true);

        gridLayout_2->addWidget(lonLatInfoCheckBox, 0, 1, 1, 1);

        compassCheckBox = new QCheckBox(groupBox_4);
        compassCheckBox->setObjectName(QStringLiteral("compassCheckBox"));
        compassCheckBox->setChecked(true);

        gridLayout_2->addWidget(compassCheckBox, 1, 1, 1, 1);

        atmosphereCheckBox = new QCheckBox(groupBox_4);
        atmosphereCheckBox->setObjectName(QStringLiteral("atmosphereCheckBox"));
        atmosphereCheckBox->setChecked(true);

        gridLayout_2->addWidget(atmosphereCheckBox, 2, 0, 1, 1);


        horizontalLayout_3->addWidget(groupBox_4);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        mainTabWidget->addTab(mainpage, QString());
        analysispage = new QWidget();
        analysispage->setObjectName(QStringLiteral("analysispage"));
        horizontalLayout_9 = new QHBoxLayout(analysispage);
        horizontalLayout_9->setSpacing(4);
        horizontalLayout_9->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalLayout_9->setContentsMargins(0, 0, 0, 0);
        groupBox_7 = new QGroupBox(analysispage);
        groupBox_7->setObjectName(QStringLiteral("groupBox_7"));
        groupBox_7->setMouseTracking(true);
        horizontalLayout_10 = new QHBoxLayout(groupBox_7);
        horizontalLayout_10->setSpacing(3);
        horizontalLayout_10->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalLayout_10->setContentsMargins(0, 0, 3, 13);
        observerPointsInRangeAnalysisToolButton = new QToolButton(groupBox_7);
        observerPointsInRangeAnalysisToolButton->setObjectName(QStringLiteral("observerPointsInRangeAnalysisToolButton"));
        QIcon icon13;
        icon13.addFile(QStringLiteral(":/QMainWindow/Resources/icon/SingleVisibility.png"), QSize(), QIcon::Normal, QIcon::On);
        observerPointsInRangeAnalysisToolButton->setIcon(icon13);
        observerPointsInRangeAnalysisToolButton->setIconSize(QSize(32, 32));
        observerPointsInRangeAnalysisToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_10->addWidget(observerPointsInRangeAnalysisToolButton);

        lineOfSightAnalysisToolButton = new QToolButton(groupBox_7);
        lineOfSightAnalysisToolButton->setObjectName(QStringLiteral("lineOfSightAnalysisToolButton"));
        QIcon icon14;
        icon14.addFile(QStringLiteral(":/QMainWindow/Resources/icon/TwoPass.png"), QSize(), QIcon::Normal, QIcon::On);
        lineOfSightAnalysisToolButton->setIcon(icon14);
        lineOfSightAnalysisToolButton->setIconSize(QSize(32, 32));
        lineOfSightAnalysisToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_10->addWidget(lineOfSightAnalysisToolButton);


        horizontalLayout_9->addWidget(groupBox_7);

        groupBox_15 = new QGroupBox(analysispage);
        groupBox_15->setObjectName(QStringLiteral("groupBox_15"));
        horizontalLayout_19 = new QHBoxLayout(groupBox_15);
        horizontalLayout_19->setSpacing(3);
        horizontalLayout_19->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_19->setObjectName(QStringLiteral("horizontalLayout_19"));
        horizontalLayout_19->setContentsMargins(0, 0, 3, 13);
        areaAnalysisToolButton = new QToolButton(groupBox_15);
        areaAnalysisToolButton->setObjectName(QStringLiteral("areaAnalysisToolButton"));
        QIcon icon15;
        icon15.addFile(QStringLiteral(":/QMainWindow/Resources/icon/areameasure.png"), QSize(), QIcon::Normal, QIcon::On);
        areaAnalysisToolButton->setIcon(icon15);
        areaAnalysisToolButton->setIconSize(QSize(32, 32));
        areaAnalysisToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_19->addWidget(areaAnalysisToolButton);

        heightAnalysisToolButton = new QToolButton(groupBox_15);
        heightAnalysisToolButton->setObjectName(QStringLiteral("heightAnalysisToolButton"));
        QIcon icon16;
        icon16.addFile(QStringLiteral(":/QMainWindow/Resources/icon/MeasuresVolume.png"), QSize(), QIcon::Normal, QIcon::On);
        heightAnalysisToolButton->setIcon(icon16);
        heightAnalysisToolButton->setIconSize(QSize(32, 32));
        heightAnalysisToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_19->addWidget(heightAnalysisToolButton);


        horizontalLayout_9->addWidget(groupBox_15);

        groupBox_8 = new QGroupBox(analysispage);
        groupBox_8->setObjectName(QStringLiteral("groupBox_8"));
        horizontalLayout_11 = new QHBoxLayout(groupBox_8);
        horizontalLayout_11->setSpacing(3);
        horizontalLayout_11->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        horizontalLayout_11->setContentsMargins(0, 0, 3, 13);
        clearAnalysisToolButton = new QToolButton(groupBox_8);
        clearAnalysisToolButton->setObjectName(QStringLiteral("clearAnalysisToolButton"));
        QIcon icon17;
        icon17.addFile(QStringLiteral(":/QMainWindow/Resources/icon/Clear.png"), QSize(), QIcon::Normal, QIcon::On);
        clearAnalysisToolButton->setIcon(icon17);
        clearAnalysisToolButton->setIconSize(QSize(32, 32));
        clearAnalysisToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_11->addWidget(clearAnalysisToolButton);


        horizontalLayout_9->addWidget(groupBox_8);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_4);

        mainTabWidget->addTab(analysispage, QString());
        sunAnalysisTab = new QWidget();
        sunAnalysisTab->setObjectName(QStringLiteral("sunAnalysisTab"));
        horizontalLayout_22 = new QHBoxLayout(sunAnalysisTab);
        horizontalLayout_22->setSpacing(6);
        horizontalLayout_22->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_22->setObjectName(QStringLiteral("horizontalLayout_22"));
        groupBox_12 = new QGroupBox(sunAnalysisTab);
        groupBox_12->setObjectName(QStringLiteral("groupBox_12"));
        horizontalLayout_16 = new QHBoxLayout(groupBox_12);
        horizontalLayout_16->setSpacing(3);
        horizontalLayout_16->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
        horizontalLayout_16->setContentsMargins(0, 3, 3, 13);
        emuToolButton = new QToolButton(groupBox_12);
        emuToolButton->setObjectName(QStringLiteral("emuToolButton"));
        QIcon icon18;
        icon18.addFile(QStringLiteral(":/QMainWindow/Resources/icon/Sunshine_48px_1183976_easyicon.net.png"), QSize(), QIcon::Normal, QIcon::On);
        emuToolButton->setIcon(icon18);
        emuToolButton->setIconSize(QSize(32, 32));
        emuToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_16->addWidget(emuToolButton);

        coverToolButton = new QToolButton(groupBox_12);
        coverToolButton->setObjectName(QStringLiteral("coverToolButton"));
        QIcon icon19;
        icon19.addFile(QStringLiteral(":/QMainWindow/Resources/icon/home_32px_1170780_easyicon.net.png"), QSize(), QIcon::Normal, QIcon::On);
        coverToolButton->setIcon(icon19);
        coverToolButton->setIconSize(QSize(32, 32));
        coverToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_16->addWidget(coverToolButton);

        coneAnalysisToolButton = new QToolButton(groupBox_12);
        coneAnalysisToolButton->setObjectName(QStringLiteral("coneAnalysisToolButton"));
        coneAnalysisToolButton->setIcon(icon13);
        coneAnalysisToolButton->setIconSize(QSize(32, 32));
        coneAnalysisToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_16->addWidget(coneAnalysisToolButton);

        regionToolButton = new QToolButton(groupBox_12);
        regionToolButton->setObjectName(QStringLiteral("regionToolButton"));
        QIcon icon20;
        icon20.addFile(QStringLiteral(":/QMainWindow/Resources/icon/region.png"), QSize(), QIcon::Normal, QIcon::On);
        regionToolButton->setIcon(icon20);
        regionToolButton->setIconSize(QSize(32, 32));
        regionToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_16->addWidget(regionToolButton);

        sunConfigToolButton = new QToolButton(groupBox_12);
        sunConfigToolButton->setObjectName(QStringLiteral("sunConfigToolButton"));
        QIcon icon21;
        icon21.addFile(QStringLiteral(":/QMainWindow/Resources/icon/Configure.png"), QSize(), QIcon::Normal, QIcon::On);
        sunConfigToolButton->setIcon(icon21);
        sunConfigToolButton->setIconSize(QSize(32, 32));
        sunConfigToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_16->addWidget(sunConfigToolButton);


        horizontalLayout_22->addWidget(groupBox_12);

        horizontalSpacer_8 = new QSpacerItem(894, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_22->addItem(horizontalSpacer_8);

        mainTabWidget->addTab(sunAnalysisTab, QString());
        DemolitionAnalysisTab = new QWidget();
        DemolitionAnalysisTab->setObjectName(QStringLiteral("DemolitionAnalysisTab"));
        horizontalLayout_23 = new QHBoxLayout(DemolitionAnalysisTab);
        horizontalLayout_23->setSpacing(6);
        horizontalLayout_23->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_23->setObjectName(QStringLiteral("horizontalLayout_23"));
        groupBox_11 = new QGroupBox(DemolitionAnalysisTab);
        groupBox_11->setObjectName(QStringLiteral("groupBox_11"));
        horizontalLayout_15 = new QHBoxLayout(groupBox_11);
        horizontalLayout_15->setSpacing(3);
        horizontalLayout_15->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        horizontalLayout_15->setContentsMargins(0, 0, 3, 13);
        relocationToolButton = new QToolButton(groupBox_11);
        relocationToolButton->setObjectName(QStringLiteral("relocationToolButton"));
        QIcon icon22;
        icon22.addFile(QStringLiteral(":/QMainWindow/Resources/icon/house.png"), QSize(), QIcon::Normal, QIcon::On);
        relocationToolButton->setIcon(icon22);
        relocationToolButton->setIconSize(QSize(32, 32));
        relocationToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_15->addWidget(relocationToolButton);

        relocationResultToolButton = new QToolButton(groupBox_11);
        relocationResultToolButton->setObjectName(QStringLiteral("relocationResultToolButton"));
        QIcon icon23;
        icon23.addFile(QStringLiteral(":/QMainWindow/Resources/icon/delete.png"), QSize(), QIcon::Normal, QIcon::On);
        relocationResultToolButton->setIcon(icon23);
        relocationResultToolButton->setIconSize(QSize(32, 32));
        relocationResultToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_15->addWidget(relocationResultToolButton);

        revokeToolButton = new QToolButton(groupBox_11);
        revokeToolButton->setObjectName(QStringLiteral("revokeToolButton"));
        QIcon icon24;
        icon24.addFile(QStringLiteral(":/QMainWindow/Resources/icon/revoke.png"), QSize(), QIcon::Normal, QIcon::On);
        revokeToolButton->setIcon(icon24);
        revokeToolButton->setIconSize(QSize(32, 32));
        revokeToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_15->addWidget(revokeToolButton);


        horizontalLayout_23->addWidget(groupBox_11);

        horizontalSpacer_9 = new QSpacerItem(1152, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_23->addItem(horizontalSpacer_9);

        mainTabWidget->addTab(DemolitionAnalysisTab, QString());
        compareAnalysisTab = new QWidget();
        compareAnalysisTab->setObjectName(QStringLiteral("compareAnalysisTab"));
        horizontalLayout_24 = new QHBoxLayout(compareAnalysisTab);
        horizontalLayout_24->setSpacing(6);
        horizontalLayout_24->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_24->setObjectName(QStringLiteral("horizontalLayout_24"));
        groupBox_13 = new QGroupBox(compareAnalysisTab);
        groupBox_13->setObjectName(QStringLiteral("groupBox_13"));
        horizontalLayout_17 = new QHBoxLayout(groupBox_13);
        horizontalLayout_17->setSpacing(3);
        horizontalLayout_17->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_17->setObjectName(QStringLiteral("horizontalLayout_17"));
        horizontalLayout_17->setContentsMargins(0, 0, 3, 13);
        CompareCfgToolButton = new QToolButton(groupBox_13);
        CompareCfgToolButton->setObjectName(QStringLiteral("CompareCfgToolButton"));
        CompareCfgToolButton->setIcon(icon21);
        CompareCfgToolButton->setIconSize(QSize(32, 32));
        CompareCfgToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_17->addWidget(CompareCfgToolButton);

        splitScreenToolButton = new QToolButton(groupBox_13);
        splitScreenToolButton->setObjectName(QStringLiteral("splitScreenToolButton"));
        QIcon icon25;
        icon25.addFile(QStringLiteral(":/QMainWindow/Resources/icon/split.png"), QSize(), QIcon::Normal, QIcon::On);
        splitScreenToolButton->setIcon(icon25);
        splitScreenToolButton->setIconSize(QSize(32, 32));
        splitScreenToolButton->setCheckable(true);
        splitScreenToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_17->addWidget(splitScreenToolButton);


        horizontalLayout_24->addWidget(groupBox_13);

        horizontalSpacer_10 = new QSpacerItem(1196, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_24->addItem(horizontalSpacer_10);

        mainTabWidget->addTab(compareAnalysisTab, QString());
        asisPage = new QWidget();
        asisPage->setObjectName(QStringLiteral("asisPage"));
        horizontalLayout_21 = new QHBoxLayout(asisPage);
        horizontalLayout_21->setSpacing(6);
        horizontalLayout_21->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_21->setObjectName(QStringLiteral("horizontalLayout_21"));
        groupBox_14 = new QGroupBox(asisPage);
        groupBox_14->setObjectName(QStringLiteral("groupBox_14"));
        horizontalLayout_18 = new QHBoxLayout(groupBox_14);
        horizontalLayout_18->setSpacing(3);
        horizontalLayout_18->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_18->setObjectName(QStringLiteral("horizontalLayout_18"));
        horizontalLayout_18->setContentsMargins(0, 0, 3, 13);
        rectSeleteToolButton = new QToolButton(groupBox_14);
        rectSeleteToolButton->setObjectName(QStringLiteral("rectSeleteToolButton"));
        sizePolicy.setHeightForWidth(rectSeleteToolButton->sizePolicy().hasHeightForWidth());
        rectSeleteToolButton->setSizePolicy(sizePolicy);
        rectSeleteToolButton->setStyleSheet(QStringLiteral(""));
        rectSeleteToolButton->setIcon(icon6);
        rectSeleteToolButton->setIconSize(QSize(32, 32));
        rectSeleteToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_18->addWidget(rectSeleteToolButton);

        AddCModelButton = new QToolButton(groupBox_14);
        AddCModelButton->setObjectName(QStringLiteral("AddCModelButton"));
        sizePolicy.setHeightForWidth(AddCModelButton->sizePolicy().hasHeightForWidth());
        AddCModelButton->setSizePolicy(sizePolicy);
        AddCModelButton->setStyleSheet(QStringLiteral(""));
        AddCModelButton->setIcon(icon11);
        AddCModelButton->setIconSize(QSize(32, 32));
        AddCModelButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_18->addWidget(AddCModelButton);

        deleteCModeltoolButton = new QToolButton(groupBox_14);
        deleteCModeltoolButton->setObjectName(QStringLiteral("deleteCModeltoolButton"));
        sizePolicy.setHeightForWidth(deleteCModeltoolButton->sizePolicy().hasHeightForWidth());
        deleteCModeltoolButton->setSizePolicy(sizePolicy);
        deleteCModeltoolButton->setStyleSheet(QStringLiteral(""));
        deleteCModeltoolButton->setIcon(icon23);
        deleteCModeltoolButton->setIconSize(QSize(32, 32));
        deleteCModeltoolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_18->addWidget(deleteCModeltoolButton);

        seletBuildToolButton = new QToolButton(groupBox_14);
        seletBuildToolButton->setObjectName(QStringLiteral("seletBuildToolButton"));
        sizePolicy.setHeightForWidth(seletBuildToolButton->sizePolicy().hasHeightForWidth());
        seletBuildToolButton->setSizePolicy(sizePolicy);
        seletBuildToolButton->setStyleSheet(QStringLiteral(""));
        seletBuildToolButton->setIcon(icon5);
        seletBuildToolButton->setIconSize(QSize(32, 32));
        seletBuildToolButton->setCheckable(true);
        seletBuildToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_18->addWidget(seletBuildToolButton);

        modifyTexToolButton = new QToolButton(groupBox_14);
        modifyTexToolButton->setObjectName(QStringLiteral("modifyTexToolButton"));
        sizePolicy.setHeightForWidth(modifyTexToolButton->sizePolicy().hasHeightForWidth());
        modifyTexToolButton->setSizePolicy(sizePolicy);
        modifyTexToolButton->setStyleSheet(QStringLiteral(""));
        QIcon icon26;
        icon26.addFile(QStringLiteral(":/QMainWindow/Resources/icon/picture_edit.png"), QSize(), QIcon::Normal, QIcon::On);
        modifyTexToolButton->setIcon(icon26);
        modifyTexToolButton->setIconSize(QSize(32, 32));
        modifyTexToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_18->addWidget(modifyTexToolButton);

        transToolButton = new QToolButton(groupBox_14);
        transToolButton->setObjectName(QStringLiteral("transToolButton"));
        sizePolicy.setHeightForWidth(transToolButton->sizePolicy().hasHeightForWidth());
        transToolButton->setSizePolicy(sizePolicy);
        transToolButton->setStyleSheet(QStringLiteral(""));
        QIcon icon27;
        icon27.addFile(QStringLiteral(":/QMainWindow/Resources/icon/move_32px_1205817_easyicon.net.png"), QSize(), QIcon::Normal, QIcon::On);
        transToolButton->setIcon(icon27);
        transToolButton->setIconSize(QSize(32, 32));
        transToolButton->setCheckable(true);
        transToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_18->addWidget(transToolButton);

        rotateToolButton = new QToolButton(groupBox_14);
        rotateToolButton->setObjectName(QStringLiteral("rotateToolButton"));
        sizePolicy.setHeightForWidth(rotateToolButton->sizePolicy().hasHeightForWidth());
        rotateToolButton->setSizePolicy(sizePolicy);
        rotateToolButton->setStyleSheet(QStringLiteral(""));
        QIcon icon28;
        icon28.addFile(QStringLiteral(":/QMainWindow/Resources/icon/rotate_32px_1175655_easyicon.net.png"), QSize(), QIcon::Normal, QIcon::On);
        rotateToolButton->setIcon(icon28);
        rotateToolButton->setIconSize(QSize(32, 32));
        rotateToolButton->setCheckable(true);
        rotateToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_18->addWidget(rotateToolButton);

        stretchToolButton = new QToolButton(groupBox_14);
        stretchToolButton->setObjectName(QStringLiteral("stretchToolButton"));
        sizePolicy.setHeightForWidth(stretchToolButton->sizePolicy().hasHeightForWidth());
        stretchToolButton->setSizePolicy(sizePolicy);
        stretchToolButton->setStyleSheet(QStringLiteral(""));
        QIcon icon29;
        icon29.addFile(QStringLiteral(":/QMainWindow/Resources/icon/Zoom_in_32px_1176980_easyicon.net.png"), QSize(), QIcon::Normal, QIcon::On);
        stretchToolButton->setIcon(icon29);
        stretchToolButton->setIconSize(QSize(32, 32));
        stretchToolButton->setCheckable(true);
        stretchToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_18->addWidget(stretchToolButton);

        analysisToolButton = new QToolButton(groupBox_14);
        analysisToolButton->setObjectName(QStringLiteral("analysisToolButton"));
        sizePolicy.setHeightForWidth(analysisToolButton->sizePolicy().hasHeightForWidth());
        analysisToolButton->setSizePolicy(sizePolicy);
        analysisToolButton->setStyleSheet(QStringLiteral(""));
        QIcon icon30;
        icon30.addFile(QStringLiteral(":/QMainWindow/Resources/icon/analysis.png"), QSize(), QIcon::Normal, QIcon::On);
        analysisToolButton->setIcon(icon30);
        analysisToolButton->setIconSize(QSize(32, 32));
        analysisToolButton->setCheckable(true);
        analysisToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_18->addWidget(analysisToolButton);

        sunEmuToolButton = new QToolButton(groupBox_14);
        sunEmuToolButton->setObjectName(QStringLiteral("sunEmuToolButton"));
        sizePolicy.setHeightForWidth(sunEmuToolButton->sizePolicy().hasHeightForWidth());
        sunEmuToolButton->setSizePolicy(sizePolicy);
        sunEmuToolButton->setStyleSheet(QStringLiteral(""));
        sunEmuToolButton->setIcon(icon18);
        sunEmuToolButton->setIconSize(QSize(32, 32));
        sunEmuToolButton->setCheckable(true);
        sunEmuToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_18->addWidget(sunEmuToolButton);

        canceltoolButton = new QToolButton(groupBox_14);
        canceltoolButton->setObjectName(QStringLiteral("canceltoolButton"));
        sizePolicy.setHeightForWidth(canceltoolButton->sizePolicy().hasHeightForWidth());
        canceltoolButton->setSizePolicy(sizePolicy);
        canceltoolButton->setStyleSheet(QStringLiteral(""));
        QIcon icon31;
        icon31.addFile(QStringLiteral(":/QMainWindow/Resources/icon/cross_32px_1113381_easyicon.net.png"), QSize(), QIcon::Normal, QIcon::On);
        canceltoolButton->setIcon(icon31);
        canceltoolButton->setIconSize(QSize(32, 32));
        canceltoolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_18->addWidget(canceltoolButton);


        horizontalLayout_21->addWidget(groupBox_14);

        horizontalSpacer_7 = new QSpacerItem(720, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_21->addItem(horizontalSpacer_7);

        mainTabWidget->addTab(asisPage, QString());
        particlePage = new QWidget();
        particlePage->setObjectName(QStringLiteral("particlePage"));
        horizontalLayout_20 = new QHBoxLayout(particlePage);
        horizontalLayout_20->setSpacing(4);
        horizontalLayout_20->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_20->setObjectName(QStringLiteral("horizontalLayout_20"));
        horizontalLayout_20->setContentsMargins(10, 0, 0, 0);
        groupBox_5 = new QGroupBox(particlePage);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        gridLayout_4 = new QGridLayout(groupBox_5);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setHorizontalSpacing(3);
        gridLayout_4->setVerticalSpacing(0);
        gridLayout_4->setContentsMargins(0, 0, 3, 13);
        smokeCheckBox = new QCheckBox(groupBox_5);
        smokeCheckBox->setObjectName(QStringLiteral("smokeCheckBox"));

        gridLayout_4->addWidget(smokeCheckBox, 0, 0, 1, 1);

        fireCheckBox = new QCheckBox(groupBox_5);
        fireCheckBox->setObjectName(QStringLiteral("fireCheckBox"));

        gridLayout_4->addWidget(fireCheckBox, 0, 1, 1, 1);

        groupBox_151 = new QGroupBox(groupBox_5);
        groupBox_151->setObjectName(QStringLiteral("groupBox_151"));
        groupBox_151->setStyleSheet(QLatin1String("border-left: 0px solid gray;\n"
"border-top: 0px solid gray;\n"
"border-right:0px solid gray4;\n"
""));
        horizontalLayout_191 = new QHBoxLayout(groupBox_151);
        horizontalLayout_191->setSpacing(3);
        horizontalLayout_191->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_191->setObjectName(QStringLiteral("horizontalLayout_191"));
        horizontalLayout_191->setContentsMargins(0, 0, 3, 35);
        smallRadioBtn = new QRadioButton(groupBox_151);
        smallRadioBtn->setObjectName(QStringLiteral("smallRadioBtn"));
        smallRadioBtn->setChecked(true);

        horizontalLayout_191->addWidget(smallRadioBtn);

        middleRadioBtn = new QRadioButton(groupBox_151);
        middleRadioBtn->setObjectName(QStringLiteral("middleRadioBtn"));

        horizontalLayout_191->addWidget(middleRadioBtn);

        heavyRadioBtn = new QRadioButton(groupBox_151);
        heavyRadioBtn->setObjectName(QStringLiteral("heavyRadioBtn"));

        horizontalLayout_191->addWidget(heavyRadioBtn);


        gridLayout_4->addWidget(groupBox_151, 0, 2, 2, 1);

        rainCheckBox = new QCheckBox(groupBox_5);
        rainCheckBox->setObjectName(QStringLiteral("rainCheckBox"));

        gridLayout_4->addWidget(rainCheckBox, 1, 0, 1, 1);

        snowCheckBox = new QCheckBox(groupBox_5);
        snowCheckBox->setObjectName(QStringLiteral("snowCheckBox"));

        gridLayout_4->addWidget(snowCheckBox, 1, 1, 1, 1);


        horizontalLayout_20->addWidget(groupBox_5);

        groupBox_9 = new QGroupBox(particlePage);
        groupBox_9->setObjectName(QStringLiteral("groupBox_9"));
        horizontalLayout_12 = new QHBoxLayout(groupBox_9);
        horizontalLayout_12->setSpacing(3);
        horizontalLayout_12->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        horizontalLayout_12->setContentsMargins(0, 0, 3, 13);
        playMusic = new QToolButton(groupBox_9);
        playMusic->setObjectName(QStringLiteral("playMusic"));
        playMusic->setIcon(icon30);
        playMusic->setIconSize(QSize(32, 32));
        playMusic->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_12->addWidget(playMusic);


        horizontalLayout_20->addWidget(groupBox_9);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_20->addItem(horizontalSpacer_6);

        mainTabWidget->addTab(particlePage, QString());

        horizontalLayout->addWidget(mainTabWidget);


        gridLayout->addWidget(widget, 1, 0, 1, 1);


        retranslateUi(QtExplorerClass);

        mainTabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(QtExplorerClass);
    } // setupUi

    void retranslateUi(QWidget *QtExplorerClass)
    {
        QtExplorerClass->setWindowTitle(QApplication::translate("QtExplorerClass", "GeoGlobe", Q_NULLPTR));
        toolButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        saveWSToolButton->setToolTip(QApplication::translate("QtExplorerClass", "\344\277\235\345\255\230", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        saveWSToolButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        openWSToolButton->setToolTip(QApplication::translate("QtExplorerClass", "\346\211\223\345\274\200", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        openWSToolButton->setText(QString());
        label->setText(QApplication::translate("QtExplorerClass", "\347\246\271\345\267\236\345\270\202\344\270\211\347\273\264\346\231\272\346\205\247\350\247\204\345\210\222\347\263\273\347\273\237", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("QtExplorerClass", "\346\225\260\346\215\256", Q_NULLPTR));
        menuFilePushButton->setText(QApplication::translate("QtExplorerClass", "\346\226\207\344\273\266", Q_NULLPTR));
        loadLocalDataToolButton->setText(QApplication::translate("QtExplorerClass", "\346\234\254\345\234\260", Q_NULLPTR));
        loadServerToolButton->setText(QApplication::translate("QtExplorerClass", "\346\234\215\345\212\241", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("QtExplorerClass", "\346\265\217\350\247\210", Q_NULLPTR));
        selectToolButton->setText(QApplication::translate("QtExplorerClass", "\351\200\211\346\213\251", Q_NULLPTR));
        polygonQueryToolButton->setText(QApplication::translate("QtExplorerClass", "\345\244\232\350\276\271\345\275\242\346\213\276\345\217\226", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("QtExplorerClass", "\351\243\236\350\241\214", Q_NULLPTR));
        addFlyPointToolButton->setText(QApplication::translate("QtExplorerClass", "\346\267\273\345\212\240\350\212\202\347\202\271", Q_NULLPTR));
        startFlyToolButton->setText(QApplication::translate("QtExplorerClass", "\345\274\200\345\247\213\351\243\236\350\241\214", Q_NULLPTR));
        stopFlyToolButton->setText(QApplication::translate("QtExplorerClass", "\345\201\234\346\255\242\351\243\236\350\241\214", Q_NULLPTR));
        saveFlyToolButton->setText(QApplication::translate("QtExplorerClass", "\344\277\235\345\255\230\351\243\236\350\241\214\350\267\257\345\276\204", Q_NULLPTR));
        loadFlyToolButton->setText(QApplication::translate("QtExplorerClass", "\345\212\240\350\275\275\351\243\236\350\241\214", Q_NULLPTR));
        groupBox_6->setTitle(QApplication::translate("QtExplorerClass", "\345\277\253\347\205\247", Q_NULLPTR));
        snapShootToolButton->setText(QApplication::translate("QtExplorerClass", "\345\277\253\347\205\247", Q_NULLPTR));
        groupBox_4->setTitle(QApplication::translate("QtExplorerClass", "\345\206\205\345\256\271\347\252\227\345\217\243", Q_NULLPTR));
        layerManageCheckBox->setText(QApplication::translate("QtExplorerClass", "\346\225\260\346\215\256\347\256\241\347\220\206", Q_NULLPTR));
        birdsEyecheckBox->setText(QApplication::translate("QtExplorerClass", "\351\271\260\347\234\274\345\274\200\345\205\263", Q_NULLPTR));
        lonLatInfoCheckBox->setText(QApplication::translate("QtExplorerClass", "\347\273\217\347\272\254\345\272\246", Q_NULLPTR));
        compassCheckBox->setText(QApplication::translate("QtExplorerClass", "\346\214\207\345\215\227\351\222\210", Q_NULLPTR));
        atmosphereCheckBox->setText(QApplication::translate("QtExplorerClass", "\345\244\247\346\260\224\345\234\210", Q_NULLPTR));
        mainTabWidget->setTabText(mainTabWidget->indexOf(mainpage), QApplication::translate("QtExplorerClass", "\344\270\273\351\241\265", Q_NULLPTR));
        groupBox_7->setTitle(QApplication::translate("QtExplorerClass", "\351\200\232\350\247\206\345\210\206\346\236\220", Q_NULLPTR));
        observerPointsInRangeAnalysisToolButton->setText(QApplication::translate("QtExplorerClass", "\350\247\206\345\237\237\345\210\206\346\236\220", Q_NULLPTR));
        lineOfSightAnalysisToolButton->setText(QApplication::translate("QtExplorerClass", "\351\200\232\350\247\206\345\210\206\346\236\220", Q_NULLPTR));
        groupBox_15->setTitle(QApplication::translate("QtExplorerClass", "\351\207\217\347\256\227\345\210\206\346\236\220", Q_NULLPTR));
        areaAnalysisToolButton->setText(QApplication::translate("QtExplorerClass", "\351\235\242\347\247\257\351\207\217\347\256\227", Q_NULLPTR));
        heightAnalysisToolButton->setText(QApplication::translate("QtExplorerClass", "\351\253\230\345\272\246\351\207\217\347\256\227", Q_NULLPTR));
        groupBox_8->setTitle(QApplication::translate("QtExplorerClass", "\347\274\226\350\276\221", Q_NULLPTR));
        clearAnalysisToolButton->setText(QApplication::translate("QtExplorerClass", "\346\270\205\351\231\244\345\210\206\346\236\220\347\273\223\346\236\234", Q_NULLPTR));
        mainTabWidget->setTabText(mainTabWidget->indexOf(analysispage), QApplication::translate("QtExplorerClass", "\351\207\217\347\256\227\344\270\216\345\210\206\346\236\220", Q_NULLPTR));
        groupBox_12->setTitle(QApplication::translate("QtExplorerClass", "\346\227\245\347\205\247\345\210\206\346\236\220", Q_NULLPTR));
        emuToolButton->setText(QApplication::translate("QtExplorerClass", "\346\227\245\347\205\247\344\273\277\347\234\237", Q_NULLPTR));
        coverToolButton->setText(QApplication::translate("QtExplorerClass", "\346\227\245\347\205\247\351\201\256\346\214\241\345\210\206\346\236\220", Q_NULLPTR));
        coneAnalysisToolButton->setText(QApplication::translate("QtExplorerClass", "\346\227\245\347\205\247\351\224\245\351\235\242\345\210\206\346\236\220", Q_NULLPTR));
        regionToolButton->setText(QApplication::translate("QtExplorerClass", "\346\227\245\347\205\247\345\214\272\345\237\237\345\210\206\346\236\220", Q_NULLPTR));
        sunConfigToolButton->setText(QApplication::translate("QtExplorerClass", "\346\227\245\347\205\247\351\205\215\347\275\256", Q_NULLPTR));
        mainTabWidget->setTabText(mainTabWidget->indexOf(sunAnalysisTab), QApplication::translate("QtExplorerClass", "\346\227\245\347\205\247\345\210\206\346\236\220", Q_NULLPTR));
        groupBox_11->setTitle(QApplication::translate("QtExplorerClass", "\346\213\206\350\277\201\345\210\206\346\236\220", Q_NULLPTR));
        relocationToolButton->setText(QApplication::translate("QtExplorerClass", "\346\213\206\350\277\201\345\210\206\346\236\220", Q_NULLPTR));
        relocationResultToolButton->setText(QApplication::translate("QtExplorerClass", "\346\213\206\351\231\244", Q_NULLPTR));
        revokeToolButton->setText(QApplication::translate("QtExplorerClass", "\346\201\242\345\244\215", Q_NULLPTR));
        mainTabWidget->setTabText(mainTabWidget->indexOf(DemolitionAnalysisTab), QApplication::translate("QtExplorerClass", "\346\213\206\350\277\201\345\210\206\346\236\220", Q_NULLPTR));
        groupBox_13->setTitle(QApplication::translate("QtExplorerClass", "\345\257\271\346\257\224\345\210\206\346\236\220", Q_NULLPTR));
        CompareCfgToolButton->setText(QApplication::translate("QtExplorerClass", "\346\226\271\346\241\210\351\205\215\347\275\256", Q_NULLPTR));
        splitScreenToolButton->setText(QApplication::translate("QtExplorerClass", "\345\257\271\346\257\224\344\273\277\347\234\237", Q_NULLPTR));
        mainTabWidget->setTabText(mainTabWidget->indexOf(compareAnalysisTab), QApplication::translate("QtExplorerClass", "\346\226\271\346\241\210\345\257\271\346\257\224", Q_NULLPTR));
        groupBox_14->setTitle(QApplication::translate("QtExplorerClass", "\350\276\205\345\212\251\345\206\263\347\255\226", Q_NULLPTR));
        rectSeleteToolButton->setText(QApplication::translate("QtExplorerClass", "\345\214\272\345\237\237\351\200\211\345\217\226", Q_NULLPTR));
        AddCModelButton->setText(QApplication::translate("QtExplorerClass", "\345\212\240\350\275\275", Q_NULLPTR));
        deleteCModeltoolButton->setText(QApplication::translate("QtExplorerClass", "\345\210\240\351\231\244", Q_NULLPTR));
        seletBuildToolButton->setText(QApplication::translate("QtExplorerClass", "\345\273\272\347\255\221\351\200\211\345\217\226", Q_NULLPTR));
        modifyTexToolButton->setText(QApplication::translate("QtExplorerClass", "\344\277\256\346\224\271\346\235\220\350\264\250", Q_NULLPTR));
        transToolButton->setText(QApplication::translate("QtExplorerClass", "\345\271\263\347\247\273", Q_NULLPTR));
        rotateToolButton->setText(QApplication::translate("QtExplorerClass", "\346\227\213\350\275\254", Q_NULLPTR));
        stretchToolButton->setText(QApplication::translate("QtExplorerClass", "\346\213\211\344\274\270", Q_NULLPTR));
        analysisToolButton->setText(QApplication::translate("QtExplorerClass", "\345\210\206\346\236\220", Q_NULLPTR));
        sunEmuToolButton->setText(QApplication::translate("QtExplorerClass", "\346\227\245\347\205\247\344\273\277\347\234\237", Q_NULLPTR));
        canceltoolButton->setText(QApplication::translate("QtExplorerClass", "\345\217\226\346\266\210", Q_NULLPTR));
        mainTabWidget->setTabText(mainTabWidget->indexOf(asisPage), QApplication::translate("QtExplorerClass", "\350\276\205\345\212\251\345\206\263\347\255\226", Q_NULLPTR));
        groupBox_5->setTitle(QApplication::translate("QtExplorerClass", "\347\262\222\345\255\220\347\211\271\346\225\210", Q_NULLPTR));
        smokeCheckBox->setText(QApplication::translate("QtExplorerClass", "\351\233\276", Q_NULLPTR));
        fireCheckBox->setText(QApplication::translate("QtExplorerClass", "\346\260\264", Q_NULLPTR));
        groupBox_151->setTitle(QApplication::translate("QtExplorerClass", "\351\233\250\346\260\264\345\257\206\345\272\246", Q_NULLPTR));
        smallRadioBtn->setText(QApplication::translate("QtExplorerClass", "\345\260\217", Q_NULLPTR));
        middleRadioBtn->setText(QApplication::translate("QtExplorerClass", "\344\270\255", Q_NULLPTR));
        heavyRadioBtn->setText(QApplication::translate("QtExplorerClass", "\345\244\247", Q_NULLPTR));
        rainCheckBox->setText(QApplication::translate("QtExplorerClass", "\351\233\250", Q_NULLPTR));
        snowCheckBox->setText(QApplication::translate("QtExplorerClass", "\351\233\252", Q_NULLPTR));
        groupBox_9->setTitle(QApplication::translate("QtExplorerClass", "\345\244\232\345\252\222\344\275\223", Q_NULLPTR));
        playMusic->setText(QApplication::translate("QtExplorerClass", "\351\237\263\344\271\220", Q_NULLPTR));
        mainTabWidget->setTabText(mainTabWidget->indexOf(particlePage), QApplication::translate("QtExplorerClass", "\347\262\222\345\255\220\347\211\271\346\225\210", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QtExplorerClass: public Ui_QtExplorerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTEXPLORER_H
