#include <QMessageBox>
#include "cameratetherwidget.h"

CameraTetherWidget::CameraTetherWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	inunsupportedmode = false;
//device = 0;
	selectedCamera = 0;
	last = -1;
	ifsleepneeded = false;
	timer = new QTimer();
	connect(timer,SIGNAL(timeout()),this,SLOT(on_timer_timeout()));

	timelapsetimer = new QTimer();
	connect(timelapsetimer, SIGNAL(timeout()),this,SLOT(on_timelapse_timeout()));

	//setupshuttertable();
	//setupwhitebalancetable();
	//setupimageformats();

	shootingraw = false;

	//QCameraManager *cm = new QCameraManager();

	//QList<QCamera *> selectedCameras = cm->showdialog(false);


	//if(selectedCameras.size() <= 0)
	//	nocameradetected();
	//else
	//{
	//	selectedCamera = selectedCameras.at(0);
	//	cm->selectCamera(selectedCamera);
	//	connect(selectedCamera,SIGNAL(image_captured(QImage)),this, SLOT(on_qcamera_image_captured(QImage)));
	//	cameradetected();
	//}

	//delete cm;


	//WIA::DeviceManager *wiaManager = new WIA::DeviceManager();

	//if(wiaManager->DeviceInfos()->Count() <= 0)
	//{
	//	nocameradetected();
	//}
	//else {

	//	cameradetected();
	//}
}

CameraTetherWidget::~CameraTetherWidget()
{
	if(selectedCamera)
		selectedCamera->QCDisconnect();
}

void CameraTetherWidget::setCamera(QCamera *camera, QCameraManager *cm)
{

	selectedCamera = camera;
	cm->selectCamera(camera);
	connect(camera,SIGNAL(image_captured(QImage)),this, SLOT(on_qcamera_image_captured(QImage)));
	connect(camera,SIGNAL(destroyed(QObject *)),this,SLOT(on_qcamera_destroyed(QObject *)));
	connect(camera,SIGNAL(camera_busy(bool)),this,SLOT(on_qcamera_busy(bool)));
	cameradetected();


}

void CameraTetherWidget::on_qcamera_destroyed(QObject *cam)
{
	this->close();
}

void CameraTetherWidget::nocameradetected()
{
		ui.gbcamera->setTitle("No camera selected!");
		ui.pbShutter->setEnabled(false);
		ui.tbISO->setEnabled(false);
		ui.tbWB->setEnabled(false);
		ui.tbFMT->setEnabled(false);
		ui.dial1->setEnabled(false);
		ui.hsbDial2->setEnabled(false);
		ui.hsExposureComp->setEnabled(false);
		ui.gbImagePreview->setEnabled(false);
		//device = 0;
}

void CameraTetherWidget::on_qcamera_busy(bool isbusy)
{
	if(isbusy)
	{
		ui.pbShutter->setEnabled(false);
		ui.dial1->setEnabled(false);
		ui.hsbDial2->setEnabled(false);
		ui.gbBulbMode->setEnabled(false);
		ui.hsExposureComp->setEnabled(false);
		ui.gbTimelapse->setEnabled(false);
		ui.cbLiveView->setEnabled(false);
	}
	else
	{
		ui.pbShutter->setEnabled(true);
		ui.dial1->setEnabled(true);
		ui.hsbDial2->setEnabled(true);
		ui.gbBulbMode->setEnabled(selectedCamera->hasBulbMode());
		ui.hsExposureComp->setEnabled(true);
		ui.gbTimelapse->setEnabled(true);
		ui.cbLiveView->setEnabled(true);
	}
}


void CameraTetherWidget::on_wiadlg_exception(int err, QString err1, QString err2, QString err3)
{
	qDebug(qPrintable(QString("%1").arg(err).arg(err1).arg(err2).arg(err3)));
}

