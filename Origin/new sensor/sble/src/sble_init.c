/*
 * sble_init.c
 *
 *  Created on: 16.07.2012
 *      Author: kindt
 */
#include "sble_config.h"
#include "sble_scheduler.h"
#include "sble_debug.h"
#include "../bglib/cmd_def.h"
#include "sble_bgapi_call.h"*/
#include "sble_init.h"

void sble_init(uint8_t* param){
	if(param == NULL){
		SBLE_ERROR("got NULL pointer as param!");
	}

	sble_io_init(param);
	sble_state_init();
	sble_scheduler_init();


	//drop some connections that might still exist
	uint8_t cnt;
	uint32_t success;
	for(cnt = 0; cnt < 10; cnt++){

		sble_call_bl(ble_cmd_connection_disconnect_idx,0);
		sble_scheduler_lock_mutex();
		if(dstate.flags & SBLE_STATE_CMD_SUCCESS != 0){
			success = 1;
		}else{
			success = 0;
		}
		sble_scheduler_unlock_mutex();

		if(success){
			sble_scheduler_autoclear_prevent(SBLE_STATE_DISCONNECT_EVENT);
			sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_DISCONNECT_EVENT);
		}

	}

}

void sble_shutdown(){
	sble_io_disconnect(SBLE_THREAD_MAIN);
	sble_state_finalize();
}
