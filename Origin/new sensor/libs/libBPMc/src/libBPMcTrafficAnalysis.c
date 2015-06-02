/*
 * libBPMcTrafficAnalysis.c
 *
 *  Created on: 31.10.2013
 *      Author: kindt
 */



#include "libBPMcTrafficAnalysis.h"
#include "libBPMcDebug.h"
int32_t libBPMcTrafficAnalysis_getNextStableClass(libBPMcTrafficMeasurement* m,libBPMcMatrix *distribution, double epsilon, double penaltyTime){
	uint32_t cnt = 0;
	double maxProbab = 0.0;
	if((distribution->y != 1)||(distribution->x != libBPMcTrafficMeasurement_getNClasses(m))){
		LIBBPMC_DEBUG("Dimension missmatch => no next stable state can be classified\n");
		return -1;
	}
	maxProbab = 0.0;
	uint32_t maxState;
	for(cnt = 0; cnt < libBPMcTrafficMeasurement_getNClasses(m); cnt++){
		if(distribution->data[0][cnt] > epsilon){
			if(distribution->data[0][cnt] > maxProbab){
				maxProbab = distribution->data[0][cnt];
				maxState = cnt;
			}
		}
	}
	if(maxProbab >= epsilon){
		if(libBPMcTrafficMeasurement_getAvgResidenceDuration(m,maxState) > penaltyTime){
			return maxState;
		}else{
			return -1.0;
		}
	}else{
		return -1.0;
	}
}

uint32_t libBPMcTrafficAnalysis_getMostLikelySuccessor(libBPMcTrafficMeasurement* trm, uint32_t startClass, double* transProbab){
	uint32_t cnt;
	libBPMcMatrix probabMatrixTransRate;
	libBPMcMatrix probabMatrixClass;
	libBPMcTrafficMeasurement_getProbabilityMatrix(trm,TRANSITION_PROBAB_TRANSRATE,&probabMatrixTransRate,0,1);
	libBPMcTrafficMeasurement_getProbabilityMatrix(trm,TRANSITION_PROBAB_CLASSES,&probabMatrixClass,0,1);
	double maxProbab = 0.0;
	uint32_t maxIndex = trm->nClasses + 1;
	for(cnt = 0; cnt < trm->nClasses; cnt++){
		if(startClass != cnt){
			if(probabMatrixClass.data[startClass][cnt] > maxProbab){
				maxIndex = cnt;
				maxProbab = probabMatrixClass.data[startClass][cnt];
			}
		}
	}
	if(transProbab != NULL){
		*transProbab = maxProbab;
	}
	libBPMcMatrix_destroy(&probabMatrixTransRate);
	libBPMcMatrix_destroy(&probabMatrixClass);
	return maxIndex;
}

