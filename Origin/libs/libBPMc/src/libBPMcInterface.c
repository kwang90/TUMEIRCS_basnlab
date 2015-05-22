/*
 * libBPMcInterface.c
 *
 *  Created on: 16.10.2013
 *      Author: kindt
 */

#include "libBPMcInterface.h"

void libBPMcInterface_init(libBPMcInterface* interface) {
	interface->setConIntCallBack = NULL;
	interface->addCallBackIn = NULL;
	interface->addCallBackOut = NULL;
	interface->readCallBackIn = NULL;
	interface->readCallBackOut = NULL;
	interface->setStartInfoCallBack = NULL;
	interface->setLatencyConstraintsCallBack = NULL;
	interface->setRoleCallBack = NULL;
	interface->setBufferPtrCallBack = NULL;
	interface->getParamCallBack = NULL;
	interface->setParamCallBack = NULL;
	interface->getParamListCallBack = NULL;
	interface->getIdentCallBack = NULL;
	interface->destroyCallBack = NULL;
	interface->internalData = NULL;
	interface->connectionUpdateCallBack = NULL;
	interface->active = 0;
	interface->tcMax = 0;
	interface->tcMin = 0;
	interface->bufOut = NULL;
	interface->nSeqCurrent = 0.0;
	interface->nSeqScheduled = 0.0;
	interface->idleLine.nMicrosecondsTimeout = 	LIBBPMC_PARAM_IDLE_LINE_TIMEOUT_MICROSECONDS;
	interface->idleLine.active = 0;
	interface->tcCurrent = 0;
	interface->tcScheduled = 0;
}

void libBPMcInterface_setSetConIntCallBack(libBPMcInterface* interface,
		libBPMcSetConIntCallBack cb) {
	interface->setConIntCallBack = cb;
}

void libBPMcInterface_setAddCallBackIn(libBPMcInterface* interface,
		libBPMcAddCallBack cb) {
	interface->addCallBackIn = cb;
}

void libBPMcInterface_setAddCallBackOut(libBPMcInterface* interface,
		libBPMcAddCallBack cb) {
	interface->addCallBackOut = cb;
}

void libBPMcInterface_setReadCallBackIn(libBPMcInterface* interface,
		libBPMcAddCallBack cb) {
	interface->readCallBackIn = cb;
}

void libBPMcInterface_setReadCallBackOut(libBPMcInterface* interface,
		libBPMcAddCallBack cb) {
	interface->readCallBackOut = cb;
}

void libBPMcInterface_setSetStartInfoCallBack(libBPMcInterface* interface,
		libBPMcSetStartInfoCallBack cb) {
	interface->setStartInfoCallBack = cb;
}

void libBPMcInterface_setSetLatencyConstraintsCallBack(
		libBPMcInterface* interface, libBPMcSetLatencyConstraintsCallBack cb) {
	interface->setLatencyConstraintsCallBack = cb;
}

void libBPMcInterface_setSetTrafficBoundsCallBack(libBPMcInterface* interface,
		libBPMcSetTrafficBoundsCallBack cb) {
	interface->setTrafficBoundsCallBack = cb;
}

void libBPMcInterface_setSetRoleCallBack(libBPMcInterface* interface,
		libBPMcSetRoleCallBack cb) {
	interface->setRoleCallBack = cb;
}

void libBPMcInterface_setSetBufferPtrCallBack(libBPMcInterface* interface,
		libBPMcSetBufferPtrCallBack cb) {
	interface->setBufferPtrCallBack = cb;
}

void libBPMcInterface_setGetParamCallBack(libBPMcInterface* interface,
		libBPMcGetParamCallBack cb) {
	interface->getParamCallBack = cb;
}

void libBPMcInterface_setSetParamCallBack(libBPMcInterface* interface,
		libBPMcSetParamCallBack cb) {
	interface->setParamCallBack = cb;
}

void libBPMcInterface_setGetParamListCallBack(libBPMcInterface* interface,
		libBPMcGetParamListCallBack cb) {
	interface->getParamListCallBack = cb;
}

void libBPMcInterface_setGetIdentCallBack(libBPMcInterface* interface,
		libBPMcGetIdentCallBack cb) {
	interface->getIdentCallBack = cb;
}

void libBPMcInterface_setActivationCallBack(libBPMcInterface* interface,
		libBPMcActivationCallBack cb) {
	interface->activationCallBack = cb;
}

void libBPMcInterface_setDestroyCallBack(libBPMcInterface* interface,
		libBPMcDestroyCallBack cb) {
	interface->destroyCallBack = cb;
}

void libBPMcInterface_setConnectionUpdateCallBack(libBPMcInterface* interface,
		libBPMcConnectionUpdateCallBack cb) {
	interface->connectionUpdateCallBack = cb;
}

uint8_t libBPMcInterface_callSetConIntCallBack(libBPMcInterface* interface, uint32_t tcNew, double nSeqNew) {
	interface->nSeqScheduled = nSeqNew;
	if (interface->setConIntCallBack != NULL) {
		if(interface->setConIntCallBack(interface, tcNew, nSeqNew)){
			interface->tcScheduled = tcNew;
			return 1;
		}else{
			return 0;
		}
	} else {
		return 0;
	}
}

