/*
  * libBPMcStrategyBuffer.c
 *
 *  Created on: 17.10.2013
 *      Author: kindt
 */



#include "libBPMcStrategyBuffer.h"
#include "../libBPMcDebug.h"
//#include <bleEnergyModel/src/ble_model.h>
#include "../../../bleEnergyModel/src/ble_model_connected.h"
#include "../../../bleEnergyModel/src/ble_model_connection_establishment.h"
#include "../../../bleEnergyModel/src/ble_model_scanning.h"
#include "../../../rcs_dsp/rcs_dsp_ll.h"
#include "../../../virThread/virThread.h"
#include "../libBPMcBuffer.h"
#include "../libBPMcInterface.h"
#include "../libBPMcParams.h"
//#include "../libBPMcPlatform.h"
#include "../libBPMcTraffic.h"

#if LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_POSIX
#include <time.h>
#elif LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <time.h>
#include <hhd_rtc.h>
#include <hhd_bleSLI.h>

#else
ERROR - WRONG PLATTFORM 		//compile time error
#endif
void libBPMcStrategyBuffer_init(libBPMcInterface* interface){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) malloc(sizeof(libBPMcStrageyBufferInternals));
	interface->internalData = (void*) bufferInternals;
	bufferInternals->connectionIntervalMax = 3200;		//default max. connection interval- can be modified by setLantecyConstraintsCallBack()


	//Set all callbacks needed for power management
	libBPMcInterface_setActivationCallBack(interface,libBPMcStrategyBuffer_setActivation);
	libBPMcInterface_setAddCallBackOut(interface,libBPMcStrategyBuffer_addOut);
	libBPMcInterface_setReadCallBackOut(interface,libBPMcStrategyBuffer_readOut);
	libBPMcInterface_setSetBufferPtrCallBack(interface,libBPMcStrategyBuffer_setBuffer);
	libBPMcInterface_setSetStartInfoCallBack(interface, libBPMcStrategyBuffer_startInfo);
	libBPMcInterface_setDestroyCallBack(interface,libBPMcStrategyBuffer_destory);
	libBPMcInterface_setSetRoleCallBack(interface,libBPMcStrategyBuffer_setRole);
	libBPMcInterface_setSetLatencyConstraintsCallBack(interface,libBPMcStrategyBuffer_setLatencyConstraints);
	libBPMcInterface_setSetTrafficBoundsCallBack(interface,libBPMcStrategyBuffer_setTrafficBounds);
	libBPMcInterface_setConnectionUpdateCallBack(interface,libBPMcStrategyBuffer_connectionUpdated);
	//Set all callbacks needed for parameter exchange
	libBPMcInterface_setGetIdentCallBack(interface,libBPMcStrategyBuffer_getIdent);
	libBPMcInterface_setGetParamListCallBack(interface,libBPMcStrategyBuffer_getParamList);
	libBPMcInterface_setGetParamCallBack(interface,libBPMcStrategyBuffer_getParam);
	libBPMcInterface_setSetParamCallBack(interface,libBPMcStrategyBuffer_setParam);
	bufferInternals->bufIn = NULL;
	bufferInternals->bufOut = NULL;
	bufferInternals->currentConnectionInterval = 6;
	bufferInternals->currentNSeq = LIBBPMC_PARAM_BLE_NSEQ_MAX;
	bufferInternals->nextNSeq = bufferInternals->currentNSeq;
	bufferInternals->levelIncPercentage = atof(LIBBPMC_STRATEGY_BUFFER_LEVELINC_PERCENTAGE_DEFAULT);
	bufferInternals->state = LIBBPMC_STRATEGY_BUFFER_STATE_NORMAL;
	bufferInternals->avgRate = 0;
	bufferInternals->smoothingFactor = LIBBPMC_STRATEGY_BUFFER_SMOOTHING_FACTOR_DEFAULT;
	bufferInternals->tLastAdd.tv_sec = 0;
	bufferInternals->tLastAdd.tv_nsec = 0;

	bufferInternals->tTask.tv_sec = 0;
	bufferInternals->tTask.tv_nsec = 0;
	bufferInternals->decreaseSuppression = LIBBPMC_STRATEGY_BUFFER_DECREASE_SUPPPRESSION_INTERVAL_STEPS;
	bufferInternals->task = LIBBPMC_STRATEGY_BUFFER_TASK_CHECK_OVERRUN;
	bufferInternals->intervalSafetyMargin = LIBBPMC_STRATEGY_BUFFER_INTERVAL_SAFETY_MARGIN_DEFAULT;
	virThread_mutex_init(&(bufferInternals->mutex),0);
	virThread_sema_init(&(bufferInternals->triggerSema),0);
	virThread_thread_create(&(bufferInternals->thread),libBPMcStrategyBuffer_bufferThread,interface,"strategyBuffer",1000,0);

}

