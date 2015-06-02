/*
 * libBPMcStrategyLazyDecrease.c
 *
 *  Created on: 17.10.2013
 *      Author: kindt
 */


#include "../libBPMcInterface.h"
#include "libBPMcStrategyLazyDecrease.h"
#include "../libBPMcRateCalc.h"
#include "../libBPMcTraffic.h"

#include <float.h> //for DBL_MAX
#include "../libBPMcParams.h"


#if LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_POSIX
#include <time.h>
#elif LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <time.h>
#include <hhd_rtc.h>
#else
ERROR - WRONG PLATTFORM 		//compile time error
#endif
libBPMcInterface* globalLazyDecreaseIf;
void libBPMcStrategyLazyDecrease_init(libBPMcInterface* interface){
	libBPMcStrageyLazyDecreaseInternals *lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) malloc(sizeof(libBPMcStrageyLazyDecreaseInternals));
	globalLazyDecreaseIf = interface;
	interface->internalData = (void*) lazyDecreaseInternals;

	//Set all callbacks needed for power management
	libBPMcInterface_setActivationCallBack(interface,libBPMcStrategyLazyDecrease_setActivation);
	libBPMcInterface_setAddCallBackOut(interface,libBPMcStrategyLazyDecrease_addOut);
	libBPMcInterface_setSetTrafficBoundsCallBack(interface,libBPMcStrategyLazyDecrease_setTrafficBounds);
	libBPMcInterface_setSetBufferPtrCallBack(interface,libBPMcStrategyLazyDecrease_setBuffer);
	libBPMcInterface_setSetStartInfoCallBack(interface, libBPMcStrategyLazyDecrease_startInfo);
	libBPMcInterface_setDestroyCallBack(interface,libBPMcStrategyLazyDecrease_destory);
	libBPMcInterface_setSetRoleCallBack(interface,libBPMcStrategyLazyDecrease_setRole);
	libBPMcInterface_setSetLatencyConstraintsCallBack(interface,libBPMcStrategyLazyDecrease_setLatencyConstraints);
	libBPMcInterface_setConnectionUpdateCallBack(interface, libBPMcStrategyLazyDecrease_connectionUpdated);
	//Set all callbacks needed for parameter exchange
	libBPMcInterface_setGetIdentCallBack(interface,libBPMcStrategyLazyDecrease_getIdent);
	libBPMcInterface_setGetParamListCallBack(interface,libBPMcStrategyLazyDecrease_getParamList);
	libBPMcInterface_setGetParamCallBack(interface,libBPMcStrategyLazyDecrease_getParam);
	libBPMcInterface_setSetParamCallBack(interface,libBPMcStrategyLazyDecrease_setParam);
	virThread_mutex_init(&lazyDecreaseInternals->dataMutex,0);
	lazyDecreaseInternals->bufIn = NULL;
	lazyDecreaseInternals->bufOut = NULL;
	lazyDecreaseInternals->updatePeriodMs = LIBBPMC_STRATEGY_LAZY_DECREASE_INTERVAL_DEFAULT;
	lazyDecreaseInternals->rateMin = 0;
	lazyDecreaseInternals->rateMax = DBL_MAX;
	lazyDecreaseInternals->currentRate = 0.0;
	lazyDecreaseInternals->lastLoggedRate = 0;
	lazyDecreaseInternals->connectionIntervalDecreaseAbs = LIBBPMC_STRATEGY_LAZY_DECREASE_INTERVAL_REDUCTION_PER_STEP;
	lazyDecreaseInternals->connectionIntervalDecreaseRel = LIBBPMC_STRATEGY_LAZY_DECREASE_RELATIVE_RATIO;
	lazyDecreaseInternals->currentConnectionInterval = LIBBPMC_PARAMS_TC_START;
	lazyDecreaseInternals->lastTimeStamp.tv_sec = 0;
	lazyDecreaseInternals->lastTimeStamp.tv_nsec = 0;
	lazyDecreaseInternals->bytesWrittenSinceLastCheck = 0;
	lazyDecreaseInternals->nWritesSinceLastCheck = 0;
	lazyDecreaseInternals->lastCheckTimeStamp.tv_sec = 0;
	lazyDecreaseInternals->lastCheckTimeStamp.tv_nsec = 0;
	lazyDecreaseInternals->connectionIntervalMax = 3200;		//default max. connection interval- can be modified by setLantecyConstraintsCallBack()
	lazyDecreaseInternals->intervalSafetyMargin = LIBBPMC_STRATEGY_LAZY_DECREASE_INTERVAL_SAFETY_MARGIN_DEFAULT;
	lazyDecreaseInternals->useRelativeDecrease = LIBBPMC_STRATEGY_LAZY_DECREASE_USE_RELATIVE_DEC;
	lazyDecreaseInternals->updateState = 0;
	virThread_timer_create(&lazyDecreaseInternals->updateTimer,lazyDecreaseInternals->updatePeriodMs*1000,libBPMcStrategyLazyDecrease_updateTimerCallback,(void*) interface,0);
	//virThread_timer_stop(&lazyDecreaseInternals->updateTimer);
	//printf("initialized.\n");
}

