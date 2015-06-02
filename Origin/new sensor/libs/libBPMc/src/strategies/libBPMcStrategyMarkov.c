/*
 * libBPMcStrategyMarkov.c
 *
 *  Created on: 17.10.2013
 *      Author: kindt
 */



#include "../libBPMcInterface.h"
#include "../libBPMcTrafficAnalysis.h"
#include "../libBPMcTrafficMeasurement.h"
#include "../libBPMcParams.h"
#include "libBPMcStrategyMarkov.h"
#include "../libBPMcDebug.h"
#include <bleEnergyModel/src/ble_model.h>
#include "../libBPMcRateCalc.h"

void libBPMcStrategyMarkov_init(libBPMcInterface* interface){
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) malloc(sizeof(libBPMcStrageyMarkovInternals));
	interface->internalData = (void*) markovInternals;
	markovInternals->connectionIntervalMax = 3200;		//default max. connection interval- can be modified by setLantecyConstraintsCallBack()
	markovInternals->maxTraffic = LIBBPMC_STRATEGY_MARKOV_DEFAULT_MAXTRAFFIC;
	markovInternals->exponSmoothingFactor = LIBBPMC_STRATEGY_MARKOV_DEFAULT_SMOOTHING_FACTOR;
	markovInternals->lastTimeStamp.tv_sec = 0;
	markovInternals->lastTimeStamp.tv_nsec = 0;
	markovInternals->meanTimeBetweenWrites = 0.0;
	markovInternals->currentNSeq = LIBBPMC_STRATEGY_MARKOV_BLE_NSEQ_MAX;
	markovInternals->currentClass = 0;
	markovInternals->currentRate = 0.0;
	markovInternals->currentAppRate = 0.0;
	markovInternals->currentConnectionInterval = 0;
	markovInternals->bufIn = NULL;
	markovInternals->bufOut = NULL;
	markovInternals->inGoingClassLastDetermination = -1;
	markovInternals->updatePending = 0;

	//Set all callbacks needed for power management
	libBPMcInterface_setActivationCallBack(interface,libBPMcStrategyMarkov_setActivation);
	libBPMcInterface_setAddCallBackOut(interface,libBPMcStrategyMarkov_addOut);
	libBPMcInterface_setSetBufferPtrCallBack(interface,libBPMcStrategyMarkov_setBuffer);
	libBPMcInterface_setSetStartInfoCallBack(interface, libBPMcStrategyMarkov_startInfo);
	libBPMcInterface_setDestroyCallBack(interface,libBPMcStrategyMarkov_destory);
	libBPMcInterface_setSetRoleCallBack(interface,libBPMcStrategyMarkov_setRole);
	libBPMcInterface_setSetLatencyConstraintsCallBack(interface,libBPMcStrategyMarkov_setLatencyConstraints);
	libBPMcInterface_setSetTrafficBoundsCallBack(interface,libBPMcStrategyMarkov_updateTrafficBounds);
	//Set all callbacks needed for parameter exchange
	libBPMcInterface_setGetIdentCallBack(interface,libBPMcStrategyMarkov_getIdent);
	libBPMcInterface_setGetParamListCallBack(interface,libBPMcStrategyMarkov_getParamList);
	libBPMcInterface_setGetParamCallBack(interface,libBPMcStrategyMarkov_getParam);
	libBPMcInterface_setSetParamCallBack(interface,libBPMcStrategyMarkov_setParam);
	libBPMcTrafficMeasurement_init(&(markovInternals->trm),LIBBPMC_STRATEGY_MARKOV_DEFAULT_NCLASSES);
	libBPMcStrategyMarkov_updateTrafficBounds(interface,0,LIBBPMC_STRATEGY_MARKOV_DEFAULT_MAXTRAFFIC);

	//prepare determination thread
	virThread_sema_init(&markovInternals->determinationSema,0);
	virThread_thread_create(&markovInternals->determinationThreadHandle,libBPMcStrategyMarkov_determinationThreadFunc,interface,"MarkovDetermination",LIBBPMC_STRATEGY_MARKOV_FREERTOS_STACK_SIZE,LIBBPMC_STRATEGY_MARKOV_FREERTOS_PRIORITY);


	markovInternals->recurrenceCnt = 0;
}

