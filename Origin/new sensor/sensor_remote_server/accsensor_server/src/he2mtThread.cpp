/*
 * he2mtThread.cpp
 *
 *  Created on: 20.06.2013
 *      Author: kindt
 */
#include "he2mtThread.h"
#include <iostream>
#include <time.h>
#include <string.h>
#include <xmlrpc-c/client_simple.hpp>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#define HE2MT_THREAD_BUFFER_DEFAULT_LENGTH 10
#define HE2MT_THREAD_BUFFER_SAFETY_MARGIN 100
using namespace std;

	he2mtThread::he2mtThread():
	sema(1), sdi()
	{
		accValues = (float*) malloc((3*(HE2MT_THREAD_BUFFER_DEFAULT_LENGTH + HE2MT_THREAD_BUFFER_SAFETY_MARGIN + 10))* sizeof(float));
		success = false;
		activityWaiting = false;
		authorizationRequested = false;
		sdi.init();
		rawAccBufferLength = HE2MT_THREAD_BUFFER_DEFAULT_LENGTH;
		accelerationsWaiting = 0;
		accSamplingPeriod = 0.0025;
		sprintf(bleMAC,"000000000000");
		//waitSema.acquire();
	};

	he2mtThread::~he2mtThread(){
		free(accValues);
	}



	void he2mtThread::setRawAccBufferLength(uint32_t nTriples){
		printf("updating buffer length to %d triples\n",nTriples);
		sema.acquire();
		rawAccBufferLength = nTriples;
		free(accValues);
		accValues = (float*) malloc((3*rawAccBufferLength + (HE2MT_THREAD_BUFFER_SAFETY_MARGIN)) * sizeof(float));
		if(accValues == NULL){
			printf("malloc() failed.");
			exit(1);
		}
		sema.release();
	}
	bool he2mtThread::isSuccess(){
		bool rv;

		sema.acquire();
			if(sdi.getLastError() == HE2MT_SDI_ERROR_SUCCESS){
				rv = true;
			}else{
				rv = false;
			}
		sema.release();

		return rv;
	}



	bool he2mtThread::setNextAcceleration(float nextAccX, float nextAccY, float nextAccZ){
		bool rv = false;
		sema.acquire();
		if(accelerationsWaiting == 0){
	 		time(&(nextTimestampRawAcc));
		}
		if(accelerationsWaiting < rawAccBufferLength + HE2MT_THREAD_BUFFER_SAFETY_MARGIN){
			accValues[3*accelerationsWaiting] = nextAccX;
			accValues[3*accelerationsWaiting+1] = nextAccY;
			accValues[3*accelerationsWaiting+2] = nextAccZ;
		accelerationsWaiting++;
		}else{
			printf("WARNING: HE2mT SDI buffer full.\n");
		}
		if(accelerationsWaiting >= rawAccBufferLength){
			rv = true;
			waitSema.release();
			sema.release();
		}else{
			sema.release();
		}
		return rv;
	}
	void he2mtThread::setNextActivity(rcsActivity::activity_type activity){
		sema.acquire();
 		time(&(nextTimestampActivity));
 		this->nextActivity = activity;
 		activityWaiting = true;
 		sema.release();
 		waitSema.release();
	}

	void he2mtThread::execHe2mtTransaction(){
		char* buf;
		char tmp[257];
		struct tm* timeInfo;
		while(1){

				cout<<"HE2mt thread Waiting for Data..."<<endl;
				waitSema.acquire();
				sema.acquire();
				if(authorizationRequested){
					sdi.authorize(client_id,client_secret,username,password);
					authorizationRequested = false;
				}
				if(activityWaiting){
					timeInfo = localtime(&nextTimestampRawAcc);
					sdi.setSensorData(HE2MT_SDI_DATA_SLOT_ACTIVITY,bleMAC,asctime(timeInfo),1,0,&nextActivity);
					activityWaiting = false;
				};
				if(accelerationsWaiting >= rawAccBufferLength){
					timeInfo = localtime(&nextTimestampRawAcc);
					sdi.setSensorData(HE2MT_SDI_DATA_SLOT_RAWACC,bleMAC,asctime(timeInfo),accSamplingPeriod,accelerationsWaiting,accValues);
					accelerationsWaiting = 0;
				}

				sema.release();
				Q_EMIT he2mtTransactionFinished();
		}
		this->thread()->quit();
	};



	void he2mtThread::sendOut(){

		waitSema.release();
	}


	bool he2mtThread::authorize(char* client_id, char* client_secret, char* username, char* password){
		sema.acquire();
		strncpy(this->client_id, client_id,256);
		strncpy(this->client_secret, client_secret,256);
		strncpy(this->username, username,256);
		strncpy(this->password, password,256);
		authorizationRequested = true;
		sema.release();
		waitSema.release();
		return true;
	}

	he2mtSDI* he2mtThread::getSDI(){
		return &sdi;
	}


	void he2mtThread::setBLEMAC(char* mac){
		sema.acquire();
		strncpy(bleMAC,mac,12);
		sema.release();
	}

