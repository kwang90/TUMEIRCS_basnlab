/*
 * mainWindowImpl.h
 *
 *  Created on: 20.06.2013
 *      Author:Philipp Kindt
 */

#include <QtGui>
#include <QThread>
#include "xmlrpc.h"
#include "mainWindowImpl.h"
#include <iostream>
#include "streamingThread.h"
#include "motdet/processdata.h"
#include "motdet/lococheck.h"
#include <locale.h>
using namespace std;

mainWindowImpl::mainWindowImpl(QWidget* parent)
{
	setupUi(this);
	connect(actionQuit, SIGNAL(activated()), qApp, SLOT(quit()));
	connect(actionAbout, SIGNAL(activated()), this, SLOT(about()));
	reqThread = new QThread((QObject*) this);
	xmlRpc.moveToThread(reqThread);
	reqThreadRaw = new QThread((QObject*) this);
	xmlRpcRaw.moveToThread(reqThreadRaw);
	he2mtSDIthread = new QThread((QObject*) this);
	he2mtSDI.moveToThread(he2mtSDIthread);

	connect(reqThread,SIGNAL(finished()),this,SLOT(xmlRpcCallFinished()));
	connect(reqThread,SIGNAL(started()),&xmlRpc,SLOT(execXmlRpcCall()));
	connect(reqThreadRaw,SIGNAL(started()),&xmlRpcRaw,SLOT(execXmlRpcCall()));
	connect(&xmlRpcRaw,SIGNAL(xmlRawCallFinished()),this, SLOT(xmlRpcCallFinishedRaw()));
	connect(he2mtSDIthread,SIGNAL(started()),&he2mtSDI,SLOT(execHe2mtTransaction()));
	connect(&he2mtSDI,SIGNAL(he2mtTransactionFinished()),this, SLOT(he2mtTransactionFinished()));

	connect(pbSetActivity,SIGNAL(clicked()),this,SLOT(setActivity()));
	connect(pbSetAcceleration, SIGNAL(clicked()), this,SLOT(setAccelerationRaw()));
	connect(pbSetXmlRpcData,SIGNAL(clicked()), this, SLOT(updateServerUrl()));
	connect(manualActivity, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(setActivity()));
	updateServerUrl();


	connect(pbHe2mtSignOn,SIGNAL(clicked()),this,SLOT(he2mtAuthorize()));

	//set up thread for data streaming
	sThread = new streamingThread();		// the streaming-thread class contains the code for data streaming
	sThreadPhy = new QThread(this);			// the "physical" repsentation of the thread (control structures,...)
	sThread->moveToThread(sThreadPhy);
	sThread->setMainWindowImpl(this);

	//animation Timer
	animTimer = new QTimer(this);

	//activity detection timer
	activityDetectionTimer = new QTimer(this);

	//rawStreamingTimer
	xmlRpcStreamingTimerRaw = new QTimer(this);

	//sensor streaming UI
	connect(this->pbStreaming,SIGNAL(clicked()),this,SLOT(startStopStreaming()));
	connect(sThreadPhy,SIGNAL(started()),sThread,SLOT(doStreaming()));
	connect(sThreadPhy,SIGNAL(finished()),this,SLOT(streamingFinished()));
	connect(animTimer,SIGNAL(timeout()), this, SLOT(animate()));
	connect(xmlRpcStreamingTimerRaw,SIGNAL(timeout()), this, SLOT(xmlRpcStreamRaw()));
	connect(activityDetectionTimer,SIGNAL(timeout()), this, SLOT(animateActivity()));
	connect(cbActivtyDetection,SIGNAL(toggled(bool)), this, SLOT(toggleActivityDetection()));
	connect(cbXmllRpcStreamingRaw,SIGNAL(toggled(bool)), this, SLOT(toggleAccelerationStreamingRaw()));
	connect(sbSampingRateRaw, SIGNAL(valueChanged(int)), this, SLOT(updateAccelerationStreamingRawPeriod()));

	//streaming source selection
	strcpy(devNode,"/dev/ttyACM1");
	protocol = PROTOCOL_BLE_E1CH;
	sswi = new selectSourceWidgetImpl(this);
	sswi->setMainWindowImpl(this);
	connect(actionSensorStreaming,SIGNAL(activated()),this,SLOT(showSelectSourceWidget()));

	lococheck* myLoco = sThread->getMotDet()->lc;


	//connect motDetLib settinggs
    connect(sbRunning, SIGNAL(valueChanged(double)),myLoco, SLOT(setTrRunning(double)));
    connect(sbWalkingZ, SIGNAL(valueChanged(double)),myLoco, SLOT(setTrWalkingZ(double)));
    connect(sbWalkingY, SIGNAL(valueChanged(double)),myLoco, SLOT(setTrWalkingY(double)));
    connect(sbRestingX, SIGNAL(valueChanged(double)),myLoco, SLOT(setTrXth(double)));
    connect(sbRestingY, SIGNAL(valueChanged(double)),myLoco, SLOT(setTrYth(double)));
    connect(sbRestingZ, SIGNAL(valueChanged(double)),myLoco, SLOT(setTrZth(double)));

    //he2mt SDI
	connect(accBuferLength, SIGNAL(valueChanged(int)), this, SLOT(setRawAccBufferSize()));

	//generic parameter interface for he2mtSDI
	he2mtGenParamWidget = new genericParametrizationWidgetImpl(this);
	he2mtGenParamWidget->setParamInterface(he2mtSDI.getSDI());
	he2mtSDIParamInterfaceDrawSpace->setWidget(he2mtGenParamWidget);
	he2mtGenParamWidget->updateGui();
	he2mtGenParamWidget->show();

	connect(pbUpdateHe2mtParamInterface, SIGNAL(clicked()), this, SLOT(paramInterface2Gui()));
	char tmpStr[257];
	he2mtSDI.getSDI()->getIdent(tmpStr);
	he2mtSDIIdent->setText(tmpStr);
	he2mtSDI.getSDI()->getParam("vendor",tmpStr);
	he2mtSDIVendor->setText(tmpStr);

    //start xmlrpcraw-thread
    reqThreadRaw->start();
    he2mtSDIthread->start();

};