void libBPMcStrategyMarkov_updateTrafficBounds(libBPMcInterface* interface, double minTraffic, double maximumTraffic){
	uint32_t cnt;
	double classBorder = 0;
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;
	markovInternals->maxTraffic = maximumTraffic;
	for(cnt = 0; cnt < libBPMcTrafficMeasurement_getNClasses(&(markovInternals->trm)); cnt++){
		classBorder = markovInternals->maxTraffic * (double) (cnt + 1)/libBPMcTrafficMeasurement_getNClasses(&(markovInternals->trm));
		LIBBPMC_DEBUG("Setting classborder %d to %f bytes/s",cnt,classBorder);
		libBPMcTrafficMeasurement_setClassBorder(&(markovInternals->trm),cnt,classBorder);
	}
}
void libBPMcStrategyMarkov_startInfo(libBPMcInterface* interface, int32_t tcInitial, double nSeqInitial){
	//LIBBPMC_DEBUG("Markov strategy has been informed: current connection interval is: %d\n",tcInitial);
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;
	markovInternals->currentConnectionInterval = tcInitial;
	markovInternals->currentNSeq = nSeqInitial;
	markovInternals->currentRate = tcInitial * 20.0 * LIBBPMC_STRATEGY_MARKOV_BLE_NSEQ_MAX / 0.00125 ;
	markovInternals->currentClass = libBPMcTrafficMeasurement_determineClass(&(markovInternals->trm),markovInternals->currentRate);

}

void libBPMcStrategyMarkov_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax){
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;
	markovInternals->connectionIntervalMax = tcMax;
}

void libBPMcStrategyMarkov_setRole(libBPMcInterface* interface, uint8_t masterOrSlave){
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;
	markovInternals->masterOrSlave = masterOrSlave;
}

