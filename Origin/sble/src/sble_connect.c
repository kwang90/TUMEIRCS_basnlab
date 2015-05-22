/*
 * sble_connect.c
 *
 *  Created on: 09.07.2012
 *      Author: kindt
 */

#include "sble_connect.h"
#include "sble_scheduler.h"
#include <string.h>
uint8_t sble_connect_to_any(uint16_t con_int_min, uint16_t con_int_max, uint16_t timeout, uint16_t latency)
{
		SBLE_DEBUG("Starting GAP discovery...");
		SBLE_DEBUG("waiting for event.");

		sble_scheduler_lock_mutex();
		dstate.evt_clear_list &= ~(SBLE_STATE_SCAN_RESPONSE_EVENT);
		sble_scheduler_unlock_mutex();

		sble_call_bl(ble_cmd_gap_discover_idx,1);
		//printf("Waiting for scan response event...\n");
		sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_SCAN_RESPONSE_EVENT);

		//printf("Found remote device. Connecting...\n");
		SBLE_DEBUG("Found remote device. Connecting...");
		sble_scheduler_lock_mutex();

		dstate.evt_clear_list &= ~(SBLE_STATE_CONNECTION_EVENT);
		sble_scheduler_unlock_mutex();
		sble_print_hex_array(dstate.addr,16);
		//sble_call_bl(ble_cmd_gap_connect_direct_idx,dstate.addr, gap_address_type_random, con_int_min,con_int_max,timeout,latency);
		sble_call_bl(ble_cmd_gap_connect_direct_idx,dstate.addr, gap_address_type_public, con_int_min,con_int_max,timeout,latency);
		sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_CONNECTION_EVENT);
		SBLE_DEBUG("Connected.");
	//	sble_call_bl(ble_cmd_gap_end_procedure_idx,245);

		uint8_t ccon;
		sble_scheduler_lock_mutex();
		ccon = dstate.current_con;
		sble_scheduler_unlock_mutex();
		return ccon;

}

uint8_t sble_wait_for_new_connection(){
	sble_scheduler_lock_mutex();
	dstate.evt_clear_list &= ~(SBLE_STATE_CONNECTION_EVENT);
	sble_scheduler_unlock_mutex();
	sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_CONNECTION_EVENT);
	uint8_t ccon;
	sble_scheduler_lock_mutex();
	ccon = dstate.current_con;
	sble_scheduler_unlock_mutex();
	return ccon;

}

void sble_make_connectable_by_any(uint16_t adv_int_min,uint16_t adv_int_max){
			sble_call_bl(ble_cmd_gap_set_adv_parameters_idx,adv_int_min,adv_int_max,0x01);		//better: 0x07 for all channels
			sble_call_bl(ble_cmd_gap_set_mode_idx, gap_general_discoverable,gap_undirected_connectable);
			SBLE_DEBUG("Starting BLE advertising sequence...");
};

uint8_t sble_connect_to(sble_array* addr, sble_addr_type type,uint16_t con_interval_min, uint16_t con_interval_max, uint16_t timeout, uint16_t slave_latency){
			if(addr->len != 6){
				SBLE_ERROR("Wrong address lenght. Address must have 48 Bits / 6 Bytes lenght. Not attemting to connect remote device.");
			}
			SBLE_DEBUG_CON("Connecting to:");
			sble_print_hex_array(addr->data, addr->len);
			sble_scheduler_lock_mutex();
			dstate.evt_clear_list &= ~(SBLE_STATE_CONNECTION_EVENT);
			sble_scheduler_unlock_mutex();
			sble_call_bl(ble_cmd_gap_connect_direct_idx, addr->data,(type==SBLE_ADDRESS_PUBLIC)?gap_address_type_public:gap_address_type_random,con_interval_min, con_interval_max,timeout,slave_latency);
			sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_CONNECTION_EVENT);
			SBLE_DEBUG("Connected.");
			uint8_t ccon;
			sble_scheduler_lock_mutex();
			ccon = dstate.current_con;
			sble_scheduler_unlock_mutex();
			return ccon;
};


sble_bool sble_disconnect(uint8_t con){
	sble_scheduler_lock_mutex();
	if(!(dstate.cons_activity_map & (1<<con))){
		sble_scheduler_unlock_mutex();

		SBLE_ERROR_CONTINUABLE("Invalid Connection: %d",con);
		return SBLE_FALSE;
	}
	sble_scheduler_unlock_mutex();

	sble_call_bl(ble_cmd_connection_disconnect_idx,con);
	return SBLE_TRUE;

}