mainWindowImpl::~mainWindowImpl(){
	delete reqThread;
	delete reqThreadRaw;
	delete sswi;
	delete animTimer;
	delete activityDetectionTimer;
	delete sThread;
	delete sThreadPhy;
	delete he2mtGenParamWidget;

}

void mainWindowImpl::about(){
	QMessageBox msgBox;
	msgBox.setText("<b>AccSensor Server</b><br>A server for RCS mobile sensor nodes<br>&copy; 2013, 2014 Lehrstuhl f&uuml;r Realzeit-Computersysteme (RCS)<br> Technische Universit&auml;t M&uuml;nchen (TUM)<br>Arcistr. 21<br>80333 M&uuml;nchen <br><br>&copy; 2013 Philipp Kindt &lt;<a href = 'mailto:kindt@rcs.ei.tum.de'>kindt@rcs.ei.tum.de</a>&gt;<br>&copy; 2013 Dr. Robert Diemer &lt;<a href = 'mailto:diemer@rcs.ei.tum.de'>diemer@rcs.ei.tum.de</a>&gt;<br> <hr> This programm uses the following librarys:<br> <b>QT 4.8.4</b>, (c) Digia. License: LGPL. See COPYING.QT for details. <br> <b>JSONCPP</b>: written by Baptiste Lepilleur. License: public domain/MIT, see COPYING.JSONCPP<br><b>librest-0.7</b>: Intel/various authors. license: LGPL, see COPYING.LGPL<br><b>glibc</b>: Various authors, LGPL/GPL and others, see COPYING.LGPL and COPYING.LIBC<br><b>libXMLrpc</b>: various authors, see COPYING.LIBXMLRPC");
//	msgBox.setText("<b>AccSensor Server</b><br>A server for RCS mobile sensor nodes<br>&copy; 2013, Lehrstuhl f&uuml;r Realzeit-Computersysteme (RCS)<br> Technische Universit&auml;t M&uuml;nchen (TUM)<br>Arcistr. 21<br>80333 M&uuml;nchen <br><br>&copy; 2013 Philipp Kindt &lt;<a href = 'mailto:kindt@rcs.ei.tum.de'>kindt@rcs.ei.tum.de</a>&gt;");

	msgBox.exec();

}

