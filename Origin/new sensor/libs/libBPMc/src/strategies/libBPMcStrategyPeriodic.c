/*
 * libBPMcStrategyPeriodic.c
 *
 *  Created on: 17.10.2013
 *      Author: kindt
 */


#include "../libBPMcInterface.h"
#include "libBPMcStrategyPeriodic.h"
#include <float.h> //for DBL_MAX
#include "../libBPMcParams.h"
#include "../libBPMcDebug.h"

#if LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_POSIX
#include <time.h>
#elif LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <time.h>
#include <hhd_rtc.h>
#else
ERROR - WRONG PLATTFORM 		//compile time error
#endif

void libBPMcStrategyPeriodic_init(libBPMcInterface* interface){
	LIBBPMC_DEBUG("initializing periodic strategy\n");
	libBPMcStrageyPeriodicInternals *periodicInternals = (libBPMcStrageyPeriodicInternals*) malloc(sizeof(libBPMcStrageyPeriodicInternals));
	interface->internalData = (void*) periodicInternals;
	periodicInternals->connectionIntervalMax = 3200;		//default max. connection interval- can be modified by setLantecyConstraintsCallBack()
	periodicInternals->intervalSafetyMargin = LIBBPMC_STRATEGY_PERIODIC_INTERVAL_SAFETY_MARGIN_DEFAULT;

	//Set all callbacks needed for power management
	libBPMcInterface_setActivationCallBack(interface,libBPMcStrategyPeriodic_setActivation);
	libBPMcInterface_setAddCallBackOut(interface,libBPMcStrategyPeriodic_addOut);
	libBPMcInterface_setSetTrafficBoundsCallBack(interface,libBPMcStrategyPeriodic_setTrafficBounds);
	libBPMcInterface_setSetBufferPtrCallBack(interface,libBPMcStrategyPeriodic_setBuffer);
	libBPMcInterface_setSetStartInfoCallBack(interface, libBPMcStrategyPeriodic_startInfo);
	libBPMcInterface_setDestroyCallBack(interface,libBPMcStrategyPeriodic_destory);
	libBPMcInterface_setSetRoleCallBack(interface,libBPMcStrategyPeriodic_setRole);
	libBPMcInterface_setSetLatencyConstraintsCallBack(interface,libBPMcStrategyPeriodic_setLatencyConstraints);
	libBPMcInterface_setConnectionUpdateCallBack(interface, libBPMcStrategyPeriodic_connectionUpdated);
	//Set all callbacks needed for parameter exchange
	libBPMcInterface_setGetIdentCallBack(interface,libBPMcStrategyPeriodic_getIdent);
	libBPMcInterface_setGetParamListCallBack(interface,libBPMcStrategyPeriodic_getParamList);
	libBPMcInterface_setGetParamCallBack(interface,libBPMcStrategyPeriodic_getParam);
	libBPMcInterface_setSetParamCallBack(interface,libBPMcStrategyPeriodic_setParam);
	LIBBPMC_DEBUG("Creating mutex\n");

	virThread_mutex_init(&periodicInternals->dataMutex,0);
	LIBBPMC_DEBUG("Creating sema\n");
	virThread_sema_init(&periodicInternals->semaUpdateOccured,1);
	LIBBPMC_DEBUG("Creating thread\n");
	virThread_thread_create(&periodicInternals->updateThread,libBPMcStrategyPeriodic_updateThread,interface,"strategyPeriodic",1000,0);
	periodicInternals->bufIn = NULL;
	periodicInternals->bufOut = NULL;
	periodicInternals->updatePeriodMs = LIBBPMC_STRATEGY_PERIODIC_INTERVAL_DEFAULT;
	periodicInternals->rateMin = 0;
	periodicInternals->rateMax = DBL_MAX;
	periodicInternals->smoothingFactor = LIBBPMC_STRATEGY_PERIODIC_SMOOTHING_FACTOR_DEFAULT;
	periodicInternals->smoothedRate = 0.0;
	periodicInternals->currentRate = 0.0;
	periodicInternals->currentConnectionInterval = 6.0;
	periodicInternals->lastTimeStamp.tv_sec = 0;
	periodicInternals->lastTimeStamp.tv_nsec = 0;
	periodicInternals->bytesWrittenSinceLastUpdate = 0;
	periodicInternals->useAvgTraffic = LIBBPMC_STRATEGY_PERIODIC_USE_AVERAGE_DEFAULT;
	periodicInternals->nWritesSinceLastUpdate = 0;
	periodicInternals->lastUpdateTimeStamp.tv_sec = 0;
	periodicInternals->lastUpdateTimeStamp.tv_nsec = 0;
	LIBBPMC_DEBUG("init done");

}

void libBPMcStrategyPeriodic_startInfo(libBPMcInterface* interface, int32_t tcInitial, double nSeqInitial){
	//printf("Periodic strategy initialized.");
	libBPMcStrageyPeriodicInternals *periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;
	periodicInternals->currentConnectionInterval = tcInitial;
}

