/*
 * sble_scheduler.c
 *
 *  Created on: 06.07.2012
 *      Author: kindt
 */

#include "sble_scheduler.h"
#include "sble_state.h"
#include "sble_io.h"
#include "sble_debug.h"
#include "sble_bgapi_call.h"

#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <FreeRTOS.h>
#include <task.h>
#include <portmacro.h>
#include <semphr.h>




void* sble_callback_dispatcher(void* threadarg) {
	SBLE_DEBUG("Callback dispatcher thread has started.");
	sble_scheduler_wakeup(SBLE_THREAD_MAIN);
	sble_scheduler_lock_mutex();
	while (dstate.filedescriptor >= 0)
	{

		//			SBLE_DEBUG("Waiting for API RSP/EVT...");
		if (dstate.flags & SBLE_STATE_TERMINATE)
		{
			sble_scheduler_unlock_mutex();

			SBLE_DEBUG("Dispatcher thread terminating due to external request.");
			return NULL;
		}

		SBLE_DEBUG("IO/R...");
		sble_scheduler_unlock_mutex();

		if (sble_io_read() <= 0)
		{
			SBLE_DEBUG("IO/Read done.");

			SBLE_DEBUG("Read failed - terminating dispatcher thread...");
			sble_io_disconnect();
			sble_scheduler_wakeup(SBLE_THREAD_MAIN);
			sble_scheduler_wakeup(SBLE_THREAD_EVENT_EXTERNAL);
			sble_scheduler_wakeup(SBLE_THREAD_CMD_EXTERNAL);
			sble_scheduler_events_set(SBLE_STATE_TERMINATE);
			SBLE_DEBUG("goodbye!");
			return NULL;
		}

		sble_scheduler_lock_mutex();
		dstate.flags_thread2 |= dstate.flags;
		/*
		SBLE_DEBUG("CBD: GOT API RSP/EVT after read...");
		SBLE_DEBUG_CON("Flags: ");
		sble_print_bitfield(dstate.flags,32);
		*/

		if (dstate.flags & SBLE_STATE_EVENT)
		{
			if (dstate.flags & dstate.evt_clear_list)
			{
				/*
				SBLE_DEBUG("Autoclear for event flag...");
				SBLE_DEBUG_CON("Flags: ");
				sble_print_bitfield(dstate.flags,32);
				SBLE_DEBUG_CON("EVC: ");
				sble_print_bitfield(dstate.evt_clear_list,32);
				*/

				dstate.flags &= ~(SBLE_STATE_EVENT);
				dstate.flags &= ~(dstate.evt_clear_list);
				//SBLE_DEBUG_CON("Flags (after): ");
				//sble_print_bitfield(dstate.flags,32);
			}
		}

		if (dstate.flags_thread2 & SBLE_STATE_EVENT)
		{
			/*
			SBLE_DEBUG_CON("Ext: Flags: ");
			sble_print_bitfield(dstate.flags_thread2,32);
			 */
			if(dstate.flags_thread2 & SBLE_STATE_IO_PORT_STATUS){
				//printf("-wu\n");
				//sble_schedulerFRT_trigger_hw_wakeup();


			}
			if (dstate.flags_thread2 & dstate.evt_clear_list_thread2)
			{
/*
						SBLE_DEBUG("Ext: Autoclear for event flag...");
						SBLE_DEBUG_CON("Ext: EVC: ");
						sble_print_bitfield(dstate.evt_clear_list_thread2,32);
*/
					dstate.flags_thread2 &= ~(SBLE_STATE_EVENT);
					dstate.flags_thread2 &= ~(dstate.evt_clear_list_thread2);
/*
						SBLE_DEBUG_CON("Ext: Flags (after): ");
						 sble_print_bitfield(dstate.flags_thread2,32);
						SBLE_DEBUG("\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!\nExt: DROPPED EVENT\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
*/

			}
		}
		sble_print_bitfield(dstate.flags_thread2,32);

		//external thread requests sensing a command
		if((!(dstate.flags & SBLE_STATE_CMD_SENT)) && (dstate.cmdExtRequest == SBLE_EXTCMD_REQUEST))
		{
			SBLE_DEBUG("unlock send thread");
			dstate.cmdExtRequest = SBLE_EXTCMD_ACTIVE;
			sble_scheduler_unlock_mutex();
			sble_scheduler_wakeup(SBLE_THREAD_CMD_EXTERNAL);
			sble_scheduler_lock_mutex();

		//external thread waits for response AND response came in
		}
		else if((dstate.flags & SBLE_STATE_RESPONSE_RECEIVED)&&(dstate.flags_sched_internal & SBLE_FLAG_SCHED_FOR_RESPONSE_ACK)&&(dstate.cmdExtRequest == SBLE_EXTCMD_ACTIVE))
		{
			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("wake up external CMD thread");
			sble_scheduler_wakeup(SBLE_THREAD_CMD_EXTERNAL);
			sble_scheduler_wait(SBLE_THREAD_DISPATCHER);
			sble_scheduler_lock_mutex();
		}


		//events for thread 2
		if (dstate.flags_thread2 & SBLE_STATE_EVENT)
		{
			SBLE_DEBUG("Ext: Waiting for EVENT flag to be cleared (thread 2)...");

			//if there is no external cmd  request or data are received then wake up main thread
			//else wake up the sending thread

			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("wake up external cmd thread (thread2)");
			sble_scheduler_wakeup(SBLE_THREAD_EVENT_EXTERNAL);
			sble_scheduler_wait(SBLE_THREAD_DISPATCHER);
			if (dstate.flags_thread2 & SBLE_STATE_EVENT){
							SBLE_DEBUG("thread 2: event NOT cleared.\n");

						}else{
							SBLE_DEBUG("thread 2: event cleared.\n");
						}
			sble_scheduler_lock_mutex();

		}
				//SBLE_DEBUG("next cycle...");


		//events and commands for thread 1
		if ((dstate.flags & SBLE_STATE_EVENT)|| ((dstate.flags & SBLE_STATE_RESPONSE_RECEIVED)&& (dstate.flags_sched_internal& SBLE_FLAG_SCHED_FOR_RESPONSE_ACK)&&(dstate.cmdExtRequest == SBLE_EXTCMD_IDLE)))
		{
			SBLE_DEBUG("Waiting for EVENT flag to be cleared...");

			//if there is no send request or data are received then wake up main thread
			//else wake up the external cmd thread

			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("wake up main thread");
			sble_scheduler_wakeup(SBLE_THREAD_MAIN);
			sble_scheduler_wait(SBLE_THREAD_DISPATCHER);


			sble_scheduler_lock_mutex();
		}
		SBLE_DEBUG("next cycle...");

	}

	return NULL;
}