uint8_t libBPMcStrategyMarkov_determineConnectionIntervalUpdate(libBPMcInterface* interface,double rateNew, double* nextRate, int32_t* nextClass){
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;
	uint32_t tcOld, tcNew;
	double nSeqNew;
	uint32_t classNew;
	double accProbab;
	double sumOfResidenceDur;
	uint32_t tmpClass;
	uint32_t tcTmp;
	double tmpAvgRate;
	double tmpTransProbab;
	double expectedNRepetitions;
	double expectedTimeInRecurrence;
	//Detect a class change in the application data rate. If no such class chance occures, then
	//do not update
	classNew =  libBPMcTrafficMeasurement_determineClass(&(markovInternals->trm),rateNew);
	if((markovInternals->inGoingClassLastDetermination == classNew)&&(!markovInternals->updatePending)){
		return 0;		//no class change in input rate
	}
	markovInternals->inGoingClassLastDetermination = classNew;


	//if the class in the application data rate has changed, but the new class coincides with the current class of the output data
	//stream, then do not update
	if(classNew == markovInternals->currentClass){
		printf("\nNew class and old class are identical - do not update. Classes: Old: %d, New: %d - rates: %f -> %f\n",markovInternals->currentClass, classNew, markovInternals->currentRate,rateNew);
		return 0;	//no class change in output rate
	}else{
		printf("\nConsidering class change. Classes: Old: %d, New: %d - rates: %f -> %f\n",markovInternals->currentClass, classNew, markovInternals->currentRate,rateNew);
	}
	markovInternals->updatePending = 0;
	//fill the values of tcOld, tcNew, nSeqOld, nSeqNew
	tcOld = markovInternals->currentConnectionInterval;
	libBPMcStrategyMarkov_getConnectionParams(interface,rateNew,&tcNew, &nSeqNew);

	//if an update to a higher value than allowed is checked, then reduce this one.
	if(tcNew > markovInternals->connectionIntervalMax){
		tcNew = markovInternals->connectionIntervalMax;

	}

	//after such a possible reduction, we have to re-check weather old and new connection interval is the same
	if(tcOld == tcNew){
		LIBBPMC_DEBUG("\nNew (ideal) interval and old interval are identical to %d - do not update. Classes: Old: %d, New: %d - rates: %f -> %f\n",tcNew, markovInternals->currentClass, classNew, markovInternals->currentRate,rateNew);
		return 0;	//class change does not correspond to a change of tc
	}


	libBPMcMatrix distribution;	//predicted state distribution matrix

	//determine penalty time:
	//compute the time the new state needs to last at least for to make an update pay off
	double penaltyTime = libBPMcStrategyMarkov_getPenaltyTime(interface,markovInternals->currentConnectionInterval, tcNew,markovInternals->currentNSeq,nSeqNew,markovInternals->bufOut);
	LIBBPMC_DEBUG("For new class %d: penalty time: %f - residence duration:%f\n",classNew, penaltyTime,libBPMcTrafficMeasurement_getAvgResidenceDuration(&(markovInternals->trm),classNew));



	//check if the average residence duration is longer than the penalty time
	//	 LIBBPMC_DEBUG("[rate increase] Determining: Reduce connection interval %d -> %d? Avg class duration is %f s, penalty Time is %f s",tcOld, tcNew,libBPMcTrafficMeasurement_getAvgResidenceDuration(&(markovInternals->trm),classNew),penaltyTime);
	if(libBPMcTrafficMeasurement_getAvgResidenceDuration(&(markovInternals->trm),classNew) > penaltyTime){
		//update!
		*nextClass = classNew;
		*nextRate = libBPMcTrafficMeasurement_getTrafficInClass(&(markovInternals->trm),classNew, CLASSTRAFFIC_RELATION_MAX);
		LIBBPMC_DEBUG("=> Result: New state seems to be stable. Reducing interval from %d to %d?",tcOld, tcNew);
		markovInternals->recurrenceCnt = 0;
		return 1;		//residence duration overcomes penalty


	}else{
		//do not update
		return 0;



		printf("=> New State unstable. Traveling along the Markov chain.\n");
		//travel along states until  either 1) a stable state has been reached, or 2) The average traffic pays off and can be treatened as a stable starte, or 3) Predicting gets unreliable
		tmpClass = classNew;
		accProbab = 1;
		sumOfResidenceDur = libBPMcTrafficMeasurement_getAvgResidenceDuration(&(markovInternals->trm),tmpClass);
		tmpAvgRate = libBPMcTrafficMeasurement_getTrafficInClass(&(markovInternals->trm),tmpClass,CLASSTRAFFIC_RELATION_MAX) * libBPMcTrafficMeasurement_getAvgResidenceDuration(&(markovInternals->trm),tmpClass);
		printf("initial class of travel: %d\n",tmpClass);
		while(1){
			tmpClass = libBPMcTrafficAnalysis_getMostLikelySuccessor(&(markovInternals->trm),tmpClass,&tmpTransProbab);
			printf("==> most likely successor: %d with probability %f,tmpClass\n",tmpClass, tmpTransProbab);
			accProbab = tmpTransProbab;
			sumOfResidenceDur += libBPMcTrafficMeasurement_getAvgResidenceDuration(&(markovInternals->trm),tmpClass);
			tmpAvgRate += libBPMcTrafficMeasurement_getTrafficInClass(&(markovInternals->trm),tmpClass,CLASSTRAFFIC_RELATION_MAX) * libBPMcTrafficMeasurement_getAvgResidenceDuration(&(markovInternals->trm),tmpClass);


			//Recurrence
/*
			if(tmpClass == classNew){
				expectedNRepetitions = (accProbab  / (1.0 - accProbab));
				expectedTimeInRecurrence = (expectedNRepetitions - 1)*(sumOfClassTimes);
				printf(" => Recurrence detected. Expected instances: %d, expected time: %f\n",expectedNRepetitions, expectedTimeInRecurrence);
				if(penaltyTime < expectedTimeInRecurrence + sumOfResidenceDur){
							*nextClass = libBPMcTrafficMeasurement_determineClass(&(markovInternals->trm),tmpAvgRate/sumOfResidenceDur);
							*nextRate = libBPMcTrafficMeasurement_getTrafficInClass(&(markovInternals->trm),*nextClass,CLASSTRAFFIC_RELATION_MAX);

							printf(" => Average class seems stable => updating to average class %d (%f by/s ideal %f by/s in class)",*nextClass,tmpAvgRate/sumOfResidenceDur, *nextRate);
							return 1;
				}else{

					return 0;
				}
	}
*/

			//The Traffic is unpredictable.
			//We do not update - in case our buffer gets to full, the emergency procedure will prevent us from messing the buffer
			if(accProbab < 0.5){
				printf(" => No prediction can be made => Not updating");
				return 0;
			}


			//check if average state is stable enough => go to avg

			tcTmp = libBPMcRateCalc_rate2TcAvg(libBPMcTrafficMeasurement_determineClass(&(markovInternals->trm),tmpAvgRate/sumOfResidenceDur),nSeqNew);
			penaltyTime = libBPMcStrategyMarkov_getPenaltyTime(interface,markovInternals->currentConnectionInterval, tcTmp,markovInternals->currentNSeq,nSeqNew,markovInternals->bufOut);

			if(penaltyTime < sumOfResidenceDur){
				*nextClass = libBPMcTrafficMeasurement_determineClass(&(markovInternals->trm),tmpAvgRate/sumOfResidenceDur);
				*nextRate = libBPMcTrafficMeasurement_getTrafficInClass(&(markovInternals->trm),*nextClass,CLASSTRAFFIC_RELATION_MAX);

				printf(" => Average class seems stable => updating to average class %d (%f by/s ideal %f by/s in class)",*nextClass,tmpAvgRate/sumOfResidenceDur, *nextRate);
				return 1;
			}




			//update Penalty time and check if the current tmpClass is stable
			tcTmp = libBPMcRateCalc_rate2TcAvg(libBPMcTrafficMeasurement_getTrafficInClass(&(markovInternals->trm),tmpClass,CLASSTRAFFIC_RELATION_MAX),nSeqNew);
			penaltyTime = libBPMcStrategyMarkov_getPenaltyTime(interface,markovInternals->currentConnectionInterval, tcTmp,markovInternals->currentNSeq,nSeqNew,markovInternals->bufOut);
			if(penaltyTime < libBPMcTrafficMeasurement_getAvgResidenceDuration(&(markovInternals->trm),tmpClass)){
				printf(" => Resulting class seems stable => updating to average class %d (%f by/s in class)",tmpClass,libBPMcTrafficMeasurement_getTrafficInClass(&(markovInternals->trm),tmpClass,CLASSTRAFFIC_RELATION_MAX));

				*nextClass = libBPMcTrafficMeasurement_determineClass(&(markovInternals->trm),tmpAvgRate/sumOfResidenceDur);
				*nextRate = libBPMcTrafficMeasurement_getTrafficInClass(&(markovInternals->trm),tmpClass,CLASSTRAFFIC_RELATION_MAX);
				return 1;
			}

		}
	}
		/*
			 return 0;
//			 LIBBPMC_DEBUG("[rate increase] Result: New state instable - no changes!");

				libBPMcTrafficAnalysis_getStateDistributionAfterTime(&(markovInternals->trm),penaltyTime,&distribution,classNew,1);
		 *nextClass = libBPMcTrafficAnalysis_getNextStableClass(&(markovInternals->trm),&distribution,LIBBPMC_STRATEGY_MARKOV_STABLE_STATE_EPSILON,penaltyTime);
				if(markovInternals->currentClass == *nextClass){
					LIBBPMC_DEBUG("[rate increase] Result: transient will end in same state. Not changing state.");
				}
				if(*nextClass >= 0){
		 *nextRate = libBPMcTrafficMeasurement_getTrafficInClass(&(markovInternals->trm),*nextClass, CLASSTRAFFIC_RELATION_MAX);
					 libBPMcMatrix_destroy(&distribution);
					 LIBBPMC_DEBUG("[rate increase] Result: Transient state with stable end. Choosing different class. New class is %d instead of %d, rate is %f instead of %f",*nextClass, classNew, rateNew, *nextRate);
					 return 1;
				}else{
		 *nextRate = libBPMcTrafficAnalysis_getTrafficPrediction(&(markovInternals->trm),&distribution,CLASSTRAFFIC_RELATION_AVG);
					 libBPMcMatrix_destroy(&distribution);
		 *nextClass = libBPMcTrafficMeasurement_determineClass(&(markovInternals->trm),*nextRate);
					if(markovInternals->currentClass == *nextClass){
						LIBBPMC_DEBUG("[rate increase] Result: AVG traffic remains the same. Not changing state.");
						return 0;
					}

					 LIBBPMC_DEBUG("[rate increase] Result: Recurrent states. Choosing average class. New class is %d instead of %d, rate is %f instead of %f",*nextClass, classNew, *nextRate, rateNew);
					 return 1;
				}
		 */

}