void libBPMcStrategyBuffer_startInfo(libBPMcInterface* interface, int32_t tcInitial,double nSeqInitial){
	LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("Start Info received.");
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	virThread_mutex_lock(&bufferInternals->mutex);
	bufferInternals->currentConnectionInterval = tcInitial;
	bufferInternals->currentNSeq = nSeqInitial;
	virThread_mutex_unlock(&bufferInternals->mutex);

}

void libBPMcStrategyBuffer_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	virThread_mutex_lock(&bufferInternals->mutex);
	bufferInternals->connectionIntervalMax = tcMax;
	LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("=> setLatencyConstraint: Max. ConInt: %d-%d\n",bufferInternals->connectionIntervalMax,tcMax);
	virThread_mutex_unlock(&bufferInternals->mutex);

}

void libBPMcStrategyBuffer_setTrafficBounds(libBPMcInterface* interface, double rateMin, double rateMax){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	virThread_mutex_lock(&bufferInternals->mutex);
	bufferInternals->rateMin = rateMin;
	bufferInternals->rateMax = rateMax;
	LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("traffic bounds: [%f - %f]\n",rateMin, rateMax);
	virThread_mutex_unlock(&bufferInternals->mutex);

}

void libBPMcStrategyBuffer_setRole(libBPMcInterface* interface, uint8_t masterOrSlave){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	virThread_mutex_lock(&bufferInternals->mutex);
	bufferInternals->masterOrSlave = masterOrSlave;
	virThread_mutex_unlock(&bufferInternals->mutex);

}

void libBPMcStrategyBuffer_addOut(libBPMcInterface* interface, libBPMcTraffic tr){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	double currentRate;
	uint32_t bufferLevel = libBPMcBuffer_getBytesUsed(bufferInternals->bufOut);
	uint32_t tcNew,nSeqNew;
	struct timespec tNow = tr.time;

	if(interface->active){
		bufferInternals->bytesWritten = tr.nBytes;
		bufferInternals->currentRate = (double)bufferInternals->bytesWritten / libBPMc_Traffic_getTimeDifference(bufferInternals->tLastAdd,tNow);
		bufferInternals->tLastAdd = tNow;
		bufferInternals->bytesWrittenSinceLastUpdateScheduled += tr.nBytes;
		if(bufferInternals->state == LIBBPMC_STRATEGY_BUFFER_STATE_NORMAL){
			bufferInternals->task = LIBBPMC_STRATEGY_BUFFER_TASK_CHECK_OVERRUN;
			bufferInternals->tTask = tNow;
			virThread_sema_unlock(&bufferInternals->triggerSema);

		}
	}else{
		LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("not increasing connection interval - disactivated.\n");

	}
}



void libBPMcStrategyBuffer_readOut(libBPMcInterface* interface, libBPMcTraffic tr){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	double avgRate;
	uint32_t tcNew;
	uint32_t bufferLevel = libBPMcBuffer_getBytesUsed(bufferInternals->bufOut);
	double nSeqNew;
	if(interface->active){
		if(bufferInternals->state == LIBBPMC_STRATEGY_BUFFER_STATE_NORMAL){
			bufferInternals->task = LIBBPMC_STRATEGY_BUFFER_TASK_CHECK_UNDERRUN;
			virThread_sema_unlock(&bufferInternals->triggerSema);
		}else{
			LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("not checking for underrung - wrong state.\n");
		}
	}else{

		LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("not decreasing connection interval - disactivated.\n");
	}
}

void libBPMcStrategyBuffer_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	virThread_mutex_lock(&bufferInternals->mutex);

	bufferInternals->bufIn = bufIn;
	bufferInternals->bufOut = bufOut;
	virThread_mutex_unlock(&bufferInternals->mutex);

}

