/*
 * mainWindowImpl.c
 *
 *  Created on: 14.03.2013
 *      Author: kindt
 */
#include "mainWindowImpl.h"
#include "streamingThread.h"
#include <QtGui>
#include <iostream>
#include <stdio.h>
#include <qpainter.h>
#include <string.h>
MainWindowImpl::MainWindowImpl(QWidget* parent){
	setupUi(this);


	rxOk = NULL;
	rxFail = NULL;
	//set up default protocol / devnode and initialize variables
	strcpy(this->devNode,"/dev/ttyACM1");
	protocol = PROTOCOL_BLE_E1CH;
	packetsReceived = 0;

	//set up thread for data streaming
	sThread = new streamingThread();		// the streaming-thread class contains the code for data streaming
	sThreadPhy = new QThread(this);			// the "physical" repsentation of the thread (control structures,...)
	sThread->moveToThread(sThreadPhy);
	sThread->setMainWindowImpl(this);

	//animation Timer
	animTimer = new QTimer(this);

	//setup the "select source widget" which allows choosing the device node and protocol
	sswi = new selectSourceWidgetImpl(0);
	sswi->setMainWindowImpl(this);


	//connect various signals
	connect(this->pbStreaming,SIGNAL(clicked()),this,SLOT(startStopStreaming()));
	connect(sThreadPhy,SIGNAL(started()),sThread,SLOT(doStreaming()));
	connect(sThreadPhy,SIGNAL(finished()),this,SLOT(streamingFinished()));
	connect(animTimer,SIGNAL(timeout()), this, SLOT(animate()));
	connect(actionStreamingSource, SIGNAL(activated()),this,SLOT(showSelectSourceWidget()));
	connect(sbViewDuration, SIGNAL(valueChanged(int)),this,SLOT(resizeDisplayWidgets()));
	connect(pbExportCSV,SIGNAL(clicked()),this,SLOT(exportCSV()));
	connect(actionQuit,SIGNAL(activated()),qApp,SLOT(quit()));
	connect(sThread,SIGNAL(indicateErrorRate(QString)),this,SLOT(showErrorRate(QString)));


	//set up data display widgets for raw signals. aX, aY,... are the other scroll ares; dAx,dAy,... the inner widgets
	//that actually show data
	dAx = new displayWidget();
	dAx->setMainWindowImpl(this);
	dAx->resize(aX->width()-10,aX->height() - 10);
	printf("1)\n");
	dAx->setScaleFactor(2000);
	printf("2)\n");
	aX->setWidget(dAx);


	dAy= new displayWidget();
	dAy->setMainWindowImpl(this);
	dAy->resize(aY->width() - 10,aY->height() - 10);
	dAy->setScaleFactor(2000);
	aY->setWidget(dAy);

	dAz= new displayWidget();
	dAz->resize(aZ->width() - 10,aZ->height() - 10);
	dAz->setMainWindowImpl(this);
	dAz->setScaleFactor(2000);
	aZ->setWidget(dAz);

	dRSSI= new displayWidget();
	dRSSI->resize(rssi->width() - 10,rssi->height() - 10);
	dRSSI->setMainWindowImpl(this);
	dRSSI->setScaleFactor(25.0);
	rssi->setWidget(dRSSI);

	dGoodPut= new displayWidget();
	goodPut->setWidget(dGoodPut);
	dGoodPut->resize(goodPut->width() - 10,goodPut->height() - 10);
	dGoodPut->setMainWindowImpl(this);
	dGoodPut->setScaleFactor(-2500);

	dErrRate = new displayWidget();
	dErrRate->setMainWindowImpl(this);
	dErrRate->resize(errRate->width(),errRate->height() - 10);
	dErrRate->setScaleFactor(-800.0);
	errRate->setWidget(dErrRate);

	//connect signals for raw data scrolling
	connect(aZ->horizontalScrollBar(),SIGNAL(sliderMoved(int)),aX->horizontalScrollBar(),SLOT(setValue(int)));
	connect(aZ->horizontalScrollBar(),SIGNAL(sliderMoved(int)),aY->horizontalScrollBar(),SLOT(setValue(int)));
	//connect(errRate->horizontalScrollBar(),SIGNAL(sliderMoved(int)),goodPut->horizontalScrollBar(),SLOT(setValue(int)));
	connect(errRate->horizontalScrollBar(),SIGNAL(sliderMoved(int)),rssi->horizontalScrollBar(),SLOT(setValue(int)));

	//adjust data view duration spinbox value
	sbViewDuration->setValue(dAx->width());

	//set up spectrum display widgets
	sX.setDataLenght(dAx->width());
	dSx.resize(dAx->width(),dAx->height());
	dSx.setMainWindowImpl(this);
	sVx->setWidget(&dSx);

	sY.setDataLenght(dAy->width());
	dSy.resize(dAy->width(),dAy->height());
	dSy.setMainWindowImpl(this);
	sVy->setWidget(&dSy);

	sZ.setDataLenght(dAz->width());
	dSz.resize(dAz->width(),dAz->height());
	dSz.setMainWindowImpl(this);
	sVz->setWidget(&dSz);

	sRSSI.setDataLenght(dRSSI->width());
	dSrssi.resize(dRSSI->width(),dRSSI->height());
	dSrssi.setMainWindowImpl(this);
	sVrssi->setWidget(&dSrssi);

	rxOk = new libHDRHistory<uint32_t>(dAx->width());
	rxFail = new libHDRHistory<uint32_t>(dAx->width());
	throughputHistory = new libHDRHistory<double>(dAx->width());
	timeStampHistory = new libHDRHistory<double>(dAx->width());

}