void mainWindowImpl::setRawAccBufferSize(){
	he2mtSDI.setRawAccBufferLength(accBuferLength->value());
}
void mainWindowImpl::setActivity(){
	if(manualActivity->currentRow() == -1){
		cerr<<"no activity seleceted in list."<<endl;
		return;
	}
	if(cbActivateManualXmlRPC->isChecked()){
		xmlRpc.setNextActivity((rcsActivity::activity_type) manualActivity->currentRow());
		reqThread->start();
	}
	if(cbActivateManualHe2mt->isChecked()){

		QPalette pal = labelHe2mtState->palette();
		pal.setColor(labelXmlRpcStateRaw->backgroundRole(),QColor(Qt::yellow));
		he2mtSDI.setNextActivity((rcsActivity::activity_type) manualActivity->currentRow());
		labelHe2mtState->setPalette(pal);
		labelHe2mtState->setAutoFillBackground(true);
		labelHe2mtState->update();
		labelHe2mtState->setText(QString("Busy"));
	}

}

void mainWindowImpl::setAccelerationRaw(){
	if(cbActivateManualXmlRPC->isChecked()){

		xmlRpcRaw.setNextAcceleration(dsAccX->value() * 100,dsAccY->value() * 100,dsAccZ->value() * 100);
		xmlRpcRaw.sendOut();
	}
	if(cbActivateManualHe2mt->isChecked()){
		if(he2mtSDI.setNextAcceleration(dsAccX->value(),dsAccY->value(),dsAccZ->value())){
			QPalette pal = labelHe2mtState->palette();
			pal.setColor(labelXmlRpcStateRaw->backgroundRole(),QColor(Qt::yellow));
			labelHe2mtState->setPalette(pal);
			labelHe2mtState->setAutoFillBackground(true);
			labelHe2mtState->update();
			labelHe2mtState->setText(QString("Busy"));
		}
	}
}
void mainWindowImpl::he2mtTransactionFinished(){
	QPalette pal = labelHe2mtState->palette();

	if(he2mtSDI.isSuccess()){
		labelHe2mtState->setText(QString("success"));
		pal.setColor(labelHe2mtState->backgroundRole(),QColor(Qt::green));
	}else{
		labelHe2mtState->setText(QString("failure"));
		pal.setColor(labelHe2mtState->backgroundRole(),QColor(Qt::red));

	}

	labelHe2mtState->setPalette(pal);
	labelHe2mtState->setAutoFillBackground(true);
	labelHe2mtState->update();

}

void mainWindowImpl::xmlRpcCallFinished(){
	QPalette pal = labelXmlRpcState->palette();

	if(xmlRpc.isSuccess()){
		labelXmlRpcState->setText(QString("success"));
		pal.setColor(labelXmlRpcState->backgroundRole(),QColor(Qt::green));
	}else{
		labelXmlRpcState->setText(QString("failure"));
		pal.setColor(labelXmlRpcState->backgroundRole(),QColor(Qt::red));

	}

	labelXmlRpcState->setPalette(pal);
	labelXmlRpcState->setAutoFillBackground(true);
	labelXmlRpcState->update();

}

void mainWindowImpl::xmlRpcCallFinishedRaw(){
	QPalette pal = labelXmlRpcStateRaw->palette();

	if(xmlRpcRaw.isSuccess()){
		labelXmlRpcStateRaw->setText(QString("success"));
		pal.setColor(labelXmlRpcStateRaw->backgroundRole(),QColor(Qt::green));
	}else{
		labelXmlRpcStateRaw->setText(QString("failure"));
		pal.setColor(labelXmlRpcStateRaw->backgroundRole(),QColor(Qt::red));

	}

	labelXmlRpcStateRaw->setPalette(pal);
	labelXmlRpcStateRaw->setAutoFillBackground(true);
	labelXmlRpcStateRaw->update();

}


void mainWindowImpl::updateServerUrl(){
	xmlRpc.setServerUrl(xmlRpcUrl->text().toAscii().data());
	xmlRpcRaw.setServerUrl(xmlRpcUrl->text().toAscii().data());
	cout<<"server URL set to: "<<xmlRpcUrl->text().toAscii().data()<<endl;
}
void mainWindowImpl::setProtocol(protocol_t prot){
	protocol = prot;
}

void mainWindowImpl::setDeviceNode(uint8_t* node){
	strcpy(devNode, (const char*)node);
	cout<<"deviceNode set to: "<<devNode<<endl;
};

mainWindowImpl::protocol_t mainWindowImpl::getProtocol(){
	return protocol;
};


char* mainWindowImpl::getDeviceNode(){
	return devNode;
}

