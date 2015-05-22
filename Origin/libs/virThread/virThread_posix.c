/*
 * virThread_posix.c
 *
 *  Created on: 20.11.2013
 *      Author: kindt
 */

#include "virThread.h"

#if VIRTHREAD_PLATFORM_ARCHITECTURE==VIRTHREAD_PLATFORM_ARCHITECTURE_POSIX

int32_t virThread_thread_create(virThread_thread_t *thread, virThread_threadRoutine_t threadRoutine, void* threadArg, char* taskName, uint32_t freeRTOSStackSize, uint32_t freeRTOSPriority){
	return pthread_create(thread, NULL, threadRoutine, threadArg);
}
int32_t virThread_mutex_init(virThread_mutex_t *mutex, virThread_mutexValue_t locked){
	int32_t rv = pthread_mutex_init(mutex,NULL);
	if(locked){
		pthread_mutex_lock(mutex);
	}
	return rv;
}

int32_t virThread_sema_init(virThread_sema_t *sema, virThread_semaValue_t initial_value){
		return sem_init(sema, 0, initial_value);
}

int32_t virThread_mutex_lock(virThread_mutex_t *mutex){
	return pthread_mutex_lock(mutex);
}
int32_t virThread_mutex_unlock(virThread_mutex_t *mutex){
	return pthread_mutex_unlock(mutex);

}
int32_t virThread_sema_lock(virThread_sema_t *sema){
	return sem_wait(sema);
}
int32_t virThread_sema_unlock(virThread_sema_t *sema){
	return sem_post(sema);
}
int32_t virThread_mutex_destroy(virThread_sema_t *mutex){
	return pthread_mutex_destroy(mutex);
}

int32_t virThread_sema_destroy(virThread_sema_t *sema){
	return sem_destroy(sema);
}
void virThread_delay_ms(uint32_t ms) {
	usleep(ms*1000);
}

uint8_t virThread_timer_create(virThread_timer_t *timer, uint32_t nMicroSeconds, virThread_timerRoutine_t routine, void* functionParam, uint8_t doAutoRelaod){
	struct sigevent sev;
	memset(&sev, 0, sizeof(struct sigevent));
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = routine;
	sev.sigev_value.sival_ptr = functionParam;
	return (timer_create(CLOCK_MONOTONIC,&sev, timer)>-1);

}

uint8_t virThread_timer_start(virThread_timer_t *timer,uint32_t nMicroSeconds, uint8_t doAutoReload){
	if(timer != NULL){
		struct itimerspec i;
		struct itimerspec c;
		timer_gettime(*timer,&c);
//printf("current: %u s %u ns\n",c.it_value.tv_sec, c.it_value.tv_nsec);
		if(nMicroSeconds >= 1000000){
			i.it_value.tv_sec = nMicroSeconds / 1000000;
		}else{
			i.it_value.tv_sec = 0;
		}
		i.it_value.tv_nsec = (nMicroSeconds - i.it_value.tv_sec*1000000) * 1000;
//		printf("sec: %u - nSec: %u\n",i.it_value.tv_sec,i.it_value.tv_nsec);

		if(doAutoReload){
			i.it_interval.tv_sec = i.it_value.tv_sec;
			i.it_interval.tv_nsec = i.it_value.tv_nsec;
		}else{
			i.it_interval.tv_sec = 0;
			i.it_interval.tv_nsec = 0;

		}

	return (timer_settime(*timer,0,&i,NULL)>-1);
	}
}

uint8_t virThread_timer_stop(virThread_timer_t *timer){
	if(timer != NULL){
		return (virThread_timer_start(timer,0,0) > -1);
	}
}

uint8_t virThread_timer_delete(virThread_timer_t *timer){
	if(timer != NULL){
		return (timer_delete(timer) > -1);
	}

}


#endif
