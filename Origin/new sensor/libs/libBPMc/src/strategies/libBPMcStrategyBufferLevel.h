/**
 * \file libBPMcStrategyBufferLevel.h
 * \brief BufferLevel for libBPMc Power Management Strategies (PMS)
 *
 * date: 7.11.13
 * author: Mathias Gopp
 *
 * Happy Power-Saving!
 */

#ifndef LIBBPMCSTRATEGYBUFFERLEVEL_H_
#define LIBBPMCSTRATEGYBUFFERLEVEL_H_

#include "../libBPMcInterface.h"
#include "../libBPMcTraffic.h"
#include "../../../virThread/virThread.h"

#include <inttypes.h>

//Datarate
#define numOfIntervals 1 //0.5 for slow(10kbit/s) || 2.5 for fast(50kbit/s)

//write output into .csv file
//0 -> no output file
//1 -> output file
#define csvOutputBuffervalues 0
#define csvOutputTime 0



///Internal data structure storing all information the PMS needs
typedef struct _libBPMCinterfaceInternalsBufferLevel
{
	uint32_t connectionInterval;
	libBPMcBuffer *bufIn;
	libBPMcBuffer *bufOut;
	uint32_t increment;
	uint32_t updateInterval;
	uint8_t minLevel;
	uint8_t maxLevel;
	uint32_t minConInt;
	uint32_t maxConInt;
	libBPMcInterface* interface;
} libBPMcStrategyBufferLevelInternals;

typedef struct _currentTrafficBufferLevel
{
	uint8_t status;
	uint32_t numberOfBytes;
	libBPMcTraffic startTime;
	libBPMcTraffic endTime;
} currentTrafficBufferLevel;

libBPMcStrategyBufferLevelInternals *BufferLevelParameter;

currentTrafficBufferLevel *BufferLevelCurrentTraffic;

virThread_thread_t libBPMcStrategyBufferLevel_Thread;

virThread_mutex_t libBPMcStrategyBufferLevel_data_mutex;

/**
 * \brief Initialize the PMS
 * This function is called by the application or SLI whenever the PMS is loaded. It is the first function that is called.
 * It will register all relevant callback functions and allocate space for internal data
 * @param interface Pointer to an initialized libBPMcInterface struct.
 *
 */
void libBPMcStrategyBufferLevel_init(libBPMcInterface* interface);

/**
 * \brief Called before the first call to \ref libBPMcStrategyBufferLevel_addOut() to inform the PMS about the initial connection interval that
 * has been set before the PMS has been active.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcInitial Initial connection interval in terms of multiples of 0.625 ms
 */
void libBPMcStrategyBufferLevel_startInfo(libBPMcInterface* interface, int32_t tcInitial,double nSeqInitial);

/**
 * \brief Called whenever outgoing traffic is sent to the outgoing buffer.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tr Number of bytes sent along with a timestamp
 */
void libBPMcStrategyBufferLevel_addOut(libBPMcInterface* interface, libBPMcTraffic tr);

/**
 * \brief Called before the first call to \ref libBPMcStrategyBufferLevel_addOut()
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param bufIn Pointer to the buffer containing data received via the wireless link to be read by the application
 * @param bufOut Pointer to the buffer cotaining data written by the application to be sent by the application
 */
void libBPMcStrategyBufferLevel_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut);

/**
 * \brief Called before the first call to \ref libBPMcStrategyBufferLevel_addOut() to inform the PMS about the latency constraints in terms of the
 * lowest (tcMin) and highest (tcMax) connection interval allowed.
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param tcMin Minimum connection interval allowed as multiples of 0.625 ms
 * @param tcMax Maximum connection interval allowed as multiples of 0.625 ms
 */
void libBPMcStrategyBufferLevel_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax);

/**
 * \brief This function is called when the PMS ist to be activated or disactivated. May happen anytime after the initialization, but will usually be
 * called after \ref libBPMcStrategyBufferLevel_startInfo() and \ref libBPMcStrategyBufferLevel_setBuffer() has been called
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param active 1 => Activate Power Management strategy; 0=> disactivate Power Management strategy
 */
void libBPMStrategyBufferLevel_setActivation(libBPMcInterface* interface, uint8_t active);

/**
 * \brief Destructor
 * This function is called whenever the PMS is unloaded to allow for a safe destruction of the PMS
 * @param interface Pointer to initialized libBPMcInterface structure.
 */
void libBPMcStrategyBufferLevel_destory(libBPMcInterface* interface);

/**
 * \brief Copy the name of the PMS to a pre-allocated string.
 * @param interface Pointer to initialized libBPMcInterface structure.
 * Max. length: 40 Bytes / 39 chars
 */
void libBPMcStrategyBufferLevel_getIdent(libBPMcInterface* interface,char* ident);

/**
 * \brief Allocates and returns a string containing a machine-readable description of all parameters offered.
 * See \ref libBPMcInterface_callGetParamListCallBack()
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @return
 */
char* libBPMcStrategyBufferLevel_getParamList(libBPMcInterface* interface);

/**
 * \brief Write a parameter value.
 * See \ref libBPMcInterface_callSetParamCallBack()
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param param Name of the parameter to be written
 * @param value New value of parameter to be written write
 * @return 0 => failure, 1=>success
 */
uint8_t libBPMcStrategyBufferLevel_setParam(libBPMcInterface* interface,const char* param, const char* value);

/**
 * \brief Read a parameter value.
 * See \ref libBPMcInterface_callGetParamCallBack()
 *
 * @param interface Pointer to initialized libBPMcInterface structure.
 * @param param Name of the parameter
 * @param value Value of parameter
 * @return 0 => failure, 1=>success
 */
uint8_t libBPMcStrategyBufferLevel_getParam(libBPMcInterface* interface,const char* param, char* value);

/**
 * \brief In this thread the strategy is implemented
 * The buffer has a minimum and a maximum level, given in percent
 * Depending on that, if the fill level is smaller then the minimum level, the connection speed gets updated
 * On the other hand if the fill level is higher then the maximum levle, the connection speed gets increased
 */
void* libBPMcStrategyBufferLevel_update_thread(void* param);

/**
 * \brief Calculates the current traffic
 *
 * @return current traffic
 */
double getCurrentTraffic_bufferLevel(void);

#endif /* LIBBPMCSTRATEGYBUFFERLEVEL_H_ */
