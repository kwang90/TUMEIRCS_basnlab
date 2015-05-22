/*
 * mainWindowImpl.c
 *
 *  Created on: 14.03.2013
 *      Author: kindt
 */
#include "ui_accView.h"

#include <QtGui>
#include "streamingThread.h"
#include "displayWidget.h"
#include "selectSourceWidgetImpl.h"
#include <QTimer>
#include "spectrum.h"
#include "displaySpectrum.h"
#include <QSemaphore>
#include <inttypes.h>
#include <QString>
#include "libHDRHistory.h"
class MainWindowImpl : public QMainWindow, private Ui::MainWindow {
	Q_OBJECT
public:
	enum protocol_t{PROTOCOL_ACM, PROTOCOL_BLE, PROTOCOL_BLE_RAW, PROTOCOL_USB_RAW, PROTOCOL_BLE_E1CH};
	protocol_t protocol;
private:
	char devNode[255];
	streamingThread* sThread;
	QThread* sThreadPhy;
	selectSourceWidgetImpl* sswi;

public:
	MainWindowImpl(QWidget* parent = 0);
	~MainWindowImpl();
	void setDevNode(char* devNode);
	char* getDevNode();
	bool getContErrorRate();

	displayWidget* dAx;
	displayWidget* dAy;
	displayWidget* dAz;
	displayWidget* dRSSI;
	displayWidget* dGoodPut;
	displayWidget* dErrRate;

	spectrum sX;
	displaySpectrum dSx;
	spectrum sY;
	displaySpectrum dSy;
	spectrum sZ;
	displaySpectrum dSz;
	spectrum sRSSI;
	displaySpectrum dSrssi;

	QTimer* animTimer;

	uint32_t packetsReceived;
	libHDRHistory<uint32_t> *rxOk;
	libHDRHistory<uint32_t> *rxFail;
	libHDRHistory<double> *throughputHistory;
	libHDRHistory<double> *timeStampHistory;
	uint32_t sumRxOk;
	uint32_t sumRxFail;
	void setProtocol(protocol_t prot);
	protocol_t getProtocol();
	void setDeviceNode(uint8_t* node);
	char* getDeviceNode();
	void addTimeStamp(double timestamp, uint32_t repetitions);
	void addPacketStats(uint32_t rxOk, uint32_t rxFail, uint32_t repetitions);
	void addThroughPut(double throughput, uint32_t repetitions);

	void resetPacketStats();
public slots:
	void startStopStreaming();
	void streamingFinished();
	void animate();
	void showSelectSourceWidget();
	void resizeDisplayWidgets();
	void exportCSV();
	void showErrorRate(QString info);


};