//non-threadsafe function as thread safety is not required.
void sble_scheduler_init()
{
	vSemaphoreCreateBinary(dstate.sleepMain);
	vSemaphoreCreateBinary(dstate.sleepDispatcher);
	vSemaphoreCreateBinary(dstate.sleepCmdThread);
	vSemaphoreCreateBinary(dstate.sleepEventThread);
	vSemaphoreCreateBinary(dstate.sleepIOP);

	dstate.mutexData = xSemaphoreCreateMutex();
	dstate.mutexCmd = xSemaphoreCreateMutex();
	dstate.mutexMultExtThread = xSemaphoreCreateMutex();
	dstate.sleepSync = xSemaphoreCreateCounting(2,0);

#ifdef SBLE_DEBUG_RTOS_KERNEL
	vQueueAddToRegistry(dstate.sleepMain,"SBLE_SleepMain");
	vQueueAddToRegistry(dstate.sleepDispatcher,"SBLE_SleepDispatcher");
	vQueueAddToRegistry(dstate.sleepCmdThread,"SBLE_SleepCmdThread");
	vQueueAddToRegistry(dstate.sleepEventThread,"SBLE_SleepEventThread");
	vQueueAddToRegistry(dstate.sleepIOP,"SBLE_SleepIOP");

	vQueueAddToRegistry(dstate.sleepSync,"SBLE_SleepSync");
	vQueueAddToRegistry(dstate.mutexData,"SBLE_DataMutex");
	vQueueAddToRegistry(dstate.mutexCmd,"SBLE_CmdMutex");
	vQueueAddToRegistry(dstate.mutexMultExtThread,"SBLE_MultExtThreadSyncMutex");

#endif
	xSemaphoreTake(dstate.sleepMain,portMAX_DELAY);
	xSemaphoreTake(dstate.sleepDispatcher,portMAX_DELAY);
	xSemaphoreTake(dstate.sleepCmdThread,portMAX_DELAY);
	xSemaphoreTake(dstate.sleepEventThread,portMAX_DELAY);
	xSemaphoreTake(dstate.sleepIOP,portMAX_DELAY);
	xSemaphoreGive(dstate.mutexCmd);
	xSemaphoreGive(dstate.mutexData);
	xSemaphoreGive(dstate.mutexMultExtThread);
	dstate.main_thread = xTaskGetCurrentTaskHandle();
	SBLE_DEBUG("Main thread handle is: %x",dstate.main_thread);


	//setup callback dispatcher
	sble_scheduler_dispatcher_start();

	SBLE_DEBUG("Initialization of SBLE scheduler done...");
}

