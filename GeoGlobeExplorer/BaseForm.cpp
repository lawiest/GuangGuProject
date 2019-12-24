#include "BaseForm.h"


BaseForm::BaseForm(QWidget *parent)
{
	//设置此属性会让窗口关闭时把自己销毁
	setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
	//this->setMouseTracking(true);

	mouse_press = false;
	createBaseMenu();

	//this->setWindowModality(Qt::WindowModal);
	this->setEnabled(true);
}


BaseForm::~BaseForm()
{

}

//在窗口右上角创建最大化最小化、关闭窗口等基本按钮
void BaseForm::createBaseMenu()
{
	//构建最小化、最大化、关闭按钮
	//minButton = new QToolButton(this);
	closeButton= new QToolButton(this);
	//minButton->hide();
	closeButton->hide();

	//connect(minButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
	//connect(closeButton,SIGNAL(clicked()),this,SLOT(close()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	//获取最小化、关闭按钮图标
	//QPixmap minPix  = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);

	//设置最小化、关闭按钮图标
	//minButton->setIcon(minPix);
	//minButton->setIconSize(QSize(32,32));
	closeButton->setIcon(closePix);

	//设置最小化、关闭按钮在界面的位置
	//minButton->setGeometry(this->width()-67,11,32,32);
	closeButton->setGeometry(this->width()-35,11,32,32);

	//设置鼠标移至按钮上的提示信息
	//minButton->setToolTip(QString::fromLocal8Bit("最小化"));
	closeButton->setToolTip(QString::fromLocal8Bit("关闭"));
	//设置最小化、关闭按钮的样式

	//minButton->setStyleSheet("QToolButton{background-color:transparent;}"
	//	"QToolButton:hover{background-color:rgba(220, 224, 231,150);border:0px;}");

	closeButton->setStyleSheet("QToolButton{background-color:transparent;}"
		"QToolButton::hover{background-color:rgba(255, 0, 0,150);border:0px;}");
}

//鼠标按下事件
void BaseForm::mousePressEvent(QMouseEvent *event)
{  
	if(event->button() == Qt::LeftButton)
	{
		mouse_press = true;
		//鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
		move_point = event->pos();;
	}
}

//鼠标移动事件
void BaseForm::mouseMoveEvent(QMouseEvent *event)
{
	//若鼠标左键被按下
	if(mouse_press)
	{
		//鼠标相对于屏幕的位置
		QPoint move_pos = event->globalPos();

		//移动主窗体位置
		this->move(move_pos - move_point);

		QRect rect;
		rect = QApplication::desktop()->screenGeometry();

		int nWidth = this->width();
		int nheight = this->height();

		if(this->width()==rect.width() && this->height() ==rect.height())
		{
			QPixmap maxPix = style()->standardPixmap(QStyle::SP_TitleBarMaxButton);
			//maxButton->setIcon(maxPix);
			showNormal();
		}
	}
}

//鼠标释放事件
void BaseForm::mouseReleaseEvent(QMouseEvent *event)
{
	//设置鼠标为未被按下
	mouse_press = false;

	emit signalMouseReleaseEvent(event);
}

//绘制方法，目前此方法只是为边框添加阴影效果
void BaseForm::paintEvent(QPaintEvent *event)
{
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	path.addRect(BASE_SHADOWWIDTH, BASE_SHADOWWIDTH, this->width()-20, this->height()-20);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.fillPath(path, QBrush(Qt::white));

	QColor color(0, 0, 0);
	for(int i=0; i<BASE_SHADOWWIDTH; i++)
	{
		QPainterPath path;
		path.setFillRule(Qt::WindingFill);
		path.addRect(BASE_SHADOWWIDTH-i, BASE_SHADOWWIDTH-i, this->width()-(BASE_SHADOWWIDTH-i)*2, this->height()-(BASE_SHADOWWIDTH-i)*2);
		color.setAlpha(150 - qSqrt(i)*50);
		painter.setPen(color);
		painter.drawPath(path);
	}

	//minButton->setGeometry(this->width()-74,10,32,32);
	closeButton->setGeometry(this->width()-42,10,32,32);
} 

void BaseForm::okBtnClick()
{
	this->close();
}

void BaseForm::cancelBtnClick()
{
	this->close();
}