void CameraTetherWidget::cameradetected()
{

	selectedCamera->QCConnect();
	//props = selectedCamera->getCameraProperties();
	ui.gbcamera->setTitle(selectedCamera->model());
//	if(0 != props){
	setupexposureindexes();
	setupfnumbers();
	setupimageformats();
	setupexposuretimes();
	setupwhitebalance();
//	}
	setupbatterystatus();
	ui.gbBulbMode->setEnabled(selectedCamera->hasBulbMode());
	if(selectedCamera->hasBulbMode() && !selectedCamera->canSetBulbMode()){
		QMessageBox mb;
		mb.setText("This camera supports bulb mode, but it must be set manually");
		mb.exec();
	}
	ui.cbLiveView->setEnabled(selectedCamera->hasLiveView());
	QObject::connect(selectedCamera,&QCamera::camera_needs_input,this, &CameraTetherWidget::on_qcamera_needs_input);
	QObject::connect(selectedCamera,&QCamera::camera_property_changed,this, &CameraTetherWidget::on_qcamera_property_changed);
}
void CameraTetherWidget::on_qcamera_needs_input(QString text)
{
		QMessageBox mb;
		mb.setText(text);
		mb.exec();
}

void CameraTetherWidget::on_qcamera_property_changed(QCameraProperties::QCameraPropertyTypes prop, QVariant value)
{
	switch(prop)
	{
		case QCameraProperties::ResolutionMode:
			setupimageformats();
			break;
		case QCameraProperties::Aperture:
			setupfnumbers();
			setupexposuretimes();
			break;
		case QCameraProperties::Iso:
			setupexposureindexes();
			break;
		case QCameraProperties::ExposureTimes:
			setupfnumbers();
			setupexposuretimes();
			break;
		case QCameraProperties::BatteryLevel:
			ui.pbBattery->setValue(value.toInt());
			break;
	}
}


void CameraTetherWidget::setupwhitebalancetable()
{
	//_whitebalances.clear();

	//_whitebalances.insert(2,"Auto");
	//_whitebalances.insert(4,"Daylight");
	//_whitebalances.insert(5,"Flourescent");
	//_whitebalances.insert(6,"Incandescent");
	//_whitebalances.insert(7,"Flash");
	//_whitebalances.insert(32784,"Cloudy");
	//_whitebalances.insert(32785,"Shade");
	//_whitebalances.insert(32786,"Kelvin");
	//_whitebalances.insert(32787,"Custom");
}
void CameraTetherWidget::setupwhitebalance()
{
	//WIA::IProperty *prop = device->Properties()->Item(QVariant("White Balance"));
	//ui.lbWhiteBalance->setText(_whitebalances.value(prop->Value().toInt()));

	QList<QVariant> wbvalues = selectedCamera->getCameraProperty(QCameraProperties::WhiteBalanceMode)->values();
	QList<QString> wbkeys = selectedCamera->getCameraProperty(QCameraProperties::WhiteBalanceMode)->keys();
	QVariant currentValue = selectedCamera->getCameraProperty(QCameraProperties::WhiteBalanceMode)->value();

		//WIA::IProperty *prop = device->Properties()->Item(QVariant("Exposure Index"));

		//if(!prop->IsReadOnly())
		//{
			hasISO = true;
			ui.lbWhiteBalance->setText(QString("%1").arg(wbkeys.at(wbvalues.indexOf(currentValue))));
			
			_whitebalances.clear();
			for(int i=0; i < wbkeys.count(); i++)
			{
				_whitebalances.append(QString("%1").arg(wbkeys.at(i)));
			}
			ui.lbWhiteBalance->setDisabled(false);
		//}
		//else
		//{
		//	hasISO = false;
		//	ui.tbISO->setDisabled(true);
		//}

}


void CameraTetherWidget::setupbatterystatus()
{
	if(selectedCamera)
		ui.pbBattery->setValue(selectedCamera->batteryLevel());
}