//non-threadsafe function as thread safety is not required.
void sble_scheduler_dispatcher_start() {
	//SBLE_DEBUG("Starting synchronization thread...");
	//xTaskCreate((pdTASK_CODE) _sble_scheduler_stm32f4_synchronization_task,( signed char * )"BLE sync",SBLE_CONFIG_FREERTOS_STACK_SIZE_CBD,(void*) NULL,3,( xTaskHandle * ) &(dstate.sync_thread));

	//xSemaphoreTake(dstate.sleepMain,portMAX_DELAY);

	SBLE_DEBUG("Starting callback dispatcher thread...");
	xTaskCreate((pdTASK_CODE) sble_callback_dispatcher,( signed char * )"BLEcbD",SBLE_CONFIG_FREERTOS_STACK_SIZE_TS,(void*)NULL,3,( xTaskHandle * )& (dstate.sble_thread));
//	xSemaphoreTake(dstate.sleepMain,portMAX_DELAY);
	sble_scheduler_wait(SBLE_THREAD_MAIN);


}

//P.Kindt, 27.10.2012 - internal note:
//Debug-messages here will cause a bus-fault due to
//dereferencing the adress 0xa5a5a5a5 (poison-value for stack-
//overflow-detection of FreeRTOS). WHY?
void sble_scheduler_wait(sble_thread thread)
{
	sble_scheduler_lock_mutex();
	if (dstate.flags & SBLE_STATE_TERMINATE)
	{
		sble_scheduler_unlock_mutex();
		return;
	}
	sble_scheduler_unlock_mutex();

	if(thread == SBLE_THREAD_DISPATCHER)
	{
		SBLE_DEBUG("Sending DISPATCHER thread asleep");
		//printf("Sending DISPATCHER thread asleep\n");
		//xSemaphoreGive(dstate.sleepSync);


		xSemaphoreTake(dstate.sleepDispatcher,portMAX_DELAY);
		//xSemaphoreGive(dstate.sleepMain);

		SBLE_DEBUG("DISPATCHER Thread woke up.");
		//printf("DISPATCHER Thread woke up.\n");

	}

	if(thread == SBLE_THREAD_MAIN)
	{
		SBLE_DEBUG("Sending MAIN thread asleep");
		//printf("Sending MAIN thread asleep\n");
		//xSemaphoreGive(dstate.sleepSync);
		xSemaphoreTake(dstate.sleepMain,portMAX_DELAY);
		//xSemaphoreGive(dstate.sleepDispatcher);
		SBLE_DEBUG("MAIN Thread woke up.");
		//printf("MAIN Thread woke up.\n");
	}

	if(thread == SBLE_THREAD_CMD_EXTERNAL)
	{
		SBLE_DEBUG("Sending External CMD thread asleep");
		//printf("Sending External CMD thread asleep\n");
		//xSemaphoreGive(dstate.sleepSync);
		xSemaphoreTake(dstate.sleepCmdThread,portMAX_DELAY);
		//xSemaphoreGive(dstate.sleepDispatcher);
		SBLE_DEBUG("External CMD thread woke up.");
		//printf("External CMD thread woke up.\n");
	}

	if(thread == SBLE_THREAD_EVENT_EXTERNAL)
	{
		SBLE_DEBUG("Sending External Event thread asleep");
		//xSemaphoreGive(dstate.sleepSync);
		xSemaphoreTake(dstate.sleepEventThread,portMAX_DELAY);
		//xSemaphoreGive(dstate.sleepDispatcher);
		SBLE_DEBUG("External Event thread woke up.");
	}
}