void libBPMcStrategyLazyDecrease_startInfo(libBPMcInterface* interface, int32_t tcInitial, double nSeqInitial){
	libBPMcStrageyLazyDecreaseInternals *lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;
	lazyDecreaseInternals->currentConnectionInterval = tcInitial;
}

void libBPMcStrategyLazyDecrease_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax){
	libBPMcStrageyLazyDecreaseInternals *lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;
	lazyDecreaseInternals->connectionIntervalMax = tcMax;
	lazyDecreaseInternals->connectionIntervalMin = tcMin;
}

void libBPMcStrategyLazyDecrease_setTrafficBounds(libBPMcInterface* interface, double rateMin, double rateMax){
	libBPMcStrageyLazyDecreaseInternals *lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;
	virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);
	lazyDecreaseInternals->rateMin = rateMin;
	lazyDecreaseInternals->rateMax = rateMax;
	virThread_mutex_unlock(&lazyDecreaseInternals->dataMutex);

}

void libBPMcStrategyLazyDecrease_setRole(libBPMcInterface* interface, uint8_t masterOrSlave){
	libBPMcStrageyLazyDecreaseInternals *lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;
	virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);
	lazyDecreaseInternals->masterOrSlave = masterOrSlave;
	virThread_mutex_unlock(&lazyDecreaseInternals->dataMutex);

}

void libBPMcStrategyLazyDecrease_addOut(libBPMcInterface* interface, libBPMcTraffic tr){
	double durationSinceLast;
	double tmp;
	if(interface->active){
		libBPMcStrageyLazyDecreaseInternals *lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;
		double currentOutputRate = libBPMcRateCalc_Tc2RateAvg(interface->tcScheduled, interface->nSeqScheduled);
	//	printf("tc = %d, nSeq = %f, oR = %f\n",interface->tcScheduled,interface->nSeqScheduled, libBPMcRateCalc_Tc2RateAvg(interface->tcScheduled, interface->nSeqScheduled));

		virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);

		lazyDecreaseInternals->bytesWrittenSinceLastCheck += tr.nBytes;
		lazyDecreaseInternals->nWritesSinceLastCheck += 1;
		if(lazyDecreaseInternals->lastTimeStamp.tv_sec != 0){
			lazyDecreaseInternals->currentRate = tr.nBytes / libBPMc_Traffic_getTimeDifference(lazyDecreaseInternals->lastCheckTimeStamp, tr.time);
			durationSinceLast = libBPMc_Traffic_getTimeDifference(lazyDecreaseInternals->lastTimeStamp,tr.time);
			lazyDecreaseInternals->currentRate = (double)tr.nBytes/durationSinceLast;
		//	printf("add: rate = %f by/s - output: %f by/s\n",lazyDecreaseInternals->currentRate, currentOutputRate);
			if(lazyDecreaseInternals->currentRate > currentOutputRate){
		//		printf("Updating conInt to %d\n",interface->tcMin);
				if(lazyDecreaseInternals->updateState == 0){
					virThread_mutex_unlock(&lazyDecreaseInternals->dataMutex);
					if(libBPMcInterface_callSetConIntCallBack(interface,interface->tcMin,interface->nSeqCurrent)){
						virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);
						lazyDecreaseInternals->updateState = 1;
						lazyDecreaseInternals->bytesWrittenSinceLastCheck = 0;
						lazyDecreaseInternals->nWritesSinceLastCheck = 0;
						lazyDecreaseInternals->lastCheckTimeStamp = tr.time;
						virThread_mutex_unlock(&lazyDecreaseInternals->dataMutex);

						virThread_timer_start(&lazyDecreaseInternals->updateTimer,lazyDecreaseInternals->updatePeriodMs * 1000,0);
						virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);

					}else{
						virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);
					}
				}


			}

		}

		lazyDecreaseInternals->lastTimeStamp = tr.time;
		virThread_mutex_unlock(&lazyDecreaseInternals->dataMutex);
	}else{
		//printf("not adjusting connection interval - disactivated.\n");
	}
}


