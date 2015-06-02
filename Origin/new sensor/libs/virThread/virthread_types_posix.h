/*
 * virThread_types_posix.h
 *
 *  Created on: 20.11.2013
 *      Author: kindt
 */

#ifndef VIRTHREAD_TYPES_POSIX_H_
#define VIRTHREAD_TYPES_POSIX_H_

#include "virThread_platform.h"
#include <time.h>

#if VIRTHREAD_PLATFORM_ARCHITECTURE==VIRTHREAD_PLATFORM_ARCHITECTURE_POSIX
	#include <pthread.h>
	#include <semaphore.h>
	#include <unistd.h>
	#include <signal.h>
	typedef pthread_t virThread_thread_t;
	typedef pthread_mutex_t virThread_mutex_t;
	typedef sem_t virThread_sema_t;
	typedef uint8_t virThread_semaValue_t;
	typedef uint8_t virThread_mutexValue_t;
	typedef void*(*virThread_threadRoutine_t)();
	typedef timer_t virThread_timer_t;
	typedef void*(*virThread_timerRoutine_t)(virThread_timer_t*);

#endif
#endif /* VIRTHREAD_TYPES_POSIX_H_ */
