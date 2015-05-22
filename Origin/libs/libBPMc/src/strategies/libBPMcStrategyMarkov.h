/**
 * \file libBPMcStrategyMarkov.h
 * \brief Continuois time nased Markov strategy libBPMc
 *
 * Nov. 2013, Philipp Kindt
 *
 */

#ifndef LIBBPMCSTRATEGYMARKOV_H_
#define LIBBPMCSTRATEGYMARKOV_H_

#include "../libBPMcInterface.h"
#include "../libBPMcTraffic.h"
#include "../libBPMcTrafficMeasurement.h"
#include "../libBPMcTrafficAnalysis.h"
#include "../libBPMcParams.h"
#include "../libBPMcPlatform.h"
#include <virThread/virThread.h>
#include <time.h>
#if LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <FreeRTOSConfig.h>
#elif LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_POSIX

#else
ERROR - WRONG PLATTFORM ARCHITECTURE
#endif

#define LIBBPMC_STRATEGY_MARKOV_DEFAULT_NCLASSES 10
#define LIBBPMC_STRATEGY_MARKOV_DEFAULT_MAXTRAFFIC LIBBPMC_PARAM_DEFAULT_MAXTRAFFIC
#define LIBBPMC_STRATEGY_MARKOV_DEFAULT_SMOOTHING_FACTOR 1.0
#define LIBBPMC_STRATEGY_MARKOV_BLE_NSEQ_MAX LIBBPMC_PARAM_BLE_NSEQ_MAX
#define LIBBPMC_STRATEGY_MARKOV_BLE_TXPOWER 3
#define LIBBPMC_STRATEGY_MARKOV_STABLE_STATE_EPSILON 0.5
#define LIBBPMC_STRATEGY_MARKOV_RECURRENCE_HEURISTIC_MAXSTEPS 10
#define LIBBPMC_STRATEGY_MARKOV_MAX_ACCEPTED_BUFFERFILLING_ON_DECREASE 0.01
#define LIBBPMC_STRATEGY_MARKOV_TC_SAFETY_MARGIN LIBBPMC_PARAM_QUALITY_OF_SERVICE_TC_REDUCTION							//added to optimal connection interval

#if LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <FreeRTOS.h>
#define LIBBPMC_STRATEGY_MARKOV_FREERTOS_PRIORITY 0
#else
#define LIBBPMC_STRATEGY_MARKOV_FREERTOS_PRIORITY 0
#endif

#define LIBBPMC_STRATEGY_MARKOV_FREERTOS_STACK_SIZE 1000

///Internal data structure storing all information the PMS needs
typedef struct _libBPMcStrageyMarkovInternals{
	uint32_t nextConnectionInterval;		//the next connection interval this strategy is going to set as a multiple of 0.625 ms
	libBPMcBuffer *bufIn;					//pointer to the queue that contains all data that has been received but not been processed by the application
	libBPMcBuffer *bufOut;					//pointer to the queue that contains all data that is to be sent via BLE
	uint8_t masterOrSlave;					//role ot this PMS; 0=>slave, 1=>master
	uint32_t currentConnectionInterval;
	double currentNSeq;
	uint32_t connectionIntervalMax;			//the maximum connection interval this PMS will choose as a multiple of 0.625 ms
	double meanTimeBetweenWrites;			//the refresh frequency in ms
	double exponSmoothingFactor;
	libBPMcTrafficMeasurement trm;
	double maxTraffic;
	struct timespec lastTimeStamp;
	double currentRate;
	double currentAppRate;
	uint32_t currentClass;						// the current outgoing class. When an update is scheduled, the outgoing class differs from this value until the updated parameters become valid
	int32_t inGoingClassLastDetermination;		// the class when an update was determined previously
	virThread_thread_t determinationThreadHandle;
	virThread_sema_t determinationSema;
	uint32_t recurrenceCnt;						//If the system is in a recurrent situation, this variable contains the number number it has already repeated
	uint8_t updatePending;
} libBPMcStrageyMarkovInternals;


/**
 * \brief Initialize the PMS
 * This function is called by the application or SLI whenever the PMS is loaded. It is the first function that is called.
 * It will register all relevant callback functions and allocate space for internal data
 * @param interface Pointer to an initialized libBPMcInterface struct.
 *
 */
void libBPMcStrategyMarkov_init(libBPMcInterface* interface);

/**
 * \brief Called before the first call to \ref libBPMcStrategyMarkov_addOut() to inform the PMS about the initial connection interval that
 * has been set before the PMS has been active.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcInitial Initial connection interval in terms of multiples of 0.625 ms
 */
void libBPMcStrategyMarkov_startInfo(libBPMcInterface* interface, int32_t tcInitial,double nSeqInitial);

