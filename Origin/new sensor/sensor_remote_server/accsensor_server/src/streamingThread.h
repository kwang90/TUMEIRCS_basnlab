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
#include <QList>
#include <inttypes.h>
#include "motdet/processdata.h"

class mainWindowImpl;
class streamingThread : public QObject{

	Q_OBJECT

public:
	enum streamingState_t{STATE_IDLE, STATE_CONNECTING, STATE_STREAMING,STATE_DISCONNECTING};

private:
	int fd;
	bool terminate;
	mainWindowImpl* mwi;
	char* devNode;
	double dataPeriod;
	QSemaphore sema;
	streamingState_t streamingState;
	QList<float> aX;
	QList<float> aY;
	QList<float> aZ;
	QList<float> lastAx;
	QList<float> lastAy;
	QList<float> lastAz;

	uint32_t activity;
	processdata *motDet;

public Q_SLOTS:
	void doStreaming();

public:
	streamingThread();
	~streamingThread();
	void stopStreaming();
	void setMainWindowImpl(mainWindowImpl* mwi);
	void acmStreaming();
	void bleStreaming();
	void bleRawStreaming();
	void e1chStreaming();

	double getDataPeriod();
	streamingState_t getStreamingState();
	char* getRemoteAddress();
	void addToBuf(float aX, float aY, float aZ);
	QList<float>* getAx();
	QList<float>* getAy();
	QList<float>* getAz();
	QList<float>* getLastAx();
	QList<float>* getLastAy();
	QList<float>* getLastAz();

	uint32_t getActivity();
	void performActivityDetection(QList<float> aX,QList<float> aY,QList<float> aZ);
	void acquireSema();
	void releaseSema();

	processdata* getMotDet();

};


#endif