void libBPMcStrategyLazyDecrease_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut){
	libBPMcStrageyLazyDecreaseInternals *lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;

	virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);
	lazyDecreaseInternals->bufIn = bufIn;
	lazyDecreaseInternals->bufOut = bufOut;
	virThread_mutex_unlock(&lazyDecreaseInternals->dataMutex);

}

void libBPMcStrategyLazyDecrease_destory(libBPMcInterface* interface){
	libBPMcStrageyLazyDecreaseInternals *lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;
	free(lazyDecreaseInternals);
	virThread_mutex_destroy(&lazyDecreaseInternals->dataMutex);
	free(interface);
	//printf("destroy\n");
}


void libBPMcStrategyLazyDecrease_getIdent(libBPMcInterface* interface,char* ident){
	strcpy(ident, "libBPMcStrategyLazyDecrease");
}

char* libBPMcStrategyLazyDecrease_getParamList(libBPMcInterface* interface){

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

uint8_t libBPMcStrategyLazyDecrease_setParam(libBPMcInterface* interface,const char* param, const char* value){
	libBPMcStrageyLazyDecreaseInternals *lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;

	if(strcmp(param,"updatePeriod")==0){
		//printf("setting update period to %d ms,\n",atoi(value));
		virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);

		lazyDecreaseInternals->updatePeriodMs = atoi(value);
		virThread_mutex_unlock(&lazyDecreaseInternals->dataMutex);

	}else{
		return 0;		//failure
	}
	return 1;
}

uint8_t libBPMcStrategyLazyDecrease_getParam(libBPMcInterface* interface,const char* param, char* value){
	libBPMcStrageyLazyDecreaseInternals *lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;

	if(value == NULL){
		return 0;	//error
	}
	if(strcmp(param,"updatePeriod")==0){
		virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);

		//sprintf(value, "%d",lazyDecreaseInternals->updatePeriodMs);
		virThread_mutex_unlock(&lazyDecreaseInternals->dataMutex);

	}else{
		return 0;		//failure
	}
	return 1;

}

void libBPMcStrategyLazyDecrease_setActivation(libBPMcInterface* interface, uint8_t active){
	libBPMcStrageyLazyDecreaseInternals* lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;
	interface->active = active;
	if(active && lazyDecreaseInternals->masterOrSlave){
		virThread_timer_start(&lazyDecreaseInternals->updateTimer,lazyDecreaseInternals->updatePeriodMs * 1000,0);
		//printf("acrivated lazyDec Strategy.\n");

	}else{
		virThread_timer_stop(&lazyDecreaseInternals->updateTimer);
	}

}

void libBPMcStrategyLazyDecrease_connectionUpdated(libBPMcInterface* interface, uint32_t tcNew, uint32_t latencyNew, uint32_t timeoutNew){
	libBPMcStrageyLazyDecreaseInternals* lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;
	if(interface->active){
		//printf("Connection parameters updated. New Interval: %d * 1.25 ms, new slave latency: %d, new supervision timeout %d * 1.25 ms\n", tcNew, latencyNew, timeoutNew);
		virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);
		lazyDecreaseInternals->currentConnectionInterval = tcNew;
		lazyDecreaseInternals->updateState = 0;
		virThread_mutex_unlock(&lazyDecreaseInternals->dataMutex);

	}
}