void libBPMcStrategyMarkov_determinationThreadFunc(void* param){
	double resultRate;
	uint32_t resultClass;
	uint32_t tcNew;
	double nSeqNew;
	uint32_t currentState;
	libBPMcInterface* interface = (libBPMcInterface*) param;
		libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;
		while(1){
			virThread_sema_lock(&markovInternals->determinationSema);
			if(libBPMcStrategyMarkov_determineConnectionIntervalUpdate(interface,markovInternals->currentAppRate,&resultRate, &resultClass)){
				printf("==== ResultRate = %f\n",resultRate);
				libBPMcStrategyMarkov_getConnectionParams(interface,resultRate,&tcNew,&nSeqNew);
						printf("Resulting rate for update is %f => tcNew = %d, nSeqNew = %f\n",resultRate, tcNew, nSeqNew);
					//set new connection interval
					//curretState = libBPMcTrafficMeasurement_determineClass(&(markovInternals->trm),markovInternals->currentRate)
					if(interface->active){
						if(libBPMcInterface_callSetConIntCallBack(interface,tcNew, nSeqNew)){
							//LIBBPMC_DEBUG("Update sccessful.\n");
							//LIBBPMC_DEBUG("-new connection parameters apply.-");
							//LIBBPMC_DEBUG("\t tc=%d, rate = %f, class = %d, nSeq = %f",tcNew,resultRate,resultClass, nSeqNew);
							markovInternals->currentConnectionInterval = tcNew;
							markovInternals->currentNSeq = nSeqNew;
							markovInternals->currentClass = resultClass;
							markovInternals->currentRate = resultRate;
							markovInternals->updatePending = 0;

						}else{
							LIBBPMC_DEBUG("Update denied.\n");
							markovInternals->updatePending = 1;
						}
					}else{
						LIBBPMC_DEBUG("Interface inactive.\n");
					}
				}
		}
}
void libBPMcStrategyMarkov_addOut(libBPMcInterface* interface, libBPMcTraffic tr){
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;
	double duration;

	libBPMcTrafficMeasurement_add(&(markovInternals->trm),tr);
	//determine mean time between writes
	if(markovInternals->lastTimeStamp.tv_sec != 0){

		duration = libBPMc_Traffic_getTimeDifference(markovInternals->lastTimeStamp,tr.time);
		markovInternals->currentAppRate = (double)tr.nBytes / duration;
		virThread_sema_unlock(&markovInternals->determinationSema);			//trigger a determination
//		LIBBPMC_DEBUG("ADD(): %d bytes, duration since last: %f, rate: %f, free space in Buffer: %d bytes\n",tr.nBytes, duration, tr.nBytes / duration,libBPMcBuffer_getBytesLeft(markovInternals->bufIn));
//		markovInternals->meanTimeBetweenWrites = (1.0-markovInternals->exponSmoothingFactor) * markovInternals->meanTimeBetweenWrites;
//		markovInternals->meanTimeBetweenWrites += (markovInternals->exponSmoothingFactor) * duration;
		//	LIBBPMC_DEBUG("Time between writes: %f s - mean Time between writes: %f",markovInternals->meanTimeBetweenWrites);



	}
	markovInternals->lastTimeStamp = tr.time;
}


