/*
 * libBPMcStrategyEmergencyTest.c
 *
 *  Created on: 17.10.2013
 *      Author: kindt
 */



#include "../libBPMcInterface.h"
#include "libBPMcStrategyEmergencyTest.h"
#include <float.h> //for DBL_MAX

void libBPMcStrategyEmergencyTest_init(libBPMcInterface* interface){
	libBPMcStrageyEmergencyTestInternals *emergencyTestInternals = (libBPMcStrageyEmergencyTestInternals*) malloc(sizeof(libBPMcStrageyEmergencyTestInternals));
	interface->internalData = (void*) emergencyTestInternals;
	emergencyTestInternals->connectionIntervalMax = 3200;		//default max. connection interval- can be modified by setLantecyConstraintsCallBack()


	//Set all callbacks needed for power management
	libBPMcInterface_setActivationCallBack(interface,libBPMcStrategyEmergencyTest_setActivation);
	libBPMcInterface_setAddCallBackOut(interface,libBPMcStrategyEmergencyTest_addOut);
	libBPMcInterface_setReadCallBackOut(interface,libBPMcStrategyEmergencyTest_readOut);
	libBPMcInterface_setSetTrafficBoundsCallBack(interface,libBPMcStrategyEmergencyTest_setTrafficBounds);
	libBPMcInterface_setSetBufferPtrCallBack(interface,libBPMcStrategyEmergencyTest_setBuffer);
	libBPMcInterface_setSetStartInfoCallBack(interface, libBPMcStrategyEmergencyTest_startInfo);
	libBPMcInterface_setDestroyCallBack(interface,libBPMcStrategyEmergencyTest_destory);
	libBPMcInterface_setSetRoleCallBack(interface,libBPMcStrategyEmergencyTest_setRole);
	libBPMcInterface_setSetLatencyConstraintsCallBack(interface,libBPMcStrategyEmergencyTest_setLatencyConstraints);
	libBPMcInterface_setConnectionUpdateCallBack(interface, libBPMcStrategyEmergencyTest_connectionUpdated);
	//Set all callbacks needed for parameter exchange
	libBPMcInterface_setGetIdentCallBack(interface,libBPMcStrategyEmergencyTest_getIdent);
	libBPMcInterface_setGetParamListCallBack(interface,libBPMcStrategyEmergencyTest_getParamList);
	libBPMcInterface_setGetParamCallBack(interface,libBPMcStrategyEmergencyTest_getParam);
	libBPMcInterface_setSetParamCallBack(interface,libBPMcStrategyEmergencyTest_setParam);
	emergencyTestInternals->bufIn = NULL;
	emergencyTestInternals->bufOut = NULL;
	emergencyTestInternals->increment = 1;
	emergencyTestInternals->rateMin = 0;
	emergencyTestInternals->rateMax = DBL_MAX;

}

void libBPMcStrategyEmergencyTest_startInfo(libBPMcInterface* interface, int32_t tcInitial, double nSeqInitial){
	printf("EmergencyTest strategy initialized.");
	libBPMcStrageyEmergencyTestInternals *emergencyTestInternals = (libBPMcStrageyEmergencyTestInternals*) interface->internalData;
	emergencyTestInternals->nextConnectionInterval = tcInitial;
}

void libBPMcStrategyEmergencyTest_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax){
	libBPMcStrageyEmergencyTestInternals *emergencyTestInternals = (libBPMcStrageyEmergencyTestInternals*) interface->internalData;
	emergencyTestInternals->connectionIntervalMax = tcMax;
}

void libBPMcStrategyEmergencyTest_setTrafficBounds(libBPMcInterface* interface, double rateMin, double rateMax){
	libBPMcStrageyEmergencyTestInternals *emergencyTestInternals = (libBPMcStrageyEmergencyTestInternals*) interface->internalData;
	emergencyTestInternals->rateMin = rateMin;
	emergencyTestInternals->rateMax = rateMax;
}

void libBPMcStrategyEmergencyTest_setRole(libBPMcInterface* interface, uint8_t masterOrSlave){
	libBPMcStrageyEmergencyTestInternals *emergencyTestInternals = (libBPMcStrageyEmergencyTestInternals*) interface->internalData;
	emergencyTestInternals->masterOrSlave = masterOrSlave;
}

