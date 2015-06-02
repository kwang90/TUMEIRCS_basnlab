/*
 * libBPMcTrafficMeasurement.c
 *
 *  Created on: 25.10.2013
 *      Author: kindt
 */

#include <inttypes.h>
#include <malloc.h>
#include <string.h>		//for memset()
#include <math.h>		//for INFINITY-macro
#include <float.h>
#include "libBPMcTrafficMeasurement.h"
#include "libBPMcDebug.h"

void libBPMcTrafficMeasurement_init(libBPMcTrafficMeasurement *trm, uint32_t nClasses){
	uint32_t cnt;
	trm->nClasses = nClasses;
	trm->classBorders = malloc(nClasses * sizeof(double));
	trm->transitionCntClassWhole = malloc(nClasses*sizeof(uint32_t));
	trm->transitionCntClass = malloc(nClasses*sizeof(uint32_t*));
	trm->avgResidenceDuration = (double*) malloc(nClasses * sizeof(double));;

	trm->currentClass = 0;
	trm->previousClass = 0;
	trm->writeCnt = 0;
	trm->lastClassChangeTime.tv_sec = 0;
	trm->lastClassChangeTime.tv_nsec = 0;
	trm->lastWriteTime.tv_sec = 0;
	trm->lastWriteTime.tv_nsec = 0;
	for(cnt = 0; cnt < trm->nClasses; cnt++){
		trm->classBorders[cnt] = 0;
		trm->transitionCntClassWhole[cnt] = 0;
		trm->avgResidenceDuration[cnt] = 0.0;
		trm->transitionCntClass[cnt] = malloc(nClasses*sizeof(uint32_t));
		memset(trm->transitionCntClass[cnt],0,nClasses*sizeof(uint32_t));
	}
}

void libBPMcTrafficMeasurement_destroy(libBPMcTrafficMeasurement *trm){
	uint32_t cnt;
	if(trm == NULL){
		return;
	};
	for(cnt = 0; cnt < trm->nClasses; cnt++){
		free(trm->transitionCntClass[cnt]);
	}
	free(trm->classBorders);
	free(trm->transitionCntClassWhole);
	free(trm->transitionCntClass);
	free(trm->avgResidenceDuration);

}

uint8_t libBPMcTrafficMeasurement_setClassBorder(libBPMcTrafficMeasurement *trm, uint32_t classNo, double border){
	if(trm == NULL){
		return 0;
	}
	if(classNo >= trm->nClasses){
		return 0;
	}
	trm->classBorders[classNo] = border;
	return 1;
}
uint32_t libBPMcTrafficMeasurement_determineClass(libBPMcTrafficMeasurement *trm, double trafficRate){
	uint32_t cnt = 0;
	for(cnt = 0; cnt < trm->nClasses; cnt++){
		if(trafficRate < trm->classBorders[cnt]){
			return cnt;
		}else{

		}
	}
	return trm->nClasses - 1;
}

uint8_t libBPMcTrafficMeasurement_add(libBPMcTrafficMeasurement *trm, libBPMcTraffic tr){
	double timeDifferenceLastWrite = 0.0;
	double timeDifferenceLastClassChange = 0.0;

	double trafficRate = 0.0;
	double timePerByte;
	uint32_t trClass = 0;

	if(trm->writeCnt > 0){
		timeDifferenceLastWrite = libBPMc_Traffic_getTimeDifference(trm->lastWriteTime,tr.time);
		trafficRate = (double) tr.nBytes / timeDifferenceLastWrite;
		trClass = libBPMcTrafficMeasurement_determineClass(trm,trafficRate);
		//LIBBPMC_DEBUG("trafficMeasurement::add() : rate = %f, class %d, dT = %f s",trafficRate,trClass, timeDifferenceLastWrite);

	}else{
		trm->lastClassChangeTime = tr.time;
	}

	//Continuous markov chain data 5/2014
	if(trm->writeCnt > 1){
		if(trClass != trm->currentClass){
			timeDifferenceLastClassChange = libBPMc_Traffic_getTimeDifference(trm->lastClassChangeTime,trm->lastWriteTime);
			printf("Class change %d => %d, duration: %f s\n",trm->currentClass,trClass,timeDifferenceLastClassChange);
			trm->transitionCntClassWhole[trm->currentClass]++;
			trm->transitionCntClass[trm->currentClass][trClass]++;
			if(trm->lastClassChangeTime.tv_sec > 0){
				trm->avgResidenceDuration[trm->currentClass] += timeDifferenceLastClassChange;

			}
			trm->lastClassChangeTime = trm->lastWriteTime;
		}
	}



	trm->lastWriteTime = tr.time;
	trm->previousClass = trm->currentClass;
	trm->currentClass = trClass;
	trm->writeCnt++;
}


double libBPMcTrafficMeasurement_getAvgResidenceDuration(libBPMcTrafficMeasurement *trm, uint32_t classNo){

	if(trm == NULL){
		return -1.0;
	}
	if(classNo >= trm->nClasses){
		return -1.0;
	}
	if(trm->transitionCntClassWhole[classNo] == 0){
		return DBL_MAX;
	}

	return trm->avgResidenceDuration[classNo]/trm->transitionCntClassWhole[classNo];


}


