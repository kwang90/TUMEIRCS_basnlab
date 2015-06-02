/*
 * sble_state.c
 *
 *  Created on: 06.07.2012
 *      Author: kindt
 */

#include "sble_state.h"
#include "sble_ll.h"
#include <stdlib.h>

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
#include <pthread.h>
#elif SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif

sble_driver_state dstate = {
#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
		.cons = NULL,
		.filedescriptor = -1,
		.flags = 0,
		.flags_thread2 = 0,
		.cons_length = 0,
		.sble_thread = 0,
		.evt_clear_list = SBLE_EVENT_CLEAR_LIST_DEFAULT,
		.evt_clear_list_thread2 = SBLE_EVENT_CLEAR_LIST_DEFAULT| SBLE_STATE_EVENT,
		.cons_activity_map = 0,
		.flags_sched_internal = 0,
		.ll_gatt_values = NULL,
		.pl_gatt_value = NULL,
		.callback_disconnect = NULL,
		.wuf = NULL,
		.reset = NULL,
		.cmdExtRequest = SBLE_EXTCMD_IDLE,
		.sble_sending_mode = SBLE_ATTCLIENT_SENDING_MODE_CONFIRMED,
#ifdef SBLE_CFG_TIME_PROFILING
		.tScanSet = 0,
#endif

#else
		.cons = NULL,
		.flags = 0,
		.cons_length = 0,
		.sble_thread = 0,
		.filedescriptor = -1,
		.evt_clear_list = SBLE_EVENT_CLEAR_LIST_DEFAULT,
		.evt_clear_list_thread2 = SBLE_EVENT_CLEAR_LIST_DEFAULT| SBLE_STATE_EVENT,
		.cons_activity_map = 0,
		.flags_sched_internal = 0,
		.ll_gatt_values = NULL,
		.pl_gatt_value = NULL,
		.callback_disconnect = NULL,
		.wuf = NULL,
		.reset = NULL,
		.cmdExtRequest = SBLE_EXTCMD_IDLE,
		.sble_sending_mode = SBLE_ATTCLIENT_SENDING_MODE_CONFIRMED,
#endif
};


void sble_state_init(){
	dstate.flags = 0;
	dstate.flags_thread2 = 0;
	dstate.evt_clear_list = SBLE_EVENT_CLEAR_LIST_DEFAULT;
	dstate.evt_clear_list_thread2 = SBLE_EVENT_CLEAR_LIST_DEFAULT | SBLE_STATE_EVENT;
	dstate.cons_length = 0;
	dstate.ll_gatt_values = (sble_ll*) malloc(sizeof(sble_ll));
	sble_ll_init(dstate.ll_gatt_values);
	dstate.pl_gatt_value = NULL;
	dstate.callback_disconnect = NULL;
	dstate.wuf = NULL;
	dstate.reset = NULL;
	dstate.connection_update_callback = NULL;
	dstate.cmdExtRequest = SBLE_EXTCMD_IDLE;
	#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
	pthread_mutex_init(&dstate.mutexCmd,NULL);
	#elif SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
		//created by scheduler
	#endif

}

void sble_state_finalize(){
	sble_ll_free_nodes(dstate.ll_gatt_values, SBLE_TRUE);
	dstate.cons = NULL;
	dstate.flags = 0;
	dstate.evt_clear_list = SBLE_EVENT_CLEAR_LIST_DEFAULT;
	dstate.cons_length = 0;
	dstate.pl_gatt_value = NULL;
	dstate.callback_disconnect = NULL;
	dstate.cmdExtRequest = SBLE_EXTCMD_IDLE;
	dstate.wuf = NULL;
	dstate.reset = NULL;
	dstate.connection_update_callback = NULL;
	#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
	pthread_mutex_destroy(&dstate.mutexCmd);
	#endif

}

void sble_state_register_connection_update_callback(sble_connection_update_callback_t cb){
	dstate.connection_update_callback = cb;
}