void mainWindowImpl::toggleActivityDetection(){
	if(cbActivtyDetection->isChecked()){
		cout<<"Activating activity detection algorithm"<<endl;
		activityDetectionTimer->start(1000);
	}else{
		cout<<"Disactivating activity detection algorithm"<<endl;
		activityDetectionTimer->stop();
		labelDetectedActivity->setPixmap(QPixmap(QString(":/icons/unknown.gif")));

	}
}

void mainWindowImpl::updateAccelerationStreamingRawPeriod(){
	int32_t period = (int)(1000.0 / (double)(sbSampingRateRaw->value()));
	if(cbXmllRpcStreamingRaw->isChecked()){
		xmlRpcStreamingTimerRaw->setInterval(period);
		cout<<"Setting XmlRPC Streaming period to "<<period<<" ms."<<endl;

	};
}

void mainWindowImpl::toggleAccelerationStreamingRaw(){
	int32_t period = (int)(1000.0 / (double)(sbSampingRateRaw->value()));
	if(cbXmllRpcStreamingRaw->isChecked()){
		cout<<"Activating Raw XmlRPC Streaming with period of "<<period<<" ms."<<endl;
		xmlRpcStreamingTimerRaw->start(period);
	}else{
		cout<<"Disactivating Raw XMLRPC Streaming"<<endl;
		xmlRpcStreamingTimerRaw->stop();
	}
}


void mainWindowImpl::startStopStreaming(){
	if(pbStreaming->isChecked()){
		//packetsReceived = 0;

		//start Streaming Thread
		sThreadPhy->start();

		//start adnimation timer with ~24 shots/second
		animTimer->start(42);

		//start activity detection timer with ~shot/second
		if(cbActivtyDetection->isChecked()){
			activityDetectionTimer->start(1000);
		}
	}else{
		sThread->stopStreaming();
		animTimer->stop();
		activityDetectionTimer->stop();
		labelDetectedActivity->setPixmap(QPixmap(QString(":/icons/unknown.gif")));

	}
}


//will be called after the streaming thread has terminated
void mainWindowImpl::streamingFinished(){
	std::cout<<"Data streaming stopped."<<std::endl;
	animate();
	pbStreaming->setChecked(false);
	animTimer->stop();
	activityDetectionTimer->stop();

}

