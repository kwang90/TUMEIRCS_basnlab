/**
 * \file libBPMcStrategySpeedAdjustment.h
 * \brief SpeedAdjustment for libBPMc Power Management Strategies (PMS)
 *
 * Oct. 2013, Philipp Kindt
 *
 * This file can act both as an example for a PMS and a SpeedAdjustment for other PMS.
 * It Eacht time data is to be sent, it increases the connection interval by a parametrizable amount of time.
 *
 * How to get started with your own PMS:
 *
 * 1) Copy this file and libBPMcStrategySpeedAdjustment.c to a new file containing the new name "myStrategy" instead of "SpeedAdjustment"
 * 2) Search and Replace "SpeedAdjustment" by "myStrategy" in both files: /SpeedAdjustment/myStrategy/gi
 *    Take care about the big case macros (LIBBPMCSTRATEGYSpeedAdjustment_H_,...)
 * 3) Adjust the code and extend.
 *
 * Happy Power-Saving!
 */

#ifndef LIBBPMCSTRATEGYSPEEDADJUSTMENT_H_
#define LIBBPMCSTRATEGYSPEEDADJUSTMENT_H_

#include "../libBPMcInterface.h"
#include "../libBPMcTraffic.h"
#include "../../../virThread/virThread.h"

#include <inttypes.h>

#define numOfIntervals 1 //0.5 for slow(10kbit/s) || 2.5 for fast(50kbit/s)

///Internal data structure storing all information the PMS needs
typedef struct _libBPMcStrageySpeedAdjustmentInternals
{
	uint32_t nextConnectionInterval;
	libBPMcBuffer *bufIn;
	libBPMcBuffer *bufOut;
	uint32_t increment;
	double updateInterval;
	uint32_t minConInt;
	uint32_t maxConInt;
	libBPMcInterface* interface;
} libBPMcStrageySpeedAdjustmentInternals;

typedef struct _currentTraffic
{
	uint8_t status;
	uint32_t numberOfBytes;
	libBPMcTraffic startTime;
	libBPMcTraffic endTime;
} currentTraffic;


libBPMcStrageySpeedAdjustmentInternals *SpeedAdjustmentParameter;

currentTraffic *SpeedAdjustmentCurrentTraffic;

//#if LIBBPMC_PLATFORM_ARCHITECTURE==LIBBPMC_PLATFORM_ARCHITECTURE_ARCHITECTURE_POSIX

virThread_thread_t libBPMcStrategySpeedAdjustment_Thread;

virThread_mutex_t libBPMcStrategySpeedAdjustment_data_mutex;
//#endif
/**
 * \brief Initialize the PMS
 * This function is called by the application or SLI whenever the PMS is loaded. It is the first function that is called.
 * It will register all relevant callback functions and allocate space for internal data
 * @param interface Pointer to an initialized libBPMcInterface struct.
 *
 */
void libBPMcStrategySpeedAdjustment_init(libBPMcInterface* interface);

/**
 * \brief Called before the first call to \ref libBPMcStrategySpeedAdjustment_addOut() to inform the PMS about the initial connection interval that
 * has been set before the PMS has been active.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcInitial Initial connection interval in terms of multiples of 0.625 ms
 */
void libBPMcStrategySpeedAdjustment_startInfo(libBPMcInterface* interface, int32_t tcInitial, double nSeqInitial);

/**
 * \brief Called before the first call to \ref libBPMcStrategySpeedAdjustment_addOut() to inform the PMS about the latency constraints in terms of the
 * lowest (tcMin) and highest (tcMax) connection interval allowed.
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcMin Minimum connection interval allowed as multiples of 0.625 ms
 * @param tcMax Maximum connection interval allowed as multiples of 0.625 ms
 */
void libBPMcStrategySpeedAdjustment_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax);

/**
 * \brief Called whenever outgoing traffic is sent to the outgoing buffer.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tr Number of bytes sent along with a timestamp
 */
void libBPMcStrategySpeedAdjustment_addOut(libBPMcInterface* interface, libBPMcTraffic tr);

/**
 * \brief Called before the first call to \ref libBPMcStrategySpeedAdjustment_addOut()
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param bufIn Pointer to the buffer containing data received via the wireless link to be read by the application
 * @param bufOut Pointer to the buffer cotaining data written by the application to be sent by the application
 */
void libBPMcStrategySpeedAdjustment_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut);

/**
 * \brief This function is called when the PMS ist to be activated or disactivated. May happen anytime after the initialization, but will usually be
 * called after \ref libBPMcStrategySpeedAdjustment_startInfo() and \ref libBPMcStrategySpeedAdjustment_setBuffer() has been called
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param active 1 => Activate Power Management strategy; 0=> disactivate Power Management strategy
 */
void libBPMStrategySpeedAdjustment_setActivation(libBPMcInterface* interface, uint8_t active);

/**
 * \brief Destructor
 * This function is called whenever the PMS is unloaded to allow for a safe destruction of the PMS
 * @param interface Pointer to initialized libBPMcInterface structure.
 */
void libBPMcStrategySpeedAdjustment_destory(libBPMcInterface* interface);

/**
 * \brief Copy the name of the PMS to a pre-allocated string.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * Max. length: 40 Bytes / 39 chars
 */
void libBPMcStrategySpeedAdjustment_getIdent(libBPMcInterface* interface,char* ident);

/**
 * \brief Allocates and returns a string containing a machine-readable description of all parameters offered.
 * See \ref libBPMcInterface_callGetParamListCallBack()
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @return
 */
char* libBPMcStrategySpeedAdjustment_getParamList(libBPMcInterface* interface);

/**
 * \brief Write a parameter value.
 * See \ref libBPMcInterface_callSetParamCallBack()
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param param Name of the parameter to be written
 * @param value New value of parameter to be written write
 * @return 0 => failure, 1=>success
 */
uint8_t libBPMcStrategySpeedAdjustment_setParam(libBPMcInterface* interface,const char* param, const char* value);

/**
 * \brief Read a parameter value.
 * See \ref libBPMcInterface_callGetParamCallBack()
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param param Name of the parameter
 * @param value Value of parameter
 * @return 0 => failure, 1=>success
 */
uint8_t libBPMcStrategySpeedAdjustment_getParam(libBPMcInterface* interface,const char* param, char* value);

/**
 * \brief Contains the strategy
 * The speed gets an update in periodic time steps
 */
void* libBPMcStrategySpeedAdjustment_update_thread(void* param);

/**
 * \brief Calculates the current traffic
 *
 * @return current traffic
 */
double getCurrentTraffic(void);

#endif /* LIBBPMCSTRATEGYSPEEDADJUSTMENT_H_ */
