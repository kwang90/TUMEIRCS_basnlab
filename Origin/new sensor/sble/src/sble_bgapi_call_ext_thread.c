/*
 * sble_bgapi_call_multi_threading.c
 *
 *  Created on: 04.09.2013
 *      Author: Mathias Gopp
 */

#include "sble_bgapi_call_ext_thread.h"
#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
#include <pthread.h>
#elif SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
//#include <hhd_delay.h>
#endif


void sble_bgapi_call_internal_bl_init_ext_thread(uint8_t message,...)
{
#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
		pthread_mutex_lock(&dstate.mutexCmd);
#elif SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
		xSemaphoreTake(dstate.mutexCmd,portMAX_DELAY);
#else
		WRONG PLATTFORM		//will throw compile-time error
#endif
	sble_scheduler_lock_mutex();
	if(dstate.flags & SBLE_STATE_CMD_SENT)
	{
		dstate.cmdExtRequest = SBLE_EXTCMD_REQUEST;
		sble_scheduler_unlock_mutex();

		SBLE_DEBUG("wait for send permission");
		sble_scheduler_wait(SBLE_THREAD_CMD_EXTERNAL);
		sble_scheduler_lock_mutex();

	}

	dstate.cmdExtRequest = SBLE_EXTCMD_ACTIVE;

	//used for wackeup pin
	/*if(dstate.wuf != NULL)
	{
		dstate.evt_clear_list_thread2 |= SBLE_STATE_IO_PORT_STATUS;
		sble_scheduler_unlock_mutex();
		dstate.wuf(1);
		sble_scheduler_wait_for_event_ext_thread(SBLE_THREAD_EVENT_EXTERNAL,SBLE_STATE_IO_PORT_STATUS);
		sble_scheduler_lock_mutex();
		dstate.evt_clear_list_thread2 &= ~(SBLE_STATE_IO_PORT_STATUS);

	}*/

	if(dstate.wuf != NULL)
	{
#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX


		//dstate.evt_clear_list_thread2 &= ~(SBLE_STATE_IO_PORT_STATUS);
		dstate.evt_clear_list_thread2 &= ~(SBLE_STATE_IO_PORT_STATUS|SBLE_STATE_EVENT);
		sble_scheduler_unlock_mutex();

		dstate.wuf(1);
	//	printf("eWWU\n");
		if(!(dstate.flags_thread2 & SBLE_STATE_IO_PORT_STATUS))
		{
			sble_scheduler_wait_for_event_no_reset_ext_thread(SBLE_THREAD_EVENT_EXTERNAL,SBLE_STATE_IO_PORT_STATUS);
		}
	//	printf("eWR\n");
		sble_scheduler_lock_mutex();
#else

		sble_scheduler_unlock_mutex();
		//printf("eWWU\n");

		dstate.wuf(1);
		//if(!(dstate.flags_thread2 & SBLE_STATE_IO_PORT_STATUS)){
	/*
		if(!(dstate.flags_thread2 & SBLE_STATE_IO_PORT_STATUS))
		{
			//sble_scheduler_unlock_mutex();
			//printf("waiting for port event main thread\n");
			sble_scheduler_wait_for_event_ext_thread(SBLE_THREAD_EVENT_EXTERNAL,SBLE_STATE_IO_PORT_STATUS);
			//printf("port event received by main thread\n");
		}
		*/
	//	dstate.wuf(0);

		sble_schedulerFRT_wait_for_hw_wakeup();
		//}
		//printf("eWR\n");

		sble_scheduler_lock_mutex();

#endif
	}

	dstate.flags |= SBLE_STATE_CMD_SENT;
	dstate.flags &= ~(SBLE_STATE_RESPONSE_RECEIVED|SBLE_STATE_IO_PORT_STATUS);
	dstate.flags_thread2 &= ~(SBLE_STATE_RESPONSE_RECEIVED|SBLE_STATE_IO_PORT_STATUS);
	dstate.flags_sched_internal |= SBLE_FLAG_SCHED_FOR_RESPONSE_ACK;
	sble_scheduler_unlock_mutex();
	//printf("eC\n");
	 SBLE_DEBUG("Issuing cmd (magic)...");
}

sble_bool sble_bgapi_call_internal_bl_delay_ext_thread()
{
	sble_bool rv;
	sble_scheduler_lock_mutex();

	if(dstate.flags & SBLE_STATE_TERMINATE)
	{
		sble_scheduler_unlock_mutex();

		return SBLE_FALSE;
	}

	sble_scheduler_unlock_mutex();
	if(dstate.wuf != NULL)
	{
		dstate.wuf(0);
	}

	//hhd_delay(1);

	//SBLE_DEBUG("waiting for response to call...");
//	sble_scheduler_wakeup(SBLE_THREAD_DISPATCHER);
	while(1)
	{
		//SBLE_DEBUG("Call: Waiting for scheduler event, going to sleep...");
		//printf("eWfR\n");
		sble_scheduler_wait(SBLE_THREAD_CMD_EXTERNAL);
		sble_scheduler_lock_mutex();

		if(dstate.flags & SBLE_STATE_RESPONSE_RECEIVED)
		{

			SBLE_DEBUG("got response...");
			dstate.flags &= ~(SBLE_STATE_CMD_SENT);
			dstate.flags &= ~(SBLE_STATE_RESPONSE_RECEIVED);
			dstate.flags_sched_internal &= ~(SBLE_FLAG_SCHED_FOR_RESPONSE_ACK);

			dstate.cmdExtRequest = SBLE_EXTCMD_IDLE;


			sble_scheduler_unlock_mutex();

			sble_scheduler_wakeup(SBLE_THREAD_DISPATCHER);

			if((dstate.flags & SBLE_STATE_CMD_SUCCESS) != 0){
				rv = SBLE_TRUE;
				SBLE_DEBUG("Command handling done (true).");
			}else{
				rv = SBLE_FALSE;
				SBLE_DEBUG("Command handling done (false).");
			}

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX

		pthread_mutex_unlock(&dstate.mutexCmd);
#elif SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
		xSemaphoreGive(dstate.mutexCmd);
#else
		WRONG PLATTFORM //will cause compile error
#endif
			//printf("eF\n");
			return rv;
		}
		else
		{
		/*
			SBLE_DEBUG("got something but no response.");
			 SBLE_DEBUG_CON("Flags: ");
			 sble_print_bitfield(dstate.flags,32);
			 */
			sble_scheduler_unlock_mutex();
			sble_scheduler_wakeup(SBLE_THREAD_DISPATCHER);

		}

	}
}



