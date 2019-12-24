#ifndef _LOAD_OGC_SERVICE_WINDOW
#define _LOAD_OGC_SERVICE_WINDOW

#include "BaseForm.h"
#include "ui_LoadOGCServiceWindow.h"

class LoadOGCServiceWindow:public BaseForm
{
public:
	LoadOGCServiceWindow();
	~LoadOGCServiceWindow();

	Ui::loadOGCServiceWindow ui;
};

#endif