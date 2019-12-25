#include <qglobal.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QClipboard>
#include <QDesktopServices>
#include <QTextEdit>
#include <QTextCodec>
#include <QLineEdit>
#include <QTextDocumentWriter>
#include <QAbstractTextDocumentLayout>
#include <QFileDialog>
#include <QFile>
#include <QPrinter>
#include <QSpinBox>
#include <QPrintDialog>
#include <QFontComboBox>
#include <QTextDocument>
#include <QLabel>
#include <QToolBar>
#include <QDockWidget>
#include <QListWidget>
#include <QPrintPreviewDialog>
#include <QTextList>
#include <QColorDialog>
#include <QMessageBox>
#include <QFontDialog>
#include <QDockWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QListWidget>
#include <QBuffer>
#include <QUrl>
#include <QPainter>

#include <QtnRibbonStyle.h>
#include <QtnRibbonCustomizePage.h>

//#include "aboutdialog.h"
#include "ribbonpopularpage.h"
#include "mainwindow.h"
#include "ui_ribbonsample.h"
#include "OsgInterface.h"
#include "HeightAnalysisCallBack.h"
#include "ObliqueImageLayer.h"
#include "LoadOGCServiceWindow.h"
#include "BillBoardElement.h"
#include "GeoBillBoardElement.h"
#include <canvas.h>
#include <TextureMovieElement.h>
#include <OverlayElement.h>
#include <WFSLayer.h>
#include <DistanceAnalysis3D.h>
#include <AreaAnalysis3D.h>
#include <PointSymbolElement.h>
#include <LineSymbolElement.h>
#include <PolygonSymbolElement.h>
#include <TerrainProfileAnalysis.h>
#include <OsgbProfileAnalysis.h>
#include <PointSymbolElement3D.h>
#include <LineSymbolElement3D.h>
#include <LonAndLatElement.h>
#include <VectorLayer.h>
#include <fstream>
#include <StreamMovieElement.h>
#include <MarathonAnalysis.h>
#include <DrawElmentAnalysis.h>
#include <EditElmentAnalysis.h>
#include <VolumeAnalysis.h>
#include <TextElement.h>
#include <FollowerCamera.h>
#include <LinePictureElement.h>
#include <Animator.h>
#include "3rdparty\gdal\ogrsf_frmts.h"

using namespace Qtitan;
using namespace UTILITY_NAME;

static QString categoryPopularCommands = QObject::tr("Popular Commands");
static QString categoryAllCommands     = QObject::tr("All Commands");
static QString categoryMainPages       = QObject::tr("Main Pages");

//test
GLOBE_NAME::AnalysisBasePtr ptrAnalysis;
GLOBE_NAME::LayerPtr ptrLayer;
GLOBE_NAME::StreamMovieElementPtr ptrMovie;
GLOBE_NAME::ObliqueImageLayerPtr ptrOblique;
GLOBE_NAME::GeoBillBoardElementPtr ptrGeoElement;

#define CURL_SIZEOF_LONG sizeof(long)

/* MainWindow */
MainWindow::MainWindow(QWidget* parent)
    : RibbonWindow(parent)
{
	CPLSetConfigOption("GDAL_HTTP_UNSAFESSL", "YES");//禁止htpp请求时验证
	CPLSetConfigOption("GDAL_DATA", "./gdal-data");//设置GDAL_Data的空间参考数据
	OGRRegisterAll();
    createAction();
    createMenuFile();
    createQuickAccessBar();
 
	UTILITY_NAME::GsConfig config("./Resources/config/AppConfig.xml");
	m_Config = config;

    createRibbon();
    createStatusBar();
    //createDockWindows();
	//m_ptrMapView = new GLOBE_NAME::GsMapView(this);
	m_ptrGeoSpace3D = new GLOBE_NAME::GsGeospace3DQt(); // m_ptrMapView->GeoSpace3D();

	//m_ptrGeoSpace3D->ElementBox()->LonAndLatElement()->Visible(false);

	GLOBE_NAME::LonAndLatElementPtr ptrLonLat = dynamic_cast<GLOBE_NAME::LonAndLatElement*>(m_ptrGeoSpace3D->ElementBox()->LonAndLatElement());
	ptrLonLat->OnClicked().Add(this, &MainWindow::OnLonLatClicked);

	//m_mdiArea = new QMdiArea(this);
	//m_mdiArea->setDocumentMode(true);
	//m_mdiArea->setLineWidth(3);
	//m_mdiArea->setFrameShape(QFrame::Panel);
	//m_mdiArea->setFrameShadow(QFrame::Sunken);
	//m_mdiArea->setViewMode(QMdiArea::TabbedView);
	//m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	//m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	//m_mdiArea->addSubWindow(m_ptrGeoSpace3D);

	//setCentralWidget(m_mdiArea);
	m_ptrGeoSpace3D->setSrsInfo("EPSG:4547", KERNEL_NAME::GsRawPoint3D(541011, 3369298, 0));

	setWindowIcon(QIcon("./Resources/icon/main.ico"));
	setWindowTitle("GeoGlobe Explorer");
    setCentralWidget(m_ptrGeoSpace3D/*m_ptrMapView*/);
	setStyleTheme(OfficeStyle::Office2013White);

    connect(ribbonBar(), SIGNAL(showRibbonContextMenu(QMenu*, QContextMenuEvent*)), this, SLOT(showRibbonContextMenu(QMenu*, QContextMenuEvent*)));
    ribbonBar()->setFrameThemeEnabled();
    m_stateWindow = windowState();

	QRect rect = QApplication::desktop()->availableGeometry();
	setGeometry(rect.width() / 6, rect.height()/5, 2 * rect.width() / 3, 2 * rect.height() / 3);
}

void MainWindow::OnLonLatClicked(GLOBE_NAME::Element* ptrLonLat)
{

}

MainWindow::~MainWindow()
{

}

QString MainWindow::ribbonBackground() const
{
    return m_curRibbonBackground;
}