void libBPMcStrategyPeriodic_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax){
	libBPMcStrageyPeriodicInternals *periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;
	periodicInternals->connectionIntervalMax = tcMax;
}

void libBPMcStrategyPeriodic_setTrafficBounds(libBPMcInterface* interface, double rateMin, double rateMax){
	libBPMcStrageyPeriodicInternals *periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;
	virThread_mutex_lock(&periodicInternals->dataMutex);
	periodicInternals->rateMin = rateMin;
	periodicInternals->rateMax = rateMax;
	virThread_mutex_unlock(&periodicInternals->dataMutex);

}

void libBPMcStrategyPeriodic_setRole(libBPMcInterface* interface, uint8_t masterOrSlave){
	libBPMcStrageyPeriodicInternals *periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;
	virThread_mutex_lock(&periodicInternals->dataMutex);
	periodicInternals->masterOrSlave = masterOrSlave;
	virThread_mutex_unlock(&periodicInternals->dataMutex);

}

void libBPMcStrategyPeriodic_addOut(libBPMcInterface* interface, libBPMcTraffic tr){
	double durationSinceLast;
	double tmp;
	if(interface->active){
		libBPMcStrageyPeriodicInternals *periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;

		virThread_mutex_lock(&periodicInternals->dataMutex);
		periodicInternals->bytesWrittenSinceLastUpdate += tr.nBytes;
		periodicInternals->nWritesSinceLastUpdate += 1;
		if(periodicInternals->lastTimeStamp.tv_sec != 0){
			durationSinceLast = libBPMc_Traffic_getTimeDifference(periodicInternals->lastTimeStamp,tr.time);
			periodicInternals->currentRate = (double)tr.nBytes/durationSinceLast;
			if(periodicInternals->smoothedRate == 0.0){
				periodicInternals->smoothedRate = periodicInternals->currentRate;
			}else{
				tmp  = periodicInternals->currentRate * periodicInternals->smoothingFactor + (1.0-periodicInternals->smoothingFactor)*periodicInternals->smoothedRate;
				periodicInternals->smoothedRate = tmp;
			}
		}
		//printf("Adding traffic: %d bytes at %llu seconds %llu nanoseconds, currentRate = %f b/s, smoothedRate = %f b/s, fillLevel = %d /%d bytes\n",tr.nBytes, tr.time.tv_sec, tr.time.tv_nsec,periodicInternals->currentRate,periodicInternals->smoothedRate, libBPMcBuffer_getBytesUsed(periodicInternals->bufOut),libBPMcBuffer_getSize(periodicInternals->bufOut));

		periodicInternals->lastTimeStamp = tr.time;
		virThread_mutex_unlock(&periodicInternals->dataMutex);

	}else{
		//printf("not adjusting connection interval - disactivated.\n");
	}
}


void libBPMcStrategyPeriodic_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut){
	libBPMcStrageyPeriodicInternals *periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;

	virThread_mutex_lock(&periodicInternals->dataMutex);
	periodicInternals->bufIn = bufIn;
	periodicInternals->bufOut = bufOut;
	virThread_mutex_unlock(&periodicInternals->dataMutex);

}

void libBPMcStrategyPeriodic_destory(libBPMcInterface* interface){
	libBPMcStrageyPeriodicInternals *periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;
	free(periodicInternals);
	virThread_sema_destroy(&periodicInternals->semaUpdateOccured);
	virThread_mutex_destroy(&periodicInternals->dataMutex);
	free(interface);
	//printf("destroy\n");
}


void libBPMcStrategyPeriodic_getIdent(libBPMcInterface* interface,char* ident){
	strcpy(ident, "libBPMcStrategyPeriodic");
}

char* libBPMcStrategyPeriodic_getParamList(libBPMcInterface* interface){

	char* paramList = (char*) malloc(1000);
	strcpy(paramList,"");
	char* rv;
	char lineBuf[256];
	sprintf(lineBuf,"%s|%s|%d|%d\n","updatePeriod","d",1,99999);
	strcat(paramList, lineBuf);
	rv = strdup(paramList);
	free(paramList);
	return rv;
}

uint8_t libBPMcStrategyPeriodic_setParam(libBPMcInterface* interface,const char* param, const char* value){
	libBPMcStrageyPeriodicInternals *periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;

	if(strcmp(param,"updatePeriod")==0){
		printf("setting update period to %d ms,\n",atoi(value));
		virThread_mutex_lock(&periodicInternals->dataMutex);

		periodicInternals->updatePeriodMs = atoi(value);
		virThread_mutex_unlock(&periodicInternals->dataMutex);

	}else{
		return 0;		//failure
	}
	return 1;
}

uint8_t libBPMcStrategyPeriodic_getParam(libBPMcInterface* interface,const char* param, char* value){
	libBPMcStrageyPeriodicInternals *periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;

	if(value == NULL){
		return 0;	//error
	}
	if(strcmp(param,"updatePeriod")==0){
		virThread_mutex_lock(&periodicInternals->dataMutex);

		sprintf(value, "%d",periodicInternals->updatePeriodMs);
		virThread_mutex_unlock(&periodicInternals->dataMutex);

	}else{
		return 0;		//failure
	}
	return 1;

}

