/**
 * \file libBPMcStrategyBuffer.h
 * \brief Buffer for libBPMc Power Management Strategies (PMS)
 *
 * Oct. 2013, Philipp Kindt
 *
 * Buffer-Level based strategy
 */

#ifndef LIBBPMCSTRATEGYBUFFER_H_
#define LIBBPMCSTRATEGYBUFFER_H_

#include "../libBPMcInterface.h"
#include "../libBPMcTraffic.h"
#include <time.h>
#include <virThread.h>



///Default percentage for an increase
#define LIBBPMC_STRATEGY_BUFFER_LEVELINC_PERCENTAGE_DEFAULT "0.05"
///Default smoothing factor alpha
//#define LIBBPMC_STRATEGY_BUFFER_SMOOTHING_FACTOR_DEFAULT 0.5
#define LIBBPMC_STRATEGY_BUFFER_SMOOTHING_FACTOR_DEFAULT 0.9

//1 => use blocking time for rate decrease (=interval increase). 0=> Do not use blocking time
#define LIBBPMC_STRATEGY_BUFFER_USE_PENALTY_RATEDEC 1

//1 => Use whole buffer; 0=> Only use nBytesControl
#define LIBBPMC_STRATEGY_BUFFER_USE_WHOLE_BUFFER 1


//defaults for ecg
/*
#define LIBBPMC_STRATEGY_BUFFER_LEVELINC_PERCENTAGE_DEFAULT "0.2"
 #define LIBBPMC_STRATEGY_BUFFER_SMOOTHING_FACTOR_DEFAULT 0.9
 #define LIBBPMC_STRATEGY_BUFFER_USE_PENALTY_RATEINC 0
 #define LIBBPMC_STRATEGY_BUFFER_USE_PENALTY_RATEDEC 1

*/
#define LIBBPMC_STRATEGY_BUFFER_INTERVAL_SAFETY_MARGIN_DEFAULT (LIBBPMC_PARAM_QUALITY_OF_SERVICE_TC_REDUCTION)
#define LIBBPMC_STRATEGY_BUFFER_DECREASE_SUPPPRESSION_INTERVAL_STEPS 0
#define LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER 3
#define LIBBPMC_STRATEGY_BUFFER_INFOMESSAGES 0

#if LIBBPMC_STRATEGY_BUFFER_INFOMESSAGES == 1
#define LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE(...) fprintf(stderr,__VA_ARGS__)
#else
#define LIBBPMC_STRATEGY_BUFFER_INFOMESSAGE(...)
#endif
typedef enum _libBPMcStrategyBufferState_t{LIBBPMC_STRATEGY_BUFFER_STATE_NORMAL, LIBBPMC_STRATEGY_BUFFER_STATE_WAIT_FOR_INCREASE, LIBBPMC_STRATEGY_BUFFER_STATE_WAIT_FOR_DECREASE} libBPMcStrategyBufferState_t;
typedef enum _libBPMcStrategyBufferTask_t{LIBBPMC_STRATEGY_BUFFER_TASK_CHECK_OVERRUN, LIBBPMC_STRATEGY_BUFFER_TASK_CHECK_UNDERRUN} libBPMcStrategyBufferTask_t;

///Internal data structure storing all information the PMS needs
typedef struct _libBPMcBufferInternals{
	uint32_t currentConnectionInterval;
	double currentNSeq;
	double nextNSeq;
	libBPMcBuffer *bufIn;					///<pointer to the queue that contains all data that has been received but not been processed by the application
	libBPMcBuffer *bufOut;					///<pointer to the queue that contains all data that is to be sent via BLE
	uint32_t connectionIntervalMax;			///<the maximum connection interval this PMS will choose as a multiple of 0.625 ms
	uint8_t masterOrSlave;					///<role ot this PMS; 0=>slave, 1=>master
	double rateMin;
	double rateMax;

	uint32_t levelEmergency;
	double levelIncPercentage;
	double currentRate;
	uint32_t levelIncThresholdIncrease;
	uint32_t levelIncThreshold;
	uint32_t intervalSafetyMargin;
	uint32_t decreaseSuppression;
	struct timespec tLastUpdate;					///<the time the last update took place (took place means: the connection started using the new parameters )
	struct timespec tUpdateScheduled;				///<the time the last update was requested (scheduled)
	uint32_t bytesWrittenSinceLastUpdateScheduled;	///< the number of bytes since tUpdateScheduled
	double dNextUpdateAllowed;
	uint32_t levelLastUpdate;						///<the level of the buffer when the last update took place (really took place means: Connection works with new parameters)

	uint32_t bytesWritten;							///<number of bytes written

	struct timespec tLastAdd;						///<The time of the second last call to addOut()
	struct timespec tTask;							///<Time the processing/determination task get as its "now"

	libBPMcStrategyBufferTask_t	task; 				///<processing/determination task

	double avgRate;									///<The smoothed application data rate
	double smoothingFactor;							///< Factor (alpha) for exponential smoothing according to http://en.wikipedia.org/wiki/Exponential_smoothing : Rate(t) = alpha*Rate(t) + (1-alpha)*Rate(t-1)

	libBPMcStrategyBufferState_t state;				///< state, can be normal, decreasing or increasing

	virThread_mutex_t mutex;

	virThread_sema_t triggerSema;
	virThread_thread_t thread;
	uint8_t dynamicPayOff;							/// 0=> use static decreaseSuppresion field; 1=> use dynamic suppression
} libBPMcStrageyBufferInternals;


/**
 * \brief Initialize the PMS
 * This function is called by the application or SLI whenever the PMS is loaded. It is the first function that is called.
 * It will register all relevant callback functions and allocate space for internal data
 * @param interface Pointer to an initialized libBPMcInterface struct.
 *
 */