void MainWindow::setRibbonBackground(const QString& name)
{
    if (m_curRibbonBackground != name)
    {
        m_curRibbonBackground = name;
        updateRibbonBackground();
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
}

void MainWindow::createAction()
{
    m_actionFileNew = new QAction(::qtnIcon(Image_New), tr("&New"), this);
    m_actionFileNew->setPriority(QAction::LowPriority);
    m_actionFileNew->setShortcut(QKeySequence::New);
    m_actionFileNew->setStatusTip(tr("Create a new document"));
    m_actionFileNew->setToolTip(tr("New"));
    //connect(m_actionFileNew, SIGNAL(triggered()), this, SLOT(fileNew()));

    m_actionOpenFile = new QAction(::qtnIcon(Image_Open), tr("&Open..."), this);
    m_actionOpenFile->setShortcut(QKeySequence::Open);
    m_actionOpenFile->setToolTip(tr("Open"));
    m_actionOpenFile->setStatusTip(tr("Open an existing document"));
    //connect(m_actionOpenFile, SIGNAL(triggered()), this, SLOT(fileOpen()));

    m_actionSaveFile = new QAction(::qtnIcon(Image_Save), tr("&Save"), this);
    m_actionSaveFile->setShortcut(QKeySequence::Save);
    m_actionSaveFile->setToolTip(tr("Save"));
    m_actionSaveFile->setStatusTip(tr("Save the active document"));
    m_actionSaveFile->setEnabled(false);
    //connect(m_actionSaveFile, SIGNAL(triggered()), this, SLOT(fileSave()));

    m_actionPrint = new QAction(::qtnIcon(Image_Print), tr("&Print..."), this);
    m_actionPrint->setPriority(QAction::LowPriority);    
    m_actionPrint->setShortcut(QKeySequence::Print);
    m_actionPrint->setToolTip(tr("Select a printer, number of copies, and other printing options before printing"));
    //connect(m_actionPrint, SIGNAL(triggered()), this, SLOT(filePrint()));

    m_actionPrintSetup = new QAction(QIcon(":/res/largePrintSetup.png"), tr("P&rint Setup"), this);
    m_actionPrintSetup->setToolTip(tr("Change the printer and printing options"));

    m_actionPrintPreview = new QAction(QIcon(":/res/largePrintPreview.png"), tr("Print Pre&view..."), this);
    m_actionPrintPreview->setToolTip(tr("Preview and make changes to pages before printing"));
    //connect(m_actionPrintPreview, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
}

void MainWindow::createMenuFile()
{
    QIcon iconLogo;

    iconLogo.addPixmap(QPixmap(":/res/qtitanlogo32x32.png"));
    if(QAction* actionFile = ribbonBar()->addSystemButton(iconLogo, QString::fromLocal8Bit("&文件"))) 
    {
        actionFile->setToolTip(tr("Click here to see everything you can do with your document"));
        RibbonSystemPopupBar* popupBar = qobject_cast<RibbonSystemPopupBar*>(actionFile->menu());

        popupBar->addAction(m_actionFileNew);
        popupBar->addAction(m_actionOpenFile);
        popupBar->addAction(m_actionSaveFile);

        QAction* actionSaveAsFile = popupBar->addAction(::qtnIcon(Image_SaveAs), tr("Save &As..."));
        actionSaveAsFile->setPriority(QAction::LowPriority);
        actionSaveAsFile->setToolTip(tr("Save As"));
        actionSaveAsFile->setStatusTip(tr("Save the active document with a new name"));
        //connect(actionSaveAsFile, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

        popupBar->addSeparator();

        QAction* actionPagePrint = new QAction(m_actionPrint->icon(), m_actionPrint->text(), this);
        popupBar->addAction(actionPagePrint);
        if (RibbonPageSystemPopup* pageSystemPopup = popupBar->addPageSystemPopup(tr("Preview and print the document"), actionPagePrint, true))
        {
            //connect(actionPagePrint, SIGNAL(triggered()), m_actionPrint, SIGNAL(triggered()));
            pageSystemPopup->setMinimumWidth(296);
            pageSystemPopup->addAction(m_actionPrint);
            pageSystemPopup->addAction(m_actionPrintSetup);
            pageSystemPopup->addAction(m_actionPrintPreview);
        }

        QAction* actionPrepare = popupBar->addAction(QIcon(":/res/largePrepare.png"), tr("Pr&epare"));
        popupBar->addAction(actionPrepare);
        
        QAction* actionSend = popupBar->addAction(QIcon(":/res/largeSend.png"), tr("Sen&d"));
        popupBar->addAction(actionSend);
        if (RibbonPageSystemPopup* pageSystemPopup = popupBar->addPageSystemPopup(tr("Preview and print the document"), actionSend, false))
        {
            pageSystemPopup->setMinimumWidth(296);
            QAction* actionMail = pageSystemPopup->addAction(QIcon(":/res/largeMail.png"), tr("E-Mail"));
            actionMail->setToolTip(tr("Send the active document by e-mail"));
            pageSystemPopup->addAction(actionMail);

            QAction* actionIntenetFax = pageSystemPopup->addAction(QIcon(":/res/largeInternetfix.png"), tr("Intenet Fax"));
            actionIntenetFax->setToolTip(tr("Use an Internet fax service to fax the document"));
            pageSystemPopup->addAction(actionIntenetFax);
        }

        popupBar->addSeparator();
        QAction* actionClose = popupBar->addAction(::qtnIcon(Image_Close), tr("&Close"));
        actionClose->setShortcut(tr("Ctrl+C"));
        actionClose->setStatusTip(tr("Exit"));
        actionClose->setEnabled(false);

        QAction* actionExit =  popupBar->addPopupBarAction(tr("Exit Sample"));
        //connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

        QAction* actionOption = popupBar->addPopupBarAction(tr("Option"));
        actionOption->setEnabled(false);

        if (RibbonPageSystemRecentFileList* pageRecentFile = popupBar->addPageRecentFile(tr("Recent Documents")))
        {
            Q_UNUSED(pageRecentFile);
        }
    }
}

void MainWindow::createQuickAccessBar()
{
    if (RibbonQuickAccessBar* quickAccessBar = ribbonBar()->quickAccessBar())
    {
        QAction* action = quickAccessBar->actionCustomizeButton();
        action->setToolTip(tr("Customize Quick Access Bar"));

        quickAccessBar->addAction(m_actionFileNew);
        quickAccessBar->setActionVisible(m_actionFileNew, false);

        quickAccessBar->addAction(m_actionOpenFile);
        quickAccessBar->setActionVisible(m_actionOpenFile, false);

        quickAccessBar->addAction(m_actionSaveFile);

        m_actionUndo = quickAccessBar->addAction(::qtnIcon(Image_Undo), tr("&Undo"));
        m_actionUndo->setShortcut(QKeySequence::Undo);
        m_actionRedo = quickAccessBar->addAction(::qtnIcon(Image_Redo), tr("&Redo"));
        m_actionRedo->setShortcut(QKeySequence::Redo);
        quickAccessBar->addAction(m_actionPrint);
        ribbonBar()->showQuickAccess(true);
    }
}

void MainWindow::createRibbon()
{
	if (!m_Config.IsValid())
		return;
	
	GsVector<GsConfig> vecConfig = m_Config["App"]["Ribbon"]["Pages"].Children();

	for (GsVector<GsConfig>::iterator it = vecConfig.begin() ; it!=vecConfig.end() ; it++)
	{
		if ((*it).Name() != "Page")
			continue;

		GsString gstrPageName = (*it).Description();
		RibbonPage* page = ribbonBar()->addPage(gstrPageName.c_str());

		if (page)
			createRibbonGroup(page,*it);
	}
}

void MainWindow::createRibbonGroup(RibbonPage* ptrPage, GsConfig config)
{
	GsVector<GsConfig> vecConfig = config.Children();

	for (GsVector<GsConfig>::iterator it = vecConfig.begin(); it != vecConfig.end(); it++)
	{
		if ((*it).Name() != "Group")
			continue;

		GsString gstrGroupName = (*it).Description();

		if (RibbonGroup* groupClipboard = ptrPage->addGroup(gstrGroupName.c_str()))
			createRibbonControl(groupClipboard, *it);
	}
}

void MainWindow::createRibbonControl(RibbonGroup* ptrGroup, GsConfig config)
{
	GsVector<GsConfig> vecConfig = config.Children();
	
	for (GsVector<GsConfig>::iterator it = vecConfig.begin(); it != vecConfig.end(); it++)
	{
		GsString gstrControlName = (*it).Name();

		if (gstrControlName == "Menu")
		{
			GsString gstrIconPath = (*it)["Icon"].StringValue("");
			QMenu* ptrMenu = ptrGroup->addMenu(QIcon(gstrIconPath.c_str()), (*it).Description().c_str(), Qt::ToolButtonTextUnderIcon);

			if (ptrMenu)
				createRibbonMenuItem(ptrMenu, *it);
		}
		else if(gstrControlName == "Button")
		{
			QString qstrSlotName = (*it)["SlotName"].StringValue("").c_str();
			GsString gstrIconPath = (*it)["Icon"].StringValue("");
			QAction* ptrAction = ptrGroup->addAction(QIcon(gstrIconPath.c_str()), (*it).Description().c_str(), Qt::ToolButtonTextUnderIcon);
			ConnectSlot(ptrAction, qstrSlotName);
		}
		else if (gstrControlName == "CheckBox")
		{
			QString qstrSlotName = (*it)["SlotName"].StringValue("").c_str();
			GsString gstrIconPath = (*it)["Icon"].StringValue("");
			QAction* ptrAction = ptrGroup->addAction(QIcon(gstrIconPath.c_str()), (*it).Description().c_str(), Qt::ToolButtonTextUnderIcon);
			ConnectSlot(ptrAction, qstrSlotName);
		}
	}
}

void MainWindow::createRibbonMenuItem(QMenu* ptrMenu, GsConfig config)
{
	GsVector<GsConfig> vecConfig = config.Children();

	for (GsVector<GsConfig>::iterator it = vecConfig.begin(); it != vecConfig.end(); it++)
	{
		if ((*it).Name() != "Item")
			continue;;

		GsString gstrIconPath = (*it)["Icon"].StringValue("");
		QString qstrSlotName = (*it)["SlotName"].StringValue("").c_str();

		GsString gstrMenuItemName = (*it).Description();
		QAction* ptrAction = ptrMenu->addAction(QIcon(gstrIconPath.c_str()),gstrMenuItemName.c_str());

		ConnectSlot(ptrAction, qstrSlotName);
	}
}

void MainWindow::ConnectSlot(QAction* ptrAction, QString qstrSlotName)
{
	if (qstrSlotName == "AddTerrain")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddTerrain()));
	else if (qstrSlotName == "AddImage")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddImage()));
	else if (qstrSlotName == "AddVector")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddVector()));
	else if(qstrSlotName == "AddPlaceName")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddPlaceName()));
	else if (qstrSlotName == "AddModel")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddModel()));
	else if (qstrSlotName == "AddObliqueImage")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddObliqueImage()));
	else if (qstrSlotName == "HeightAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(HeightAnalysis()));
	else if (qstrSlotName == "DistanceAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(DistanceAnalysis()));
	else if (qstrSlotName == "OPInRangeAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(OPInRangeAnalysis()));
	else if (qstrSlotName == "LineOfSightAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(LineOfSightAnalysis()));
	else if (qstrSlotName == "MultipleLineOfSightAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(MultipleLineOfSightAnalysis()));
	else if (qstrSlotName == "AreaMeasureAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AreaAnalysis()));
	else if (qstrSlotName == "RadarAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(RadarAnalysis()));
	else if (qstrSlotName == "RemoveAllAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(RemoveAllAnalysis()));
	else if (qstrSlotName == "RemoveAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(RemoveAnalysis()));
	else if (qstrSlotName == "AddModelElement")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddModelElement()));
	else if (qstrSlotName == "AddLonAndLatElement")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddLonAndLatElement()));
	else if (qstrSlotName == "AddCompassElement")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddCompassElement()));
	else if (qstrSlotName == "AddBillBoardElement")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddBillBoardElement()));
	else if (qstrSlotName == "AddGeoBillBoardElement")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddGeoBillBoardElement()));
	else if (qstrSlotName == "Test")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(Test()));
	else if (qstrSlotName == "AddFlyPoints")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddFlyPoints()));
	else if (qstrSlotName == "StartFly")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(StartFly()));
	else if (qstrSlotName == "StopFly")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(StopFly()));
	else if (qstrSlotName == "FastSpeed")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(FastSpeed()));
	else if (qstrSlotName == "ReduceSpeed")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(ReduceSpeed()));
	else if (qstrSlotName == "SaveFlyPath")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(SaveFlyPath()));
	else if (qstrSlotName == "LoadFlyPath")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(LoadFlyPath()));
	else if (qstrSlotName == "ReSetFlyPath")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(ReSetFlyPath()));
	else if (qstrSlotName == "PointRotationM")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(BeginRotation()));
	else if (qstrSlotName == "PointRotationH")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(FastRotationSpeed()));
	else if (qstrSlotName == "PointRotationL")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(ReduceRotationSpeed()));
	else if (qstrSlotName == "StopRotation")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(StopRotation()));
	else if (qstrSlotName == "StartFreeFly")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(StartFreeFly()));
	else if (qstrSlotName == "StopFreeFly")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(StopFreeFly()));
	else if (qstrSlotName == "ThrowingEnable")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(ThrowingEnable()));
	else if (qstrSlotName == "AddServer")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AddServer()));
	else if(qstrSlotName == "DistanceAnalysis3D")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(DistanceAnalysis3D()));
	else if (qstrSlotName == "AreaMeasureAnalysis3D")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(AreaMeasureAnalysis3D()));	
	else if (qstrSlotName == "TerrainProfileAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(TerrainProfileAnalysis()));
	else if (qstrSlotName == "Marathon")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(Marathon()));
	else if (qstrSlotName == "PauseFly")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(PauseFly()));
	else if (qstrSlotName == "BackFly")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(BackFly()));
	else if (qstrSlotName == "ReStartFly")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(RestartFly()));
	else if(qstrSlotName=="EditElement")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(EditElement()));
	else if (qstrSlotName == "VolumeAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(VolumeAnalysis()));
	else if (qstrSlotName == "OsgbProfileAnalysis")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(OsgbProfileAnalysis()));
	else if (qstrSlotName == "PlaceNameChange")
		connect(ptrAction, SIGNAL(triggered()), this, SLOT(PlaceNameChange()));
}