void CameraTetherWidget::setupexposurecompensation()
{
	//_camerasupportedexposurecomp.clear();
	//WIA::IProperty *prop = device->Properties()->Item(QVariant("Exposure Compensation"));

	//if(!prop->IsReadOnly())
	//{
	//	int smallest = 0;
	//	int largest =0;
	//	for(int j=1; j <= prop->SubTypeValues()->Count(); j++)
	//	{

	//		QVariant value = prop->SubTypeValues()->Item(j);

	//		int fvalue = value.toInt();
	//		
	//		if(fvalue > 32768.)
	//		{
	//			fvalue -= 65536.;
	//		}

	//		if(fvalue < smallest)
	//			smallest = fvalue;

	//		if(fvalue > largest)
	//			largest = fvalue;


	//		_camerasupportedexposurecomp.append(fvalue);

	//		
	//	}
	//	int step = abs(abs(_camerasupportedexposurecomp.at(0)) - abs(_camerasupportedexposurecomp.at(1)));
	//	ui.hsExposureComp->setSingleStep(step);
	//	ui.hsExposureComp->setTickInterval(step * 2);

	//	ui.hsExposureComp->setMinimum(smallest);
	//	ui.hsExposureComp->setMaximum(largest);

	//	int current = prop->Value().toInt();

	//	if(current > 32768)
	//		current -= 65536.;

	//	ui.hsExposureComp->setValue(current);
	//	ui.hsExposureComp->setEnabled(true);
	//	ui.lbExposureComp->setText(QString("%1").arg((double)current/1000.));
	//}
	//else
	//{
	//	ui.hsExposureComp->setEnabled(false);
	//	
	//}




}
void CameraTetherWidget::setupexposuretimes()
{
	   _camerasupportedexposuretimes.clear();
		//WIA::IProperty *prop = device->Properties()->Item(QVariant("Exposure Time"));

		//
		//if(!prop->IsReadOnly()){
			hasExposureTimes = true;	
		//	int val = prop->Value().toInt();
		//	if(val == -1 && !inunsupportedmode){
		//		//if(!initializing)
		//		inunsupportedmode = true;
		//		QMessageBox::warning(this,"Camera In Bulb mode","Your camera is set to bulb/or some other unsupported mode, remote shutter control disabled");
		//		ui.pbShutter->setEnabled(false);
		//		ui.pbTimeLapse->setEnabled(false);
		//		ui.gbTimelapse->setChecked(false);
		//		ui.gbTimelapse->setEnabled(false);
		//	}
		//	else{
		//		inunsupportedmode = false;
				ui.pbShutter->setEnabled(true);
				ui.gbTimelapse->setEnabled(true);
		//	}

		QList<QVariant> timevalues = selectedCamera->getCameraProperty(QCameraProperties::ExposureTimes)->values();
		QList<QString> timekeys = selectedCamera->getCameraProperty(QCameraProperties::ExposureTimes)->keys();
		QVariant currentValue = selectedCamera->getCameraProperty(QCameraProperties::ExposureTimes)->value();



			//if(prop->Value().toInt() >= 10000)
			//	ifsleepneeded = true;
			//else
			//	ifsleepneeded = false;


			for(int i=0; i < timekeys.count(); i++)
			{
				_camerasupportedexposuretimes.append(timekeys);
			}
			int exposuretimeselectedindex = timevalues.indexOf(currentValue);

			ui.lbShutter->setText(timekeys.at(exposuretimeselectedindex));
			ui.hsbDial2->setMinimum(0);
			ui.hsbDial2->setMaximum(_camerasupportedexposuretimes.count() - 1);
			ui.hsbDial2->setValue(exposuretimeselectedindex);
		//}
		//else
		//{
		//	hasExposureTimes = false;
		//}

}
void CameraTetherWidget::setupexposureindexes()
{

	QList<QVariant> isovalues = selectedCamera->getCameraProperty(QCameraProperties::Iso)->values();
		QList<QString> isokeys = selectedCamera->getCameraProperty(QCameraProperties::Iso)->keys();
		QVariant currentValue = selectedCamera->getCameraProperty(QCameraProperties::Iso)->value();

		//WIA::IProperty *prop = device->Properties()->Item(QVariant("Exposure Index"));

		//if(!prop->IsReadOnly())
		//{
			hasISO = true;
			ui.lbISO->setText(QString("%1").arg(isokeys.at(isovalues.indexOf(currentValue))));
			
			_isonumbers.clear();
			for(int i=0; i < isokeys.count(); i++)
			{
				_isonumbers.append(QString("%1").arg(isovalues.at(i).toString()));
			}
			ui.tbISO->setDisabled(false);
		//}
		//else
		//{
		//	hasISO = false;
		//	ui.tbISO->setDisabled(true);
		//}

}
void CameraTetherWidget::setupfnumbers()
{
	_fnumbers.clear();
	QList<QVariant> fnumbervalues = selectedCamera->getCameraProperty(QCameraProperties::Aperture)->values(); //camera values
	QList<QString> fnumberkeys = selectedCamera->getCameraProperty(QCameraProperties::Aperture)->keys(); //display
	QVariant currentValue = selectedCamera->getCameraProperty(QCameraProperties::Aperture)->value();

	int index = fnumbervalues.indexOf(currentValue);

	if(index < 0) index = 0;
	//WIA::IProperty *prop = device->Properties()->Item(QVariant("F Number"));

	//if(!prop->IsReadOnly())
	//{
		hasFNumbers = true;
		ui.lbApeture->setText(QString("%1").arg(fnumberkeys.at(index)));

		_fnumbers.clear();
		for(int i=0; i < fnumberkeys.count(); i++)
		{
			_fnumbers.append(fnumberkeys.at(i));
		}
		ui.dial1->setDisabled(false);
		ui.dial1->setMinimum(0);
		ui.dial1->setMaximum(_fnumbers.count() - 1);
		ui.dial1->setValue(index);
	//}
	//else
	//{
	//	hasFNumbers = false;
	//	ui.dial1->setDisabled(true);
	//}


}
void CameraTetherWidget::setupshuttertable()
{
	//_allexposuretimes.insert(1,"6400");
	//_allexposuretimes.insert(2,"4000");
	//_allexposuretimes.insert(3,"3200");
	//_allexposuretimes.insert(4,"2500");
	//_allexposuretimes.insert(5,"2000");
	//_allexposuretimes.insert(6,"1600");
	//_allexposuretimes.insert(8,"1250");
	//_allexposuretimes.insert(10,"1000");
	//_allexposuretimes.insert(12,"800");
	//_allexposuretimes.insert(13,"750");
	//_allexposuretimes.insert(15,"640");
	//_allexposuretimes.insert(20,"500");
	//_allexposuretimes.insert(25,"400");
	//_allexposuretimes.insert(28,"350");
	//_allexposuretimes.insert(31,"320");
	//_allexposuretimes.insert(40,"250");
	//_allexposuretimes.insert(50,"200");
	//_allexposuretimes.insert(55,"180");
	//_allexposuretimes.insert(62,"160");
	//_allexposuretimes.insert(80,"125");
	//_allexposuretimes.insert(100,"100");
	//_allexposuretimes.insert(111,"90");
	//_allexposuretimes.insert(125,"80");
	//_allexposuretimes.insert(166,"60");
	//_allexposuretimes.insert(200,"50");
	//_allexposuretimes.insert(222,"45");
	//_allexposuretimes.insert(250,"40");
	//_allexposuretimes.insert(333,"30");
	//_allexposuretimes.insert(400,"25");
	//_allexposuretimes.insert(500,"20");
	//_allexposuretimes.insert(666,"15");
	//_allexposuretimes.insert(769,"13");
	//_allexposuretimes.insert(1000,"10");
	//_allexposuretimes.insert(1250,"8");
	//_allexposuretimes.insert(1666,"6");
	//_allexposuretimes.insert(2000,"5");
	//_allexposuretimes.insert(2500,"4");
	//_allexposuretimes.insert(3333,"3");
	//_allexposuretimes.insert(4000,"2.5");
	//_allexposuretimes.insert(5000,"2");
	//_allexposuretimes.insert(6250,"1.6");
	//_allexposuretimes.insert(6666,"1.5");
	//_allexposuretimes.insert(7692,"1.3");
	//_allexposuretimes.insert(10000,"1\"");
	//_allexposuretimes.insert(13000,"1.3\"");
	//_allexposuretimes.insert(15000,"1.5\"");
	//_allexposuretimes.insert(16000,"1.6\"");
	//_allexposuretimes.insert(20000,"2\"");
	//_allexposuretimes.insert(25000,"2.5\"");
	//_allexposuretimes.insert(30000,"3\"");
	//_allexposuretimes.insert(40000,"4\"");
	//_allexposuretimes.insert(50000,"5\"");
	//_allexposuretimes.insert(60000,"6\"");
	//_allexposuretimes.insert(80000,"8\"");
	//_allexposuretimes.insert(100000,"10\"");
	//_allexposuretimes.insert(130000,"13\"");
	//_allexposuretimes.insert(150000,"15\"");
	//_allexposuretimes.insert(200000,"20\"");
	//_allexposuretimes.insert(250000,"25\"");
	//_allexposuretimes.insert(300000,"30\"");
	//_allexposuretimes.insert(-1,"bulb");
}