void sble_scheduler_unlock_mutex()
{
	xSemaphoreGive(dstate.mutexData);
}

void sble_scheduler_lock_mutex()
{
	xSemaphoreTake(dstate.mutexData,portMAX_DELAY);
}

void sble_schedulerFRT_wait_for_hw_wakeup(){
	xSemaphoreGive(dstate.sleepIOP);
	xSemaphoreTake(dstate.sleepIOP,portMAX_DELAY);
	xSemaphoreTake(dstate.sleepIOP,portMAX_DELAY);
}
void sble_schedulerFRT_trigger_hw_wakeup(){
	xSemaphoreGive(dstate.sleepIOP);
}

void sble_scheduler_start_extThreadAccess(){
	//if(strcmp(pcTaskGetTaskName(NULL),"strategyBuffer") == 0){
	//	printf("Tu\n");
//	}else{
	//	printf("Ts\n");
//	}
	xSemaphoreTake(dstate.mutexMultExtThread,portMAX_DELAY);
//	if(strcmp(pcTaskGetTaskName(NULL),"strategyBuffer") == 0){
	//	printf("Au\n");
//	}else{
	//	printf("As\n");
//	}
}

void sble_scheduler_finish_extThreadAccess(){
	xSemaphoreGive(dstate.mutexMultExtThread);
	if(strcmp(pcTaskGetTaskName(NULL),"strategyBuffer") == 0){
	//	printf("Eu\n");
	}else{
	//	printf("Es\n");
	}

}

void sble_scheduler_wakeup(sble_thread thread) {

	if(thread == SBLE_THREAD_DISPATCHER)
	{
		SBLE_DEBUG("Waking Up Dispatcher thread.");
		xSemaphoreGive(dstate.sleepDispatcher);

		//		SBLE_DEBUG("main thread to wait for dispatcher thread going asleep...");

	}

	if(thread == SBLE_THREAD_MAIN)
	{
		SBLE_DEBUG("Waking Up main thread.");
		xSemaphoreGive(dstate.sleepMain);

		//		SBLE_DEBUG("suspending dispatcher thread to wait for main thread going asleep...");
	}

	if(thread == SBLE_THREAD_CMD_EXTERNAL)
	{
		SBLE_DEBUG("Waking Up External CMD Thread.");
		xSemaphoreGive(dstate.sleepCmdThread);

	}
	if(thread == SBLE_THREAD_EVENT_EXTERNAL)
	{
		SBLE_DEBUG("Waking Up External Event Thread.");
		xSemaphoreGive(dstate.sleepEventThread);
	}

	//xSemaphoreTake((thread==SBLE_THREAD_MAIN)?dstate.sleepDispatcher:dstate.sleepMain,portMAX_DELAY);
	//	SBLE_DEBUG("Suspension over.");

}


void sble_scheduler_autoclear_prevent(uint32_t events) {
	sble_scheduler_lock_mutex();
	dstate.evt_clear_list &= ~(events);
	sble_scheduler_unlock_mutex();
}

void sble_scheduler_autoclear_do(uint32_t events) {
	sble_scheduler_lock_mutex();
	dstate.evt_clear_list |= (events);
	sble_scheduler_unlock_mutex();

}

