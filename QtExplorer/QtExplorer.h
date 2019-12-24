#pragma once

#include <QtWidgets/QWidget>
#include "ui_QtExplorer.h"
#include <QPainter>
#include <qt/GsGeospace3DQt.h>

class QTreeWidgetItem;
class QSplitter;
class QDockWidget;
class QListWidget;
class QTreeWidgetItem;
class QTreeWidget;
class QListWidgetItem;
class ViewerWidget;

namespace osg
{
	class Group;
}

#define PADDING 1
#define SHADOWWIDTH  0.5

enum Direction {
	UP = 0,
	DOWN = 1,
	LEFT,
	RIGHT,
	LEFTTOP,
	LEFTBOTTOM,
	RIGHTBOTTOM,
	RIGHTTOP,
	NONE
};

#define WINOWS_WIDTH 1366
#define WINOWS_HEIGHT 768

class MenuItemWidget : public QWidget
{
	Q_OBJECT

public:
	MenuItemWidget(const QPixmap& icon, const QString& text, QWidget *parent = 0)
	{
		QLabel* label_Icon = new QLabel(this);
		label_Icon->setFixedSize(32, 32);
		label_Icon->setScaledContents(true);
		label_Icon->setPixmap(icon);

		QLabel* label_Text = new QLabel(text, this);

		QHBoxLayout* layout = new QHBoxLayout;
		layout->setContentsMargins(20, 6, 50, 6);
		layout->setSpacing(10);
		layout->addWidget(label_Icon);
		layout->addWidget(label_Text);
		setLayout(layout);

		setFixedWidth(240);
	}
	~MenuItemWidget()
	{}

protected:
	void paintEvent(QPaintEvent* e) {
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}

private:
};

class QtExplorer : public QWidget
{
	Q_OBJECT

public:
	QtExplorer(QWidget *parent = Q_NULLPTR);
	//绘制方法，目前此方法只是为边框添加阴影效果
	void paintEvent(QPaintEvent *event);
	//移动的距离
	QPoint move_point;
	//鼠标按下
	bool mouse_press;
	//鼠标按下事件
	void mousePressEvent(QMouseEvent *event);
	//鼠标释放事件
	void mouseReleaseEvent(QMouseEvent *event);
	//鼠标移动事件
	void mouseMoveEvent(QMouseEvent *event);
	//windows窗口消息处理，目前处理了WM_NCHITTEST消息，实现无边框程序拖拽大小功能
	//bool winEvent(MSG * message, long * result);
	bool nativeEvent(const QByteArray &eventType, void *message, long *result);
	//创建图层管理Dockwindow
	void createDockWindows();
	//创建文件菜单目录
	void createFileMainMenu();
	//鼠标双击标题栏最大化程序
	void mouseDoubleClickEvent(QMouseEvent * event);
	//在窗口右上角创建最大化最小化、关闭窗口等基本按钮
	void createBaseMenu();
	void ConnectSlots();
	
	//创建QAction
	void createActions();

public slots:
	bool closeWidget();
	//全屏或正常显示主窗口槽函数
	void ShowFullOrNomalWindow();
	void AddImageSlot();
	void AddTerrainSlot();
	void AddPlaceNameSlot();
	void AddObliqueImage();

private:
	Ui::QtExplorerClass ui;

	//窗口最小化按钮
	QToolButton *minButton;
	//窗口关闭按钮
	QToolButton *closeButton;
	//窗口最大化按钮
	QToolButton *maxButton;
	//窗口正常大小窗口
	QToolButton *norButton;
	//阴影的宽度
	int m_shadowSize;
	//保存上一次主窗口最大化之前之前窗口的尺寸
	QRect m_rect;
	bool m_bIsMaxSize;

	//osg主窗口容器
	QSplitter *m_mainSplitter;
	//图层管理停靠窗口容器
	QSplitter *m_layermanaSplitter;
	//osg主窗口容器
	QSplitter *m_osgWidgetSplitter;

	//图层管理停靠窗口对象
	QDockWidget *m_layerDockWidget;

	QDockWidget* tempLayerDockWidget;
	QTreeWidgetItem* tempTopImageItem;
	QTreeWidgetItem* tempTopTerrainItem;
	QTreeWidgetItem* tempTopVectorItem;
	QTreeWidgetItem* tempTopPlaceNameItem;
	QTreeWidgetItem* tempTopModelItem;
	QTreeWidgetItem* tempTopPipeModelItem;

	//快照停靠窗口对象
	QDockWidget *m_snapShootDockWidget;
	//图层管理停靠窗口对象
	QDockWidget *m_layerDockWidget2;
	//图层管理图片容器
	QListWidget* m_ptrSnapListWidget;

	//图层管理停靠窗口树型控件根节点集
	QList<QTreeWidgetItem*> *m_listTopQTreeTtem;
	//图层管理停靠窗口2树型控件根节点集
	QList<QTreeWidgetItem*> *m_listTopQTreeTtem2;
	//树形控件对象
	QTreeWidget *m_topTreeView;
	//树形控件对象2
	QTreeWidget *m_topTreeView2;
	//影像图层父节点
	QTreeWidgetItem *m_topImageItem;
	//影像图层父节点2
	QTreeWidgetItem *m_topImageItem2;
	//地形图层父节点
	QTreeWidgetItem *m_topTerrainItem;
	//地形图层父节点2
	QTreeWidgetItem *m_topTerrainItem2;
	//矢量图层父节点
	QTreeWidgetItem *m_topVectorItem;
	//矢量图层父节点2
	QTreeWidgetItem *m_topVectorItem2;
	//三维地名图层父节点
	QTreeWidgetItem *m_topPlaceNameItem;
	//三维地名图层父节点2
	QTreeWidgetItem *m_topPlaceNameItem2;
	//模型图层父节点
	QTreeWidgetItem *m_topModelItem;
	//模型图层父节点2
	QTreeWidgetItem *m_topModelItem2;
	//管线图层父节点
	QTreeWidgetItem *m_topPipeModelItem;
	//管线图层父节点2
	QTreeWidgetItem *m_topPipeModelItem2;
	//当前被选中图层节点
	QTreeWidgetItem *m_currentSelectedItem;
	//当前被选中图层节点2
	QTreeWidgetItem *m_currentSelectedItem2;
	QListWidgetItem *m_currentSelectedListWidgetItem;

	//加载本地文件目录对象
	QMenu *loadLocalDataMenu;
	//加载本地文件目录对象
	QMenu *loadLocalDataMenu2;
	//加载服务数据目录
	QMenu *loadServiceDataMenu;

	//加载地形Act
	QAction *addTerrainAct;
	//加载影像Act
	QAction *addImageAct;
	//加载模型Act
	QAction *addModelAct;
	//加载管线Act
	QAction *AddPipeModelAct;
	//加载倾斜摄影Act
	QAction *addObliqueModelAct;
	//加载三维地名Act
	QAction *addPlaceNameAct;
	//加载三维地名Act2
	QAction *addPlaceName2Act;
	//加载矢量Act
	QAction *addVectorAct;
	//加载矢量Act2
	QAction *addVector2Act;
	//加载影像Act2（也就是第二个球）
	QAction *addImage2Act;
	//加载地形Act2
	QAction *addTerrain2Act;
	//加载模型Act2
	QAction *addModel2Act;
	//加载倾斜摄影Act2
	QAction *addObliqueModel2Act;
	//加载管线Act2
	QAction *AddPipeModel2Act;

	QAction *addGeoTileServiceAct;
	QAction *addOGCServiceAct;


	GLOBE_NAME::GsGeospace3DQtPtr m_ptrOsgViewerWidget;

};