double libBPMcTrafficMeasurement_getTransitionProbability(libBPMcTrafficMeasurement* trm, libBPMcTrafficMeasurementTransitionProbabType type, uint32_t classSrc, uint32_t classDest){
	uint32_t cnt = 0;
	double rv = 0;
	if(trm == NULL){
		return -1.0;
	}
	if(classSrc >= trm->nClasses){
		return -1.0;
	}
	if(classDest >= trm->nClasses){
		return -1.0;
	}

	switch(type){
	case TRANSITION_PROBAB_CLASSES:

		if(trm->transitionCntClassWhole[classSrc] == 0){
				return 0.0;	//By definition
		}else{
			return (double) trm->transitionCntClass[classSrc][classDest]/(double) trm->transitionCntClassWhole[classSrc];
		}
		break;
	case TRANSITION_PROBAB_TRANSRATE:
		if(trm->transitionCntClassWhole[classSrc] == 0){
				return 0.0;	//By definition
		}else{
			if(classSrc != classDest){
				return (double) 1.0/libBPMcTrafficMeasurement_getAvgResidenceDuration(trm,classSrc) * trm->transitionCntClass[classSrc][classDest]/(double) trm->transitionCntClassWhole[classSrc];
			}else{
				for(cnt = 0; cnt < trm->nClasses; cnt++){
					if(cnt != classSrc){
						rv += (double) 1.0/libBPMcTrafficMeasurement_getAvgResidenceDuration(trm,classSrc) * trm->transitionCntClass[classSrc][cnt]/(double) trm->transitionCntClassWhole[classSrc];
					}
				}
				return -1* rv;
			}

		}
		break;
	}
}
uint32_t libBPMcTrafficMeasurement_getNClasses(libBPMcTrafficMeasurement* trm){
	return trm->nClasses;
}

void libBPMcTrafficMeasurement_printTransitionMatrix(libBPMcTrafficMeasurement* trm, libBPMcTrafficMeasurementTransitionProbabType type){
	uint32_t cnt1;
	uint32_t cnt2;
	printf("\t\t");
	for(cnt1 = 0; cnt1 < trm->nClasses; cnt1++){
		printf("[%d]\t\t",cnt1);
	}
	printf("\n");

	for(cnt1 = 0; cnt1 < trm->nClasses; cnt1++){			//all dest
		for(cnt2 = 0; cnt2 < trm->nClasses; cnt2++){		//all src classes
			if(cnt2 == 0){
				printf("[%d]\t\t",cnt1);
			};
			printf("% -7.5g\t\t",(double) libBPMcTrafficMeasurement_getTransitionProbability(trm, type, cnt1, cnt2));
		}
		printf("\n");
	}
}

int32_t libBPMcTrafficMeasurement_getProbabilityMatrix(libBPMcTrafficMeasurement* trm, libBPMcTrafficMeasurementTransitionProbabType type, libBPMcMatrix* matrixResult, uint8_t intensity, uint8_t init){
	uint32_t cntSrc;
	uint32_t cntDest;
	if(trm == NULL){
		return -1.0;
	}
	if(init){
		libBPMcMatrix_init(matrixResult,trm->nClasses,trm->nClasses);

	}else{
		if((matrixResult->x != matrixResult->y)||(matrixResult->x != trm->nClasses)){
			return -1;
		}
	}

	for(cntSrc = 0; cntSrc < trm->nClasses; cntSrc++){
		for(cntDest = 0; cntDest < trm->nClasses; cntDest++){
			matrixResult->data[cntSrc][cntDest] = libBPMcTrafficMeasurement_getTransitionProbability(trm,type,cntSrc,cntDest);
			if((cntSrc == cntDest)&&(intensity)){
				matrixResult->data[cntSrc][cntDest] = (matrixResult->data[cntSrc][cntDest]) - 1;
			}
		}
	}

}

double libBPMcTrafficMeasurement_getTrafficInClass(libBPMcTrafficMeasurement* trm, uint32_t classNo, libBPMcTrafficMeasurement_classTrafficRelation rel){
	double traffic = 0.0;
	if((trm->nClasses == 0)||(classNo > trm->nClasses)){
		printf("invalid classNo or trm uninitalized. ClassNo is %d, trm has %d classes.",classNo, trm->nClasses);
		return -1.0;
	}

	switch(rel){
		case CLASSTRAFFIC_RELATION_MIN:
			if(classNo == 0){
				traffic = 0.0;
			}else{
				traffic = trm->classBorders[classNo - 1];
			}
			break;
		case CLASSTRAFFIC_RELATION_MAX:
			traffic = trm->classBorders[classNo];
			break;
		case CLASSTRAFFIC_RELATION_AVG:
			if(classNo == 0){
				traffic = trm->classBorders[0]/2.0;
			}else{
				traffic = (trm->classBorders[classNo] + trm->classBorders[classNo - 1]) / 2.0;
			}
			break;
		default:
			printf("wrong class to traffic relation\n");
			traffic = -1.0;
			break;
	}
//	printf("TRAFFIC: class %d, relation %d, traffic: %f\n",classNo,rel,traffic);
	return traffic;
}