void sble_scheduler_events_clear(uint32_t events) {
	sble_scheduler_lock_mutex();
	dstate.flags &= ~(events);
	sble_scheduler_unlock_mutex();

}

void sble_scheduler_events_set(uint32_t events) {
	sble_scheduler_lock_mutex();
	dstate.flags |= (events);
	sble_scheduler_unlock_mutex();

}

void sble_scheduler_wait_for_event(sble_thread thread, uint32_t event_flag_mask) {
	uint8_t first = 0;
	while (1)
	{
//		SBLE_DEBUG("not got any event yet, going to sleep...");

		if(!first)
		{
			SBLE_DEBUG("waiting for event ...");
			sble_scheduler_wait(thread);
		}
		sble_scheduler_lock_mutex();

		if (dstate.flags & event_flag_mask)
		{
			dstate.flags &= ~(event_flag_mask);
			dstate.evt_clear_list |= (event_flag_mask);
			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("An event waited for was received. returning control flow to applicaton");


			//wakeup other thread (usually CBD thread)
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_MAIN : SBLE_THREAD_DISPATCHER);
			}

			SBLE_DEBUG("returning...");
			return;

		}
		else
		{
			dstate.flags &= ~(SBLE_STATE_EVENT);
			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("got something else than event waiting for...");
			/*

			 SBLE_DEBUG_CON("Event flags: ")
			 sble_print_bitfield(dstate.flags,16);
			 SBLE_DEBUG_CON("waiting for: ")
			 sble_print_bitfield(event_flag_mask,16);
			 */
			//wakeup other thread (usually CBD thread)
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_MAIN : SBLE_THREAD_DISPATCHER);

			}
			SBLE_DEBUG("cbd woke up - next wait cycle.");
		}

		first = 0;
	}
}

void sble_scheduler_wait_for_event_ext_thread(sble_thread thread, uint32_t event_flag_mask)
{
	SBLE_DEBUG("FreeRTOS wait for event");
	uint8_t first = 0;

	while (1)
	{
		if(!first)
		{
			//printf("-wfe\n");
			SBLE_DEBUG("waiting for event ...");
			sble_print_bitfield(dstate.flags_thread2,32);
			//sble_print_bitfield(dstate.flags_thread2,32);
			sble_scheduler_wait(thread);
		}
		sble_scheduler_lock_mutex();

		if (dstate.flags_thread2 & event_flag_mask)
		{
			dstate.flags_thread2 &= ~(event_flag_mask);
			dstate.evt_clear_list_thread2 |= (event_flag_mask) | SBLE_STATE_EVENT;
			dstate.flags_thread2 &= ~(SBLE_STATE_EVENT);			//required in oppsite to first thread variant as bgapi_call for
			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("Thread 2: An event waited for was received. returning control flow to applicaton");
			//printf("-succ\n");

			//wakeup other thread (usually CBD thread)
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_EVENT_EXTERNAL : SBLE_THREAD_DISPATCHER);
			}

			SBLE_DEBUG("returning...");

			return;

		}
		else
		{
			dstate.flags_thread2 &= ~(SBLE_STATE_EVENT);
			sble_scheduler_unlock_mutex();
			//printf("-diff\n");

			SBLE_DEBUG("Thread2: got something else than event waiting for...");
			/*

			 SBLE_DEBUG_CON("Event flags: ")
			 sble_print_bitfield(dstate.flags,16);
			 SBLE_DEBUG_CON("waiting for: ")
			 sble_print_bitfield(event_flag_mask,16);
			 */
			//wakeup other thread (usually CBD thread)

			//printf("got something else\n");

			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_EVENT_EXTERNAL : SBLE_THREAD_DISPATCHER);
			}

			SBLE_DEBUG("Thread 2: cbd woke up - next wait cycle.");
		}

		first = 0;
	}
}