void CameraTetherWidget::setupimageformats()
{
	_imageformats.clear();
	QList<QVariant> formatvalues = selectedCamera->getCameraProperty(QCameraProperties::ResolutionMode)->values();//props->getPropertyValues(QCameraProperties::ResolutionMode);
	QList<QString> formatkeys = selectedCamera->getCameraProperty(QCameraProperties::ResolutionMode)->keys(); //props->getPropertyKeys(QCameraProperties::ResolutionMode);
	QVariant currentValue = selectedCamera->getCameraProperty(QCameraProperties::ResolutionMode)->value();

	for(int i=0; i < formatkeys.count(); i++)
	{
		_imageformats.append(QString("%1").arg(formatvalues.at(i).toString()));
	}

	ui.lbFormat->setText(QString("%1").arg(formatkeys.at(formatvalues.indexOf(currentValue))));

	////_imageformats.insert("{B96B3CAB-0728-11D3-9D7B-0000F81EF32E}","BMP");
	////_imageformats.insert("{B96B3CB0-0728-11D3-9D7B-0000F81EF32E}","GIF");
	//_imageformats.insert("{B96B3CAE-0728-11D3-9D7B-0000F81EF32E}","JPG");
	////_imageformats.insert("{B96B3CAF-0728-11D3-9D7B-0000F81EF32E}","PNG");
	////_imageformats.insert("{B96B3CB1-0728-11D3-9D7B-0000F81EF32E}","TIFF");
	//_imageformats.insert("{B96B3CA9-0728-11D3-9D7B-0000F81EF32E}","RAW");
}
void CameraTetherWidget::on_hsExposureComp_valueChanged()
{
	//int current = ui.hsExposureComp->value();

	//int closestsmall;
	//int closestlarge;
	//for(int j=0; j <  _camerasupportedexposurecomp.count()-2; j++)
	//{
	//	if(current > _camerasupportedexposurecomp.at(j) && current < _camerasupportedexposurecomp.at(j+1))
	//	{
	//		closestlarge = _camerasupportedexposurecomp.at(j+1);
	//		closestsmall = _camerasupportedexposurecomp.at(j);
	//		if(current - closestsmall > closestlarge - current)
	//			current = closestlarge;
	//		else
	//			current = closestsmall;
	//	}
	//}
	//
	//ui.hsExposureComp->setValue(current);
	//ui.lbExposureComp->setText(QString("%1").arg((double)current/1000.));


	//if(current < 0)
	//	current += 65536;

	//device->Properties()->Item(QVariant("Exposure Compensation"))->SetValue(QVariant(current));


	
}
void CameraTetherWidget::on_dial1_valueChanged(){
	
	if(selectedCamera == 0) return;
	int index = ui.dial1->value();

	if(ui.tbWB->isChecked() && hasWhiteBalance)
	{
		QString whitebal = _whitebalances.at(index);

		QList<QVariant> wbvalues = selectedCamera->getCameraProperty(QCameraProperties::WhiteBalanceMode)->values();
		QList<QString> wbkeys = selectedCamera->getCameraProperty(QCameraProperties::WhiteBalanceMode)->keys();

		QVariant value(wbvalues.at(index));
		selectedCamera->setCameraProperty(QCameraProperties::WhiteBalance, value);

//		device->Properties()->Item(QVariant("White Balance"))->SetValue(QVariant(_whitebalances.keys().at(index)));
		ui.lbWhiteBalance->setText(_whitebalances.at(index));

	}
	else if(ui.tbISO->isChecked() && hasISO)
	{
		if(index >= _isonumbers.count()) index = 0;
		if(index < 0) index = _isonumbers.count() -1;

		QList<QVariant> isovalues = selectedCamera->getCameraProperty(QCameraProperties::Iso)->values();
		QList<QString> isokeys = selectedCamera->getCameraProperty(QCameraProperties::Iso)->keys();

		QVariant value(isovalues.at(index));
		selectedCamera->setCameraProperty(QCameraProperties::Iso, value);
//		device->Properties()->Item(QVariant("Exposure Index"))->SetValue(value);
		ui.lbISO->setText(QString("%1").arg(isokeys.at(isovalues.indexOf(value))));

	}
	else if(ui.tbFMT->isChecked())
	{

		QList<QVariant> formatvalues = selectedCamera->getCameraProperty(QCameraProperties::ResolutionMode)->values();
		QList<QString> formatkeys = selectedCamera->getCameraProperty(QCameraProperties::ResolutionMode)->keys();
		
		QVariant value(formatvalues.at(index));
		selectedCamera->setCameraProperty(QCameraProperties::ResolutionMode, value);
		ui.lbFormat->setText(QString("%1").arg(formatkeys.at(index)));

//		device->Properties()->Item(QVariant("Format"))->SetValue(value);

		//if(value == wiaFormatRAW)
		//{
		//	shootingraw = true;
		////	ui.gbImagePreview->setEnabled(false);
		////	if(ui.gbImagePreview->isChecked())
		////		ui.gbImagePreview->setChecked(false);
		//}
		//else
		//{
		//	shootingraw = false;
		////	ui.gbImagePreview->setEnabled(true);
		//}
		//ui.lbFormat->setText(QString("%1").arg(_imageformats.value(_imageformats.keys().at(index))));

	}
	else if(hasExposureTimes)
	{
		ui.lbApeture->setText(QString("%1").arg(_fnumbers.at(index)));
		QList<QVariant> fnumbervalues = selectedCamera->getCameraProperty(QCameraProperties::Aperture)->values(); //camera values


//		device->Properties()->Item(QVariant("F Number"))->SetValue(value);
		selectedCamera->setCameraProperty(QCameraProperties::Aperture, fnumbervalues.at(index));

//		setupexposuretimes();
	}
}

