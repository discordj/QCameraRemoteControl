#include "qcameraremotecontrol.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCameraRemoteControl w;
	w.show();
	return a.exec();
}