void sble_scheduler_wait_for_event_no_reset_ext_thread(sble_thread thread, uint32_t event_flag_mask)
{
	SBLE_DEBUG("FreeRTOS wait for event");
	uint8_t first = 0;

	while (1)
	{
		if(!first)
		{
			//printf("-wfe\n");
			SBLE_DEBUG("waiting for event ...");
			sble_print_bitfield(dstate.flags_thread2,32);
			//sble_print_bitfield(dstate.flags_thread2,32);
			sble_scheduler_wait(thread);
		}
		sble_scheduler_lock_mutex();

		if (dstate.flags_thread2 & event_flag_mask)
		{
		//	dstate.flags_thread2 &= ~(event_flag_mask);
		//	dstate.evt_clear_list_thread2 |= (event_flag_mask) | SBLE_STATE_EVENT;
			dstate.flags_thread2 &= ~(SBLE_STATE_EVENT);			//required in oppsite to first thread variant as bgapi_call for
			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("An event waited for was received. returning control flow to applicaton");
			//printf("-succ\n");

			//wakeup other thread (usually CBD thread)
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_EVENT_EXTERNAL : SBLE_THREAD_DISPATCHER);
			}

			SBLE_DEBUG("returning...");

			return;

		}
		else
		{
			dstate.flags_thread2 &= ~(SBLE_STATE_EVENT);
			sble_scheduler_unlock_mutex();
			//printf("-diff\n");

			SBLE_DEBUG("got something else than event waiting for...");
			/*

			 SBLE_DEBUG_CON("Event flags: ")
			 sble_print_bitfield(dstate.flags,16);
			 SBLE_DEBUG_CON("waiting for: ")
			 sble_print_bitfield(event_flag_mask,16);
			 */
			//wakeup other thread (usually CBD thread)

			//printf("got something else\n");

			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_EVENT_EXTERNAL : SBLE_THREAD_DISPATCHER);
			}

			SBLE_DEBUG("cbd woke up - next wait cycle.");
		}

		first = 0;
	}
}

void sble_scheduler_wait_for_event_no_reset(sble_thread thread, uint32_t event_flag_mask) {
	uint8_t first = 0;

	while (1) {

		//		SBLE_DEBUG("not got any event yet, going to sleep...");
		SBLE_DEBUG("waiting for event nr...");
		if(!first){
			sble_scheduler_wait(thread);

		}
		SBLE_DEBUG("evnr woken up......");

		sble_scheduler_lock_mutex();


		if (dstate.flags & event_flag_mask) {
		//	dstate.flags &= ~(event_flag_mask);
		//	dstate.evt_clear_list |= (event_flag_mask);
		SBLE_DEBUG("An event waited for was received. returning control flow to applicaton");
		//wakeup other thread (usually CBD thread)
			sble_scheduler_unlock_mutex();
			if(!first){

				sble_scheduler_wakeup(
					(thread == SBLE_THREAD_DISPATCHER) ?
							SBLE_THREAD_MAIN : SBLE_THREAD_DISPATCHER);
				SBLE_DEBUG("returing.")
			}

			return;

		} else {
			dstate.flags &= ~(SBLE_STATE_EVENT);
			SBLE_DEBUG("got soething else than event waiting for.");
			/*

			 SBLE_DEBUG_CON("Event flags: ")
			 sble_print_bitfield(dstate.flags,16);
			 SBLE_DEBUG_CON("waiting for: ")
			 sble_print_bitfield(event_flag_mask,16);
			 */
			//wakeup other thread (usually CBD thread)
			sble_scheduler_unlock_mutex();
			if(!first){
			sble_scheduler_wakeup(
							(thread == SBLE_THREAD_DISPATCHER) ?
									SBLE_THREAD_MAIN : SBLE_THREAD_DISPATCHER);
			}
			SBLE_DEBUG("woke up CBD - next wait cycle nr");

		}

		first = 0;
	}
}


void sble_scheduler_dispatcher_shutdown(sble_thread thread) {
	SBLE_DEBUG("shutting down dispatcher thread.");
	sble_scheduler_lock_mutex();
	dstate.flags |= SBLE_STATE_TERMINATE;
	sble_scheduler_unlock_mutex();

	sble_scheduler_wakeup(SBLE_THREAD_DISPATCHER);
}

#endif
