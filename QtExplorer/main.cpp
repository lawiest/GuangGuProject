#include "QtExplorer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtExplorer w;
	w.show();
	return a.exec();
}
