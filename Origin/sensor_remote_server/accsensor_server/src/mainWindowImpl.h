/*
 * mainWindowImpl.h - Implementation of the GUI
 *
 *  Created on: 20.06.2013
 *  Author: Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#ifndef MAINWINDOWIMPL_H_
#define MAINWINDOWIMPL_H_

#include <QtGui>
#include <QApplication>
#include <QThread>
#include "ui_mainWindow.h"
#include "xmlrpcthread.h"
#include "he2mtThread.h"
#include "xmlrpcthreadrawdata.h"
#include <inttypes.h>
#include "streamingThread.h"
#include "selectSourceWidgetImpl.h"
#include <QTimer>
#include "genericParametrizationWidgetImpl.hpp"
class mainWindowImpl: public QMainWindow, protected Ui::MainWindow{
	Q_OBJECT

public:
		enum protocol_t{PROTOCOL_ACM, PROTOCOL_BLE, PROTOCOL_BLE_RAW, PROTOCOL_BLE_E1CH};
		protocol_t protocol;
private:
		xmlRpcThread xmlRpc;			//This object will handle the actual XML RPC call and is supposed to be run in its own thread (reqThread)
		QThread *reqThread;				//Thread for xmlRpc-object's execXmlRpcCall()-method
		xmlRpcThreadRawData xmlRpcRaw;	//Object to handle XML-RPC requests with raw data
		QThread *reqThreadRaw;			//Thread to run the xmlRpcRaw-function in
		he2mtThread he2mtSDI;
		QThread *he2mtSDIthread;
		char devNode[255];
		streamingThread* sThread;
		QThread* sThreadPhy;
		selectSourceWidgetImpl* sswi;
		QTimer* animTimer;
		QTimer* activityDetectionTimer;
		QTimer* xmlRpcStreamingTimerRaw;
		genericParametrizationWidgetImpl *he2mtGenParamWidget;
public:
		mainWindowImpl(QWidget* parent = 0);
		~mainWindowImpl();

		QCheckBox* getCbActivityDetection();
		QCheckBox* getCbXmllRpcStreamingRaw();
		QCheckBox* getCbHe2mtActivateActivity();
		QCheckBox* getCbHe2mtActivateRaw();

public Q_SLOTS:

		void about();					//shows the about-window
		void setActivity();				//set the activity chosen in the list
		void setAccelerationRaw();		//set the acceleration choosen with the raw interface
		void xmlRpcCallFinished();		//will be called if XML RPC Call thread has finished
		void xmlRpcCallFinishedRaw();		//will be called if XML RPC Call thread has finished for raw acceleration
		void he2mtTransactionFinished();
		void updateServerUrl();			//set XML-RPC-Server-URL from GUI
		void setProtocol(protocol_t prot);
		protocol_t getProtocol();
		void setDeviceNode(uint8_t* node);
		char* getDeviceNode();
		void startStopStreaming();
		void streamingFinished();
		void animate();
		void animateActivity();
		void showSelectSourceWidget();
		QThread& getStreamingThreadPhy();
		void toggleActivityDetection();
		void xmlRpcStreamRaw();
		void toggleAccelerationStreamingRaw();
		void updateAccelerationStreamingRawPeriod();
		he2mtThread* getHe2mtSDIThread();
		bool he2mtAuthorize();
		void paramInterface2Gui();
		void setRawAccBufferSize();
};

#endif /* MAINWINDOWIMPL_H_ */
