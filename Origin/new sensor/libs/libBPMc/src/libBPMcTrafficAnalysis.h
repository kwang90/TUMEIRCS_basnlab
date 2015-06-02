/**
 * \file libBPMcTrafficAnalysis.h
 * \brief Analyze traffic recorded by \ref libBPMcTrafficMeasurement.h
 *
 *  Created on: 28.10.2013
 *      Author: kindt
 */

#ifndef LIBBPMCTRAFFICANALYSIS_H_
#define LIBBPMCTRAFFICANALYSIS_H_


#include "libBPMcTrafficMeasurement.h"
#include "libBPMcTraffic.h"
#define LIBBPMCTRAFFICANALYSIS_TAYLOR_SERIES_FACTOR 20
/**
 * \brief Starting from a given distribution, get next stable traffic class
 *  If a starting distribution is given, this function gets the next, direct-reachable traffic class that
 *  <ul>
 *  <li> Has a class probability in the initial distribution greater than epsilon
 *  <li> Is the most likely direct successor of the current class
 *  <li> Has an average residence duration of at least penaltyTime
 *  </ul>
 * @param m libBPMcTrafficMeasurement class to work on
 * @param distribution Starting distribution. This is a matrix containing class probabilities. It has the dimension of the number of states x 1, and the sum of its elements are 1. Example: [0.4 0.6 0 0] with four states
 * @param epsilon Minimum class probability the a class of the original distribution needs to have to be considered
 * @param penaltyTime Average residence duration a successor class must have at least.
 * @return Number of class found, or -1 if no such class exists.
 */
int32_t libBPMcTrafficAnalysis_getNextStableClass(libBPMcTrafficMeasurement* m,libBPMcMatrix *distribution, double epsilon, double penaltyTime);

/**
 * \brief Makes a prediction on the future distribution.
 * From a given starting class , this function predicts the distribution after a given amount of time.
 * In this computation, all possible paths are examined.
 * As the computation involves a matrix exponential function, the computation is very complex and perhaps not suited for embedded MCUs.
 * @param trm libBPMcTrafficMeasurement class to work on
 * @param time Number of seconds to predict the future distribution after
 * @param distribution Resulting (predicted) distribution matrix of dimension {nStates x 1}, for example [0.5 0.1 0.1 0.3]
 * @param currentState The class at the beginning of the examination
 * @param init 1=> the distribution matrix will be newly created, 0=> the distribution matrix will not be created => the distribution parameter must be an allready allocated matrix having the right dimension.
 */
void libBPMcTrafficAnalysis_getStateDistributionAfterTime(libBPMcTrafficMeasurement *trm,double time, libBPMcMatrix *distribution,uint32_t currentState, uint8_t init);

/**
 * \brief estimates the average traffic of a given (future) class distribution.
 * This function calculates the estimated traffic of a given distribution, i.e. the traffic of each class multiplied by its likelyhood.
 * @param trm libBPMcTrafficMeasurement class to work on
 * @param distribution State distribution the average traffic should be calculated for. This is a matrix of dimension {nStates x 1}, for example [0.5 0.1 0.1 0.3]
 * @param rel A relation how the traffic of a class corresponds to its class. Can be minimum, average or maximum class traffic
 * @return
 */
double libBPMcTrafficAnalysis_getTrafficPrediction(libBPMcTrafficMeasurement *trm, libBPMcMatrix *distribution,libBPMcTrafficMeasurement_classTrafficRelation rel);

/**
 * \brief Checks, if a class is a recurrent class.
 * This is done by a heuristic that is computationally inexpensive that can be run on an embedded MCU. However, it follows just the path having the most likely transition probabilities from each class.
 * Hence, some recurrent situations are not detected.
 * @param trm libBPMcTrafficMeasurement class to work on
 * @param startClass The class to start the examination at
 * @param maxSteps Maximum number of class transitions examined
 * @param classDest If a stable class is reached, the class reached will be stored in this variable. If no stable class is reached, it is -1. Can also be NULL,
 * @param avgTraffic If recurrent states are detected, the average traffic of this recurrent situation is stored in this variable. Might also be NULL. Only defined if a recurrent situation has been detected.
 * @param recurrenceEquivalentResidenceDuration The equivalent residence duration (i.e., the duration of all states of recurrence summed up) is stored into this pointer. Only defined if a recurrent situation has been detected.  Might also be NULL.
 * @param expectedRecurrenceRepetitions The expected number of repetitions for this recurrence. This means: How often is the recurrent series of classes repeating? Example: Consider the classes 1 2 3 1 2 3 1 2 3 1 2 3 4 5 6, then, for the series 1,2,3, this parameter has the value 4.0. The duration of one single instance of all repeating class in a recurrence is defined by recurrenceEquivalentResidenceDuration / expectedRecurrenceRepetitions. Only defined if a recurrent situation has been detected. Might also be NULL.
 * @param recurrenceNClasses Number of classes the detected recurrence lasts. Example: Consider the series of classes 1 2 3 1 2 3 5 6 7, then this parameter has the value 3 because one period of the recurrence consists of 3 classes. Only defined if a recurrent situation has been detected. Might also be NULL.
 * @param rel A relation how the traffic of a class corresponds to its class. Can be minimum, average or maximum class traffic
 * @return 0=> Non-recurrent situation, 1=> recurrent situation
 */
uint8_t libBPMcTrafficAnalysis_checkRecurrenceHeuristic(libBPMcTrafficMeasurement* trm, uint32_t startClass,uint32_t classBefore, uint32_t maxSteps, int32_t* classDest, double* avgTraffic,double* recurrenceEquivalentResidenceDuration,double* expectedRecurrenceRepetitions, uint32_t *recurrenceNClasses, libBPMcTrafficMeasurement_classTrafficRelation rel);


uint32_t libBPMcTrafficAnalysis_getMostLikelySuccessor(libBPMcTrafficMeasurement* trm, uint32_t startClass, double* transProbab);

#endif /* LIBBPMCTRAFFICANALYSIS_H_ */
