/**
 * \file libBPMcStrategySkeleton.h
 * \brief Skeleton for libBPMc Power Management Strategies (PMS)
 *
 * Oct. 2013, Philipp Kindt
 *
 * This file can act both as an example for a PMS and a skeleton for other PMS.
 * It Eacht time data is to be sent, it increases the connection interval by a parametrizable amount of time.
 *
 * How to get started with your own PMS:
 *
 * 1) Copy this file and libBPMcStrategySkeleton.c to a new file containing the new name "myStrategy" instead of "Skeleton"
 * 2) Search and Replace "Skeleton" by "myStrategy" in both files: /Skeleton/myStrategy/gi
 *    Take care about the big case macros (LIBBPMCSTRATEGYSKELETON_H_,...)
 * 3) Adjust the code and extend.
 *
 * Happy Power-Saving!
 */

#ifndef LIBBPMCSTRATEGYSKELETON_H_
#define LIBBPMCSTRATEGYSKELETON_H_

#include "../libBPMcInterface.h"
#include "../libBPMcTraffic.h"


///Internal data structure storing all information the PMS nends
typedef struct _libBPMcSkeletonInternals{
	uint32_t nextConnectionInterval;		//the next connection interval this strategy is going to set as a multiple of 0.625 ms
	libBPMcBuffer *bufIn;					//pointer to the queue that contains all data that has been received but not been processed by the application
	libBPMcBuffer *bufOut;					//pointer to the queue that contains all data that is to be sent via BLE
	uint32_t increment;						//the incerement of the connection interval for each call as a multiple of 0.625 ms
	uint32_t connectionIntervalMax;			//the maximum connection interval this PMS will choose as a multiple of 0.625 ms
	uint8_t masterOrSlave;					//role ot this PMS; 0=>slave, 1=>master
	double rateMin;							//minumum possible input data rate
	double rateMax;							//maximum possible input data rate
	double currentNSeq;						//the current number of packet pairs per connection event
} libBPMcStrageySkeletonInternals;


/**
 * \brief Initialize the PMS
 * This function is called by the application or SLI whenever the PMS is loaded. It is the first function that is called.
 * It will register all relevant callback functions and allocate space for internal data
 * @param interface Pointer to an initialized libBPMcInterface struct.
 *
 */
void libBPMcStrategySkeleton_init(libBPMcInterface* interface);

/**
 * \brief Called before the first call to \ref libBPMcStrategySkeleton_addOut() to inform the PMS about the initial connection interval that
 * has been set before the PMS has been active.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcInitial Initial connection interval in terms of multiples of 0.625 ms
 */
void libBPMcStrategySkeleton_startInfo(libBPMcInterface* interface, int32_t tcInitial, double nSeqInitial);

/**
 * \brief Called before the first call to \ref libBPMcStrategySkeleton_addOut() to inform the PMS about the latency constraints in terms of the
 * lowest (tcMin) and highest (tcMax) connection interval allowed.
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcMin Minimum connection interval allowed as multiples of 0.625 ms
 * @param tcMax Maximum connection interval allowed as multiples of 0.625 ms
 */
void libBPMcStrategySkeleton_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax);

/**
 * \brief Called before the first call to \ref libBPMcStrategySkeleton_addOut() to inform the PMS about the minimum and maximum traffic rate that might occur.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param rateMin Minimum traffic that might occur [bytes/s]
 * @param rateMax Maximum traffic that might occur [bytes/s];
 */
void libBPMcStrategySkeleton_setTrafficBounds(libBPMcInterface* interface, double rateMin, double rateMax);

/**
 * \brief Called before the first call to \ref libBPMcStrategySkeleton_addOut() to inform the PMS about it role (master or slave)
 * In Master mode, a call to \ref libBPMcInterface_callSetConIntCallBack() will cause a direct update of the connection interval ASAP.
 * In Slave mode, a call to \ref libBPMcInterface_callSetConIntCallBack() can have - depending on the implementation of the SLIs local and remote and on the remote PMS - the following effect:
 *  1) no effect
 *  2) a suggestion on how to update the connection interval is sent to the slave PMS. The mater's PMS will then choose its own connection interval or take the suggestion (reccomended)
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param masterOrSlave 1 => master Role, 0 => slave Role
 */
void libBPMcStrategySkeleton_setRole(libBPMcInterface* interface, uint8_t masterOrSlave);

/**
 * \brief Called whenever outgoing traffic is sent to the outgoing buffer.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tr Number of bytes sent along with a timestamp
 */
void libBPMcStrategySkeleton_addOut(libBPMcInterface* interface, libBPMcTraffic tr);

/**
 * \brief Called whenever outgoing traffic is read from the outgoing buffer (and sent via BLE).
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tr Number of bytes read along with a timestamp containing the time of the read request
 */
void libBPMcStrategySkeleton_readOut(libBPMcInterface* interface, libBPMcTraffic tr);

/**
 * \brief Called before the first call to \ref libBPMcStrategySkeleton_addOut()
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param bufIn Pointer to the buffer containing data received via the wireless link to be read by the application
 * @param bufOut Pointer to the buffer cotaining data written by the application to be sent by the application
 */
void libBPMcStrategySkeleton_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut);

/**
 * \brief This function is called when the PMS ist to be activated or disactivated. May happen anytime after the initialization, but will usually be
 * called after \ref libBPMcStrategySkeleton_startInfo() and \ref libBPMcStrategySkeleton_setBuffer() has been called
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param active 1 => Activate Power Management strategy; 0=> disactivate Power Management strategy
 */
void libBPMcStrategySkeleton_setActivation(libBPMcInterface* interface, uint8_t active);

/**
 * \brief Destructor
 * This function is called whenever the PMS is unloaded to allow for a safe destruction of the PMS
 * @param interface Pointer to initialized libBPMcInterface structure.
 */
void libBPMcStrategySkeleton_destory(libBPMcInterface* interface);

/**
 * \brief Copy the name of the PMS to a pre-allocated string.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * Max. length: 40 Bytes / 39 chars
 */
void libBPMcStrategySkeleton_getIdent(libBPMcInterface* interface,char* ident);

/**
 * \brief Allocates and returns a string containing a machine-readable description of all parameters offered.
 * See \ref libBPMcInterface_callGetParamListCallBack()
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @return
 */
char* libBPMcStrategySkeleton_getParamList(libBPMcInterface* interface);

/**
 * \brief Write a parameter value.
 * See \ref libBPMcInterface_callSetParamCallBack()
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param param Name of the parameter to be written
 * @param value New value of parameter to be written write
 * @return 0 => failure, 1=>success
 */
uint8_t libBPMcStrategySkeleton_setParam(libBPMcInterface* interface,const char* param, const char* value);

/**
 * \brief Read a parameter value.
 * See \ref libBPMcInterface_callGetParamCallBack()
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param param Name of the parameter
 * @param value Value of parameter
 * @return 0 => failure, 1=>success
 */
uint8_t libBPMcStrategySkeleton_getParam(libBPMcInterface* interface,const char* param, char* value);


/**
 * \brief Called by SLI when a connection parameter update occurred.
 * The point in time when this callback is called is when the new parameters are actually used for the
 * connection.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcNew Connection interval after update
 * @param latencyNew Slave latency after update
 * @param timeoutNew Super vision timeout after update
 */
void libBPMcStrategySkeleton_connectionUpdated(libBPMcInterface* interface, uint32_t tcNew, uint32_t latencyNew, uint32_t timeoutNew);

#endif /* LIBBPMCSTRATEGYSKELETON_H_ */