void CameraTetherWidget::on_hsbDial2_valueChanged(){
	if(selectedCamera == 0) return;

	int index = ui.hsbDial2->value();
	QVariant selected = selectedCamera->getCameraProperty(QCameraProperties::ExposureTimes)->values().at(index);
	QString name =	selectedCamera->getCameraProperty(QCameraProperties::ExposureTimes)->keys().at(index);

	//if(selected >= 10000)
	//	ifsleepneeded = true;
	//else
	//	ifsleepneeded = false;


//	QString selectedValue = _camerasupportedexposuretimes.indexOf(currentValue);
	ui.lbShutter->setText(name);
	selectedCamera->setCameraProperty(QCameraProperties::ExposureTimes,QVariant(selected));
	//device->Properties()->Item(QVariant("Exposure Time"))->SetValue(QVariant(selected));
}

void CameraTetherWidget::on_tbISO_toggled(bool checked)
{
	if(checked)
	{
		if(ui.tbWB->isChecked())
			ui.tbWB->setChecked(false);
		if(ui.tbFMT->isChecked())
			ui.tbFMT->setChecked(false);

//		QMap<QString, QVariant> ISOs = props->getPropertyValues(QCameraProperties::Iso);
		QVariant currentValue = selectedCamera->getCameraProperty(QCameraProperties::Iso)->value();

		ui.dial1->setMinimum(0);
		ui.dial1->setMaximum(_isonumbers.count() - 1);
		ui.dial1->setValue(_isonumbers.indexOf(currentValue.toString()));

	}
	else if(!ui.tbWB->isChecked() && !ui.tbFMT->isChecked())
	{
		//WIA::IProperty *prop = device->Properties()->Item(QVariant("F Number"));
		QVariant currentValue = selectedCamera->getCameraProperty(QCameraProperties::Aperture)->value();

		QString selectedfnumber = QString("%1").arg(((float)currentValue.toInt())/100.0);
		ui.dial1->setMinimum(0);
		ui.dial1->setMaximum(_fnumbers.count() - 1);
		ui.dial1->setValue(_fnumbers.indexOf(selectedfnumber));
	}
}