GLOBE_NAME::VolumeAnalysisPtr ptrVolume;

void onVolFinish(double dbVolume)
{
	double dbVol = dbVolume;
	int i = 1;
	i++;
}

void onDrawFinish()
{
	if (ptrVolume)
	{
		ptrVolume->Calculate(30.0);
	}
}

void MainWindow::VolumeAnalysis()
{
	ptrVolume = new GLOBE_NAME::VolumeAnalysis(ptrOblique,false);
	ptrVolume->OnCalcFinish().Add(&onVolFinish);
	ptrVolume->OnDrawFinish().Add(&onDrawFinish);
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrVolume);
}

GLOBE_NAME::DrawElmentAnalysisPtr ptrDraw;
void MainWindow::Marathon()
{
	//ptrGeoElement->setPolylineAngle(30.0);

	m_ptrGeoSpace3D->AnalysisBox()->RemoveAllAnalysis();
	ptrDraw = new GLOBE_NAME::DrawElmentAnalysis();
	ptrDraw->MinPointCount(10);
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrDraw);
	ptrDraw->SetEnable(true);
	ptrDraw->OnDrawFinishEvent().Add(this, &MainWindow::OnFinish);

	/*GLOBE_NAME::MarathonAnalysisPtr ptrMarathon = new GLOBE_NAME::MarathonAnalysis();
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrMarathon);*/
}

void MainWindow::AddVector()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开矢量数据集"), ".", tr("Local Dataset(*.shp)"));
	
	GLOBE_NAME::VectorLayerPtr ptrVector = new GLOBE_NAME::VectorLayer(fileName.toLocal8Bit().data(),GLOBE_NAME::eLocal);
	ptrVector->StartPick();
	m_ptrGeoSpace3D->SceneBox()->RangeFlyTo(ptrVector->xMin(), ptrVector->xMax(), ptrVector->yMin(), ptrVector->yMax());
}

void MainWindow::AddServer()
{
	ptrLoadSerWnd = new LoadOGCServiceWindow();
	connect(ptrLoadSerWnd->ui.okPushButton, SIGNAL(clicked()), this, SLOT(AddServerSlot()));
	ptrLoadSerWnd->show();
}

void MainWindow::AddOsgbServerLoop()
{
	char filename[] = "D:\\osgbserver.txt";
	FILE *fp;
	char serverURL[1024];
	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("error!");
		return ;
	}

	while (!feof(fp))
	{
		fgets(serverURL, 1024, fp);
		std::string strServer = serverURL;
		strServer = strServer.substr(0, strServer.size()-1);

		GLOBE_NAME::LayerPtr ptrLayer = new GLOBE_NAME::ObliqueImageLayer(strServer, GLOBE_NAME::eServer);
		m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrLayer);

		printf("%s\n", serverURL);
	}

	fclose(fp);

	/*std::fstream file;
	file.open("‪D:\\osgbserver.txt");
	if (!file.is_open())
	{
		std::cout << "未成功打开文件" << std::endl;
	}

	std::string serverURL;
	while (file.getline((char*)serverURL.c_str(),256))
	{
		GLOBE_NAME::LayerPtr ptrLayer = new GLOBE_NAME::ObliqueImageLayer(serverURL, GLOBE_NAME::eServer);
		m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrLayer);
	}
	file.close();*/
}

void OnPicked(GLOBE_NAME::Feature& ptrFeature)
{
	std::string str =  ptrFeature.m_attr["fdg"];
	int i = 0;
	i++;
}

void MainWindow::AddServerSlot()
{
	ptrLoadSerWnd->hide();
	QString qstrServerType = ptrLoadSerWnd->ui.serverTypeCB->currentText();
	std::string strUrl = ptrLoadSerWnd->ui.urlLineEdit->text().toStdString();

	if (qstrServerType.toLocal8Bit() == "OGC WMTS服务")
	{
		strUrl = "http://t0.tianditu.gov.cn/img_c/wmts";
		GLOBE_NAME::Layer* ptrLayerImage = new GLOBE_NAME::ImageLayer(strUrl, GLOBE_NAME::eWMTS);
		strUrl = "http://t0.tianditu.gov.cn/cia_c/wmts";
		GLOBE_NAME::Layer* ptrLayerImageText = new GLOBE_NAME::ImageLayer(strUrl, GLOBE_NAME::eWMTS);

		strUrl = "https://t10.tianditu.gov.cn/dem_sjk/services/tile";
		GLOBE_NAME::Layer* ptrLayerTerrain = new GLOBE_NAME::TerrainLayer(strUrl, GLOBE_NAME::eGeoGlobeTile);
		if (ptrLayerTerrain)
		{
			m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrLayerImage);
			m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrLayerImageText);
			m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrLayerTerrain);
			//m_ptrGeoSpace3D->SceneBox()->RangeFlyTo(ptrLayerTerrain->xMin(), ptrLayerTerrain->xMax(), ptrLayerTerrain->yMin(), ptrLayerTerrain->yMax());
		}
	}
	else if (qstrServerType.toLocal8Bit() == "OSGB 倾斜服务")
	{
		//AddOsgbServerLoop();

		GLOBE_NAME::ObliqueImageLayerPtr ptrLayer = new GLOBE_NAME::ObliqueImageLayer(strUrl, GLOBE_NAME::eServer,0,18000);
		ptrLayer->setPickParamter("http://100.98.3.99:9010/MOVE_ONE/wfs","KUO4547");
		ptrLayer->StartPick();
		ptrLayer->OnPicked().Add(&OnPicked);
		
		//GLOBE_NAME::ObliqueImageLayer::StartChangeRangList();

		KERNEL_NAME::GsRawPoint3D pos(ptrLayer->Longitude(), ptrLayer->Latitude(), ptrLayer->Height() + 100);
		//GLOBE_NAME::TextureMovieElementPtr ptrMovie = new GLOBE_NAME::TextureMovieElement("./广场东北.rmvb", pos, 80);
		//ptrMovie->Rotate(KERNEL_NAME::GsRawPoint3D(0.0, 0.0, 1.0), 30.0);
		//KERNEL_NAME::GsRawPoint3D point = ptrMovie->getPosition();
		//point.X -= 0.0001;
		//point.Y -= 0.0005;
		//ptrMovie->setPosition(point);

		GLOBE_NAME::StreamMovieElementPtr ptrMovie = new GLOBE_NAME::StreamMovieElement(pos,1920,1080,20);
		//ptrMovie->setRectangle(400,400);

		GLOBE_NAME::OverlayElementPtr ptrOverLay = new GLOBE_NAME::OverlayElement(/*ptrModelEle*/ptrMovie, ptrLayer);
		m_ptrGeoSpace3D->ElementBox()->AddElement(ptrOverLay);
		//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrMovie);

		//m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrLayer);
		m_ptrGeoSpace3D->SceneBox()->PointFlyTo(ptrLayer->Longitude(), ptrLayer->Latitude(), ptrLayer->Height() + 2000);
	}
	else if (qstrServerType.toLocal8Bit() == "OGC WFS服务")
	{
		std::vector<std::string> vecTypeName = GLOBE_NAME::WFSLayer::getTypeNameList(strUrl);
		if (vecTypeName.size() == 0)
			return;

		GLOBE_NAME::WFSLayerPtr ptrWFSLayer = new GLOBE_NAME::WFSLayer(strUrl, vecTypeName[0], /*new GLOBE_NAME::RoadTextStyle(16,KERNEL_NAME::GsColor(255,255,255))*/ KERNEL_NAME::GsRawPoint3D(541011, 3369298,0));
		//GLOBE_NAME::WFSLayerPtr ptrRoadLayer = new GLOBE_NAME::WFSLayer(strUrl, vecTypeName[0]);
		//ptrLayer->StartPick();
		m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrWFSLayer);
		//m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrRoadLayer);
		m_ptrGeoSpace3D->SceneBox()->RangeFlyTo(ptrWFSLayer->xMin(), ptrWFSLayer->xMax(), ptrWFSLayer->yMin(), ptrWFSLayer->yMax());
	}
	else if (qstrServerType.toLocal8Bit() == "吉奥瓦片服务")
	{
		GLOBE_NAME::LayerPtr ptrLayer = new GLOBE_NAME::PlaceNameLayer(strUrl, GLOBE_NAME::eGeoGlobeTile,false,0.0,10000.0);

		if (ptrLayer)
		{
			m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrLayer);
			m_ptrGeoSpace3D->SceneBox()->RangeFlyTo(ptrLayer->xMin(), ptrLayer->xMax(), ptrLayer->yMin(), ptrLayer->yMax());
		}
	}

	ptrLoadSerWnd->close();
}

void MainWindow::ThrowingEnable()
{
	bool bPreThrowingEnable = m_ptrGeoSpace3D->SceneBox()->ThrowingEnable();

	m_ptrGeoSpace3D->SceneBox()->ThrowingEnable(!bPreThrowingEnable);
}

void MainWindow::AddFlyPoints()
{
	m_ptrGeoSpace3D->FlyBox()->addFlyPoint();
}

void MainWindow::StartFly()
{	
	std::string fileName;
	fileName = "";	
	m_ptrGeoSpace3D->FlyBox()->beginFly(fileName);
}

void MainWindow::StopFly()
{
	m_ptrGeoSpace3D->FlyBox()->stopFly();
}

void MainWindow::PauseFly()
{
	m_ptrGeoSpace3D->FlyBox()->pauseFly();
}

void MainWindow::RestartFly()
{
	m_ptrGeoSpace3D->FlyBox()->restartFly();
}