uint8_t libBPMcTrafficAnalysis_checkRecurrenceHeuristic(libBPMcTrafficMeasurement* trm, uint32_t startClass,uint32_t classBefore, uint32_t maxSteps, int32_t* classDest, double* avgTraffic,double* recurrenceEquivalentResidenceDuration,double* expectedRecurrenceRepetitions, uint32_t *recurrenceNClasses, libBPMcTrafficMeasurement_classTrafficRelation rel){

	uint32_t currentClass = startClass;
	libBPMcMatrix probabMatrixTransRate;
	libBPMcMatrix probabMatrixClass;
	libBPMcTrafficMeasurement_getProbabilityMatrix(trm,TRANSITION_PROBAB_TRANSRATE,&probabMatrixTransRate,0,1);
	libBPMcTrafficMeasurement_getProbabilityMatrix(trm,TRANSITION_PROBAB_CLASSES,&probabMatrixClass,0,1);
	double sumOfClassTimes = 0;
	double pathAccProbability = 1.0;
	double expectedTimeInRecurrence = 0;
	double avgRate = 0;
	double expectedNRepetitions;
	uint32_t step = 0;
	uint32_t stepMade = 0;
	uint32_t cnt;
	LIBBPMC_DEBUG("===Starting recurrency check starting from class %d (%f by/s) - until returning to class %d ===",startClass,libBPMcTrafficMeasurement_getTrafficInClass(trm,startClass,rel),classBefore);
	while(1){


		stepMade = 0;
		//check for a probability > 0.5
		for(cnt = 0; cnt < trm->nClasses; cnt++){
			if(currentClass != cnt){
				if(probabMatrixClass.data[currentClass][cnt] > 0.5){
					//Transition with probability > 0.5 found => make transition
					sumOfClassTimes += libBPMcTrafficMeasurement_getAvgResidenceDuration(trm,currentClass);
					pathAccProbability *= probabMatrixClass.data[currentClass][cnt];
					avgRate += libBPMcTrafficMeasurement_getTrafficInClass(trm,currentClass,rel) * libBPMcTrafficMeasurement_getAvgResidenceDuration(trm,currentClass);
					currentClass = cnt;
					printf("---> new Class: %d @ step %d, sumOfClassTimes = %f s, pathAccProb = %f\n",cnt,step,sumOfClassTimes, pathAccProbability);
					step++;
					stepMade = 1;
					break;
				}
			}
		}

		//Abortion criteria

		//1. No Step made => No probability > 0.5
		if(stepMade == 0){
			LIBBPMC_DEBUG("No singlestep probability > 0.5  at step %d => No recurrency situation",step);
			libBPMcMatrix_destroy(&probabMatrixTransRate);
			libBPMcMatrix_destroy(&probabMatrixClass);
			return 0;
		}

		//2. Overall path probabilty < 0.5
		if(pathAccProbability < 0.5){
			LIBBPMC_DEBUG("Accumulated path probabilty fallen below 0.5 at step %d / class %d (%f by/s) => No recurrent situation",step,currentClass,libBPMcTrafficMeasurement_getTrafficInClass(trm,currentClass,rel));
			libBPMcMatrix_destroy(&probabMatrixTransRate);
			libBPMcMatrix_destroy(&probabMatrixClass);
			return 0;
		}

		//3. Maximum number of steps traveled
		if(step > maxSteps){
			LIBBPMC_DEBUG("Maximum Number of steps traveled without recurrence");
			libBPMcMatrix_destroy(&probabMatrixTransRate);
			libBPMcMatrix_destroy(&probabMatrixClass);
			return 0;
		}

		//4. Recurrence detected
		if(currentClass == classBefore){
			printf("\t\t[R]\n");
			LIBBPMC_DEBUG("Recurrency detected in state %d after %d steps\n",currentClass,step);
			//compute the expected time the set of recurring states will stay
			expectedNRepetitions = (pathAccProbability  / (1.0 - pathAccProbability));
			expectedTimeInRecurrence = (expectedNRepetitions)*(sumOfClassTimes);

			avgRate /= sumOfClassTimes;

			if(recurrenceEquivalentResidenceDuration != NULL){
				*recurrenceEquivalentResidenceDuration = expectedTimeInRecurrence;
			};
			if(expectedRecurrenceRepetitions != NULL){
				*expectedRecurrenceRepetitions = pathAccProbability  / (1 - pathAccProbability);
			}
			if(recurrenceNClasses != NULL){
				*recurrenceNClasses = step;
			}

			if(avgTraffic != NULL){

				*avgTraffic = avgRate;
			}
			LIBBPMC_DEBUG("Expected time in recurrence is %f seconds / %f repetitions",expectedTimeInRecurrence,expectedNRepetitions);
			libBPMcMatrix_destroy(&probabMatrixTransRate);
			libBPMcMatrix_destroy(&probabMatrixClass);
			if(*classDest != NULL){
				*classDest = libBPMcTrafficMeasurement_determineClass(trm,avgRate);
			}

			return 1;
		}


	}
}
void libBPMcTrafficAnalysis_getStateDistributionAfterTime(libBPMcTrafficMeasurement *trm,double time, libBPMcMatrix *distribution,uint32_t currentState, uint8_t init){
	libBPMcMatrix mCurrentState;
	libBPMcMatrix intensityMatrix;
	libBPMcMatrix tmp1,tmp2;
	if(currentState >= libBPMcTrafficMeasurement_getNClasses(trm)){
		LIBBPMC_DEBUG("ERROR: currentState = %d > trm->nClasses=%d\n",currentState,libBPMcTrafficMeasurement_getNClasses(trm));
		return;
	}
	libBPMcMatrix_init(&mCurrentState,1,libBPMcTrafficMeasurement_getNClasses(trm));
	libBPMcMatrix_setToZero(&mCurrentState);
	mCurrentState.data[0][currentState] = 1.0;
	libBPMcTrafficMeasurement_getProbabilityMatrix(trm,TRANSITION_PROBAB_TRANSRATE,&intensityMatrix,0,1);
	libBPMcMatrix_multByScalar(&intensityMatrix,&tmp1,time,1);
	//printf("-TMP1:\n-");
	//libBPMcMatrix_print(&tmp1);

		libBPMcMatrix_matExpTaylor(&tmp1,&tmp2,LIBBPMCTRAFFICANALYSIS_TAYLOR_SERIES_FACTOR * libBPMcTrafficMeasurement_getNClasses(trm),1);
	//libBPMcMatrix_matExpTaylor(&tmp1,&tmp2,100,1);
	//printf("-probab mtrx:\n-");
	libBPMcMatrix_print(&tmp2);

	libBPMcMatrix_matMult(&mCurrentState,&tmp2,distribution,init);
	libBPMcMatrix_destroy(&mCurrentState);
	libBPMcMatrix_destroy(&intensityMatrix);
	libBPMcMatrix_destroy(&tmp1);
	libBPMcMatrix_destroy(&tmp2);
}


double libBPMcTrafficAnalysis_getTrafficPrediction(libBPMcTrafficMeasurement *trm, libBPMcMatrix *distribution,libBPMcTrafficMeasurement_classTrafficRelation rel){
	uint32_t trClass;
	double prediction;

	prediction = 0.0;
	if((libBPMcTrafficMeasurement_getNClasses(trm) == 0)||(libBPMcTrafficMeasurement_getNClasses(trm) != distribution->x)||(distribution->y != 1)){
		LIBBPMC_DEBUG("Matrix dimension error. Condition must be fulfilled: distriution->x == trm->nClasses && distribution->y == 1");
		return -1.0;
	}

	for(trClass = 0; trClass < distribution->x; trClass++){
		prediction += distribution->data[0][trClass] * libBPMcTrafficMeasurement_getTrafficInClass(trm,trClass,rel);
	}
	return prediction;
}