void libBPMcStrategyPeriodic_setActivation(libBPMcInterface* interface, uint8_t active){
	interface->active = active;
}

void libBPMcStrategyPeriodic_connectionUpdated(libBPMcInterface* interface, uint32_t tcNew, uint32_t latencyNew, uint32_t timeoutNew){
	libBPMcStrageyPeriodicInternals* periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;
	if(interface->active){
		//printf("Connection parameters updated. New Interval: %d * 1.25 ms, new slave latency: %d, new supervision timeout %d * 1.25 ms\n", tcNew, latencyNew, timeoutNew);
		virThread_mutex_lock(&periodicInternals->dataMutex);
		periodicInternals->currentConnectionInterval = tcNew;
		virThread_mutex_unlock(&periodicInternals->dataMutex);

		virThread_sema_unlock(&periodicInternals->semaUpdateOccured);
	}
}

void libBPMcStrategyPeriodic_updateThread(void* param){
	libBPMcInterface* interface = (libBPMcInterface*) param;
	libBPMcStrageyPeriodicInternals* periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;
	struct timespec tBefore;
	struct timespec tAfter;
	double duration;
	int32_t timeLeft;
	uint32_t tcNew;
	double nSeqNew;
	//initial wait
	while(periodicInternals->smoothedRate == 0){
		virThread_delay_ms(periodicInternals->updatePeriodMs);
	}

	while(1){
		clock_gettime(CLOCK_MONOTONIC,&tBefore);
		if(periodicInternals->lastUpdateTimeStamp.tv_sec == 0){
			periodicInternals->lastUpdateTimeStamp = tBefore;
		}
		//printf("waiting for time left (%d ms)...\n",timeLeft);
		virThread_delay_ms(periodicInternals->updatePeriodMs);
		clock_gettime(CLOCK_MONOTONIC,&tAfter);
#if LIBBPMC_STRATEGY_PERIODIC_INTERVAL_BALANCING==1
		if(periodicInternals->nWritesSinceLastUpdate >= 2){

			duration = libBPMc_Traffic_getTimeDifference(periodicInternals->lastUpdateTimeStamp,periodicInternals->lastTimeStamp);

		}else{
			duration = libBPMc_Traffic_getTimeDifference(tBefore,tAfter);

		}
#else
		duration = libBPMc_Traffic_getTimeDifference(tBefore,tAfter);

#endif
		if(periodicInternals->useAvgTraffic == 0){

			tcNew = libBPMcStrategyPeriodic_rateToInterval(interface,periodicInternals->smoothedRate,&nSeqNew);
			//printf("current rate: %f b/s\t smoothed rate: %f b/s\n",periodicInternals->currentRate, periodicInternals->smoothedRate);
		}else{
			virThread_mutex_lock(&periodicInternals->dataMutex);
			tcNew = libBPMcStrategyPeriodic_rateToInterval(interface,(double)periodicInternals->bytesWrittenSinceLastUpdate/((double) duration),&nSeqNew);
			//printf("current rate: %f b/s\t interval avg rate: %f b/s\n",periodicInternals->currentRate, (double)periodicInternals->bytesWrittenSinceLastUpdate/duration);
			virThread_mutex_unlock(&periodicInternals->dataMutex);

		}
		virThread_mutex_lock(&periodicInternals->dataMutex);

		if(tcNew != periodicInternals->currentConnectionInterval){
			virThread_mutex_unlock(&periodicInternals->dataMutex);

			//printf("updating to: tcNew = %d, nSeqNew = %f\n",tcNew,nSeqNew);
			libBPMcInterface_callSetConIntCallBack(interface,tcNew,nSeqNew);
		}else{
			virThread_mutex_unlock(&periodicInternals->dataMutex);

		}

#if LIBBPMC_STRATEGY_PERIODIC_INTERVAL_BALANCING==1
		if(periodicInternals->nWritesSinceLastUpdate >= 2){

			periodicInternals->lastUpdateTimeStamp = periodicInternals->lastTimeStamp;
		}else{
			periodicInternals->lastUpdateTimeStamp = tAfter;

		}
#else
		periodicInternals->lastUpdateTimeStamp = tAfter;

#endif
		periodicInternals->bytesWrittenSinceLastUpdate = 0;

	}

}

uint32_t libBPMcStrategyPeriodic_rateToInterval(libBPMcInterface* interface, double rate, double* nSeqNew){
	libBPMcStrageyPeriodicInternals *periodicInternals = (libBPMcStrageyPeriodicInternals*) interface->internalData;

	if(nSeqNew != NULL){
		*nSeqNew = LIBBPMC_PARAM_BLE_NSEQ_MAX;
	}
	uint32_t interval = libBPMcRateCalc_rate2TcAvg(rate,*nSeqNew)  - periodicInternals->intervalSafetyMargin;
	if(interval > periodicInternals->connectionIntervalMax){
		interval = periodicInternals->connectionIntervalMax;
	}
	if(interval < 6){
		interval = 6;
	}
	return interval;
}
