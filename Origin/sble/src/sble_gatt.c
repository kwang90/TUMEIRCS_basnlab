/*
 * sble_gatt.c
 *
 *  Created on: 12.07.2012
 *      Author: kindt
 */




#include "sble_gatt.h"

void sble_gatt_write_by_handle(uint16_t handle,sble_array* data){
	sble_call_bl(ble_cmd_attributes_write_idx,handle,0,data->len,data->data);
}

sble_payload* sble_gatt_receive(){
	sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT);
	sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_GATT_VALUE_EVENT);
	sble_scheduler_lock_mutex();
	sble_payload* pl = sble_ll_pop_from_begin(dstate.ll_gatt_values);
	sble_scheduler_unlock_mutex();
	return pl;
}

sble_payload* sble_gatt_receive_long(sble_unsigned_integer atthandle, sble_unsigned_integer data_length){
	sble_payload pl_cmp;
	pl_cmp.atthandle = atthandle;
	sble_payload *pl;
	do{
		sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT);
		sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_GATT_VALUE_EVENT);
		sble_scheduler_lock_mutex();
		pl = sble_ll_get_element(dstate.ll_gatt_values,sble_payload_comparator_by_handle,&pl_cmp);
		sble_scheduler_unlock_mutex();

		if(pl == NULL){
			return NULL;
		}
	}while(pl->data->len < data_length);
	sble_scheduler_lock_mutex();
	pl = sble_ll_pop_from_begin(dstate.ll_gatt_values);
	sble_scheduler_unlock_mutex();
	return pl;
}

sble_payload* sble_gatt_receive_until_next(){
	sble_payload *pl;
	uint32_t element_cnt, element_cnt_tmp;
	sble_scheduler_lock_mutex();
	element_cnt = sble_ll_get_nr_of_elements(dstate.ll_gatt_values);
	sble_scheduler_unlock_mutex();

	do{
		SBLE_DEBUG("rcv chunk...");

		sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT);
		sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_GATT_VALUE_EVENT);
		sble_scheduler_lock_mutex();
		element_cnt_tmp = sble_ll_get_nr_of_elements(dstate.ll_gatt_values);
		sble_scheduler_unlock_mutex();

	}while(element_cnt_tmp < element_cnt + 1);

		SBLE_DEBUG("Reception complete. Element Cnt: %d -> %d\n",element_cnt, element_cnt_tmp);
		sble_scheduler_lock_mutex();
		pl = sble_ll_pop_from_begin(dstate.ll_gatt_values);
		sble_scheduler_unlock_mutex();
		SBLE_DEBUG("removed.\n");

	return pl;
}

sble_payload* sble_gatt_read_by_handle(uint16_t handle){
	sble_scheduler_events_clear(SBLE_STATE_GATT_VALUE_RESPONSE);
	sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_RESPONSE);
	sble_call_bl(ble_cmd_attributes_read_idx,handle,0);
	sble_scheduler_autoclear_do(SBLE_STATE_GATT_VALUE_RESPONSE);
	sble_scheduler_lock_mutex();
	if(dstate.flags & SBLE_STATE_GATT_VALUE_RESPONSE){
		sble_scheduler_unlock_mutex();
		sble_scheduler_events_clear(SBLE_STATE_GATT_VALUE_RESPONSE);
		return dstate.pl_gatt_value;
	}else{
		sble_scheduler_unlock_mutex();
		return NULL;
	}
}

sble_array* sble_gatt_get_type(uint16_t handle){
	sble_call_bl(ble_cmd_attributes_read_type_idx, handle);
}

