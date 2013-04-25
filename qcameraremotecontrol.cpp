#include "qcameraremotecontrol.h"

QCameraRemoteControl::QCameraRemoteControl(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	//connect(ui.actionConnect, &QAction::triggered, this, &QCameraRemoteControl::on_actionConnect_triggered);

	cm = new QCameraManager();

}

QCameraRemoteControl::~QCameraRemoteControl()
{
	delete cm;
}

void QCameraRemoteControl::on_actionConnect_triggered()
{
	

	QList<QCamera *> selectedCameras = cm->showdialog(false);


	if(selectedCameras.size() > 0)
	{
		QDockWidget *dw = new QDockWidget(selectedCameras.at(0)->model(),this);
		CameraTetherWidget *ct = new CameraTetherWidget(dw);

		dw->setWidget(ct);
    qDebug(qPrintable("Setting camera..."));
		ct->setCamera(selectedCameras.at(0), cm);
        qDebug(qPrintable("Camera selected!"));

		dw->setFloating(true);
		dw->show();

	}

}