void MainWindow::BackFly()
{
	m_ptrGeoSpace3D->FlyBox()->backFly(true);
}

void MainWindow::FastSpeed()
{
	m_ptrGeoSpace3D->FlyBox()->fastSpeed();
}

void MainWindow::ReduceSpeed()
{
	m_ptrGeoSpace3D->FlyBox()->reduceSpeed();
}

void MainWindow::SaveFlyPath()
{
	QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("选择保存路径"), ".", tr("Local Dataset(*.path)"));
	std::string SaveFileName = fileName.toLocal8Bit();
	m_ptrGeoSpace3D->FlyBox()->saveFlyPath(SaveFileName);
}

void MainWindow::LoadFlyPath()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("加载飞行路径"), ".", tr("Local Dataset(*.path)"));

	if (fileName.isEmpty())
		return;

	std::string FlyPathFileName = fileName.toLocal8Bit();

	m_ptrGeoSpace3D->FlyBox()->beginFly(FlyPathFileName);
}

void MainWindow::ReSetFlyPath()
{
	m_ptrGeoSpace3D->FlyBox()->reSetFlyPath();
}

GLOBE_NAME::EditElmentAnalysisPtr ptrEdit;
void MainWindow::EditElement()
{
	m_ptrGeoSpace3D->AnalysisBox()->RemoveAllAnalysis();
	ptrEdit = new GLOBE_NAME::EditElmentAnalysis();
	ptrEdit->SetEnable(true);
	//ptrDraw->MaxPointCount(1);
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrEdit);
	ptrEdit->OnSelectEvent().Add(this, &MainWindow::OnSelectEvent);
}
int n = 0;
GLOBE_NAME::LinePictureElement3DPtr lastlineElment;
void MainWindow::OnFinish(const std::vector<KERNEL_NAME::GsRawPoint3D>& vecPoint)
{
	//if (vecPoint.size() == 1)
	//{
	//	// 	GLOBE_NAME::TextElementPtr ptrTextEle = new GLOBE_NAME::TextElement(vecPoint[0].X, vecPoint[0].Y, vecPoint[0].Z, "补给处");
	//	// 	m_ptrGeoSpace3D->ElementBox()->AddElement(ptrTextEle);


	//	GLOBE_NAME::GeoBillBoardElementPtr ptrGeoElement1 = new GLOBE_NAME::GeoBillBoardElement(
	//		vecPoint[0].X, vecPoint[0].Y, vecPoint[0].Z, "15",
	//		"D:/guangguviewsys/data/mapmarkers/mapmarkers1.png", 8);
	//	//ptrGeoElement1->TextSize(20);
	//	//ptrGeoElement1->setBackGrandColor(KERNEL_NAME::GsColor(255,151,51,255));
	//	ptrGeoElement1->TextColor(KERNEL_NAME::GsColor(0.0, 0.0, 0.0));
	//	//ptrGeoElement1->setLineColor(KERNEL_NAME::GsColor(0, 0, 255, 255));
	//	ptrGeoElement1->CoverAutoHide(false);

	//	m_ptrGeoSpace3D->ElementBox()->AddElement(ptrGeoElement1);

	//	return;
	//}
	//// 绘制点标注
	//if (vecPoint.size() == 1)
	//{
	//	n++;
	//	GLOBE_NAME::BillBoardElementPtr ptrBilloard = new GLOBE_NAME::BillBoardElement(vecPoint[0].X, vecPoint[0].Y, vecPoint[0].Z,std::to_string(n), "mapmarkers.png");
	//	m_ptrGeoSpace3D->ElementBox()->AddElement(ptrBilloard);
	//	return;
	//}
	// 图片线样式
	//if (lastlineElment)
	//	lastlineElment->Visible(false);
	//GLOBE_NAME::LinePictureElement3DPtr lineElment = new GLOBE_NAME::LinePictureElement3D(36, 128, "lineflash.png");
	//lineElment->setPoints(vecPoint);
	//// 添加线的动画
	//GLOBE_NAME::LineAnimatorActionPtr lineAnimator = new GLOBE_NAME::LineAnimatorAction(3000, true);
	//lineAnimator->setAphla(new GLOBE_NAME::FloatAnimator({ 0.2f,1,0.2f }));
	//lineElment->SetAnimatorAction(lineAnimator);
	//m_ptrGeoSpace3D->ElementBox()->AddElement(lineElment);
	//lastlineElment = lineElment;
	//面样式
	//GLOBE_NAME::PolygonSymbolElementPtr poly = new GLOBE_NAME::PolygonSymbolElement();
	//poly->setLineWidth(1);
	//poly->setLineColor(KERNEL_NAME::GsColor(255, 255, 0, 255));
	//poly->setPoints(vecPoint);
	//m_ptrGeoSpace3D->ElementBox()->AddElement(poly);
	GLOBE_NAME::LineSymbolElement3DPtr ptrLineSymbol = new GLOBE_NAME::LineSymbolElement3D(GLOBE_NAME::POLYGON);
	GLOBE_NAME::LineSymbolElement3DPtr ptrLineLoopSymbol = new GLOBE_NAME::LineSymbolElement3D(GLOBE_NAME::LINE_LOOP);

	for (std::vector<KERNEL_NAME::GsRawPoint3D>::const_iterator it = vecPoint.begin(); it!= vecPoint.end();it++)
	{
		ptrLineSymbol->AddPoint((*it).X, (*it).Y, (*it).Z);
		ptrLineLoopSymbol->AddPoint((*it).X, (*it).Y, (*it).Z);
	}

	ptrLineSymbol->Finish();
	ptrLineSymbol->setColor(0,0,255,125);
	//ptrLineSymbol->Rotate(KERNEL_NAME::GsRawPoint3D(0.0,0.0,1.0),45.0);
	//ptrLineLoopSymbol->Rotate(KERNEL_NAME::GsRawPoint3D(0.0, 0.0, 1.0), 45.0);
	//ptrLineLoopSymbol->setLineWidth(2);
	//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrLineLoopSymbol);
	m_ptrGeoSpace3D->ElementBox()->AddElement(ptrLineSymbol);
}

void MainWindow::OnSelectEvent(GLOBE_NAME::Element * oldEle, GLOBE_NAME::Element * newEle)
{
}

void MainWindow::BeginRotation()
{
	m_ptrGeoSpace3D->FlyBox()->beginRotation(65);
}

void MainWindow::ReduceRotationSpeed()
{
	m_ptrGeoSpace3D->FlyBox()->reduceRotaionSpeed();
}

void MainWindow::FastRotationSpeed()
{
	m_ptrGeoSpace3D->FlyBox()->fastRotationSpeed();
}

void MainWindow::StopRotation()
{
	m_ptrGeoSpace3D->FlyBox()->stopRotation();
}

void MainWindow::StartFreeFly()
{
	m_ptrGeoSpace3D->FlyBox()->beginFreeFly();
}

void MainWindow::StopFreeFly()
{
	m_ptrGeoSpace3D->FlyBox()->stopFreeFly();
}

void OnChange()
{
	int i = 0;
	i++;
}

