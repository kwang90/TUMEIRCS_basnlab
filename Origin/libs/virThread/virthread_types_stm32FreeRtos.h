/*
 * virthread_types_stm32FreeRtos.h
 *
 *  Created on: 20.11.2013
 *      Author: kindt
 */

#ifndef VIRTHREAD_TYPES_STM32FREERTOS_H_
#define VIRTHREAD_TYPES_STM32FREERTOS_H_

#include "virThread_platform.h"

#if VIRTHREAD_PLATFORM_ARCHITECTURE==VIRTHREAD_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS

	#include <FreeRTOS.h>
	#include <semphr.h>
	#include <task.h>
	#include <hhd_delay.h>
	#include <timers.h>
	typedef xTaskHandle virThread_thread_t;
	typedef xSemaphoreHandle virThread_mutex_t;
	typedef xSemaphoreHandle virThread_sema_t;
	typedef uint8_t virThread_semaValue_t;
	typedef uint8_t virThread_mutexValue_t;
	typedef void*(*virThread_threadRoutine_t)();
	typedef TimerHandle_t virThread_timer_t;
	typedef void*(*virThread_timerRoutine_t)(xTimerHandle_t);
#endif
#endif /* VIRTHREAD_TYPES_STM32FREERTOS_H_ */
