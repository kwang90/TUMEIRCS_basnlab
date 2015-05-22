/**
 * libBPMcTrafficMeasurement.h
 *
 *  Created on: 25.10.2013
 *      Author: kindt
 */

#ifndef LIBBPMCTRAFFICMEASUREMENT_H_
#define LIBBPMCTRAFFICMEASUREMENT_H_

#include <inttypes.h>
#include <time.h>
#include "libBPMcTraffic.h"
#include "libBPMcMatrix.h"
typedef enum _libBPMcTrafficMeasurementTransitionProbabType {TRANSITION_PROBAB_CLASSES,TRANSITION_PROBAB_TRANSRATE} libBPMcTrafficMeasurementTransitionProbabType;
typedef enum _libBPMcTrafficMeasurement_classTrafficRelation{CLASSTRAFFIC_RELATION_AVG, CLASSTRAFFIC_RELATION_MIN, CLASSTRAFFIC_RELATION_MAX} libBPMcTrafficMeasurement_classTrafficRelation;

typedef struct _libBPMcTrafficMeasurement{
	uint32_t nClasses;					//the number of classes
	double* classBorders;				//the upper class borders that seperate each class [0] class 0 [Border 0] class 1 [border 1] ...
	uint32_t *transitionCntClassWhole;
	uint32_t **transitionCntClass;
	uint32_t currentClass;
	uint32_t previousClass;
	struct timespec lastWriteTime;
	struct timespec lastClassChangeTime;
	uint8_t firstAdd;					//1, if the first relative add (i.e, the second add() in total) has been done
	uint32_t writeCnt;
	double* avgResidenceDuration;
} libBPMcTrafficMeasurement;

void libBPMcTrafficMeasurement_init(libBPMcTrafficMeasurement *trm, uint32_t nClasses);

void libBPMcTrafficMeasurement_destroy(libBPMcTrafficMeasurement * trm);

uint8_t libBPMcTrafficMeasurement_setClassBorder(libBPMcTrafficMeasurement *trm, uint32_t classNo, double border);
uint32_t libBPMcTrafficMeasurement_determineClass(libBPMcTrafficMeasurement *trm, double trafficRate);
uint8_t libBPMcTrafficMeasurement_add(libBPMcTrafficMeasurement *trm, libBPMcTraffic tr);
uint32_t libBPMcTrafficMeasurement_getNClasses(libBPMcTrafficMeasurement* trm);
double libBPMcTrafficMeasurement_getAvgResidenceDuration(libBPMcTrafficMeasurement *trm, uint32_t classNo);
void libBPMcTrafficMeasurement_printTransitionMatrix(libBPMcTrafficMeasurement* trm, libBPMcTrafficMeasurementTransitionProbabType type);
int32_t libBPMcTrafficMeasurement_getProbabilityMatrix(libBPMcTrafficMeasurement* trm, libBPMcTrafficMeasurementTransitionProbabType type, libBPMcMatrix* matrixResult, uint8_t intensity, uint8_t init);
double libBPMcTrafficMeasurement_getTransitionProbability(libBPMcTrafficMeasurement* trm, libBPMcTrafficMeasurementTransitionProbabType type, uint32_t classSrc, uint32_t classDest);
double libBPMcTrafficMeasurement_getTrafficInClass(libBPMcTrafficMeasurement* trm, uint32_t classNo, libBPMcTrafficMeasurement_classTrafficRelation rel);

#endif /* LIBBPMCTRAFFICMEASUREMENT_H_ */