void MainWindow::Test()
{
	//ptrOblique = new GLOBE_NAME::ObliqueImageLayer("F:/gaunggu/moxing/DSM", GLOBE_NAME::eLocal);
	//m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrOblique);
	//m_ptrGeoSpace3D->SceneBox()->PointFlyTo(ptrOblique->Longitude(), ptrOblique->Latitude(), ptrOblique->Height()+100);
	//ptrOblique->StartPick();
	////ptrMovie->setRectangle(32, 800);
	////return;

	////if (ptrDraw)
	////	ptrDraw->MaxPointCount(1);

	//GLOBE_NAME::LineSymbolElement3DPtr ptrLineSym3D = new GLOBE_NAME::LineSymbolElement3D(GLOBE_NAME::POLYGON);
	//ptrLineSym3D->AddPoint(110.0, 40.0, 1000000.0);
	//ptrLineSym3D->AddPoint(80.0, 30.0, 1000000.0);
	//ptrLineSym3D->AddPoint(80.0, 20.0, 1000000.0);
	//ptrLineSym3D->AddPoint(110.0,80.0, 1000000.0);

	//ptrLineSym3D->setColor(0,255,0,123);
	//ptrLineSym3D->setLineWidth(5);
	//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrLineSym3D);

	//m_ptrGeoSpace3D->CaptureScreen("D:/capture.png");

	//double x, y;
	//m_ptrGeoSpace3D->SceneBox()->LonLat2Window(113, 10, 0.0, x, y);

	//KERNEL_NAME::GsRawPoint3D pos(114.42646193016812, 30.442340959459351, 0.00000000000000000);
	//m_ptrGeoSpace3D->SceneBox()->DeCorrectPosByOblique(pos);


	////ptrLineSym3D->AddPoint(60.0, 20.0, 1000000.0);

	////GLOBE_NAME::PointSymbolElement3DPtr ptrPoint3D = new GLOBE_NAME::PointSymbolElement3D(114.0,30.0,10000);
	////ptrPoint3D->setColor(0,255,0);
	////m_ptrGeoSpace3D->ElementBox()->AddElement(ptrPoint3D);
	////GLOBE_NAME::PolygonSymbolElementPtr ptrPolygon = new GLOBE_NAME::PolygonSymbolElement();
	////ptrPolygon->AddPoint(110, 40, 0);
	////ptrPolygon->AddPoint(50, 40, 0);
	////ptrPolygon->AddPoint(50, 60, 0);
	////ptrPolygon->AddPoint(110, 60, 0);

	////ptrPolygon->setFillColor(0, 255, 0, 0.5);
	//////ptrPolygon->ClearFillColor();
	////m_ptrGeoSpace3D->ElementBox()->AddElement(ptrPolygon);

	GLOBE_NAME::LineSymbolElementPtr ptrLineSym = new GLOBE_NAME::LineSymbolElement();
	ptrLineSym->AddPoint(110, 40, 0);
	ptrLineSym->AddPoint(50, 40, 0);
	ptrLineSym->AddPoint(50, 60, 0);
	ptrLineSym->setColor(255, 0, 0, 0.5);
	ptrLineSym->setSize(5);
	m_ptrGeoSpace3D->ElementBox()->AddElement(ptrLineSym);

	////GLOBE_NAME::PointSymbolElementPtr ptrPointSym = new GLOBE_NAME::PointSymbolElement();

	////ptrPointSym->AddPoint(110, 40, 0);
	////ptrPointSym->AddPoint(50, 40, 0);
	////ptrPointSym->AddPoint(50, 60, 0);
	////ptrPointSym->AddPoint(110, 60, 0);

	////ptrPointSym->setColor(255, 0, 0);
	////ptrPointSym->setSize(10);

	////m_ptrGeoSpace3D->ElementBox()->AddElement(ptrPointSym);


	////ptrLayer->Visible(!ptrLayer->Visible());

	//GLOBE_NAME::BillBoardElementPtr ptrElement = new GLOBE_NAME::BillBoardElement(114.0, 30.0, 0, "abs", "D:\\ddaaa.gif");
	////ptrElement->Font("msyh.ttc");
	////ptrElement->TextColor(KERNEL_NAME::GsColor(255, 0, 0));
	//////ptrElement->TextSize(30.0);
	//ptrElement->CoverAutoHide(false);
	//ptrElement->IconAlignment(GLOBE_NAME::Alignment::ALIGN_RIGHT);
	////ptrElement->Icon("D:\\guangguviewsys\\data\\mapmarkers\\mapmarkers_face.png");
	//////ptrElement->TextHaloColor(21, 54, 85);
	//ptrElement->setPosition(113, 10, 0);
	////ptrElement->Text("啊哈");
	//ptrElement->setShadowColor(KERNEL_NAME::GsColor(255,0,0,123));
	//ptrElement->setShadowOffset(0.08);
	////ptrElement->Updata().Add(this, &MainWindow::OnBillBordUpdate);
	////GLOBE_NAME::BillBoardElement::StartPick();
	////GLOBE_NAME::BillBoardElement::EndPick();
	////GLOBE_NAME::BillBoardElement::OnPicked().Add(&OnPicked);

	////GLOBE_NAME::PointSymbolElement3DPtr ptrPoint = new GLOBE_NAME::PointSymbolElement3D(114.0, 30.0, 0);
	////ptrPoint->setSize(10);
	//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrElement);

	//ptrGeoElement = new GLOBE_NAME::GeoBillBoardElement(112.0, 30.0, 0.0, "14","D:/guangguviewsys/data/mapmarkers/mapmarkers_face.png"/*,8*/);
	////ptrGeoElement->TextColor(KERNEL_NAME::GsColor(0, 0, 0));
	//ptrGeoElement->Updata().Add(this, &MainWindow::OnBillBordUpdate);
	////ptrGeoElement->Visible(false);
	////ptrGeoElement->Visible(true);
	////ptrGeoElement->Dynamic(true); 
	////ptrGeoElement->Text("的公司的热\n狗白热化吧三甲胺大开\n大合聚魄深V比较好");
	////ptrGeoElement->TextAlignment(GLOBE_NAME::Alignment::ALIGN_RIGHT);
	////ptrGeoElement->setBackGrandColor(KERNEL_NAME::GsColor(255, 0, 0, 200));
	////ptrGeoElement->setOutLineColor(KERNEL_NAME::GsColor(0, 255, 0, 255));
	//ptrGeoElement->setLineColor(KERNEL_NAME::GsColor(0, 0, 255, 255));
	//ptrGeoElement->CoverAutoHide(false);
	//ptrGeoElement->setPolylineLen(100);
	////ptrGeoElement->Icon("D:/guangguviewsys/data/mapmarkers/mapmarkers_face.png");
	////ptrGeoElement->HightLight();


	//GLOBE_NAME::GeoBillBoardElementPtr ptrGeoElement1 = new GLOBE_NAME::GeoBillBoardElement(105.00124, 31.0, 0.0, "14", "D:/guangguviewsys/data/mapmarkers/mapmarkers1.png", 8);
	//ptrGeoElement1->TextSize(20);
	//ptrGeoElement1->TextColor(KERNEL_NAME::GsColor(254, 183, 37));
	////ptrGeoElement1->setLineColor(KERNEL_NAME::GsColor(0, 0, 255, 255));
	//ptrGeoElement1->CoverAutoHide(false);

	//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrGeoElement1);

	//std::vector<GLOBE_NAME::GeoBillBoardElementPtr> vecBillboard;

	//double lon = 114.0;
	//double lat = 30.0;
	//for (int i = 0; i < 30; i++)
	//{
		//for (int j = 0; j < 60; j++)
		//{
		//	GLOBE_NAME::GeoBillBoardElementPtr ptrE1 = new GLOBE_NAME::GeoBillBoardElement(lon += 0.001*i, lat += 0.0001*j, 0.0, "", "D:\\guangguviewsys\\data\\mapmarkers\\mapmarkers_police_selected.png");
		//	ptrE1->CoverAutoHide(false);
		//	m_ptrGeoSpace3D->ElementBox()->AddElement(ptrE1);
		//}
		//static double lat = 0.0;
		//GLOBE_NAME::GeoBillBoardElementPtr ptrE1 = new GLOBE_NAME::GeoBillBoardElement(114.0, lat += 1, 0.0, "", "D:\\guangguviewsys\\data\\mapmarkers\\mapmarkers_police_selected.png");
		//GLOBE_NAME::GeoBillBoardElementPtr ptrE2 = new GLOBE_NAME::GeoBillBoardElement(114.0, lat += 1, 0.0, "", "D:\\guangguviewsys\\data\\mapmarkers\\mapmarkers_qiangxing.png");
		//GLOBE_NAME::GeoBillBoardElementPtr ptrE3 = new GLOBE_NAME::GeoBillBoardElement(114.0, lat += 1, 0.0, "", "D:\\guangguviewsys\\data\\mapmarkers\\mapmarkers_wifi.png");
		//GLOBE_NAME::GeoBillBoardElementPtr ptrE4 = new GLOBE_NAME::GeoBillBoardElement(114.0, lat += 1, 0.0, "", "D:\\guangguviewsys\\data\\mapmarkers\\mapmarkers_police_selected.png");

		//ptrE->CoverAutoHide(false);
		//ptrE->setPolylineAngle(30.0);
		//ptrE->setPolylineLen(100);
		//ptrE->Updata().Add(this, &MainWindow::OnBillBordUpdate);
		//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrE1);
		//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrE2);
		//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrE3);
		//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrE4);
		//vecBillboard.push_back(ptrE);
	//}

	//ptrE1->Icon("D:\\guangguviewsys\\data\\mapmarkers\\range_wifi.png");

	//GLOBE_NAME::PointAnimatorActionPtr pointAnimator = new GLOBE_NAME::PointAnimatorAction(3000, 5, false);
	//pointAnimator->setAphla(new GLOBE_NAME::FloatAnimator({ 0.2f,1,0.2f }));
	////ptrGeoElement->SetAnimatorAction(pointAnimator);
	////ptrGeoElement1->SetAnimatorAction(pointAnimator);
	////ptrElement->SetAnimatorAction(pointAnimator);

	////ptrGeoElement->HightLight();
	////ptrGeoElement->setShadowColor(KERNEL_NAME::GsColor(255,0,0,125));
	////ptrGeoElement->setShadowOffset(0.5);

	GLOBE_NAME::GeoBillBoardElementPtr ptrGeoElement1 = new GLOBE_NAME::GeoBillBoardElement(114.0, 30.0, 0.0, "担保人客观内容", "D:\\guangguviewsys\\data\\mapmarkers\\mapmarkers_police_selected.png");
	//ptrGeoElement1->TextColor(KERNEL_NAME::GsColor::Black);
	ptrGeoElement1->Updata().Add(this, &MainWindow::OnBillBordUpdate);
	ptrGeoElement1->Dynamic(true);
	ptrGeoElement1->TextAlignment(GLOBE_NAME::Alignment::ALIGN_RIGHT);
	ptrGeoElement1->setLineColor(KERNEL_NAME::GsColor(0, 0, 255, 255));
	ptrGeoElement1->CoverAutoHide(false);
	ptrGeoElement1->setPolylineAngle(30.0);
	ptrGeoElement1->setPolylineLen(100);
	m_ptrGeoSpace3D->ElementBox()->AddElement(ptrGeoElement1);
	////ptrGeoElement->Font("simkai.ttf");
	//
	GLOBE_NAME::GeoBillBoardElement::StartPick();

	////GLOBE_NAME::FollowerCamera* ptrFollowerCamera = new GLOBE_NAME::FollowerCamera(GLOBE_NAME::follow);
	////ptrFollowerCamera->Bind(ptrGeoElement);
	////ptrFollowerCamera->Unbind(ptrElement);

	////GLOBE_NAME::TextureMovieElementPtr ptrMovie = new GLOBE_NAME::TextureMovieElement("./广场东北.rmvb",110.0,30.0,100.0);
	//std::function<void(int n)> f;
	////m_ptrGeoSpace3D->ElementBox()->AddElement(ptrMovie);
	//dynamic_cast<GLOBE_NAME::LonAndLatElement*>(m_ptrGeoSpace3D->ElementBox()->LonAndLatElement())->Change().Add(&OnChange);
	////m_ptrGeoSpace3D->ElementBox()->AddElement(ptrGeoElement);
	////m_ptrGeoSpace3D->ElementBox()->AddElement(ptrElement);
	////m_ptrGeoSpace3D->ElementBox()->AddElement(ptrGeoElement1);
	////m_ptrGeoSpace3D->SceneBox()->PointFlyTo(114.0, 30.0, 0.0);

	//double dbAngel = m_ptrGeoSpace3D->SceneBox()->CameraNorthAngle();

	////GLOBE_NAME::ModelElementPtr ptrModel = new GLOBE_NAME::ModelElement("E:\\HK.OSGB", 114.0, 30.0, 0.0);
	////m_ptrGeoSpace3D->ElementBox()->AddElement(ptrModel);
	////m_ptrGeoSpace3D->SceneBox()->PointFlyTo(114.0, 30.0, 10.0);
	////m_ptrGeoSpace3D->SceneBox()->PositiveAngle(0);
	////m_ptrGeoSpace3D->SceneBox()->setFixedAngle(true);
	////m_ptrGeoSpace3D->SceneBox()->setMinMaxDistance(1.0,1000000);
	////m_ptrGeoSpace3D->ElementBox()->CompassElement()->setPosition(100.0,100.0,0.0);
	////GLOBE_NAME::CompassElementPtr ptrComPass = m_ptrGeoSpace3D->ElementBox()->CompassElement();
	////ptrComPass->setAlignment(GLOBE_NAME::ALIGN_RIGHT, GLOBE_NAME::ALIGN_BOTTOM);

}

