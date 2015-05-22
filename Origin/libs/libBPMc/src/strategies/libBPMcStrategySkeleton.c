/*
 * libBPMcStrategySkeleton.c
 *
 *  Created on: 17.10.2013
 *      Author: kindt
 */



#include "../libBPMcInterface.h"
#include "libBPMcStrategySkeleton.h"
#include <float.h> //for DBL_MAX

void libBPMcStrategySkeleton_init(libBPMcInterface* interface){
	libBPMcStrageySkeletonInternals *skeletonInternals = (libBPMcStrageySkeletonInternals*) malloc(sizeof(libBPMcStrageySkeletonInternals));
	interface->internalData = (void*) skeletonInternals;
	skeletonInternals->connectionIntervalMax = 3200;		//default max. connection interval- can be modified by setLantecyConstraintsCallBack()


	//Set all callbacks needed for power management
	libBPMcInterface_setActivationCallBack(interface,libBPMcStrategySkeleton_setActivation);
	libBPMcInterface_setAddCallBackOut(interface,libBPMcStrategySkeleton_addOut);
	libBPMcInterface_setReadCallBackOut(interface,libBPMcStrategySkeleton_readOut);
	libBPMcInterface_setSetTrafficBoundsCallBack(interface,libBPMcStrategySkeleton_setTrafficBounds);
	libBPMcInterface_setSetBufferPtrCallBack(interface,libBPMcStrategySkeleton_setBuffer);
	libBPMcInterface_setSetStartInfoCallBack(interface, libBPMcStrategySkeleton_startInfo);
	libBPMcInterface_setDestroyCallBack(interface,libBPMcStrategySkeleton_destory);
	libBPMcInterface_setSetRoleCallBack(interface,libBPMcStrategySkeleton_setRole);
	libBPMcInterface_setSetLatencyConstraintsCallBack(interface,libBPMcStrategySkeleton_setLatencyConstraints);
	libBPMcInterface_setConnectionUpdateCallBack(interface, libBPMcStrategySkeleton_connectionUpdated);
	//Set all callbacks needed for parameter exchange
	libBPMcInterface_setGetIdentCallBack(interface,libBPMcStrategySkeleton_getIdent);
	libBPMcInterface_setGetParamListCallBack(interface,libBPMcStrategySkeleton_getParamList);
	libBPMcInterface_setGetParamCallBack(interface,libBPMcStrategySkeleton_getParam);
	libBPMcInterface_setSetParamCallBack(interface,libBPMcStrategySkeleton_setParam);
	skeletonInternals->bufIn = NULL;
	skeletonInternals->bufOut = NULL;
	skeletonInternals->increment = 1;
	skeletonInternals->rateMin = 0;
	skeletonInternals->rateMax = DBL_MAX;

}

void libBPMcStrategySkeleton_startInfo(libBPMcInterface* interface, int32_t tcInitial, double nSeqInitial){
	printf("Skeleton strategy initialized.");
	libBPMcStrageySkeletonInternals *skeletonInternals = (libBPMcStrageySkeletonInternals*) interface->internalData;
	skeletonInternals->nextConnectionInterval = tcInitial;
	skeletonInternals->currentNSeq = nSeqInitial;
}

void libBPMcStrategySkeleton_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax){
	libBPMcStrageySkeletonInternals *skeletonInternals = (libBPMcStrageySkeletonInternals*) interface->internalData;
	skeletonInternals->connectionIntervalMax = tcMax;
}

void libBPMcStrategySkeleton_setTrafficBounds(libBPMcInterface* interface, double rateMin, double rateMax){
	libBPMcStrageySkeletonInternals *skeletonInternals = (libBPMcStrageySkeletonInternals*) interface->internalData;
	skeletonInternals->rateMin = rateMin;
	skeletonInternals->rateMax = rateMax;
}