void CameraTetherWidget::on_tbWB_toggled(bool checked)
{

}

void CameraTetherWidget::on_tbFMT_toggled(bool checked)
{
	if(checked)
	{
		if(ui.tbISO->isChecked())
			ui.tbISO->setChecked(false);
		if(ui.tbWB->isChecked())
			ui.tbWB->setChecked(false);

		QVariant currentValue = selectedCamera->getCameraProperty(QCameraProperties::Iso)->value();
		ui.dial1->setMinimum(0);
		ui.dial1->setMaximum(_imageformats.count() - 1);
		ui.dial1->setValue(_imageformats.indexOf(currentValue.toString()));
		//ui.lbFormat->setText(_imageformats.index(prop->Value().toString()));



	}
	else if(!ui.tbISO->isChecked() && !ui.tbWB->isChecked())
	{
		QVariant currentValue = selectedCamera->getCameraProperty(QCameraProperties::Aperture)->value();

		QString selectedfnumber = QString("%1").arg(((float)currentValue.toInt())/100.0);
		ui.dial1->setMinimum(0);
		ui.dial1->setMaximum(_fnumbers.count() - 1);
		ui.dial1->setValue(_fnumbers.indexOf(selectedfnumber));
	}
}

void CameraTetherWidget::on_pbShutter_clicked()
{

	if(ui.cbLiveView->isChecked()){
		lvTimer->stop();
	}
	if(ui.gbBulbMode->isChecked())
		selectedCamera->capture(ui.sbBulbDuration->value());
	else
		selectedCamera->capture();


}