void libBPMcStrategyBuffer_destory(libBPMcInterface* interface){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	virThread_mutex_destroy(&bufferInternals->mutex);
	free(bufferInternals);
	free(interface);
}


void libBPMcStrategyBuffer_getIdent(libBPMcInterface* interface,char* ident){
	strcpy(ident, "libBPMcStrategyBuffer");
}

char* libBPMcStrategyBuffer_getParamList(libBPMcInterface* interface){

	char* paramList = (char*) malloc(1000);
	strcpy(paramList,"");
	char* rv;
	char lineBuf[256];
	sprintf(lineBuf,"%s|%s|%d|%f\n","levelIncPercentage","f",0,0.5);
	strcat(paramList, lineBuf);
	rv = strdup(paramList);
	free(paramList);
	return rv;
}

uint8_t libBPMcStrategyBuffer_setParam(libBPMcInterface* interface,const char* param, const char* value){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;

	if(strcmp(param,"levelIncPercentage")==0){

		LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("setting rate increase percentage to %f,\n",atof(value));
		bufferInternals->levelIncPercentage = atof(value);
	}else if(strcmp(param, "dummyParameter")==0){
						//do something fancy
	}else{
		return 0;		//failure
	}
	return 1;
}

uint8_t libBPMcStrategyBuffer_getParam(libBPMcInterface* interface,const char* param, char* value){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;

	if(value == NULL){
		return 0;	//error
	}
	if(strcmp(param,"levelIncPercentage")==0){

		sprintf(value, "%f",bufferInternals->levelIncPercentage);
	}else{
		return 0;		//failure
	}
	return 1;

}

void libBPMcStrategyBuffer_setActivation(libBPMcInterface* interface, uint8_t active){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	libBPMcStrategyBuffer_calcInitialLevels(interface);
	bufferInternals->state = LIBBPMC_STRATEGY_BUFFER_STATE_NORMAL;
	clock_gettime(CLOCK_MONOTONIC,&bufferInternals->tLastUpdate);
	bufferInternals->dNextUpdateAllowed = 0;
	bufferInternals->levelLastUpdate = libBPMcBuffer_getBytesUsed(bufferInternals->bufOut);
	interface->active = active;

}

