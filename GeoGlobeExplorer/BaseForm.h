#ifndef _BASE_FORM_
#define _BASE_FORM_

#include <QWidget>
#include <QToolButton>
#include <QPainter>
#include <QStyle>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <qmath.h>
#include <QDialog>

#define BASE_SHADOWWIDTH  10

class BaseForm :public QDialog
{
public:

	BaseForm(QWidget *parent = 0);
	virtual ~BaseForm();

	Q_OBJECT

public:
	//窗口最小化按钮
	QToolButton *minButton;
	//窗口关闭按钮
	QToolButton *closeButton;

public slots:
	void okBtnClick();
	void cancelBtnClick();

signals:
	void signalMouseReleaseEvent(QMouseEvent *event);

public:

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

	void createBaseMenu();
};

#endif