void MainWindow::OnBillBordUpdate(GLOBE_NAME::Element* ptrBillBord)
{
	double x, y, z;
	ptrBillBord->getPosition(x, y, z);
	x += 0.0001;
	ptrBillBord->setPosition(x,y,z);
}

void MainWindow::OnHeightAnalasisFinish(double dbResult)
{
	double dbHeight = dbResult;
}

void MainWindow::HeightAnalysis()
{
	GLOBE_NAME::HeightAnalysisPtr ptrHeight = new GLOBE_NAME::HeightAnalysis();
	ptrHeight->OnFinish().Add(this,&MainWindow::OnHeightAnalasisFinish);
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrHeight);

	ptrAnalysis = ptrHeight.p;
}

void MainWindow::OPInRangeAnalysis()
{
    GLOBE_NAME::OPInRangeAnalysisPtr ptrOPInRange = new GLOBE_NAME::OPInRangeAnalysis();
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrOPInRange);

	ptrAnalysis = ptrOPInRange.p;
}

void MainWindow::AddModelElement()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择要加载的模型"), ".", tr("Local Dataset(*.osg);;(*.osgt)"));
	if (!fileName.isEmpty())
	{
		GLOBE_NAME::ElementPtr elementPtr = new GLOBE_NAME::ModelElement(fileName.toLocal8Bit().data(), 40, 120, 5000);
		if (elementPtr)
		{
			//elementPtr->Position(40, 120, 5);
			
			m_ptrGeoSpace3D->ElementBox()->AddElement(elementPtr);
			
			m_ptrGeoSpace3D->SceneBox()->PointFlyTo(elementPtr->Longitude(), elementPtr->Latitude(), elementPtr->Height());
		}
	}
	return;
}

void MainWindow::AddLonAndLatElement()
{
	GLOBE_NAME::LonAndLatElementPtr ptrLonAndLatElement = new GLOBE_NAME::LonAndLatElement();
	m_ptrGeoSpace3D->ElementBox()->AddElement(ptrLonAndLatElement);
}

void MainWindow::AddCompassElement()
{
	GLOBE_NAME::CompassElementPtr ptrCompassElement = new GLOBE_NAME::CompassElement();
	m_ptrGeoSpace3D->ElementBox()->AddElement(ptrCompassElement);
}

void MainWindow::AddBillBoardElement()
{

//GLOBE_NAME::BillBoardElementPtr elementPtr = new GLOBE_NAME::BillBoardElement(-74, 40, 0);
	
// 	GLOBE_NAME::GeoBillBoardElementPtr elementPtr = new GLOBE_NAME::GeoBillBoardElement(-74, 40, 0, "");
// 	if (elementPtr)
// 	{
// 		m_ptrGeoSpace3D->ElementBox()->AddElement(elementPtr);
// 	}
	
	
}
void MainWindow::AddGeoBillBoardElement()
{
	//GLOBE_NAME::BillBoardElementPtr elementPtr = new GLOBE_NAME::BillBoardElement(-74, 40, 0, "ppp\n\p2p");
	////GLOBE_NAME::GeoBillBoardElementPtr elementPtr = new GLOBE_NAME::GeoBillBoardElement(-74, 40, 0, "N");
	//if (elementPtr)
	//{
	//	m_ptrGeoSpace3D->ElementBox()->AddElement(elementPtr);
	//}
}


void MainWindow::LineOfSightAnalysis()
{
	GLOBE_NAME::LineOfSightAnalysisPtr ptrLineOfSightAnalysis = new GLOBE_NAME::LineOfSightAnalysis();
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrLineOfSightAnalysis);

	ptrAnalysis = ptrLineOfSightAnalysis.p;
}

void MainWindow::MultipleLineOfSightAnalysis()
{
	GLOBE_NAME::MultipleLineOfSightAnalysisPtr ptrMultipleLineOfSightAnalysis = new GLOBE_NAME::MultipleLineOfSightAnalysis();
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrMultipleLineOfSightAnalysis);
	ptrAnalysis = ptrMultipleLineOfSightAnalysis.p;
	
}

void MainWindow::AreaAnalysis()
{
	GLOBE_NAME::AreaAnalysisPtr ptrArea = new GLOBE_NAME::AreaAnalysis();
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrArea);
	//ptrAnalysis = ptrArea.p;
}

void MainWindow::RadarAnalysis()
{
	GLOBE_NAME::RadarAnalysisPtr ptrRadar = new GLOBE_NAME::RadarAnalysis();
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrRadar);
}

void MainWindow::TerrainProfileAnalysis()
{
	GLOBE_NAME::TerrainProfileAnalysisPtr ptrTerrainProfile = new GLOBE_NAME::TerrainProfileAnalysis();
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrTerrainProfile);
}

void MainWindow::OsgbProfileAnalysis()
{
	GLOBE_NAME::OsgbProfileAnalysisPtr ptrOsgbProfile = new GLOBE_NAME::OsgbProfileAnalysis(ptrOblique);
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrOsgbProfile);
}

void MainWindow::RemoveAllAnalysis()
{	
// 	if (_distanceAnalysis == true)
// 		m_ptrGeoSpace3D->AnalysisBox()->RemoveAnalysis(ptrDistance);
// 	_distanceAnalysis = false;
	m_ptrGeoSpace3D->AnalysisBox()->RemoveAllAnalysis();
	//m_ptrGeoSpace3D->ElementBox()->RemoveAllElement();
}

void MainWindow::RemoveAnalysis()
{	
	m_ptrGeoSpace3D->AnalysisBox()->RemoveAnalysis(ptrAnalysis);
}

//cpp最上方有做test的变量 需要删除
void MainWindow::DistanceAnalysis()
{
	//GLOBE_NAME::DistanceAnalysisPtr ptrDistance = new GLOBE_NAME::DistanceAnalysis();
	GLOBE_NAME::DistanceAnalysisPtr ptrDistance = new GLOBE_NAME::DistanceAnalysis();
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrDistance);
	//ptrAnalysis = ptrDistance.p;
}

void MainWindow::DistanceAnalysis3D()
{
	GLOBE_NAME::DistanceAnalysis3DPtr ptrDistance3D = new GLOBE_NAME::DistanceAnalysis3D();
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrDistance3D);
	ptrAnalysis = ptrDistance3D.p;
}

void MainWindow::AreaMeasureAnalysis3D()
{
	GLOBE_NAME::AreaAnalysis3DPtr ptrArea3D = new GLOBE_NAME::AreaAnalysis3D();
	m_ptrGeoSpace3D->AnalysisBox()->AddAnalysis(ptrArea3D);
	ptrAnalysis = ptrArea3D.p;
}

void MainWindow::AddPlaceName()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开三维地名数据集"), ".", tr("Local Dataset(*.tfg)"));

	if (!fileName.isEmpty())
	{
		GLOBE_NAME::LayerPtr ptrLayer = new GLOBE_NAME::PlaceNameLayer(fileName.toLocal8Bit().data(), GLOBE_NAME::eLocal,false);
		//GLOBE_NAME::LayerPtr ptrLayer = new GLOBE_NAME::PlaceNameLayer("http://192.168.100.134:9010/zj/wtfs", GLOBE_NAME::eGeoGlobeTile);

		if (ptrLayer)
		{
			m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrLayer);
			m_ptrGeoSpace3D->SceneBox()->RangeFlyTo(ptrLayer->xMin(), ptrLayer->xMax(), ptrLayer->yMin(), ptrLayer->yMax());
		}
	}
}

void MainWindow::AddTerrain()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开本地地形数据集"), ".", tr("Local Dataset(*.tile)"));

	if (!fileName.isEmpty())
	{
		GLOBE_NAME::LayerPtr ptrLayer = new GLOBE_NAME::TerrainLayer(fileName.toLocal8Bit().data(), GLOBE_NAME::eLocal);
		//GLOBE_NAME::Layer* ptrLayer = new GLOBE_NAME::TerrainLayer("http://192.168.43.49:9010/WorldNewDEM/services/tile", GLOBE_NAME::eGeoGlobeTile);
		if (ptrLayer)
		{
			m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrLayer);
			m_ptrGeoSpace3D->SceneBox()->RangeFlyTo(ptrLayer->xMin(), ptrLayer->xMax(), ptrLayer->yMin(), ptrLayer->yMax());
		}
	}
}

void MainWindow::AddImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开本地影像数据集"), ".", tr("Local Dataset(*.tile)"));

	if (!fileName.isEmpty())
	{
		//KERNEL_NAME::GsSqliteGeoDatabaseFactory fac;
		//KERNEL_NAME::GsConnectProperty conn ;
		//conn.Server = QFileInfo(fileName).absoluteDir().absolutePath().toStdString() ;
		//KERNEL_NAME::GsGeoDatabasePtr gdb = fac.Open(conn);
		//KERNEL_NAME::GsTileClassPtr tileclass = gdb->OpenTileClass("DOM1.tile");

		//GLOBE_NAME::Layer* ptrLayer = new GLOBE_NAME::ImageLayer(tileclass.p);

		GLOBE_NAME::LayerPtr ptrLayer = new GLOBE_NAME::ImageLayer(fileName.toLocal8Bit().data(), GLOBE_NAME::eLocal);
		//GLOBE_NAME::Layer* ptrLayer = new GLOBE_NAME::ImageLayer("http://192.168.43.49:9010/XL_IMAGE/services/tile", GLOBE_NAME::eGeoGlobeTile);
		if (ptrLayer)
		{
			m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrLayer);
			m_ptrGeoSpace3D->SceneBox()->RangeFlyTo(ptrLayer->xMin(), ptrLayer->xMax(), ptrLayer->yMin(), ptrLayer->yMax());
		}
	}
}

