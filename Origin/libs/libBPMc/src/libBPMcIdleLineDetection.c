/*
 * libBPMcIdleLineDetection.c
 *
 *  Created on: 25.06.2014
 *      Author: kindt
 */

#ifndef LIBBPMCIDLELINEDETECTION_C_
#define LIBBPMCIDLELINEDETECTION_C_

#include "libBPMcIdleLineDetection.h"
#include "libBPMcEmergencyProcedure.h"
#include "libBPMcInterface.h"
#include "libBPMcDebug.h"


void libBPMcIdleLineDetection_forceUpdate(libBPMcInterface* interface){
#if LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
	interface = (libBPMcInterface*) pvTimerGetTimerID((TimerHandle_t) interface);
#endif
	uint32_t tcNew = interface->tcMax;
	uint32_t tcMin = libBPMcEmergencyProcedure_getWorstCaseMinConnectionInterval(interface->bufOut,interface->tcCurrent,interface->nSeqCurrent,interface->maxRate);
	if (tcNew > tcMin){
		tcNew = tcMin;
	}
	if(tcNew != interface->tcCurrent){
		libBPMcInterface_callSetConIntCallBack(interface,tcNew,interface->nSeqCurrent);
		LIBBPMC_DEBUG("Idle line detected. Update to %d (tcMin = %d)\n",tcNew, tcMin);
		LIBBPMC_DEBUG("update!\n");
	}else{
		LIBBPMC_DEBUG("No update (same interval)!\n");

	}
}


void libBPMcIdleLineDetection_init(libBPMcInterface* interface){
	LIBBPMC_DEBUG("INITIALIZING IDLE LINE DETECTION\n");
	if((interface->maxRate == 0)||(interface->tcMax == 0)){
		LIBBPMC_DEBUG("NOT activating idle line detection as interface is uninitialized.");
		return;
	}
	if(!virThread_timer_create(&(interface->idleLine.timer),interface->idleLine.nMicrosecondsTimeout,libBPMcIdleLineDetection_forceUpdate,(void*) interface, 0)){
		LIBBPMC_DEBUG("failure creating timer.");
		return;
	}
	LIBBPMC_DEBUG("Idle line detection initialized\n");
}

void libBPMcIdleLineDetection_add(libBPMcInterface* interface){
	if((interface->maxRate == 0)||(interface->tcMax == 0)){
		LIBBPMC_DEBUG("NOT activating idle line detection as interface is uninitialized.");
		return;
	};
	if(interface->idleLine.nMicrosecondsTimeout > 0){
	LIBBPMC_DEBUG("timer update with %d microseconds\n",interface->idleLine.nMicrosecondsTimeout);
		if(!virThread_timer_start(&(interface->idleLine.timer),interface->idleLine.nMicrosecondsTimeout,0)){
			LIBBPMC_DEBUG("Timer Update failed for timer %u\n",interface->idleLine.timer);
			exit(1);
		}
	}
}

void libBPMcIdleLineDetection_setTimeOut(libBPMcInterface* interface, uint32_t nMicroSeconds){
	interface->idleLine.nMicrosecondsTimeout = nMicroSeconds;
}



#endif /* LIBBPMCIDLELINEDETECTION_C_ */