void CameraTetherWidget::on_pbsetstorage_clicked()
{
	if(selectedCamera)
		selectedCamera->setImageDirectory(QFileDialog::getExistingDirectory(this));
}
void CameraTetherWidget::on_cblockunlockui_toggled(bool checked)
{
	if(checked){
		selectedCamera->unlockUI();
		ui.cblockunlockui->setChecked(checked);
	}
	else
	{
		selectedCamera->lockUI();
		ui.cblockunlockui->setChecked(checked);
	}
}

void CameraTetherWidget::on_timer_timeout()
{

}
void CameraTetherWidget::on_pbTimeLapse_clicked()
{
	if(timelapsetimer->isActive())
	{
		timelapsetimer->stop();
		ui.pbTimeLapse->setText("Start Time Lapse");
	}
	else
	{
		timelapsefreq = ui.sbtimelapsefreq->value();
		timelapsedelay = ui.sbtimelapsedelay->value();
		numshotspertimeout = ui.sbtimelapseshotsper->value();
		totalnumbershots = ui.sbtimelapsetotal->value();
		secondsdelayed = 0;
		currentnumberofshots = 0;

		//connect(timelapsetimer, &QTimer::timeout, this, &CameraTetherWidget::on_timelapse_timeout);
		timelapsetimer->start(timelapsefreq * 1000);

		ui.pbTimeLapse->setText("Stop Time Lapse");
	}
}