void libBPMcStrategyMarkov_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut){
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;
	markovInternals->bufIn = bufIn;
	markovInternals->bufOut = bufOut;
}

void libBPMcStrategyMarkov_destory(libBPMcInterface* interface){
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;
	libBPMcTrafficMeasurement_destroy(&(markovInternals->trm));
	free(markovInternals);
	free(interface);
	LIBBPMC_DEBUG("Markov strategy destroyed.\n");
}


void libBPMcStrategyMarkov_getIdent(libBPMcInterface* interface,char* ident){
	strcpy(ident, "libBPMcStrategyMarkov");
}

char* libBPMcStrategyMarkov_getParamList(libBPMcInterface* interface){

	char* paramList = (char*) malloc(1000);
	strcpy(paramList,"");
	char* rv;
	char lineBuf[256];
	sprintf(lineBuf,"%s|%s|%d|%d\n","connectionIntervalMax","d",5,3200);
	strcat(paramList, lineBuf);
	rv = strdup(paramList);
	free(paramList);
	return rv;
}

uint8_t libBPMcStrategyMarkov_setParam(libBPMcInterface* interface,const char* param, const char* value){
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;

	if(strcmp(param,"connectionIntervalMax")==0){
		LIBBPMC_DEBUG("setting param connectionIntervalMax to %f,\n",atof(value));
		markovInternals->connectionIntervalMax = atof(value);
	}else if(strcmp(param,"displayMatrices")==0){
		printf("Class probability Matrix:\n");
		libBPMcTrafficMeasurement_printTransitionMatrix(&markovInternals->trm,TRANSITION_PROBAB_CLASSES);
		printf("Transition rate Matrix:\n");
		libBPMcTrafficMeasurement_printTransitionMatrix(&markovInternals->trm,TRANSITION_PROBAB_TRANSRATE);

	}else{
		return 0;		//failure
	}
	return 1;
}