void libBPMcStrategyBuffer_connectionUpdated(libBPMcInterface* interface, uint32_t tcNew, uint32_t latencyNew, uint32_t timeoutNew){

	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	if(interface->active){
#if LIBBPMC_STRATEGY_BUFFER_INFOMESSAGES==1

		LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("Connection parameters updated. New Interval: %d * 1.25 ms, new slave latency: %d, new supervision timeout %d * 1.25 ms\n", tcNew, latencyNew, timeoutNew);
#endif
	}
	clock_gettime(CLOCK_MONOTONIC,&bufferInternals->tLastUpdate);
	virThread_mutex_lock(&bufferInternals->mutex);

	bufferInternals->levelLastUpdate = libBPMcBuffer_getBytesUsed(bufferInternals->bufOut);

	//rate increase took place
	if(bufferInternals->state == LIBBPMC_STRATEGY_BUFFER_STATE_WAIT_FOR_INCREASE){
	//	if(bufferInternals->levelLastUpdate > bufferInternals->levelIncThresholdIncrease){
			bufferInternals->levelIncThreshold = bufferInternals->levelLastUpdate;

	//		LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("adding safety bytes: %d\n",libBPMcStrategyBuffer_calcBytesOfUpdateLoss(interface,bufferInternals->avgRate,bufferInternals->currentConnectionInterval,0.5));		//2do fixme
			bufferInternals->levelIncThreshold += bufferInternals->levelIncThresholdIncrease;

			if(bufferInternals->levelIncThreshold > libBPMcBuffer_getSize(bufferInternals->bufOut)){
				bufferInternals->levelIncThreshold = libBPMcBuffer_getSize(bufferInternals->bufOut);
			}
  }


	//rate decrease took place
/*
	if(bufferInternals->state == LIBBPMC_STRATEGY_BUFFER_STATE_WAIT_FOR_DECREASE){
		if(bufferInternals->levelLastUpdate < bufferInternals->levelDecThresholdStart){
			bufferInternals->levelDecThreshold = bufferInternals->levelLastUpdate;
			fprintf(2,stderr,"substracting safety bytes: %d\n",libBPMcStrategyBuffer_calcBytesOfUpdateLoss(interface,bufferInternals->avgRate,bufferInternals->currentConnectionInterval,0.5));		//2do fixme
			if(bufferInternals->levelDecThreshold > libBPMcStrategyBuffer_calcBytesOfUpdateLoss(interface,bufferInternals->avgRate,bufferInternals->currentConnectionInterval,0.5)){
				bufferInternals->levelDecThreshold -= libBPMcStrategyBuffer_calcBytesOfUpdateLoss(interface,bufferInternals->avgRate,bufferInternals->currentConnectionInterval,0.5);		//2do fixme
			}else{
				bufferInternals->levelDecThreshold = 0;
			}
		}else{
			bufferInternals->levelDecThreshold = bufferInternals->levelDecThresholdStart;
		}
	}
*/

	if(LIBBPMC_STRATEGY_BUFFER_USE_PENALTY_RATEDEC){
		bufferInternals->dNextUpdateAllowed = libBPMcStrategyBuffer_getPenaltyTime(interface,bufferInternals->currentConnectionInterval,tcNew,bufferInternals->currentNSeq, bufferInternals->nextNSeq,bufferInternals->bufOut);
		LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("Blocking Time: %f s\n",bufferInternals->dNextUpdateAllowed);
	}

	bufferInternals->currentConnectionInterval = tcNew;
	bufferInternals->currentNSeq  = bufferInternals->nextNSeq;
	bufferInternals->state = LIBBPMC_STRATEGY_BUFFER_STATE_NORMAL;


	//if(libBPMcBuffer_getBytesUsed(bufferInternals->bufOut) <= LIBBPMC_PARAMS_IO_R_LENGTH){
		LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("adding fake write due to buffer emptyness\n");
		bufferInternals->task = LIBBPMC_STRATEGY_BUFFER_TASK_CHECK_UNDERRUN;
		virThread_mutex_unlock(&bufferInternals->mutex);
		virThread_sema_unlock(&bufferInternals->triggerSema);
	//}else{
	//	virThread_mutex_unlock(&bufferInternals->mutex);

	//}

}

uint8_t libBPMcStrategyBuffer_calcInitialLevels(libBPMcInterface* interface){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	if(bufferInternals->bufOut == NULL){
		return 0;
	}
	LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("maxRate: %f\n",bufferInternals->rateMax);
	uint32_t bufFillState = libBPMcBuffer_getBytesUsed(bufferInternals->bufOut);
	LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("Initial Buffer Fill: %d\n",bufFillState);
#if LIBBPMC_STRATEGY_BUFFER_USE_WHOLE_BUFFER==1
	bufferInternals->levelEmergency = libBPMcBuffer_getSize(bufferInternals->bufOut) - LIBBPMC_PARAMS_IO_R_LENGTH;
#else
	bufferInternals->levelEmergency = round(libBPMcBuffer_getSize(bufferInternals->bufOut) - LIBBPMC_PARAMS_IO_R_LENGTH - LIBBPMC_PARAM_WORST_CASE_WINDOW * bufferInternals->rateMax - 2.0 *  LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * bufferInternals->connectionIntervalMax * 0.00125 * (bufferInternals->rateMax - 20.0*bufferInternals->currentNSeq * (1.0- LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION)/(bufferInternals->connectionIntervalMax*0.00125)));
#endif
	bufferInternals->levelIncThresholdIncrease = (double) bufferInternals->levelEmergency * bufferInternals->levelIncPercentage;
	bufferInternals->levelIncThreshold = bufferInternals->levelIncThresholdIncrease + LIBBPMC_PARAMS_IO_R_LENGTH;
	//printf("---emergencyLevel: %d - incThreshold = %d, levelIncThresholdIncrease = %d\n",bufferInternals->levelEmergency,bufferInternals->levelIncThreshold,bufferInternals->levelIncThresholdIncrease);
	#if LIBBPMC_STRATEGY_BUFFER_INFOMESSAGES==1

	LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("Emergency Level: %d - increase level @ start: %d, increase interval:\n",bufferInternals->levelEmergency,bufferInternals->levelIncThreshold, bufferInternals->levelIncThresholdIncrease);
#endif
	return 1;
}


