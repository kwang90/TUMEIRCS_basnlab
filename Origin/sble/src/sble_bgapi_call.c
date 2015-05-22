/*
 * sble_api.h
 *
 *  Created on: 06.07.2012
 *      Author: kindt
 */

#include "sble_bgapi_call.h"
#include "sble_scheduler.h"
#include "sble_state.h"
#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
#include <pthread.h>
#elif SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif

void sble_bgapi_call_set_wakeup_cb(sble_hw_callback_t f){
	dstate.wuf = f;
}

void sble_bgapi_call_internal_bl_init(uint8_t message,...)
{

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
		pthread_mutex_lock(&dstate.mutexCmd);
#elif SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
		xSemaphoreTake(dstate.mutexCmd,portMAX_DELAY);
#endif
	sble_scheduler_lock_mutex();
	if(dstate.wuf != NULL)
	{
		dstate.evt_clear_list &= ~(SBLE_STATE_IO_PORT_STATUS|SBLE_STATE_EVENT);

		sble_scheduler_unlock_mutex();
		//sble_scheduler_autoclear_prevent(SBLE_STATE_IO_PORT_STATUS);

		dstate.wuf(1);

		SBLE_DEBUG("Wait for wakeup");
		//sble_scheduler_lock_mutex();
		if(!(dstate.flags & SBLE_STATE_IO_PORT_STATUS))
		{
			//sble_scheduler_unlock_mutex();
			//printf("waiting for port event main thread\n");
			sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_IO_PORT_STATUS);
			//printf("port event received by main thread\n");
		}
		sble_scheduler_lock_mutex();

		//dstate.flags |= SBLE_STATE_EVENT;

	}

	dstate.flags |= SBLE_STATE_CMD_SENT;
	dstate.flags &= ~(SBLE_STATE_RESPONSE_RECEIVED);

	dstate.flags_sched_internal |= SBLE_FLAG_SCHED_FOR_RESPONSE_ACK;
	sble_scheduler_unlock_mutex();

	 SBLE_DEBUG("Issuing cmd...");
}


sble_bool sble_bgapi_call_internal_bl_delay(){
	sble_bool rv;
	if(dstate.wuf != NULL)
	{
	//	printf("thread wuf 0\n");
		dstate.wuf(0);
	}

	sble_scheduler_lock_mutex();
	if(dstate.flags & SBLE_STATE_TERMINATE)
	{
		sble_scheduler_unlock_mutex();

		return SBLE_FALSE;

	}
	sble_scheduler_unlock_mutex();


	 //SBLE_DEBUG("waiting for response to call...");
//	sble_scheduler_wakeup(SBLE_THREAD_DISPATCHER);
	while(1)
	{
		SBLE_DEBUG("Call: Waiting for scheduler event/response, going to sleep...");
		sble_scheduler_wait(SBLE_THREAD_MAIN);
		SBLE_DEBUG("woken");
		sble_scheduler_lock_mutex();

		if(dstate.flags & SBLE_STATE_RESPONSE_RECEIVED)
		{
			//SBLE_DEBUG("got response...");
			dstate.flags &= ~(SBLE_STATE_RESPONSE_RECEIVED);
			dstate.flags &= ~(SBLE_STATE_CMD_SENT);
		//	dstate.flags &= ~(SBLE_STATE_EVENT);

			dstate.flags_sched_internal &= ~(SBLE_FLAG_SCHED_FOR_RESPONSE_ACK);

			sble_scheduler_unlock_mutex();
			sble_scheduler_wakeup(SBLE_THREAD_DISPATCHER);
			SBLE_DEBUG("Command handling done.");
			rv = ((dstate.flags & SBLE_STATE_CMD_SUCCESS) != 0);

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
		pthread_mutex_unlock(&dstate.mutexCmd);
#elif SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
		xSemaphoreGive(dstate.mutexCmd);
#endif
			return rv;

		}
		else
		{
			SBLE_DEBUG("got something but no response.");
			 SBLE_DEBUG_CON("Flags: ");
			 sble_print_bitfield(dstate.flags,32);
			sble_scheduler_unlock_mutex();
			sble_scheduler_wakeup(SBLE_THREAD_DISPATCHER);
		}

	}

}


