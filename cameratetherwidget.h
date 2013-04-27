#ifndef WIATETHERWIDGET_H
#define WIATETHERWIDGET_H

#include <QWidget>
#include <QMap>
#include <QList>
#include <QGraphicsScene>
#include <QPixmap>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QTimer>

#include <qcameramanager.h>

#include "ui_cameratetherwidget.h"
#include "dcrimage.h"

class CameraTetherWidget : public QWidget
{
	Q_OBJECT

public:
	CameraTetherWidget(QWidget *parent = 0);
	~CameraTetherWidget();

	void setCamera(QCamera *camera, QCameraManager *cm);

private:
	Ui::CameraTetherWidget ui;
	QMap<int, QString> _allexposuretimes;
//	QMap<int, QString> _whitebalances;
	QList<QString> _whitebalances;
	QList<QString> _fnumbers;
	QList<QString> _isonumbers;
	QList<QString> _imageformats;
	QList<QString> _camerasupportedexposuretimes;
	QList<int> _camerasupportedexposurecomp;
//	QMap<QString, QString>_imageformats;
	QString _destinationfolder;
	bool shootingraw;
	QTimer *timer;
	QTimer *timelapsetimer;
	QTimer *lvTimer;
	int timelapsefreq;
	int timelapsedelay;
	int numshotspertimeout;
	int totalnumbershots;
	int secondsdelayed;
	int currentnumberofshots;
	int last;
	bool ifsleepneeded;
	bool inunsupportedmode;

	bool hasExposureTimes;
	bool hasExposureComp;
	bool hasFNumbers;
	bool hasISO;
	bool hasWhiteBalance;

	QCamera *selectedCamera;
	QCameraProperties *props;

	//WIA::IDevice *device;
	

	
	void setupshuttertable();
	void setupfnumbers();
	void setupexposureindexes();
	void setupexposuretimes();
	void setupexposurecompensation();
	void setupbatterystatus();
	void setupwhitebalancetable();
	void setupwhitebalance();
	void setupimageformats();
	void nocameradetected();
	void cameradetected();
private slots:
	void on_dial1_valueChanged();
	void on_hsbDial2_valueChanged();
	void on_pbShutter_clicked();
	void on_hsExposureComp_valueChanged();
	void on_tbISO_toggled(bool checked);
	void on_tbWB_toggled(bool checked);
	void on_tbFMT_toggled(bool checked);
	void on_pbsetstorage_clicked();
	void on_cblockunlockui_toggled(bool checked);
	void on_timer_timeout();
	void on_pbTimeLapse_clicked();
	void on_timelapse_timeout();
	void on_wiadlg_exception(int err, QString err1, QString err2, QString err3);
	void on_gbBulbMode_toggled(bool checked);
	void on_leFilePrefix_editingFinished();
    void on_gbLiveView_toggled(bool checked);
	void on_lvTimer_timeout();
	void on_qcamera_image_captured(QImage image);
	void on_qcamera_destroyed(QObject *);
	void on_qcamera_busy(bool isbusy);
	void on_qcamera_needs_input(QString text);
	void on_qcamera_property_changed(QCameraProperties::QCameraPropertyTypes, QVariant);
signals:
	void newImageDownloaded(QString filename);
//	void imageDownloaded(QString file);
};

#endif // WIATETHERWIDGET_H