MainWindowImpl::~MainWindowImpl(){
	delete sThread;
	delete sThreadPhy;
	delete dAx;
	delete dAy;
	delete dAz;
	delete dRSSI;
	delete animTimer;
}

void MainWindowImpl::setDevNode(char* devNode){
	strcpy(this->devNode, devNode);
}

char* MainWindowImpl::getDevNode(){
	return this->devNode;
}

void MainWindowImpl::resizeDisplayWidgets(){

	sThread->lockSema();
	//newWidht is the widh of the outer widget (ScrollArea) and not of the scroll content...
	uint32_t newWidth = sbViewDuration->value();
	if(newWidth > tabWidget->width()- 70){
		newWidth = tabWidget->width() - 70;
	}

	//resize outer widgets
	aX->resize(newWidth + 10,aX->height());
	aY->resize(newWidth + 10,aY->height());
	aZ->resize(newWidth + 10,aZ->height());
	rssi->resize(newWidth + 10,rssi->height());
	errRate->resize(newWidth + 10,errRate->height());

	sVx->resize(newWidth + 10,sVx->height());
	sVy->resize(newWidth + 10,sVy->height());
	sVz->resize(newWidth + 10,sVz->height());
	sVrssi->resize(newWidth + 10,sVrssi->height());


	//resize inner widgets
	dAx->resize(sbViewDuration->value(),dAx->height());
	dAy->resize(sbViewDuration->value(),dAy->height());
	dAz->resize(sbViewDuration->value(),dAz->height());

	dRSSI->resize(sbViewDuration->value(),dRSSI->height());
	dErrRate->resize(sbViewDuration->value(),dErrRate->height());
	dGoodPut->resize(sbViewDuration->value(),dGoodPut->height());


	sX.setDataLenght(dAx->width());
	sY.setDataLenght(dAy->width());
	sZ.setDataLenght(dAz->width());
	sRSSI.setDataLenght(dRSSI->width());
	dSx.resize(dAx->width(),dAx->height());
	dSy.resize(dAy->width(),dAx->height());
	dSz.resize(dAz->width(),dAx->height());
	dSrssi.resize(dRSSI->width(),dAx->height());
	if(rxOk != NULL){
		delete timeStampHistory;
		delete rxOk;
		delete rxFail;
		delete throughputHistory;
	}
	rxOk = new libHDRHistory<uint32_t>(sbViewDuration->value());
	rxFail = new libHDRHistory<uint32_t>(sbViewDuration->value());
	throughputHistory = new libHDRHistory<double>(sbViewDuration->value());
	timeStampHistory = new libHDRHistory<double>(sbViewDuration->value());

	sThread->releaseSema();
	if((rxOk == NULL)||(rxFail == NULL)||(throughputHistory == NULL)||(timeStampHistory == NULL)){
		cerr<<"malloc() failed\n"<<endl;
		exit(1);
	}
	dAx->update();
	dAy->update();
	dAz->update();
	dRSSI->update();
	dGoodPut->update();
	dErrRate->update();
	dSx.update();
	dSy.update();
	dSz.update();
	dSrssi.update();

	//	packetsReceived = 0;


}

void MainWindowImpl::startStopStreaming(){
	if(pbStreaming->isChecked()){
		packetsReceived = 0;

		//start Streaming Thread
		sThreadPhy->start();

		//start adnimation timer with ~24 shots/second
		animTimer->start(42);
	}else{
		sThread->stopStreaming();
		animTimer->stop();
	}
}


//will be called after the streaming thread has terminated
void MainWindowImpl::streamingFinished(){
	std::cout<<"Data streaming stopped."<<std::endl;
	pbStreaming->setChecked(false);
	animTimer->stop();

}

/* This function is called periodically to update the  data view widgets.
 */