void libBPMcStrategyBuffer_init(libBPMcInterface* interface);

/**
 * \brief Called before the first call to \ref libBPMcStrategyBuffer_addOut() to inform the PMS about the initial connection interval that
 * has been set before the PMS has been active.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcInitial Initial connection interval in terms of multiples of 0.625 ms
 */
void libBPMcStrategyBuffer_startInfo(libBPMcInterface* interface, int32_t tcInitial,double nSeqInitial);

/**
 * \brief Called before the first call to \ref libBPMcStrategyBuffer_addOut() to inform the PMS about the latency constraints in terms of the
 * lowest (tcMin) and highest (tcMax) connection interval allowed.
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcMin Minimum connection interval allowed as multiples of 0.625 ms
 * @param tcMax Maximum connection interval allowed as multiples of 0.625 ms
 */
void libBPMcStrategyBuffer_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax);

/**
 * \brief Called by SLI to informa about the minumum and maxumum possible traffic rate that cold occurre
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param rateMin Minimum possible rate
 * @param rateMax Maximum possible rate
 */
void libBPMcStrategyBuffer_setTrafficBounds(libBPMcInterface* interface, double rateMin, double rateMax);


/**
 * \brief Called before the first call to \ref libBPMcStrategyBuffer_addOut() to inform the PMS about it role (master or slave)
 * In Master mode, a call to \ref libBPMcInterface_callSetConIntCallBack() will cause a direct update of the connection interval ASAP.
 * In Slave mode, a call to \ref libBPMcInterface_callSetConIntCallBack() can have - depending on the implementation of the SLIs local and remote and on the remote PMS - the following effect:
 *  1) no effect
 *  2) a suggestion on how to update the connection interval is sent to the slave PMS. The mater's PMS will then choose its own connection interval or take the suggestion (reccomended)
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param masterOrSlave 1 => master Role, 0 => slave Role
 */
void libBPMcStrategyBuffer_setRole(libBPMcInterface* interface, uint8_t masterOrSlave);

/**
 * \brief Called whenever outgoing traffic is sent to the outgoing buffer.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tr Number of bytes sent along with a timestamp
 */
void libBPMcStrategyBuffer_addOut(libBPMcInterface* interface, libBPMcTraffic tr);

/**
 * \brief Called whenever data is read fromt he outgoing buffer
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tr Number of bytes read along with a timestamp (time of reading)
 */
void libBPMcStrategyBuffer_readOut(libBPMcInterface* interface, libBPMcTraffic tr);

/**
 * \brief Called before the first call to \ref libBPMcStrategyBuffer_addOut()
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param bufIn Pointer to the buffer containing data received via the wireless link to be read by the application
 * @param bufOut Pointer to the buffer cotaining data written by the application to be sent by the application
 */
void libBPMcStrategyBuffer_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut);

/**
 * \brief This function is called when the PMS ist to be activated or disactivated. May happen anytime after the initialization, but will usually be
 * called after \ref libBPMcStrategyBuffer_startInfo() and \ref libBPMcStrategyBuffer_setBuffer() has been called
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param active 1 => Activate Power Management strategy; 0=> disactivate Power Management strategy
 */
void libBPMcStrategyBuffer_setActivation(libBPMcInterface* interface, uint8_t active);

/**
 * \brief Destructor
 * This function is called whenever the PMS is unloaded to allow for a safe destruction of the PMS
 * @param interface Pointer to initialized libBPMcInterface structure.
 */
void libBPMcStrategyBuffer_destory(libBPMcInterface* interface);

/**
 * \brief Copy the name of the PMS to a pre-allocated string.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * Max. length: 40 Bytes / 39 chars
 */
void libBPMcStrategyBuffer_getIdent(libBPMcInterface* interface,char* ident);

/**
 * \brief Allocates and returns a string containing a machine-readable description of all parameters offered.
 * See \ref libBPMcInterface_callGetParamListCallBack()
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @return
 */
char* libBPMcStrategyBuffer_getParamList(libBPMcInterface* interface);

/**
 * \brief Write a parameter value.
 * See \ref libBPMcInterface_callSetParamCallBack()
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param param Name of the parameter to be written
 * @param value New value of parameter to be written write
 * @return 0 => failure, 1=>success
 */
uint8_t libBPMcStrategyBuffer_setParam(libBPMcInterface* interface,const char* param, const char* value);

/**
 * \brief Read a parameter value.
 * See \ref libBPMcInterface_callGetParamCallBack()
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param param Name of the parameter
 * @param value Value of parameter
 * @return 0 => failure, 1=>success
 */
uint8_t libBPMcStrategyBuffer_getParam(libBPMcInterface* interface,const char* param, char* value);


/**
 * \brief called by SLI when a connection parameter update occured
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcNew Connection interval after update
 * @param latencyNew Slave latency after update
 * @param timeoutNew Super vision timeout after update
 */
void libBPMcStrategyBuffer_connectionUpdated(libBPMcInterface* interface, uint32_t tcNew, uint32_t latencyNew, uint32_t timeoutNew);

/****************************[Non-Interfaced functions]***************************************************************/
uint8_t libBPMcStrategyBuffer_calcInitialLevels(libBPMcInterface* interface);

uint32_t libBPMcStrategyBuffer_rateToInterval(libBPMcInterface* interface, double rate, double nSeqNew);

libBPMcStrategyBuffer_bufferThread(void*param);

double libBPMcStrategyBuffer_getPenaltyTime(libBPMcInterface* interface, uint32_t tcOld, uint32_t tcNew, double nSeqOld, double nSeqNew, libBPMcBuffer* bufOut);

#endif /* LIBBPMCSTRATEGYBUFFER_H_ */
