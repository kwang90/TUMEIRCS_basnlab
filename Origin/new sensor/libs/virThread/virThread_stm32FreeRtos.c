/*
 * virThread_posix.c
 *
 *  Created on: 20.11.2013
 *      Author: kindt
 */

#include "virThread.h"
#include "virthread_types_stm32FreeRtos.h"

#if VIRTHREAD_PLATFORM_ARCHITECTURE==VIRTHREAD_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS

int32_t virThread_thread_create(virThread_thread_t *thread, virThread_threadRoutine_t threadRoutine, void* threadArg, char* taskName, uint32_t freeRTOSStackSize, uint32_t freeRTOSPriority){
	return xTaskCreate(threadRoutine, taskName, freeRTOSStackSize, threadArg,freeRTOSPriority,thread);
}
int32_t virThread_mutex_init(virThread_mutex_t *mutex, virThread_mutexValue_t locked){
	*mutex = xSemaphoreCreateMutex();

	if(locked){
		xSemaphoreTake(*mutex,0);
	}
	return *mutex!=NULL;
}

int32_t virThread_sema_init(virThread_sema_t *sema, virThread_semaValue_t initial_value){
	vSemaphoreCreateBinary(*sema);

	if(initial_value == 0){
		xSemaphoreTake(*sema,0);
	}
	return *sema!=NULL;

}

int32_t virThread_mutex_lock(virThread_mutex_t *mutex){
	return xSemaphoreTake(*mutex,portMAX_DELAY);
}
int32_t virThread_mutex_unlock(virThread_mutex_t *mutex){
	return xSemaphoreGive(*mutex);

}
int32_t virThread_sema_lock(virThread_sema_t *sema){
	return xSemaphoreTake(*sema,portMAX_DELAY);
}
int32_t virThread_sema_unlock(virThread_sema_t *sema){
	return xSemaphoreGive(*sema);

	return 1;
}
int32_t virThread_mutex_destroy(virThread_sema_t *mutex){
	vSemaphoreDelete(*mutex);
	return 1;
}

int32_t virThread_sema_destroy(virThread_sema_t *sema){
	vSemaphoreDelete(*sema);
	return 1;
}

void virThread_delay_ms(uint32_t ms){
	hhd_delay(ms);
}

uint8_t virThread_timer_create(virThread_timer_t *timer, uint32_t nMicroSeconds, virThread_timerRoutine_t routine, void* functionParam, uint8_t doAutoReload){
	*timer = xTimerCreate("",hhd_delay_microseconds_to_ticks(nMicroSeconds),doAutoReload,functionParam,routine);
	if(*timer != NULL){
		return 1;
	}else{
		return 0;
	}
}

uint8_t virThread_timer_start(virThread_timer_t *timer,uint32_t nMicroSeconds, uint8_t doAutoReload){
	if(timer != NULL){
		if(xTimerStart(*timer,portMAX_DELAY)==pdFAIL){
			return 0;
		}else{
			return 1;
		}
	}
}

uint8_t virThread_timer_stop(virThread_timer_t *timer){
	if(timer != NULL){
		if(xTimerStop(*timer,0)==pdFAIL){
			return 0;
		}else{
			return 1;
		}
	}
}
uint8_t virThread_timer_delete(virThread_timer_t *timer){
	if(timer != NULL){
		if(xTimerDelete(*timer,0) == pdFAIL){
			return 0;
		}else{
			return 1;
		}
	}
}

#endif
