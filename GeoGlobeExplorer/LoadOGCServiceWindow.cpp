#include "LoadOGCServiceWindow.h"

LoadOGCServiceWindow::LoadOGCServiceWindow()
{
	ui.setupUi(this);

	connect(ui.okPushButton,SIGNAL(clicked()),this,SLOT(AddOGCServicesSlot()));
	connect(ui.nextPushButton,SIGNAL(clicked()),this,SLOT(nextStep()));
	connect(ui.prePushButton,SIGNAL(clicked()),this,SLOT(preStep()));
	//closeButton->show();
	ui.okPushButton->setEnabled(true);

	ui.nextPushButton->hide();
	ui.prePushButton->hide();
}


LoadOGCServiceWindow::~LoadOGCServiceWindow()
{

}