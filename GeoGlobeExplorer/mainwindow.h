#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ribbonwindow.h"
#include "qt/GsGeospace3DQt.h"
#include <QMdiArea>
#include <QtFramework/Map/MapView.h>
#include <vector>
#include <kernel/geometry.h>

class QTextEdit;
class QTextCharFormat;
class QFontComboBox;
class QComboBox;
class QPrinter;

namespace GeoStar
{
	namespace Kernel
	{
		namespace Globe
		{
			class Element;
		}
	}
}

namespace Qtitan
{
    class PopupColorButton;
}

class LoadOGCServiceWindow;

/* MainWindow */
class MainWindow : public RibbonWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();

public:
    QString ribbonBackground() const;
    void setRibbonBackground(const QString& name);

	void OnHeightAnalasisFinish(double dbResult);
	void OnBillBordUpdate(GLOBE_NAME::Element* ptrBillBord);
	void OnLonLatClicked(GLOBE_NAME::Element* ptrLonLat);

protected:
    virtual void closeEvent(QCloseEvent* event);

private:
    void createAction();
    void createMenuFile();
    void createQuickAccessBar();

    void createRibbon();
    void createRibbonGroup(RibbonPage* ptrPage, UTILITY_NAME::GsConfig config);
	void createRibbonControl(RibbonGroup* ptrGroup, UTILITY_NAME::GsConfig config);
	void createRibbonMenuItem(QMenu* ptrMenu, UTILITY_NAME::GsConfig config);

    void createStatusBar();
    void createDockWindows();

	void ConnectSlot(QAction* ptrAction,QString qstrSlotName);

protected Q_SLOTS:

	void AddTerrain();
	void AddImage();
	void AddVector();
	
	void AddPlaceName();
	void AddObliqueImage();
	void AddModelElement(std::string strPath, double x, double y, double z);
	void AddModel();
	//2019-3-18 三维地名纠偏
	void PlaceNameChange();

	void Test();

	void ThrowingEnable();
	void AddServer();

	void AddOsgbServerLoop();
	void AddServerSlot();

	//高度量算
	void HeightAnalysis();
	//距离量算
	void DistanceAnalysis();
	//面积量算
	void AreaAnalysis();
	//单点通视
	void OPInRangeAnalysis();
	void AddModelElement();
	//void AddBillBoardElement();
	//显示经纬度
	void AddLonAndLatElement();
	//显示指北针
	void AddCompassElement();
	//两点通视
	void LineOfSightAnalysis();
	//剖面分析
	void TerrainProfileAnalysis();
	//倾斜剖面分析
	void OsgbProfileAnalysis();
	//清除所有分析结果
	void RemoveAllAnalysis();
	//清除指定分析结果
	void RemoveAnalysis();
	//多点通视
	void MultipleLineOfSightAnalysis();
	//添加标识符
	void AddBillBoardElement();
	//添加分析标识符
	void AddGeoBillBoardElement();

	//雷达分析
	void RadarAnalysis();

	void VolumeAnalysis();
	void DistanceAnalysis3D();
	void AreaMeasureAnalysis3D();

	//告示牌的隐藏与显示
	//void BillBoardVisible();
	//轨迹的隐藏与显示
	//void TrakerVisible();
	// 编辑对象
	void EditElement();

	void OnFinish(const std::vector<KERNEL_NAME::GsRawPoint3D>& vecPoint);
	void OnSelectEvent(GLOBE_NAME::Element* oldEle, GLOBE_NAME::Element* newEle);
	//添加飞行节点
	void AddFlyPoints();
	//开始飞行
	void StartFly();
	//停止飞行
	void StopFly();
	void PauseFly();
	void RestartFly();
	void BackFly();

	//加速减速
	void FastSpeed();
	void ReduceSpeed();

	//保存飞行路径
	void SaveFlyPath();
	//加载飞行路径
	void LoadFlyPath();
	//重置飞行路径
	void ReSetFlyPath();
	//绕点飞行
	void BeginRotation();
	void ReduceRotationSpeed();
	void FastRotationSpeed();
	//停止绕点飞行
	void StopRotation();
	//自由飞行
	void StartFreeFly();
	void StopFreeFly();
	void Marathon();

    void fullScreen(bool checked);
    void stateStatusBar(int state);

    void zoomSliderScroll(int value);
    virtual void showRibbonContextMenu(QMenu* menu, QContextMenuEvent* event);
    void showCompanyWebSite();

protected:
    virtual void keyPressEvent(QKeyEvent* event);

private:
    void colorChanged(const QColor& c);
    void customization();
    void updateRibbonBackground();

protected:
	QAction* m_actionFileNew;
	QAction* m_actionOpenFile;
	QAction* m_actionSaveFile;
	QAction* m_actionPrint;
	QAction* m_actionPrintSetup;
	QAction* m_actionPrintPreview;

	QAction* m_actionUndo;
	QAction* m_actionRedo;

    QAction* m_actionCut;
    QAction* m_actionCopy;
    QAction* m_actionPaste1;
    QAction* m_actionPaste2;
    QAction* m_actionFormatPointerAction;
    QAction* m_actionSelectAll;
    QAction* m_actionFullScreen;
    QAction* m_actionPrintLayout;

    RibbonPage* m_pageHome;
    RibbonPage* m_pageInsert;
    RibbonPage* m_pageLayout;
    RibbonPage* m_pageRef;
    RibbonPage* m_pageMailings;
    RibbonPage* m_pageReview;
    RibbonPage* m_pageView;

    //QTextEdit* m_textEdit;
    QString m_fileName;
    QFontComboBox* m_comboFont;
    QComboBox* m_comboSize;
    QToolButton* m_zoomPercentage;

    RibbonPage* m_pagePictureFormat;
    Qt::WindowStates m_stateWindow;
    QString m_curRibbonBackground;

	QMdiArea* m_mdiArea;
	LoadOGCServiceWindow * ptrLoadSerWnd;

	UTILITY_NAME::GsConfig m_Config;
	GLOBE_NAME::GsGeospace3DQt* m_ptrGeoSpace3D;
	GLOBE_NAME::GsMapView * m_ptrMapView;
	bool _distanceAnalysis;

private:
    Q_DISABLE_COPY(MainWindow)
};

#endif // MAINWINDOW_H