void libBPMcStrategySkeleton_setRole(libBPMcInterface* interface, uint8_t masterOrSlave){
	libBPMcStrageySkeletonInternals *skeletonInternals = (libBPMcStrageySkeletonInternals*) interface->internalData;
	skeletonInternals->masterOrSlave = masterOrSlave;
}

void libBPMcStrategySkeleton_addOut(libBPMcInterface* interface, libBPMcTraffic tr){
	if(interface->active){
	//	printf("Adding traffic: %d bytes in %d seconds %d nanoseconds\n",tr.nBytes, tr.time.tv_sec, tr.time.tv_nsec);
		libBPMcStrageySkeletonInternals *skeletonInternals = (libBPMcStrageySkeletonInternals*) interface->internalData;
		if(skeletonInternals->nextConnectionInterval < skeletonInternals->connectionIntervalMax){
			skeletonInternals->nextConnectionInterval += skeletonInternals->increment;
		}
		//set another connection interval
		libBPMcInterface_callSetConIntCallBack(interface,skeletonInternals->nextConnectionInterval,1);		//1=> 1 sequence per connection interval is expected to be sent

	}else{
		printf("not adjusting connection interval - disactivated.\n");
	}
}

void libBPMcStrategySkeleton_readOut(libBPMcInterface* interface, libBPMcTraffic tr){
	if(interface->active){
			printf("Data read from output buffer.\n");
		}
}

void libBPMcStrategySkeleton_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut){
	libBPMcStrageySkeletonInternals *skeletonInternals = (libBPMcStrageySkeletonInternals*) interface->internalData;
	skeletonInternals->bufIn = bufIn;
	skeletonInternals->bufOut = bufOut;
}

void libBPMcStrategySkeleton_destory(libBPMcInterface* interface){
	libBPMcStrageySkeletonInternals *skeletonInternals = (libBPMcStrageySkeletonInternals*) interface->internalData;
	free(skeletonInternals);
	free(interface);
	printf("destroy\n");
}


void libBPMcStrategySkeleton_getIdent(libBPMcInterface* interface,char* ident){
	strcpy(ident, "libBPMcStrategySkeleton");
}

char* libBPMcStrategySkeleton_getParamList(libBPMcInterface* interface){

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

uint8_t libBPMcStrategySkeleton_setParam(libBPMcInterface* interface,const char* param, const char* value){
	libBPMcStrageySkeletonInternals *skeletonInternals = (libBPMcStrageySkeletonInternals*) interface->internalData;

	if(strcmp(param,"increment")==0){
		printf("setting param increment to %f,\n",atof(value));
		skeletonInternals->increment = atof(value);
	}else if(strcmp(param, "dummyParameter")==0){
						//do something fancy
	}else{
		return 0;		//failure
	}
	return 1;
}

uint8_t libBPMcStrategySkeleton_getParam(libBPMcInterface* interface,const char* param, char* value){
	libBPMcStrageySkeletonInternals *skeletonInternals = (libBPMcStrageySkeletonInternals*) interface->internalData;

	if(value == NULL){
		return 0;	//error
	}
	if(strcmp(param,"increment")==0){
		sprintf(value, "%d",skeletonInternals->increment);
	}else if(strcmp(param, "dummyParam")==0){
		sprintf(value, "%f",0.2345);		//dummyparam allways has value 0.2345
	}else{
		return 0;		//failure
	}
	return 1;

}

void libBPMcStrategySkeleton_setActivation(libBPMcInterface* interface, uint8_t active){
	interface->active = active;
}

void libBPMcStrategySkeleton_connectionUpdated(libBPMcInterface* interface, uint32_t tcNew, uint32_t latencyNew, uint32_t timeoutNew){
	libBPMcStrageySkeletonInternals *skeletonInternals = (libBPMcStrageySkeletonInternals*) interface->internalData;
	if(interface->active){
		printf("Connection parameters updated. New Interval: %d * 1.25 ms, new slave latency: %d, new supervision timeout %d * 1.25 ms\n", tcNew, latencyNew, timeoutNew);
	}
}
