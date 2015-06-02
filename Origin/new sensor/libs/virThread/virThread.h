/**
 * \file virThread.h - Virtual thread library.
 * \brief Platform independent thread library wrapper.
 *
 *  The functions of this library wrap to POSIX thread on posix and to FreeRTOS Threads on STM32F4/FreeRTOS architectures.
 *
 *
 *
 *  Created on: 20.11.2013
 *      Author: Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#ifndef VIRTHREAD_H_
#define VIRTHREAD_H_


#include <inttypes.h>
#include "virThread_platform.h"

#include "virthread_types_posix.h"
#include "virthread_types_stm32FreeRtos.h"

/**
 * \brief Create a new thread
 * Creates a new thread and starts it
 * @param thread Thread handle.
 * @param threadRoutine Function that contains the thread code
 * @param threadArg	Thread arguments
 * @param taskName Name of the task (is ignored in posix but relevant for FreeRTOS)
 * @param freeRTOSStackSize The stack size (FreeRTOS port only)
 * @param freeRTOSPriority Priority of the task (FreeRTOS port only)
 * @return 0=> success, != 0 => failure
 */
	int32_t virThread_thread_create(virThread_thread_t *thread, virThread_threadRoutine_t threadRoutine, void* threadArg, char* taskName, uint32_t freeRTOSStackSize, uint32_t freeRTOSPriority);

	/**
	 * \brief Create/initialize a new mutex
	 *
	 * @param mutex Mutex to create
	 * @param locked 1=> Mutex is initially locked, 0=>Mutex is initially unlocked
	 * @return 0=> success, != 0 => failure
	 *
	 */
	int32_t virThread_mutex_init(virThread_mutex_t *mutex, virThread_mutexValue_t locked);

	/**
	 * \brief Create/initialize a new semaphore
	 *
	 * @param sema Semaphore to create
	 * @param initial_value 0 => Mutex is initially locked, 1=>Mutex is initially unlocked.
	 * On POSIX, values > 2 are passed to sem_create, therefore a counting mutex would be created (never use!)
	 * Please note that this behaviour is the other way round with the mutexe.
	 * @return 0=> success, != 0 => failure
	 *
	 */
	int32_t virThread_sema_init(virThread_sema_t *sema, virThread_semaValue_t initial_value);

	/**
	 * \brief Lock a mutex
	 * @param mutex Mutex to lock
	 * @return 0=> success, != 0 => failure
	 */
	int32_t virThread_mutex_lock(virThread_mutex_t *mutex);

	/**
	 * \brief Unlock mutex
	 * @param mutex Mutex to unlock
	 * @return 0=> success, != 0 => failure
	 */
	int32_t virThread_mutex_unlock(virThread_mutex_t *mutex);

	/**
	 * \brief Lock semaphore
	 * @param sema Semaphore to lock
	 * @return 0=> success, != 0 => failure
	 */
	int32_t virThread_sema_lock(virThread_sema_t *sema);

	/**
	 * \brief Unlock semaphore
	 * @param sema Semaphore to unlock
	 * @return 0=> success, != 0 => failure
	 */
	int32_t virThread_sema_unlock(virThread_sema_t *sema);

	/**
	 * \brief Destroy/Free a Mutex
	 * @param mutex Mutex to destroy
	 * @return 0=> success, != 0 => failure
	 */
	int32_t virThread_mutex_destroy(virThread_sema_t *mutex);

	/**
	 * \brief Destroy semaphore
	 * @param sema Semaphore to destroy
	 * @return 0=> success, != 0 => failure
	 */
	int32_t virThread_sema_destroy(virThread_sema_t *sema);

	/**
	 * \brief Make current task sleep for a predefined amount of time
	 * The resolution (and hence the accuracy) depends on the platform:
	 * POSIX => see posix manuals of usleep
	 * FreeRTOS => max. accuracy: one tick.
	 * @param ms Number of milliseconds to wait
	 */
	void virThread_delay_ms(uint32_t ms);


	/**
	 * \brief Create a new timer.
	 * Creates a new timer. Berfore firing, the timer must be started by using \ref virThread_timer_start().
	 * Some parameters are redundant (for example, \ref virThread_timer_start() also has the nMicroSeconds - parameter.
	 * These functions should be called with the same values, since it is platform-dependant which one is actually used.
	 * Important: On POSIX, the function called when the timer expires gets the function param as its parameter.
	 * On FreeRTOS, the callback function will get an xTimerHandle_t (type provided by FreeRTOS).
	 * To get the function param under freeRTOS, one must call pvTimerGetTimerID().
	 * @param timer	A handle for the timer
	 * @param nMicroSeconds The number of microseconds to fire
	 * @param routine A routine that is called whenever the timer expires
	 * @param functioNParam a pointer that is passed to the function called once the timer expires. It can be used to give parameters to the timer callback function.
	 * @param doAutoRelaod 1 => The timer automatically restarts counting once it has expired, 0 => The timer fires only once (single-shot mode)
	 * @return 1=> success; 0=>failure
	 */
	uint8_t virThread_timer_create(virThread_timer_t *timer, uint32_t nMicroSeconds, virThread_timerRoutine_t routine, void* functionParam, uint8_t doAutoRelaod);

	/**
	 * \brief Start, Stop a timer or update its period.
	 * @param timer Timer-handle
	 * @param nMicroSeconds Number of us after which the timer expries. If the value is 0, the timer is disactivated. Any other value will start the timer.
	 * @param doAutoReload 1 => The timer is reloaded automatically after expiring; 0 => single-shot mode.
	 * @return 1=> success; 0=>failure
	 */
	uint8_t virThread_timer_start(virThread_timer_t *timer,uint32_t nMicroSeconds, uint8_t doAutoReload);

	/**
	 * \brief Stop a timer
	 * @param timer Timer-handle
	 * @return 1=> success; 0=>failure
	 */
	uint8_t virThread_timer_stop(virThread_timer_t *timer);

	/**
	 * \brief Delete a timer permanently
	 * @param timer -handle
	 * @return 1=> success; 0=>failure
	 */
	uint8_t virThread_timer_delete(virThread_timer_t *timer);

#endif /* VIRTHREAD_H_ */