void CameraTetherWidget::on_timelapse_timeout()
{
	if(timelapsedelay <= secondsdelayed)
	{
		secondsdelayed += timelapsefreq;

		for(int i=0; i < numshotspertimeout; i++)
		{
			selectedCamera->capture();
		}

		currentnumberofshots += numshotspertimeout;

		if(currentnumberofshots >= totalnumbershots)
		{
			timelapsetimer->stop();
			ui.pbTimeLapse->setText("Start Time Lapse");
		}
	}
	else
		secondsdelayed += timelapsefreq;

}
void CameraTetherWidget::on_gbBulbMode_toggled(bool checked)
{
	if(selectedCamera)
	{

		selectedCamera->setbulbmode(checked);
		if(checked && !selectedCamera->canSetBulbMode())
		{
			//selectedCamera->QCDisconnect();
			//selectedCamera->unlockUI();
			//selectedCamera->QCDisconnect();
			QMessageBox mb;
			mb.setText("Set camera to bulb mode and the lock the camera UI.");
			mb.exec();
			on_cblockunlockui_toggled(true);
			//selectedCamera->QCConnect();
			//selectedCamera->lockUI();
		}

		if(checked)
		{
			ui.hsbDial2->setEnabled(false);
			ui.lbShutter->setText("BULB"); //timekeys.at(exposuretimeselectedindex));

		}
		else
		{
			ui.hsbDial2->setEnabled(true);
			setupexposuretimes();
		}
	}
}

void CameraTetherWidget::on_leFilePrefix_editingFinished()
{
	if(selectedCamera)
		selectedCamera->setImageFilePrefix(ui.leFilePrefix->text());
}
void CameraTetherWidget::on_cbLiveView_toggled(bool checked)
{
	if(selectedCamera)
		selectedCamera->toggleLiveView(checked);

	if(checked){
		lvTimer = new QTimer(this);
		connect(lvTimer, &QTimer::timeout, this, &CameraTetherWidget::on_lvTimer_timeout);
		lvTimer->start(100);
	}
	else{
		lvTimer->stop();
		disconnect(lvTimer, &QTimer::timeout, this, &CameraTetherWidget::on_lvTimer_timeout);
		delete lvTimer;
		lvTimer = 0;
	}
}

void CameraTetherWidget::on_lvTimer_timeout(){
	if(selectedCamera){
		QPixmap *preview = selectedCamera->getLiveViewImage();
		QGraphicsScene *scene =  new QGraphicsScene();//ui.graphicsView->scene(); //
		ui.graphicsView->setScene(scene);

		scene->addPixmap(preview->scaled(ui.graphicsView->width(),ui.graphicsView->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
		ui.graphicsView->show();
	}
}

void CameraTetherWidget::on_qcamera_image_captured(QImage image)
{
	if(ui.gbImagePreview->isChecked())
	{

		QPixmap preview = QPixmap::fromImage(image);
		QGraphicsScene *scene = new QGraphicsScene();
		ui.graphicsView->setScene(scene);

		scene->addPixmap(preview.scaled(ui.graphicsView->width(),ui.graphicsView->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
		ui.graphicsView->show();
	}


	if(ui.cbLiveView->isChecked()){
		lvTimer->start();
	}

}