uint32_t libBPMcStrategyBuffer_rateToInterval(libBPMcInterface* interface, double rate, double nSeqNew){
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;

	LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("--- rateNew: %f,nSeqNew = %f - max interval: %d,\n",rate, nSeqNew,bufferInternals->connectionIntervalMax);

	uint32_t interval = (((1.0- LIBBPMC_PARAM_AVERAGE_THROUGHPUT_DEGENERATION_RELATIVE) * 20.0*(double) nSeqNew)/(rate*0.00125)) - bufferInternals->intervalSafetyMargin;
	LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("new Interval raw: %d\n",interval);
	if(interval > bufferInternals->connectionIntervalMax){
		interval = bufferInternals->connectionIntervalMax;
	}
	if(interval < 6){
		interval = 6;
	}
	LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("new Interval final (after range corrections): %d\n",interval);

	return interval;
}

libBPMcStrategyBuffer_bufferThread(void*param){
	libBPMcInterface* interface = (libBPMcInterface*) param;
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;
	libBPMcTraffic fake;
	double nSeqNew = LIBBPMC_PARAM_BLE_NSEQ_MAX;
	uint32_t tcNew;

	double currentRate;
	uint32_t bufferLevel;
	struct timespec tNow;
		while(1){
//		fprintf(stderr,"waiting for sema...\n");



			//Wait for trigger sema. When triggered, there is either a chance for over- or underrun
			virThread_sema_lock(&bufferInternals->triggerSema);

			//get current buffer level
			bufferLevel = libBPMcBuffer_getBytesUsed(bufferInternals->bufOut);
			currentRate = bufferInternals->currentRate;
			//Check for overrun
			if(bufferInternals->task == LIBBPMC_STRATEGY_BUFFER_TASK_CHECK_OVERRUN){
				if(bufferInternals->tLastAdd.tv_sec != NULL){

					//compute smoothened current rate
					//exponential smoothing
					if(bufferInternals->avgRate == 0){
						bufferInternals->avgRate = currentRate;
					}else{
						bufferInternals->avgRate = bufferInternals->smoothingFactor * currentRate + (1 - bufferInternals->smoothingFactor)*bufferInternals->avgRate;
					}

					LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("fillLevel: %d\n",libBPMcBuffer_getBytesUsed(bufferInternals->bufOut));
					LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("[addO, check bufOverrun] rate: %f b/s\t fillLevel: %d   empty: %d  inc: %d/%d  em: %d   max %d\n",bufferInternals->avgRate,libBPMcBuffer_getBytesUsed(bufferInternals->bufOut),libBPMcBuffer_getBytesLeft(bufferInternals->bufOut), bufferInternals->levelIncThreshold,bufferInternals->levelIncThresholdIncrease,bufferInternals->levelEmergency, libBPMcBuffer_getSize(bufferInternals->bufOut));


					//are we currently not updating the connetion interval. In all other states, don't do anything!
					if(bufferInternals->state == LIBBPMC_STRATEGY_BUFFER_STATE_NORMAL){

						//check if the level is above the levelIncThreshold. This is allways levelIncThresholdIncrease above the state the last increase occurred, or levelIncThresholdIncrease if it has been fallen below levelIncThresholdIncrease since the last rate increase
						if(bufferLevel > bufferInternals->levelIncThreshold){

							//decrease connection interval
							virThread_mutex_lock(&bufferInternals->mutex);
							tcNew = libBPMcStrategyBuffer_rateToInterval(interface,bufferInternals->avgRate,nSeqNew);
							LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE(" [RateInc] Rate: %f by/s => tcNew = %d\n",bufferInternals->avgRate,tcNew);

							if(tcNew < bufferInternals->currentConnectionInterval){

								LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("Increasing connection interval to %d to match avg rate %f\n",tcNew, bufferInternals->avgRate);
								if(libBPMcInterface_callSetConIntCallBack(interface,tcNew,nSeqNew)){

									clock_gettime(CLOCK_MONOTONIC,&bufferInternals->tUpdateScheduled);
									bufferInternals->state = LIBBPMC_STRATEGY_BUFFER_STATE_WAIT_FOR_INCREASE;
									bufferInternals->nextNSeq = nSeqNew;
									bufferInternals->bytesWrittenSinceLastUpdateScheduled = 0;
									LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE(" - success\n");
								}else{
#if LIBBPMC_STRATEGY_BUFFER_INFOMESSAGES==1

									LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE(" - failed.\n");
#endif
								}
							}
							virThread_mutex_unlock(&bufferInternals->mutex);

						}else{


						LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("[RateInc] New connection interval would be larger then old one > not decreasing connection interval (old.: %d - new: %d).\n",bufferInternals->currentConnectionInterval, tcNew);

						}
					}else{

						LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("Wrong state to update (something else seems to be in progress).\n");
					}
				}



		//Check for buffer underrun
			}else if(bufferInternals->task == LIBBPMC_STRATEGY_BUFFER_TASK_CHECK_UNDERRUN){

				//Only act if no update is currently scheduled
				if(bufferInternals->state == LIBBPMC_STRATEGY_BUFFER_STATE_NORMAL){

							//check if we're below the lower limit which is always LIBBPMC_PARAMS_IO_R_LENGTH
							if(bufferLevel <= LIBBPMC_PARAMS_IO_R_LENGTH){
								//decrease connection interval
								virThread_mutex_lock(&bufferInternals->mutex);

								tcNew = libBPMcStrategyBuffer_rateToInterval(interface,bufferInternals->avgRate,nSeqNew);
								LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE(" [RateDec] Rate: %f by/s with nSeq = %f => tcNew = %d\n",bufferInternals->avgRate,nSeqNew,tcNew);

								if(tcNew > bufferInternals->currentConnectionInterval){
									clock_gettime(CLOCK_MONOTONIC,&tNow);
									if(!(LIBBPMC_STRATEGY_BUFFER_USE_PENALTY_RATEDEC) || (libBPMc_Traffic_getTimeDifference(bufferInternals->tLastUpdate,tNow) > bufferInternals->dNextUpdateAllowed)){
#if LIBBPMC_STRATEGY_BUFFER_INFOMESSAGES==1
										LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("Decreasing connection interval to %d to match avg rate %f\n",tcNew, bufferInternals->avgRate);
#endif
										if(libBPMcInterface_callSetConIntCallBack(interface,tcNew,nSeqNew)){
											bufferInternals->tUpdateScheduled = tNow;
											bufferInternals->bytesWrittenSinceLastUpdateScheduled = 0;
											//reset the level-inc threshold
											bufferInternals->levelIncThreshold = bufferInternals->levelIncThresholdIncrease + LIBBPMC_PARAMS_IO_R_LENGTH;
											bufferInternals->state = LIBBPMC_STRATEGY_BUFFER_STATE_WAIT_FOR_DECREASE;
											bufferInternals->nextNSeq = nSeqNew;
											LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE(" - success;\n");
										}else{

											LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE(" - failed;\n");
										}
									}else{
										LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("[Rate Dec] Not decreasing as we are in Blocking Time of %f s (%f s have passed )\n",bufferInternals->dNextUpdateAllowed,libBPMc_Traffic_getTimeDifference(bufferInternals->tLastUpdate,tNow));
									}
								}else{
									LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("[Rate Dec] Not decreasing connection inteval as TcOld = %d > tcNew = %d!",bufferInternals->currentConnectionInterval,tcNew);
								}
								virThread_mutex_unlock(&bufferInternals->mutex);

							}
						}

			}
		}
}