void libBPMcInterface_callAddCallBackIn(libBPMcInterface* interface,
		libBPMcTraffic traffic) {
	if (interface->addCallBackIn != NULL) {
		interface->addCallBackIn(interface, traffic);
	}
}
void libBPMcInterface_callAddCallBackOut(libBPMcInterface* interface,
		libBPMcTraffic traffic) {
#if LIBBPMC_PARAM_USE_IDLE_LINE_DETECTION==1
	if (interface->idleLine.active) {
		libBPMcIdleLineDetection_add(interface);
	}
#endif
	if (interface->addCallBackOut != NULL) {
		interface->addCallBackOut(interface, traffic);
	}
}
void libBPMcInterface_callReadCallBackIn(libBPMcInterface* interface,
		libBPMcTraffic traffic) {
	if (interface->readCallBackIn != NULL) {
		interface->readCallBackIn(interface, traffic);
	}
}
void libBPMcInterface_callReadCallBackOut(libBPMcInterface* interface,
		libBPMcTraffic traffic) {
	if (interface->readCallBackOut != NULL) {
		interface->readCallBackOut(interface, traffic);
	}
}

void libBPMcInterface_callSetStartInfoCallBack(libBPMcInterface* interface,
		uint32_t tcInitial, double nSeqInitial) {
	interface->tcCurrent = tcInitial;
	interface->tcScheduled = tcInitial;
	interface->nSeqCurrent = nSeqInitial;
	interface->nSeqScheduled = nSeqInitial;

	if (interface->setStartInfoCallBack != NULL) {
		interface->setStartInfoCallBack(interface, tcInitial, nSeqInitial);
	}
}

void libBPMcInterface_callSetLatencyConstraintsCallBack(
		libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax) {
	interface->tcMax = tcMax;
	interface->tcMin = tcMin;
	if (interface->setLatencyConstraintsCallBack != NULL) {
		interface->setLatencyConstraintsCallBack(interface, tcMin, tcMax);
	}

}

void libBPMcInterface_callSetTrafficBoundsCallBack(libBPMcInterface* interface,
		double rateMin, double rateMax) {
	interface->maxRate = rateMax;
	if (interface->setTrafficBoundsCallBack != NULL) {
		interface->setTrafficBoundsCallBack(interface, rateMin, rateMax);
	}
}

void libBPMcInterface_callSetRoleCallBack(libBPMcInterface* interface,
		uint8_t masterOrSlave) {
	if (interface->setRoleCallBack != NULL) {
		interface->setRoleCallBack(interface, masterOrSlave);
	}
}

void libBPMcInterface_callSetBufferPtrCallBack(libBPMcInterface* interface,
		libBPMcBuffer *bufIn, libBPMcBuffer *bufOut) {
	interface->bufOut = bufOut;
	if (interface->setBufferPtrCallBack != NULL) {
		interface->setBufferPtrCallBack(interface, bufIn, bufOut);
	}
}

uint8_t libBPMcInterface_callGetParamCallBack(libBPMcInterface* interface,
		const char* ident, char* value) {
	if (interface->getParamCallBack != NULL) {
		return interface->getParamCallBack(interface, ident, value);
	} else {
		return 0;
	}
}

uint8_t libBPMcInterface_callSetParamCallBack(libBPMcInterface* interface,
		const char* ident, const char* value) {
	if (interface->setParamCallBack != NULL) {
		return interface->setParamCallBack(interface, ident, value);
	} else {
		return 0;
	}
}

char* libBPMcInterface_callGetParamListCallBack(libBPMcInterface* interface) {
	if (interface->getParamListCallBack != NULL) {
		return interface->getParamListCallBack(interface);
	} else {
		return NULL;
	}
}

void libBPMcInterface_callGetIdentCallBack(libBPMcInterface* interface,
		char* ident) {
	if (interface->getIdentCallBack != NULL) {
		interface->getIdentCallBack(interface, ident);
	}
}

void libBPMcInterface_callDestroyCallBack(libBPMcInterface* interface) {
	if (interface->destroyCallBack != NULL) {
		interface->destroyCallBack(interface);
	}
}

void libBPMcInterface_callActivationCallBack(libBPMcInterface* interface,
		int8_t active) {
#if LIBBPMC_PARAM_USE_IDLE_LINE_DETECTION==1
	libBPMcIdleLineDetection_init(interface);
#endif;
	if (interface->activationCallBack != NULL) {
		interface->activationCallBack(interface, active);
	}
}

void libBPMcInterface_callConnectionUpdateCallBack(libBPMcInterface* interface,
		uint32_t tcNew, uint32_t latencyNew, uint32_t timeoutNew) {
	interface->tcCurrent = tcNew;
	interface->nSeqCurrent = interface->nSeqScheduled;
	if (interface->connectionUpdateCallBack != NULL) {
		interface->connectionUpdateCallBack(interface, tcNew, latencyNew,
				timeoutNew);
	}
}

void libBPMcInterface_activateIdleLineDetection(libBPMcInterface* interface,
		uint32_t nMicroseconds) {
	interface->idleLine.active = 1;
	libBPMcIdleLineDetection_setTimeOut(interface, nMicroseconds);
}

void libBPMcInterface_disactivateIdleLineDetection(libBPMcInterface* interface) {
	interface->idleLine.active = 0;
	libBPMcIdleLineDetection_setTimeOut(interface, 0);

}