uint8_t libBPMcStrategyMarkov_getParam(libBPMcInterface* interface,const char* param, char* value){
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;

	if(value == NULL){
		return 0;	//error
	}
	if(strcmp(param,"connectionIntervalMax")==0){
		LIBBPMC_DEBUG(value, "%d",markovInternals->connectionIntervalMax);
	}else{
		return 0;		//failure
	}
	return 1;

}

void libBPMcStrategyMarkov_setActivation(libBPMcInterface* interface, uint8_t active){
	interface->active = active;
}

void libBPMcStrategyMarkov_getConnectionParams(libBPMcInterface* interface, double rate, uint32_t *tc, double *nSeq){
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;

	*nSeq = LIBBPMC_STRATEGY_MARKOV_BLE_NSEQ_MAX;

	*tc = libBPMcRateCalc_rate2TcAvg(rate,*nSeq) - LIBBPMC_STRATEGY_MARKOV_TC_SAFETY_MARGIN;
	if(*tc < 6){
		*tc = 6;
	}

	/*
	//adjustment for very low rates
	if(*tc > markovInternals->connectionIntervalMax){
		*tc = markovInternals->connectionIntervalMax;
		*nSeq = ceil((rate * (double) *tc * 0.00125) / 20.0);
		if(*nSeq > LIBBPMC_STRATEGY_MARKOV_BLE_NSEQ_MAX){
			*nSeq = LIBBPMC_STRATEGY_MARKOV_BLE_NSEQ_MAX;
		}
	}
	*/

}