/*
uint8_t libBPMcStrategyBuffer_checkEnergeticPayOff(libBPMcInterface* interface, uint32_t tcOld, double nSeqOld, uint32_t tcNew, double nSeqNew){
	double qTcOld = 0;
	double qTcNew = 0;
	double qUpdate = 0;
	qTcOld = ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcOld*0.00125,floor(nSeqOld),0,20,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER);		//master
	qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcOld*0.00125,floor(nSeqOld),20,0,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER);		//slave

	//charge consumed when updating the connection interval
	qTcNew = ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcNew*0.00125,floor(nSeqNew),0,20,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER);		//master transmission
	qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcNew*0.00125,floor(nSeqNew),20,0,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER);		//slave transmission

	qUpdate = ble_e_model_ce_getChargeForConnectionProcedure(0,SC_SCAN_TYPE_PERIODIC,1,tcOld*0.00125,tcNew*0.00125);
	qUpdate += ble_e_model_ce_getChargeForConnectionProcedure(0,SC_SCAN_TYPE_PERIODIC,0,tcOld*0.00125,tcNew*0.00125);
	//	LIBBPMC_DEBUG("QUpdate master+slave: %f\n",qUpdate);
	//	LIBBPMC_DEBUG("Intervals: %d -> %d \t Charges: qTcOld = %f, qTcNew = %f, qUpdate = %f\n",tcOld, tcNew, qTcOld, qTcNew, qUpdate);

//	double tPenalty = qUpdate / (qTcOld/(tcOld * 0.00125) - qTcNew/(tcNew*0.00125));

	//number of old connection intervals in 6 new connection intervals
	double nTcOld = LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE*tcOld/tcNew;
	if(LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * qTcNew - nTcOld * qTcOld > qUpdate){
		return 1;
	}else{
		return 0;
	}


}
*/