void libBPMcStrategyLazyDecrease_updateTimerCallback(void* param){
	libBPMcInterface* interface = (libBPMcInterface*) globalLazyDecreaseIf;
	libBPMcStrageyLazyDecreaseInternals* lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;
	struct timespec tNow;
	uint32_t reducedTc;
	double timePassedSinceLastCheck;
	double duration;
	double reducedOutputRate;
	double currentOutputRate;
	double avgRate;

	clock_gettime(CLOCK_MONOTONIC,&tNow);
	virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);

	if(lazyDecreaseInternals->lastCheckTimeStamp.tv_sec > 0){
		timePassedSinceLastCheck = libBPMc_Traffic_getTimeDifference(lazyDecreaseInternals->lastCheckTimeStamp,tNow);

		//printf("timePassedSinceLastCheck = %f\n",timePassedSinceLastCheck);
		currentOutputRate = libBPMcRateCalc_Tc2RateAvg(interface->tcScheduled, interface->nSeqScheduled);
	//	printf("or = %f, tc=%d, nSeq=%f, oR= %f\n",currentOutputRate, interface->tcScheduled, interface->nSeqScheduled,libBPMcRateCalc_Tc2RateAvg(interface->tcScheduled, interface->nSeqScheduled));

		if(lazyDecreaseInternals->useRelativeDecrease){
			reducedOutputRate = currentOutputRate - lazyDecreaseInternals->connectionIntervalDecreaseRel * (lazyDecreaseInternals->rateMax - libBPMcRateCalc_Tc2RateAvg(lazyDecreaseInternals->connectionIntervalMax,interface->nSeqScheduled));
		}else{
			printf("absolute decrease: not implemented!\n");
			while(1){
				asm("nop");
			}
		}

#if LIBBPMC_STRATEGY_LAZY_DECREASE_INTERVAL_BALANCING==1
		if(lazyDecreaseInternals->nWritesSinceLastCheck >= 2){
			duration = libBPMc_Traffic_getTimeDifference(lazyDecreaseInternals->lastCheckTimeStamp,lazyDecreaseInternals->lastTimeStamp);
			//printf("reduced duration: %f\n",duration);
		}else{
			duration = timePassedSinceLastCheck;

		}
#else
		duration = timePassedSinceLastCheck;

#endif
		//printf("nWrites = %u, bytesWritten: %u , duration: %f\n",lazyDecreaseInternals->nWritesSinceLastCheck, lazyDecreaseInternals->bytesWrittenSinceLastCheck,duration);
		avgRate = lazyDecreaseInternals->bytesWrittenSinceLastCheck / duration;

//		printf("avg rate: %f - output rate: %f, reduced output rate: %f\n",avgRate,currentOutputRate,reducedOutputRate);
		if(avgRate < reducedOutputRate){
			reducedTc = libBPMcStrategyLazyDecrease_rateToInterval(interface,reducedOutputRate,interface->nSeqScheduled);
			if(reducedTc == interface->tcScheduled){
				reducedTc++;
			}
	//		printf("reduced TC (raw): %u",reducedTc);
			if(reducedTc > interface->tcMax){
				reducedTc = interface->tcMax;
			}
	//		printf("reducing Tc: %d->%d\n",interface->tcScheduled, reducedTc);
			virThread_mutex_unlock(&lazyDecreaseInternals->dataMutex);
			if(lazyDecreaseInternals->updateState == 0){
				if(libBPMcInterface_callSetConIntCallBack(interface,reducedTc,interface->nSeqScheduled)){
					lazyDecreaseInternals->updateState = 1;
				}
			}
			virThread_mutex_lock(&lazyDecreaseInternals->dataMutex);

		}
	}
#if LIBBPMC_STRATEGY_LAZY_DECREASE_INTERVAL_BALANCING==1
	lazyDecreaseInternals->lastCheckTimeStamp = lazyDecreaseInternals->lastTimeStamp;
#else
	lazyDecreaseInternals->lastCheckTimeStamp = tNow;
#endif
	lazyDecreaseInternals->bytesWrittenSinceLastCheck = 0;
	lazyDecreaseInternals->nWritesSinceLastCheck  = 0;
	virThread_mutex_unlock(&lazyDecreaseInternals->dataMutex);

	virThread_timer_start(&lazyDecreaseInternals->updateTimer,lazyDecreaseInternals->updatePeriodMs * 1000,0);


}

uint32_t libBPMcStrategyLazyDecrease_rateToInterval(libBPMcInterface* interface, double rate, double nSeqNew){
	libBPMcStrageyLazyDecreaseInternals *lazyDecreaseInternals = (libBPMcStrageyLazyDecreaseInternals*) interface->internalData;
	//printf("rateToI: rate = %f, nSeqNew = %f",rate,nSeqNew);
	uint32_t interval = libBPMcRateCalc_rate2TcAvg(rate,nSeqNew)  - lazyDecreaseInternals->intervalSafetyMargin;
	if(interval > lazyDecreaseInternals->connectionIntervalMax){
		interval = lazyDecreaseInternals->connectionIntervalMax;
	}
	if(interval < 6){
		interval = 6;
	}
	return interval;
}