void libBPMcStrategyEmergencyTest_addOut(libBPMcInterface* interface, libBPMcTraffic tr){
	if(interface->active){
	//	printf("Adding traffic: %d bytes in %d seconds %d nanoseconds\n",tr.nBytes, tr.time.tv_sec, tr.time.tv_nsec);
		libBPMcStrageyEmergencyTestInternals *emergencyTestInternals = (libBPMcStrageyEmergencyTestInternals*) interface->internalData;
		//set another connection interval
		libBPMcInterface_callSetConIntCallBack(interface,emergencyTestInternals->connectionIntervalMax,LIBBPMC_PARAM_BLE_NSEQ_MAX);		//1=> 1 sequence per connection interval is expected to be sent

	}else{
		printf("not adjusting connection interval - disactivated.\n");
	}
}

void libBPMcStrategyEmergencyTest_readOut(libBPMcInterface* interface, libBPMcTraffic tr){
	if(interface->active){
		//	printf("Data read from output buffer.\n");
		}
}

void libBPMcStrategyEmergencyTest_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut){
	libBPMcStrageyEmergencyTestInternals *emergencyTestInternals = (libBPMcStrageyEmergencyTestInternals*) interface->internalData;
	emergencyTestInternals->bufIn = bufIn;
	emergencyTestInternals->bufOut = bufOut;
}

void libBPMcStrategyEmergencyTest_destory(libBPMcInterface* interface){
	libBPMcStrageyEmergencyTestInternals *emergencyTestInternals = (libBPMcStrageyEmergencyTestInternals*) interface->internalData;
	free(emergencyTestInternals);
	free(interface);
	printf("destroy\n");
}


void libBPMcStrategyEmergencyTest_getIdent(libBPMcInterface* interface,char* ident){
	strcpy(ident, "libBPMcStrategyEmergencyTest");
}

char* libBPMcStrategyEmergencyTest_getParamList(libBPMcInterface* interface){

	char* paramList = (char*) malloc(1000);
	strcpy(paramList,"");
	char* rv;
	char lineBuf[256];
	sprintf(lineBuf,"%s|%s|%d|%d\n","increment","d",1,500);
	strcat(paramList, lineBuf);
	sprintf(lineBuf,"%s|%s|%d|%d\n","dummyParam","f2",0,10);
	strcat(paramList, lineBuf);
	rv = strdup(paramList);
	free(paramList);
	return rv;
}

uint8_t libBPMcStrategyEmergencyTest_setParam(libBPMcInterface* interface,const char* param, const char* value){
	libBPMcStrageyEmergencyTestInternals *emergencyTestInternals = (libBPMcStrageyEmergencyTestInternals*) interface->internalData;

	if(strcmp(param,"increment")==0){
		printf("setting param increment to %f,\n",atof(value));
		emergencyTestInternals->increment = atof(value);
	}else if(strcmp(param, "dummyParameter")==0){
						//do something fancy
	}else{
		return 0;		//failure
	}
	return 1;
}

uint8_t libBPMcStrategyEmergencyTest_getParam(libBPMcInterface* interface,const char* param, char* value){
	libBPMcStrageyEmergencyTestInternals *emergencyTestInternals = (libBPMcStrageyEmergencyTestInternals*) interface->internalData;

	if(value == NULL){
		return 0;	//error
	}
	if(strcmp(param,"increment")==0){
		sprintf(value, "%d",emergencyTestInternals->increment);
	}else if(strcmp(param, "dummyParam")==0){
		sprintf(value, "%f",0.2345);		//dummyparam allways has value 0.2345
	}else{
		return 0;		//failure
	}
	return 1;

}

void libBPMcStrategyEmergencyTest_setActivation(libBPMcInterface* interface, uint8_t active){
	interface->active = active;
}

void libBPMcStrategyEmergencyTest_connectionUpdated(libBPMcInterface* interface, uint32_t tcNew, uint32_t latencyNew, uint32_t timeoutNew){
	if(interface->active){
		printf("Connection parameters updated. New Interval: %d * 1.25 ms, new slave latency: %d, new supervision timeout %d * 1.25 ms\n", tcNew, latencyNew, timeoutNew);
	}
}