void MainWindow::AddModelElement(std::string strPath,double x,double y,double z)
{
	QString m_sProjectPath = tr(strPath.c_str());
	QDir dir(m_sProjectPath);
	dir.exists();
	dir.setFilter(QDir::Files);
	QFileInfoList fileList = dir.entryInfoList();
	int fileCount = fileList.count();
	for (int i = 0; i < fileCount; i++)
	{
		QFileInfo fileInfo = fileList[i];
		QString suffix = fileInfo.suffix();

		if (QString::compare(suffix, QString(tr("osgb")), Qt::CaseInsensitive) == 0)
		{
			QString filePath = fileInfo.absoluteFilePath();/*获取文件绝对路径即全路径*/
			QString fileName = fileInfo.baseName();/*获取文件后名(不带后缀的文件名)*/

			GLOBE_NAME::ModelElementPtr ptrModel = new GLOBE_NAME::ModelElement(filePath.toStdString(), x, y, z);
			m_ptrGeoSpace3D->ElementBox()->AddElement(ptrModel);
		}
	}
}

void MainWindow::AddObliqueImage()
{
	QString qstrFileName = QFileDialog::getExistingDirectory(this);

	ptrOblique = new GLOBE_NAME::ObliqueImageLayer(qstrFileName.toLocal8Bit().data(), GLOBE_NAME::eLocal,300);
	//ptrOblique->setPickParamter("http://192.168.100.134:9010/unitdth/wfs", "XQKUO");
	//ptrOblique->StartPick();

	//AddModelElement("./osgbdata/", ptrLayer->Longitude(), ptrLayer->Latitude(), ptrLayer->Height());

	//GLOBE_NAME::ModelElementPtr ptrModel = new GLOBE_NAME::ModelElement("E:\\HK\\HK.OSGB", ptrOblique->Longitude(), ptrOblique->Latitude(), ptrOblique->Height());
	//ptrModel->setTranslate(541011, 3369298, 0);
	//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrModel);

	KERNEL_NAME::GsRawPoint3D pos(ptrOblique->Longitude(), ptrOblique->Latitude()+0.0015, ptrOblique->Height() + 100);
	GLOBE_NAME::TextureMovieElementPtr ptrMovie = new GLOBE_NAME::TextureMovieElement("./广场东北.rmvb", pos, 80);
	ptrMovie->Rotate(KERNEL_NAME::GsRawPoint3D(0.0, 0.0, 1.0), 30.0);
	KERNEL_NAME::GsRawPoint3D point = ptrMovie->getPosition();
	point.X -= 0.0001;
	point.Y -= 0.0005;
	ptrMovie->setPosition(point);
	//ptrMovie = new GLOBE_NAME::StreamMovieElement(pos,1920,1080,20.0);
	//ptrMovie->setRectangle(400,600);
	//GLOBE_NAME::TextureStreamElementPtr ptrMovie;
	//ptrMovie = new GLOBE_NAME::TextureStreamElement(pos, 80);
	//ptrMovie->Rotate(KERNEL_NAME::GsRawPoint3D(0.0,0.0,1.0),30.0);
	//KERNEL_NAME::GsRawPoint3D point = ptrMovie->getPosition();
	//point.X -= 0.0001;
	//point.Y -= 0.0005;
	//ptrMovie->setPosition(point);

	//GLOBE_NAME::ModelElementPtr ptrModelEle = new GLOBE_NAME::ModelElement("F:/GsOSGEarthDev/dependency/osg/OpenSceneGraph-Data/cow.osgt", pos.X, pos.Y, pos.Z);
	 //	point.X += 0.0012;
	 //	point.Y += 0.0018;
	 //	ptrMovie->setPosition(point);

	GLOBE_NAME::OverlayElementPtr ptrOverLay = new GLOBE_NAME::OverlayElement(/*ptrModelEle*/ptrMovie, ptrOblique);
	m_ptrGeoSpace3D->ElementBox()->AddElement(ptrOverLay);
	//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrModelEle);
	/*m_ptrGeoSpace3D->ElementBox()->AddElement(ptrModel);*/
	//m_ptrGeoSpace3D->ElementBox()->AddElement(ptrMovie);
	//ptrOverLay->removeOverlaySubgraph(ptrMovie);
	//m_ptrGeoSpace3D->LayerBox()->AddLayer(ptrOblique);
	m_ptrGeoSpace3D->SceneBox()->PointFlyTo(ptrOblique->Longitude(), ptrOblique->Latitude(), ptrOblique->Height()+200);
}

void MainWindow::AddModel()
{

}

GeoStar::Kernel::GsFeatureClassPtr createFeatureClass(const char* server, const char* file, const GeoStar::Kernel::GsFeatureClassPtr  ptrFeatureClass_old) {
	GeoStar::Kernel::GsConnectProperty conn;
	conn.Server = server;//新建数据源的路径
	GeoStar::Kernel::GsSqliteGeoDatabaseFactory fac;
	GeoStar::Kernel::GsGeoDatabasePtr ptrGDB = fac.Open(conn);
	if (!ptrGDB)
		return NULL;
	//获取几何对象类型为点、线或者面
	GeoStar::Kernel::GsGeometryType featureType = ptrFeatureClass_old->GeometryType();
	//获取矢量要素的属性字段信息
	GeoStar::Kernel::GsFields fields = ptrFeatureClass_old->Fields();
	//获取旧FCS文件信息
	GeoStar::Kernel::GsFields gsFields;
	for (int i = 0, size = fields.Fields.size(); i < size; i++)
	{
		GeoStar::Kernel::GsField gsFld;
		gsFld.Name = fields.Fields[i].Name.c_str();
		if (fields.Fields[i].Type == GeoStar::Utility::Data::eDoubleType)
		{
			gsFld.Type = GeoStar::Utility::Data::GsFieldType::eDoubleType;
			//GeoStar::Kernel::GsField gsFld(fields.Fields[i].Name.c_str(), GeoStar::Utility::Data::GsFieldType::eDoubleType);
		}
		else if (fields.Fields[i].Type == GeoStar::Utility::Data::eIntType)
		{
			gsFld.Type = GeoStar::Utility::Data::GsFieldType::eIntType;
			//GeoStar::Kernel::GsField gsFld(fields.Fields[i].Name.c_str(), GeoStar::Utility::Data::GsFieldType::eIntType);
		}
		else
		{
			gsFld.Type = GeoStar::Utility::Data::GsFieldType::eStringType;
			//GeoStar::Kernel::GsField gsFld(fields.Fields[i].Name.c_str(), GeoStar::Utility::Data::GsFieldType::eStringType);
		}
		gsFields.Fields.push_back(gsFld);
	}

	GeoStar::Kernel::GsGeometryColumnInfo colummInfo;
	//几何类型 点、线和面等类型
	colummInfo.GeometryType = featureType;
	//新建
	// 文件名称，字段信息，结构和空间参考
	GeoStar::Kernel::GsFeatureClassPtr ptrFeatureClass_new = ptrGDB->CreateFeatureClass(file, gsFields, colummInfo,
		ptrFeatureClass_old->SpatialReference());
	if (!ptrFeatureClass_new)
		return 0;
	return ptrGDB->OpenFeatureClass(file);
}

void MainWindow::PlaceNameChange()
{
	QString openFile = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开三维地名数据集"), ".", tr("Local Dataset(*.fcs)"));
	if (!openFile.isEmpty())
	{
		QString filePath = QFileInfo(openFile).path();
		QString fileName = QFileInfo(openFile).fileName();
		GeoStar::Kernel::GsConnectProperty conn;
		conn.Server = filePath.toStdString();
		GeoStar::Kernel::GsSqliteGeoDatabaseFactory fac;
		GeoStar::Kernel::GsGeoDatabasePtr ptrGDB = fac.Open(conn);
		if (!ptrGDB)
			return;
		GeoStar::Kernel::GsFeatureClassPtr oldFeatureClass = ptrGDB->OpenFeatureClass(fileName.toStdString().c_str());

		//创建新FCS文件
		QString saveFile = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存三维地名数据集"), ".", tr("Local Dataset(*.fcs)"));
		if (saveFile.isEmpty())
			return;
		QString saveFilePath = QFileInfo(saveFile).path();
		QString saveFileName = QFileInfo(saveFile).fileName();
		GeoStar::Kernel::GsFeatureClassPtr  newFeatureClass = createFeatureClass(saveFilePath.toStdString().c_str(), saveFileName.toStdString().c_str(), oldFeatureClass);
		GeoStar::Kernel::GsFields fields = oldFeatureClass->Fields();
		GeoStar::Kernel::GsFeatureCursorPtr cursor = oldFeatureClass->Search();
		GeoStar::Kernel::GsFeaturePtr  oldFeature = NULL;
		int inpter[] = { 1,2,1 };
		while ((oldFeature = cursor->Next()) != NULL)
		{
			GeoStar::Kernel::GsGeometryBlob *oldgeom = oldFeature->GeometryBlob();
			int dim = oldgeom->CoordinateDimension();
			int pointSize = oldgeom->CoordinateLength() / dim;
			double* newPoints = new double[pointSize * 3];
			double* oldPoints = oldgeom->Coordinate();
			for (int i = 0; i < pointSize; i++)
			{
				KERNEL_NAME::GsRawPoint3D point(oldPoints[i*dim], oldPoints[i*dim + 1], oldPoints[i*dim + 2]);
				m_ptrGeoSpace3D->SceneBox()->CorrectPosByOblique(point);
				int index = i * 3;
				newPoints[index] = point.X;
				newPoints[index + 1] = point.Y;
				newPoints[index + 2] = point.Z;
			}
			//创建要素
			GeoStar::Kernel::GsFeaturePtr feature = newFeatureClass->CreateFeature();
			GeoStar::Kernel::GsGeometryPtr newGeom = GeoStar::Kernel::GsGeometryFactory::CreateGeometryFromOracle(inpter, 3, newPoints, pointSize * 3, 3);
			feature->Geometry(newGeom);

			//添加相应属性
			for (int i = 0, size = fields.Fields.size(); i < size; i++)
			{
				int clumu = fields.FindField(fields.Fields[i].Name.c_str());
				if (fields.Fields[i].Type == GeoStar::Utility::Data::eGeometryType)
					continue;
				else if (fields.Fields[i].Type == GeoStar::Utility::Data::eDoubleType)
					feature->Value(clumu, oldFeature->ValueDouble(clumu));
				else if (fields.Fields[i].Type == GeoStar::Utility::Data::eFloatType)
					feature->Value(clumu, oldFeature->ValueFloat(clumu));
				else if (fields.Fields[i].Type == GeoStar::Utility::Data::eIntType)
					feature->Value(clumu, oldFeature->ValueInt(clumu));
				else
					feature->Value(clumu, oldFeature->ValueString(clumu).c_str());
			}
			feature->Store();//保存到数据源
		}
	}
}

