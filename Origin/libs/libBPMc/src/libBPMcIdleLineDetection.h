/**
 * \file libBPMcIdleLineDetection.h
 * \brief idle line detection service
 * The Idle line service monitors - once intitialized - the application data stream for idle periods.
 * Whenver the application does not write any data, the idle line detection service decreases the connection interval autonomously
 * to its maximum value Tc_max (if allowed by the emergency subsystem).
 *
 * Once activated, it can be discativated by setting timeout to 0 microseconds
 * The Idle Line Detection is activated by default when calling libBPMcInterface_callActivationCallBack.
 * To disactivate it later on, set the timeout to 0 using \ref libBPMcIdleLineDetection_setTimeOut .
 * \author Philipp Kindt <kindt@rcs.ei.tum.de>
 * \date 26.6.2014
 */

#ifndef LIBBPMCIDLELINEDETECTION_H_
#define LIBBPMCIDLELINEDETECTION_H_
#include <time.h>
#include "../../virThread/virThread.h"


/**
 * Structure that is a member of libBPMcInterface containing the specific data for the idle-line detection
 */
typedef struct _libBPMcIdleLineInternals{
	uint32_t nMicrosecondsTimeout;				///Number of microseconds (10^-6 s) for the timeout. After this amount of time, the line is assumed to be idle
	virThread_timer_t timer;					///A timer used internally for the timeout measurement
	uint8_t active;
} libBPMcIdleLineInternals;


//Fwd Declaration
struct _libBPMcInterface;

/**
 * \brief Initialization Function
 * This function initializes the idle line detection. Afterwards, it is active. The amount of microseconds
 * is \ref LIBBPMC_PARAM_IDLE_LINE_TIMEOUT_MICROSECONDS in \ref libBPMcParams.c by default,
 * Before the idle line detection can be activated, the \ref libBPMcInterface used shall be initialized. That means
 * all callbacks used, the maximum connection interval and the maximum application data rate should be set.
 * The Idle Line Detection is activated by default when calling libBPMcInterface_callActivationCallBack.
 * To disactivate it later on, set the timeout to 0 using \ref libBPMcIdleLineDetection_setTimeOut .
 * @param interface An initialized \ref libBPMcInterface
 */
void libBPMcIdleLineDetection_init(struct _libBPMcInterface* interface);

/** \brief Notifys the idle-line detection that there has been a write()-request
 * This function is called automatically whenver
 * @param interface An initialized and active \ref libBPMcInterface instance
 */
void libBPMcIdleLineDetection_add(struct _libBPMcInterface* interface);

/** \brief Internal function
 * This function is used internally for triggering a connection interfal update once the timer expires.
 * Do not use externally!
 * @param interface pointer to the libBPMcInterface that is controlling the link
 */
void libBPMcIdleLineDetection_forceUpdate(struct _libBPMcInterface* interface);

/** \brief Set the idle line detection timeout.
 * Sets the number of microseconds after which the line is considered to be idle.
 * If set to zero, the idle line-detection will be disactivated.
 *
 * @param interface An initialized and active \ref libBPMcInterface
 * @param nMicroSeconds The number of microseconds after which the line is considered to be idle. If set to 0, the idle line detection is activated.
 * A disactivated idle line detection system can be reactivated by setting a value > 0.
 * The default value is \ref LIBBPMC_PARAM_IDLE_LINE_TIMEOUT_MICROSECONDS
 */
void libBPMcIdleLineDetection_setTimeOut(struct _libBPMcInterface* interface, uint32_t nMicroSeconds);

#endif /* LIBBPMCIDLELINEDETECTION_H_ */