double libBPMcStrategyMarkov_getPenaltyTime(libBPMcInterface* interface, uint32_t tcOld, uint32_t tcNew, double nSeqOld, double nSeqNew, libBPMcBuffer* bufOut){
	uint32_t cntOld;
	uint32_t cntNew;
	double rv;
	double qTcOld, qTcNew,qUpdate;
	libBPMcStrageyMarkovInternals *markovInternals = (libBPMcStrageyMarkovInternals*) interface->internalData;


	//charge consumed without connection update per connection interval
 	 qTcOld = ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcOld*0.00125,nSeqOld,10,37,LIBBPMC_STRATEGY_MARKOV_BLE_TXPOWER,-1);			//master
     qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcOld*0.00125,nSeqOld,37,10,LIBBPMC_STRATEGY_MARKOV_BLE_TXPOWER,-1);		//slave
 	 qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcOld*0.00125,nSeqOld,15,10,LIBBPMC_STRATEGY_MARKOV_BLE_TXPOWER,-1);			//master
     qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcOld*0.00125,nSeqOld,10,15,LIBBPMC_STRATEGY_MARKOV_BLE_TXPOWER,-1);		//slave
     qTcOld /= 2.0;

	//charge consumed when updating the connection interval
	 qTcNew = ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcNew*0.00125,nSeqNew,10,37,LIBBPMC_STRATEGY_MARKOV_BLE_TXPOWER,-1);		//master transmission
	 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcNew*0.00125,nSeqNew,37,10,LIBBPMC_STRATEGY_MARKOV_BLE_TXPOWER,-1);		//slave transmission
	 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcNew*0.00125,nSeqNew,15,10,LIBBPMC_STRATEGY_MARKOV_BLE_TXPOWER,-1);		//master transmission
	 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcNew*0.00125,nSeqNew,10,15,LIBBPMC_STRATEGY_MARKOV_BLE_TXPOWER,-1);		//slave transmission
	 qTcNew /= 2.0;

	 //update cost
	 qUpdate = ble_e_model_ce_getChargeForConnectionProcedure(0,SC_SCAN_TYPE_PERIODIC,1,tcOld*0.00125,tcNew*0.00125,BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY);
     qUpdate += ble_e_model_ce_getChargeForConnectionProcedure(0,SC_SCAN_TYPE_PERIODIC,0,tcOld*0.00125,tcNew*0.00125,BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY);

     //	LIBBPMC_DEBUG("QUpdate master+slave: %f\n",qUpdate);
	 //	LIBBPMC_DEBUG("Intervals: %d -> %d \t Charges: qTcOld = %f, qTcNew = %f, qUpdate = %f\n",tcOld, tcNew, qTcOld, qTcNew, qUpdate);




	//Rate Increase (Interval gets smaller)
	if(tcNew < tcOld){
		printf("Detected: Rate increase (= connection interval gets smaller)\n");
		// The new connection interval is to be shorter than the old one (Rate increase)
		// The only reason why to follow this is that it would fill the buffer over Nb,en

		//					Nb,f					-		Rmax * NyWc * Ni,l * Tc,o 																														  		    - 		 Nw 			   -						Nwnd
		//double Nbem = libBPMcBuffer_getBytesLeft(bufOut) - markovInternals->maxTraffic * (1.0 - LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION) * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * tcOld * 0.00125 - LIBBPMC_PARAMS_IO_R_LENGTH - LIBBPMC_PARAM_WORST_CASE_WINDOW * markovInternals->maxTraffic;
		//rv = Nbem / (LIBBPMC_PARAM_AVERAGE_THROUGHPUT_DEGENERATION_RELATIVE * 20.0 * (nSeqNew/((double) tcNew * 0.00125) - nSeqOld/((double) tcOld * 0.00125)));

		//old - as in notes
		//rv = (libBPMcBuffer_getBytesLeft(bufOut) - LIBBPMC_PARAMS_IO_R_LENGTH - LIBBPMC_PARAM_WORST_CASE_WINDOW * markovInternals->maxTraffic)/(markovInternals->maxTraffic - 20.0*nSeqOld * (1.0 - LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION)/(tcOld * 0.00125)) - LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * tcOld * 0.00125;

		//experiment: never reach emergency level
		uint32_t NbEm = round(libBPMcBuffer_getSize(bufOut) - LIBBPMC_PARAMS_IO_R_LENGTH - LIBBPMC_PARAM_WORST_CASE_WINDOW * markovInternals->maxTraffic - 2.0 *  LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * markovInternals->connectionIntervalMax * 0.00125 * (markovInternals->maxTraffic - 20.0*markovInternals->currentNSeq * (1.0- LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION)/(markovInternals->connectionIntervalMax*0.00125)));
		printf("-Emergency level:%u/%u \n",NbEm,libBPMcBuffer_getSize(bufOut));
		rv = (NbEm  - libBPMcBuffer_getBytesUsed(bufOut) - LIBBPMC_PARAMS_IO_R_LENGTH - LIBBPMC_PARAM_WORST_CASE_WINDOW * markovInternals->maxTraffic)/(markovInternals->maxTraffic - 20.0*nSeqOld * (1.0 - LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION)/(tcOld * 0.00125)) - LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * tcOld * 0.00125;
		printf("---->Penalty time: %f\n",rv);

		if(rv < 0){
			return 0;
		}
	//rate decrease
	}else{
		printf("Detected: Rate decrease (= connection interval gets larger)\n");

		//approx.
		rv = qUpdate / (qTcOld/(tcOld * 0.00125) - qTcNew/(tcNew*0.00125));

	}
	if(rv < (double) tcNew * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * 0.00125){
		printf("increasing penalty to intervals until update * Tc\n");
		rv = (double) tcNew * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * 0.00125;
	}
	//LIBBPMC_DEBUG("- penalty final: %f\n",rv);

	return rv;
	/*
	//connection interval is to be increased => energy can be saved as soon as the following inequation is fulfilled:
		//floor(tPenalty/(tcOld*0.00125)) * qTcOld > qUpdateCost + floor(tPenalty/(qTcNew*0.00125)) * qTcNew;
		//this inequation is solved iteratively (2DO: better solution]
		rv = 0.0;
		cntOld = 0;
		cntNew = 0;
		while(1){

			if(floor(rv/(tcOld*0.00125)) * qTcOld > qUpdateCost + floor(rv/(tcNew * 0.00125))*qTcNew){
				return rv;
			}
			cntOld = floor(rv/(tcOld * 0.00125));
			cntNew = floor(rv/(tcNew * 0.00125));

			if(((cntOld + 1.0) * tcOld * 0.00125) <= ((cntNew + 1.0) * (double) tcNew * 0.00125)){
				rv = (cntOld + 1.0) * tcOld * 0.00125;
			}else{
				rv = (cntNew + 1.0) * tcNew * 0.00125;
			}

		}



	}
	*/
}