void MainWindowImpl::animate(){
	static char tmpTxt[100];
	sThread->lockSema();

	//update data-over-time-curves
	dAx->update();
	dAy->update();
	dAz->update();
	dErrRate->update();
	dGoodPut->update();
	dRSSI->update();

	//update spectrums
	if(tabWidget->currentWidget() == tabSpectrum){
		dSx.setData(sX.calcSpectrum());
		dSx.update();
		dSy.setData(sY.calcSpectrum());
		dSy.update();
		dSz.setData(sZ.calcSpectrum());
		dSz.update();
		dSrssi.setData(sRSSI.calcSpectrum());
		dSrssi.update();
	}

	sprintf(tmpTxt,"%.1f fps (Period: %.4f s)",1.0/sThread->getDataPeriod(),sThread->getDataPeriod());
	//update data rate widget
	labelDR->setText(tmpTxt);
	sprintf(tmpTxt,"%.0f dBm ",sThread->getRecentRSSI());
	labelRecentRSSI->setText(tmpTxt);

	sThread->releaseSema();
}

void MainWindowImpl::showSelectSourceWidget(){

	this->sswi->show();
}
void MainWindowImpl::setProtocol(protocol_t prot){
	protocol = prot;
}

void MainWindowImpl::setDeviceNode(uint8_t* node){
	strcpy(devNode, (const char*)node);
	cout<<"deviceNode set to: "<<devNode<<endl;
};

MainWindowImpl::protocol_t MainWindowImpl::getProtocol(){
	return protocol;
};


char* MainWindowImpl::getDeviceNode(){
	return devNode;
}

bool MainWindowImpl::getContErrorRate(){
	return contErrRate->isChecked();
}

void MainWindowImpl::showErrorRate(QString info){
	QMessageBox msgBox;
	msgBox.setText(info);
	msgBox.exec();

}
void MainWindowImpl::exportCSV(){
	QString filename,filename_orig;
	filename_orig = QFileDialog::getSaveFileName((QWidget*)this,QString("Save Image"),QString("./"),QString("*.csv"), NULL);
	filename = filename_orig;
	if(filename == NULL){			//"cancel" clicked...
		return;
	}

	//	filename.replace(QRegExp(".csv"), QString().sprintf("_%s.csv",));
	cout<<"Writing file: "<<filename.toLocal8Bit().data()<<endl;

	FILE* fp = fopen(filename.toLocal8Bit().data(),"w+");
	if(fp < 0){
		cout<<"Failed to create/open file:"<<filename.toLocal8Bit().data()<<endl;
		fclose(fp);
		return;

	}
	setlocale(LC_ALL,"C");
	fprintf(fp,"aX, aY, aZ, rssi, rxOk, rxFail, throughput, timestamp\n");
	sThread->lockSema();
	uint32_t samplesToWrite = dAx->getDataLength();
	if(samplesToWrite > timeStampHistory->getNrOfElements()){
		samplesToWrite = timeStampHistory->getNrOfElements();
	}
	printf("samplesToWrite = %d\n",samplesToWrite);
	for(uint32_t cnt = 0; cnt < samplesToWrite; cnt++){
		printf("1\n");
		rxOk->get(samplesToWrite -1- cnt);
		printf("2\n");
		rxFail->get(samplesToWrite-1-cnt);
		printf("3\n");
		throughputHistory->get(samplesToWrite - 1 - cnt);
		printf("4\n");
		timeStampHistory->get(samplesToWrite - 1 - cnt);
		printf("5\n");
		//RSSI data is inverted in displaywidget for intuitive visualization => re-invert before saving
		fprintf(fp, "%f, %f, %f, %f, %u, %u, %f, %f\n", (double) dAx->getData(cnt), (double) dAy->getData(cnt), (double) dAz->getData(cnt),(double) (dRSSI->getData(cnt)*-1.0) - 70.0, rxOk->get(samplesToWrite -1- cnt), rxFail->get(samplesToWrite-1-cnt), (double) throughputHistory->get(samplesToWrite - 1 - cnt), (double) timeStampHistory->get(samplesToWrite - 1 - cnt));
	}
	sThread->releaseSema();

	fclose(fp);
	cout<<"data exported."<<endl;

}

void MainWindowImpl::addTimeStamp(double timestamp, uint32_t repetitions){
	if(timeStampHistory != NULL){
		for(uint32_t cnt = 0; cnt < repetitions; cnt++){
				this->timeStampHistory->add(timestamp);
		}
	}
}

void MainWindowImpl::addPacketStats(uint32_t rxOk, uint32_t rxFail,uint32_t repetitions){
	sumRxOk += rxOk;
	sumRxFail += rxFail;
		for(uint32_t cnt = 0; cnt < repetitions; cnt++){
			this->rxOk->add(sumRxOk);
			this->rxFail->add(sumRxFail);
		}
}

void MainWindowImpl::addThroughPut(double throughput, uint32_t repetitions){
	if(throughputHistory != NULL){
		for(uint32_t cnt = 0; cnt < repetitions; cnt++){
				this->throughputHistory->add(throughput);
		}
	}
}

void MainWindowImpl::resetPacketStats(){

	sumRxOk = 0;
	sumRxFail = 0;
	rxOk->clear();
	rxFail->clear();
}