void MainWindow::createStatusBar()
{
    RibbonStatusBar* statusBar = new RibbonStatusBar();
    setStatusBar(statusBar);

	//statusBar->addAction(tr("Page 1 of 1"));
	//statusBar->addSeparator();
	//statusBar->addAction(tr("Words: 1"));
	//statusBar->addSeparator();
	//statusBar->addAction(tr("English (U.S.)"));
	//statusBar->addSeparator();
	//statusBar->addAction(tr("Track Changes: Off"));
	//statusBar->addSeparator();
	//statusBar->addAction(tr("Insert"));


    RibbonStatusBarSwitchGroup* switchGroup = new RibbonStatusBarSwitchGroup;
	/*
    QList<QAction*> actions;
    actions << m_actionPrintLayout;
    actions << m_actionFullScreen;
    QAction* actionWebLayout = new QAction(QIcon(":/res/smallweblayout.png"), tr("Web Layout"), this);
    actions << actionWebLayout;
    QAction* actionOutLine = new QAction(QIcon(":/res/smalloutline.png"), tr("Outline"), this);
    actions << actionOutLine;
    QAction* actionDraft = new QAction(QIcon(":/res/smalldraft.png"), tr("Draft"), this);
    actions << actionDraft;
    switchGroup->addActions(actions);*/
    statusBar->addPermanentWidget(switchGroup);

    m_zoomPercentage = new QToolButton(statusBar);
    m_zoomPercentage->setText(tr("100%"));
    m_zoomPercentage->setToolTip(tr("Zoom level. Click to open the Zoom dialog box."));
    statusBar->addPermanentWidget(m_zoomPercentage);
    QFontMetrics mt(statusBar->font());
    int minimumWidth = mt.boundingRect(QString("0000%")).width();
    m_zoomPercentage->setMinimumWidth(minimumWidth);

	//RibbonSliderPane* sliderPane = new RibbonSliderPane();
	//sliderPane->setScrollButtons(true);
	//sliderPane->setRange(0, 200); // range
	//sliderPane->setMaximumWidth(130);
	//sliderPane->setSingleStep(10);
	//sliderPane->setSliderPosition(100);
	//statusBar->addPermanentWidget(sliderPane, 1);
	//connect(sliderPane, SIGNAL(valueChanged(int)), this, SLOT(zoomSliderScroll(int)));
}

void MainWindow::createDockWindows()
{
    QDockWidget* dock = new QDockWidget(tr("Styles"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);

    QWidget* styleWidget = new QWidget();

    QWidget* styleGroup = new QWidget();
    QVBoxLayout* listLayout = new QVBoxLayout();
    listLayout->setMargin(0);

    QListWidget* customerList = new QListWidget();
    for (int i = 0; 10 > i; i++)
        customerList->addItems(QStringList(QObject::tr("Item %1").arg(i+1)));

    listLayout->addWidget(customerList);
    styleGroup->setLayout(listLayout);

    QGroupBox* optionsGroup = new QGroupBox(tr("Preview"));
    QVBoxLayout* optionsLayout = new QVBoxLayout();
    QCheckBox* showPreviewBox = new QCheckBox(tr("Show Preview"));
    showPreviewBox->setEnabled(false);
    QCheckBox* linkedStylesBox = new QCheckBox(tr("Disable Linked Styles"));
    optionsLayout->addWidget(showPreviewBox);
    optionsLayout->addWidget(linkedStylesBox);
    optionsGroup->setLayout(optionsLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(styleGroup);
    mainLayout->addWidget(optionsGroup);
    styleWidget->setLayout(mainLayout);

    dock->setWidget(styleWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    QList<QDockWidget*> docks; docks << dock;
    QList<int> sizes; sizes << 120;
    resizeDocks(docks, sizes, Qt::Vertical);
#endif
}

void MainWindow::fullScreen(bool checked)
{
    if (checked)
    {
        m_stateWindow = windowState();
        m_actionFullScreen->setChecked(true);
        ribbonBar()->setVisible(false);
        setWindowState(Qt::WindowFullScreen);
    }
    else
    {
        ribbonBar()->setVisible(true);
        setWindowState(m_stateWindow);
        m_actionFullScreen->setChecked(false);
    }
}

void MainWindow::stateStatusBar(int state)
{
    statusBar()->setVisible(state == Qt::Checked);
}

void MainWindow::zoomSliderScroll(int value)
{
    QString str;
    str += QObject::tr("%1%").arg(value);
    m_zoomPercentage->setText(str);
}

void MainWindow::showRibbonContextMenu(QMenu* menu, QContextMenuEvent* event)
{
    Q_UNUSED(menu);
    Q_UNUSED(event);
}

void MainWindow::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    RibbonMainWindow::keyPressEvent(event);
    if (event->key() == Qt::Key_Escape && m_actionFullScreen->isChecked())
        fullScreen(false);
}

void MainWindow::colorChanged(const QColor& c)
{
}

void MainWindow::customization()
{
    RibbonCustomizeManager* customizeManager = ribbonBar()->customizeManager();
    customizeManager->addAllActionsCategory(categoryAllCommands);

    customizeManager->addDefaultStateQAccessBar();
    customizeManager->addDefaultStateRibbonBar();

    // customization for ribbonQuickAccessBar
    customizeManager->addToCategory(categoryPopularCommands, m_actionFileNew);

    customizeManager->addToCategory(categoryPopularCommands, m_actionOpenFile);

    customizeManager->addToCategory(categoryPopularCommands, m_actionSaveFile);

    customizeManager->addToCategory(categoryPopularCommands, m_actionPrint);

    customizeManager->addToCategory(categoryPopularCommands, m_actionPrintPreview);

    customizeManager->addToCategory(categoryPopularCommands, m_actionCut);

    for (int i = 0, count = m_pageHome->groupCount(); count > i; ++i)
        customizeManager->addToCategory(m_pageHome->title(), m_pageHome->getGroup(i));
    for (int i = 0, count = m_pageLayout->groupCount(); count > i; ++i)
        customizeManager->addToCategory(m_pageLayout->title(), m_pageLayout->getGroup(i));
    for (int i = 0, count = m_pageRef->groupCount(); count > i; ++i)
        customizeManager->addToCategory(m_pageRef->title(), m_pageRef->getGroup(i));
    for (int i = 0, count = m_pageView->groupCount(); count > i; ++i)
        customizeManager->addToCategory(m_pageView->title(), m_pageView->getGroup(i));

    // customization for ribbon
    customizeManager->addToCategory(categoryMainPages, m_pageHome);
    customizeManager->addToCategory(categoryMainPages, m_pageLayout);
    customizeManager->addToCategory(categoryMainPages, m_pageRef);
    customizeManager->addToCategory(categoryMainPages, m_pageView);

    // Adding custom page to the standard ribbon customization dialog.
    RibbonCustomizeDialog* dialog = ribbonBar()->customizeDialog();
    RibbonPopularPage* popularPage = new RibbonPopularPage(this);
    dialog->insertPage(0, popularPage);

    RibbonQuickAccessBarCustomizePage* pCustomizeQuickAccessBar = qobject_cast<RibbonQuickAccessBarCustomizePage*>(dialog->pageByIndex(1));
    Q_ASSERT(pCustomizeQuickAccessBar != NULL);
    pCustomizeQuickAccessBar->addCustomCategory(categoryPopularCommands);
    pCustomizeQuickAccessBar->addCustomCategory(categoryAllCommands);
    pCustomizeQuickAccessBar->addSeparatorCategory();
    pCustomizeQuickAccessBar->addCustomCategory(m_pageHome->title());
    pCustomizeQuickAccessBar->addCustomCategory(m_pageLayout->title());
    pCustomizeQuickAccessBar->addCustomCategory(m_pageRef->title());
    pCustomizeQuickAccessBar->addCustomCategory(m_pageView->title());

    RibbonBarCustomizePage* pCustomizeRibbonBar = qobject_cast<RibbonBarCustomizePage*>(dialog->pageByIndex(2));
    Q_ASSERT(pCustomizeRibbonBar != NULL);
    pCustomizeRibbonBar->addCustomCategory(categoryPopularCommands);
    pCustomizeRibbonBar->addCustomCategory(categoryAllCommands);
    pCustomizeRibbonBar->addCustomCategory(categoryMainPages);
}

void MainWindow::updateRibbonBackground()
{
    OfficeStyle::Theme theme = m_ribbonStyle->getTheme();
    QPixmap px(m_curRibbonBackground);
    if (theme == OfficeStyle::Office2013Dark)
        ribbonBar()->setTitleBackground(QPixmap::fromImage(DrawHelpers::addaptColors(px.toImage(), qRgb(255, 255, 255), qRgb(230, 230, 230))));
    else
        ribbonBar()->setTitleBackground(px);
}