void mainWindowImpl::animate(){
	static char bitrate[50];
	streamingThread::streamingState_t state = sThread->getStreamingState();

	char* addr;
	if(state == streamingThread::STATE_IDLE){
		labelDataRate->setText(" - ");
		labelSensorStatus->setText("Disconnected");
		labelSensorAddress->setText(" - ");
	}
	if(state == streamingThread::STATE_CONNECTING){
		labelSensorStatus->setText("Connecting");
		labelSensorAddress->setText(" - ");
		labelDataRate->setText(" - ");

	}

	if(state == streamingThread::STATE_STREAMING){
		sprintf(bitrate,"%.1f Bits/s",1.0/sThread->getDataPeriod() * 20*8);
		labelDataRate->setText(bitrate);
		labelSensorStatus->setText("Streaming");
		addr = sThread->getRemoteAddress();
		if(addr == NULL){
			labelSensorAddress->setText("Unknown");
		}else{
			labelSensorAddress->setText(QString().sprintf("BLE::0x%s",addr));
			he2mtSDI.setBLEMAC(addr);

			free(addr);
		}

		labelDataRate->setText(QString().sprintf("%.1f Bit/s",1.0/sThread->getDataPeriod() * 20*8));
	}

	if(state == streamingThread::STATE_DISCONNECTING){
		he2mtSDI.setBLEMAC("unknown");
		labelSensorStatus->setText("Connecting");
		labelSensorAddress->setText(" - ");
		labelDataRate->setText(QString().sprintf(" - "));

	}

}
void mainWindowImpl::xmlRpcStreamRaw(){
   if(cbXmllRpcStreamingRaw->isChecked()){
		sThread->acquireSema();
	   if((!(sThread->getLastAx()->isEmpty()))&&(!(sThread->getLastAy()->isEmpty()))&&(!(sThread->getLastAz()->isEmpty()))){
		   //cout<<"Sending Acc: "<<sThread->getLastAx()->takeLast() * 100.0<<","<<sThread->getLastAy() * 100.0<<","<<sThread->getLastAz() * 100.0<<")"<<endl;

			   xmlRpcRaw.setNextAcceleration(sThread->getLastAx()->takeLast() * 100.0, sThread->getLastAy()->takeLast() * 100.0, sThread->getLastAz()->takeLast() * 100.0);
			   sThread->releaseSema();
			   xmlRpcRaw.sendOut();
	   }else{
		   sThread->releaseSema();

	   }
   }
}
void mainWindowImpl::animateActivity(){
	static uint32_t activityPrev = 99999;

	uint32_t activity = sThread->getActivity();

	static QMovie* movie = NULL;
	switch(activity){
		case 1:

			if(activityPrev != activity){
				if(movie != NULL){
					labelDetectedActivity->setMovie(NULL);
					delete movie;
				}
				movie = new QMovie(QString(":/icons/gifwalking2.gif"));
				labelDetectedActivity->setMovie(movie);
				movie->start();
				activityPrev = activity;

				if(cbActivityXmlRpc->isChecked()){
					xmlRpc.setNextActivity((rcsActivity::activity_type) rcsActivity::ACTIVITY_TYPE_WALKING);
					reqThread->start();
				}

				if(cbHe2mtActivateActivity->isChecked()){
					he2mtSDI.setNextActivity(rcsActivity::ACTIVITY_TYPE_WALKING);
				}

			}

		break;
		case 2:

				if(activityPrev != activity){
					if(movie != NULL){
						labelDetectedActivity->setMovie(NULL);
						delete movie;
					}
					movie = new QMovie(QString(":/icons/gifrunning2.gif"));
					labelDetectedActivity->setMovie(movie);
					movie->start();

					if(cbActivityXmlRpc->isChecked()){
						xmlRpc.setNextActivity((rcsActivity::activity_type) rcsActivity::ACTIVITY_TYPE_RUNNING);
						reqThread->start();
					}
					if(cbHe2mtActivateActivity->isChecked()){
						he2mtSDI.setNextActivity(rcsActivity::ACTIVITY_TYPE_RUNNING);
					}
					activityPrev = activity;

				}
		break;
		case 3:
			if(movie != NULL){
				labelDetectedActivity->setMovie(NULL);
				delete movie;
			}
			movie = new QMovie(QString(":/icons/gifresting2.gif"));
			labelDetectedActivity->setMovie(movie);
			movie->start();

			if(activityPrev != activity){
				if(cbActivityXmlRpc->isChecked()){
					xmlRpc.setNextActivity((rcsActivity::activity_type) rcsActivity::ACTIVITY_TYPE_RESTING);
					reqThread->start();
				}
				if(cbHe2mtActivateActivity->isChecked()){
					he2mtSDI.setNextActivity(rcsActivity::ACTIVITY_TYPE_RESTING);
				}

				activityPrev = activity;

			}

		break;
		default:

			labelDetectedActivity->setPixmap(QPixmap(QString(":/icons/unknown.gif")));
			if(activityPrev != activity){
					if(movie != NULL){
						movie->stop();
						labelDetectedActivity->setMovie(NULL);
						delete movie;
					}
					if(cbActivityXmlRpc->isChecked()){
						xmlRpc.setNextActivity((rcsActivity::activity_type) rcsActivity::ACTIVITY_TYPE_UNKNOWN);
						reqThread->start();
						activityPrev = activity;
					}
			}

		break;
			}

}
void mainWindowImpl::showSelectSourceWidget(){

	this->sswi->show();
}

QThread& mainWindowImpl::getStreamingThreadPhy(){
	return *(this->sThreadPhy);
}

QCheckBox* mainWindowImpl::getCbActivityDetection(){
	return cbActivtyDetection;
}

QCheckBox* mainWindowImpl::getCbXmllRpcStreamingRaw(){
	return cbXmllRpcStreamingRaw;
}

QCheckBox* mainWindowImpl::getCbHe2mtActivateActivity(){
	return cbHe2mtActivateActivity;
}

QCheckBox* mainWindowImpl::getCbHe2mtActivateRaw(){
	return cbHe2mtActivateRaw;
}

he2mtThread* mainWindowImpl::getHe2mtSDIThread(){
	return &he2mtSDI;
}

bool mainWindowImpl::he2mtAuthorize(){
	return he2mtSDI.authorize("id",he2mtAppSecret->text().toAscii().data(),he2mtUsername->text().toAscii().data(),he2mtPassword->text().toAscii().data());
}

void mainWindowImpl::paramInterface2Gui(){
	he2mtGenParamWidget->guiToInterface();

	he2mtSDI.getSDI()->disconnect();
	he2mtSDI.getSDI()->init();
	he2mtAuthorize();

}
