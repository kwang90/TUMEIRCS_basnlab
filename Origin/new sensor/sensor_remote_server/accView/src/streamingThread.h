/*
 * streamingThread.cpp
 *
 *  Created on: 14.03.2013
 *      Author: kindt
 */

#ifndef STREAMING_THREAD_H
#define STREAMING_THREAD_H
#include <QObject>
#include <QSemaphore>
#include <QString>
class MainWindowImpl;
class streamingThread : public QObject{

	Q_OBJECT

private:
	int fd;
	bool terminate;
	MainWindowImpl* mwi;
	char* devNode;
	double dataPeriod;
	double recentRSSI;
	QSemaphore sema;
public slots:
	void doStreaming();

public:
	streamingThread();
	void stopStreaming();
	void setMainWindowImpl(MainWindowImpl* mwi);
	void acmStreaming();
	void bleStreaming();
	void bleRawStreaming();
	void usbRawStreaming();
	void lockSema();
	void releaseSema();
	double getDataPeriod();
	double getRecentRSSI();
	void e1chStreaming();

	signals:
	void indicateErrorRate(QString info);
};


#endif