/**
 * \brief Called before the first call to \ref libBPMcStrategyMarkov_addOut() to inform the PMS about the latency constraints in terms of the
 * lowest (tcMin) and highest (tcMax) connection interval allowed.
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcMin Minimum connection interval allowed as multiples of 0.625 ms
 * @param tcMax Maximum connection interval allowed as multiples of 0.625 ms
 */
void libBPMcStrategyMarkov_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax);

/**
 * \brief Called before the first call to \ref libBPMcStrategyMarkov_addOut() to inform the PMS about it role (master or slave)
 * In Master mode, a call to \ref libBPMcInterface_callSetConIntCallBack() will cause a direct update of the connection interval ASAP.
 * In Slave mode, a call to \ref libBPMcInterface_callSetConIntCallBack() can have - depending on the implementation of the SLIs local and remote and on the remote PMS - the following effect:
 *  1) no effect
 *  2) a suggestion on how to update the connection interval is sent to the slave PMS. The mater's PMS will then choose its own connection interval or take the suggestion (reccomended)
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param masterOrSlave 1 => master Role, 0 => slave Role
 */
void libBPMcStrategyMarkov_setRole(libBPMcInterface* interface, uint8_t masterOrSlave);

/**
 * \brief Called whenever outgoing traffic is sent to the outgoing buffer.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tr Number of bytes sent along with a timestamp
 */
void libBPMcStrategyMarkov_addOut(libBPMcInterface* interface, libBPMcTraffic tr);

/**
 * \brief Called before the first call to \ref libBPMcStrategyMarkov_addOut()
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param bufIn Pointer to the buffer containing data received via the wireless link to be read by the application
 * @param bufOut Pointer to the buffer cotaining data written by the application to be sent by the application
 */
void libBPMcStrategyMarkov_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut);

/**
 * \brief This function is called when the PMS ist to be activated or disactivated. May happen anytime after the initialization, but will usually be
 * called after \ref libBPMcStrategyMarkov_startInfo() and \ref libBPMcStrategyMarkov_setBuffer() has been called
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param active 1 => Activate Power Management strategy; 0=> disactivate Power Management strategy
 */
void libBPMcStrategyMarkov_setActivation(libBPMcInterface* interface, uint8_t active);

/**
 * \brief Destructor
 * This function is called whenever the PMS is unloaded to allow for a safe destruction of the PMS
 * @param interface Pointer to initialized libBPMcInterface structure.
 */
void libBPMcStrategyMarkov_destory(libBPMcInterface* interface);

/**
 * \brief Copy the name of the PMS to a pre-allocated string.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * Max. length: 40 Bytes / 39 chars
 */
void libBPMcStrategyMarkov_getIdent(libBPMcInterface* interface,char* ident);

/**
 * \brief Allocates and returns a string containing a machine-readable description of all parameters offered.
 * See \ref libBPMcInterface_callGetParamListCallBack()
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @return
 */
char* libBPMcStrategyMarkov_getParamList(libBPMcInterface* interface);

/**
 * \brief Write a parameter value.
 * See \ref libBPMcInterface_callSetParamCallBack()
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param param Name of the parameter to be written
 * @param value New value of parameter to be written write
 * @return 0 => failure, 1=>success
 */
uint8_t libBPMcStrategyMarkov_setParam(libBPMcInterface* interface,const char* param, const char* value);

/**
 * \brief Read a parameter value.
 * See \ref libBPMcInterface_callGetParamCallBack()
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param param Name of the parameter
 * @param value Value of parameter
 * @return 0 => failure, 1=>success
 */
uint8_t libBPMcStrategyMarkov_getParam(libBPMcInterface* interface,const char* param, char* value);



/*----------------[ NON-INTERFACED FUNCTIONS]-----------------------------------------------------*/

void libBPMcStrategyMarkov_updateTrafficBounds(libBPMcInterface* interface, double minTraffic, double maximumTraffic);
void libBPMcStrategyMarkov_getConnectionParams(libBPMcInterface* interface, double rate, uint32_t *tc, double *nSeq);
double libBPMcStrategyMarkov_getPenaltyTime(libBPMcInterface* interface,uint32_t tcOld, uint32_t tcNew, double nSeqOld, double nSeqNew, libBPMcBuffer* bufOut);
uint8_t libBPMcStrategyMarkov_determineConnectionIntervalUpdate(libBPMcInterface* interface,double rateNew, double* nextRate, int32_t* nextClass);
void libBPMcStrategyMarkov_determinationThreadFunc(void* param);

#endif /* LIBBPMCSTRATEGYMARKOV_H_ */