double libBPMcStrategyBuffer_getPenaltyTime(libBPMcInterface* interface, uint32_t tcOld, uint32_t tcNew, double nSeqOld, double nSeqNew, libBPMcBuffer* bufOut){
	uint32_t cntOld;
	uint32_t cntNew;
	double rv;
	double qTcOld, qTcNew,qUpdate;
	libBPMcStrageyBufferInternals *bufferInternals = (libBPMcStrageyBufferInternals*) interface->internalData;

	//workaround
	if(nSeqOld < 1){
		nSeqOld = 1;
	}
	if(nSeqNew < 1){
		nSeqNew = 1;
	}
	//printf("penalty time hack active. nSeqNew = %f, nSeqOld = %f\n",nSeqNew, nSeqOld);

	//charge consumed without connection update per connection interval
 	 qTcOld = ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcOld*0.00125,nSeqOld,10,37,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);			//master
     qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcOld*0.00125,nSeqOld,37,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave
 	 qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcOld*0.00125,nSeqOld,15,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);			//master
     qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcOld*0.00125,nSeqOld,10,15,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave
     qTcOld /= 2.0;

	//charge consumed when updating the connection interval
	 qTcNew = ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcNew*0.00125,nSeqNew,10,37,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//master transmission
	 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcNew*0.00125,nSeqNew,37,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave transmission
	 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcNew*0.00125,nSeqNew,15,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//master transmission
	 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcNew*0.00125,nSeqNew,10,15,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave transmission
	 qTcNew /= 2.0;

	 //update cost
	 qUpdate = ble_e_model_ce_getChargeForConnectionProcedure(0,SC_SCAN_TYPE_PERIODIC,1,tcOld*0.00125,tcNew*0.00125,BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY);
     qUpdate += ble_e_model_ce_getChargeForConnectionProcedure(0,SC_SCAN_TYPE_PERIODIC,0,tcOld*0.00125,tcNew*0.00125,BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY);
	 //	LIBBPMC_DEBUG("QUpdate master+slave: %f\n",qUpdate);
	 //	LIBBPMC_DEBUG("Intervals: %d -> %d \t Charges: qTcOld = %f, qTcNew = %f, qUpdate = %f\n",tcOld, tcNew, qTcOld, qTcNew, qUpdate);




	//Rate Increase (Interval gets smaller)
	if(tcNew < tcOld){
		LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("Detected: Rate increase (= connection interval gets smaller)\n");
		// The new connection interval is to be shorter than the old one (Rate increase)
		// The only reason why to follow this is that it would fill the buffer over Nb,en



		rv = (libBPMcBuffer_getBytesLeft(bufOut) - LIBBPMC_PARAMS_IO_R_LENGTH - LIBBPMC_PARAM_WORST_CASE_WINDOW)/(bufferInternals->rateMax - 20.0*nSeqOld * (1.0 - LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION)/(tcOld * 0.00125)) - LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * tcOld * 0.00125;

	//rate decrease
	}else{
		LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("Detected: Rate decrease (= connection interval gets larger)\n");

		//approx.
		rv = qUpdate / (qTcOld/(tcOld * 0.00125) - qTcNew/(tcNew*0.00125));

	}
	if(rv < (double) tcNew * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * 0.00125){
		LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE("increasing penalty to intervals until update * Tc\n");
		rv = (double) tcNew * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * 0.00125;
	}
	//LIBBPMC_DEBUG("- penalty final: %f\n",rv);

	return rv;
}
