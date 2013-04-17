#ifndef QCAMERAREMOTECONTROL_H
#define QCAMERAREMOTECONTROL_H

#include <QMainWindow>

#include <QDockWidget>
#include <QMap>
#include "cameratetherwidget.h"
#include "ui_qcameraremotecontrol.h"

class QCameraRemoteControl : public QMainWindow
{
	Q_OBJECT

public:
	QCameraRemoteControl(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~QCameraRemoteControl();

	private slots:
	void on_actionConnect_triggered();

private:
	Ui::QCameraRemoteControlClass ui;
	QCameraManager *cm;
	QMap<QCamera *, CameraTetherWidget *>_tethers;

};

#endif // QCAMERAREMOTECONTROL_H
