/*
 * sble_init_freeRTOS.c
 *
 *  Created on: 16.07.2012
 *      Author: kindt
 */
#include "sble_init.h"
#include "sble_config.h"
#include "sble_scheduler.h"
#include "sble_debug.h"
#include "../bglib/cmd_def.h"
#include "sble_bgapi_call.h"*/
#include "sble_init.h"
#include "ports/FreeRTOS/sble_init_FreeRTOS.h"

#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS

void sble_init_hw(uint8_t* param,sble_hw_callback_t wuf, sble_hw_callback_t reset){
	if(param == NULL){
		SBLE_ERROR("got NULL pointer as param!");
	}

	dstate.wuf = wuf;
	dstate.reset = reset;
	sble_io_init(param);
	sble_state_init();
	dstate.wuf = wuf;
	dstate.reset = reset;
	sble_scheduler_init();
//	printf("activating watermarks (hw)\n");
//	sble_call_bl(ble_cmd_system_endpoint_set_watermarks_idx,0,1,1);

}

#endif
