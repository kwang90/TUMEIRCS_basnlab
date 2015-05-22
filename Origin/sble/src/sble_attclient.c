/*
 * sble_attclient.c
 *
 *  Created on: 09.07.2012
 *      Author: kindt
 */

#include "sble_attclient.h"
#include "sble_scheduler.h"
#include <fcntl.h>
#include "sble_state.h"
#include "sble_bgapi_call_ext_thread.h"


void sble_attclient_getlist(uint32_t con){
	sble_scheduler_lock_mutex();

	if(!(dstate.cons_activity_map & (1<<con))){
		sble_scheduler_unlock_mutex();
		SBLE_ERROR_CONTINUABLE("Invalid Connection: %d",con);
	}

	SBLE_DEBUG("retriving attribute list...");
	sble_ll_free_nodes(dstate.cons[con]->ll_attributes,SBLE_TRUE);
	dstate.evt_clear_list &= ~(SBLE_STATE_ATTRIBUTE_INFORMATION_FOUND_EVENT);
	sble_scheduler_unlock_mutex();
	sble_call_bl(ble_cmd_attclient_find_information_idx,con,1,0xffff);
	SBLE_DEBUG("waiting for info found event...");
	sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_ATTRIBUTE_INFORMATION_FOUND_EVENT);
	SBLE_DEBUG("waiting for procedure completed event...");
	sble_scheduler_lock_mutex();
	dstate.evt_clear_list &= ~(SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
	sble_scheduler_unlock_mutex();
	sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);

	SBLE_DEBUG("Attribute list received.");

}

sble_bool sble_attclient_is_in_list(uint32_t con, const sble_attribute* att){
	sble_scheduler_lock_mutex();
	if(!(dstate.cons_activity_map & (1<<con))){
		sble_scheduler_unlock_mutex();
		SBLE_ERROR_CONTINUABLE("Invalid Connection: %d",con);
	}
	if(sble_ll_get_element(dstate.cons[con]->ll_attributes,sble_attribute_uuid_comparator,att) != NULL){
		sble_scheduler_unlock_mutex();
		return SBLE_TRUE;
	}else{
		sble_scheduler_unlock_mutex();
		return SBLE_FALSE;
	}

}

sble_attribute* sble_attclient_get_from_list(uint32_t con, const sble_attribute* att){
	if(att == NULL)
	{
		return NULL;
	}
	sble_ll_node* node;
	sble_scheduler_lock_mutex();
	if(!(dstate.cons_activity_map & (1<<con)))
	{
		sble_scheduler_unlock_mutex();
		SBLE_ERROR_CONTINUABLE("Invalid Connection: %d",con);
	}
	if((node = sble_ll_get_element(dstate.cons[con]->ll_attributes,sble_attribute_uuid_comparator,att)) != NULL)
	{
		sble_scheduler_unlock_mutex();
		return node->data;
	}
	else
	{
		sble_scheduler_unlock_mutex();
		return NULL;
	}
}

sble_bool sble_attclient_write_by_handle(uint8_t con, uint16_t handle, sble_array* data){
	return sble_attclient_write_by_handle_multithreaded(con,handle,data,0);
}

sble_bool sble_attclient_write_by_handle_multithreaded(uint8_t con, uint16_t handle, sble_array* data, uint8_t extThread)
{
	sble_bool success = SBLE_FALSE;
	sble_unsigned_integer cnt = 0;
	sble_unsigned_integer bytes_to_send = 0;

	sble_scheduler_lock_mutex();
	static uint32_t seqCnt = 0;
	if(!(dstate.cons_activity_map & (1<<con))){
		sble_scheduler_unlock_mutex();
		SBLE_ERROR_CONTINUABLE("Invalid Connection: %d",con);
	}

	//	SBLE_DEBUG("writing attribute having handle %d (0x%x).",handle,handle);
	if(data->len <= 20){
		SBLE_DEBUG("WRITING @ connection %d, handle %d, %d bytes...",con,handle,data->len);

		//if(SBLE_ATTCLIENT_SENDING_MODE == SBLE_ATTCLIENT_SENDING_MODE_CONFIRMED)
		if(dstate.sble_sending_mode == SBLE_ATTCLIENT_SENDING_MODE_CONFIRMED)
		{
			if(!extThread)
			{
				dstate.evt_clear_list &= ~(SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
				sble_scheduler_unlock_mutex();
				sble_bgapi_call_internal_bl_init(ble_cmd_attclient_attribute_write_idx);
				ble_send_message(ble_cmd_attclient_attribute_write_idx,con,handle,data->len,data->data);
				success = sble_bgapi_call_internal_bl_delay();
				if(!success){
					SBLE_ERROR("ERROR attclient attribute write.");
					while(1){
						asm("nop");
					}
				}
				//sble_call_bl(ble_cmd_attclient_attribute_write_idx,con,handle,data->len,data->data);
				SBLE_DEBUG("WAITING FOR PROCEDURE_COMPLETE...");
				sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
				sble_scheduler_lock_mutex();
			}
			else
			{
				dstate.evt_clear_list_thread2 &= ~(SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT|SBLE_STATE_EVENT);
				sble_scheduler_unlock_mutex();
				sble_bgapi_call_internal_bl_init_ext_thread(ble_cmd_attclient_attribute_write_idx);
				ble_send_message(ble_cmd_attclient_attribute_write_idx,con,handle,data->len,data->data);
				success = sble_bgapi_call_internal_bl_delay_ext_thread();

				if(!success){
					SBLE_ERROR("ERROR attclient attribute write.");
					while(1){
						asm("nop");
					}
				}

				//sble_call_bl_ext_thread(ble_cmd_attclient_attribute_write_idx,con,handle,data->len,data->data);

				SBLE_DEBUG("WAITING FOR PROCEDURE_COMPLETE EVNT...");
				//printf("acWT\n");
				//if(!(dstate.flags_thread2 & SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT))
				//{

				sble_scheduler_start_extThreadAccess();
					sble_scheduler_wait_for_event_ext_thread(SBLE_THREAD_EVENT_EXTERNAL,SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
				sble_scheduler_finish_extThreadAccess();

					SBLE_DEBUG("PROCEDURE_COMPLETE...");

				//}
				//printf("acF\n");

				sble_scheduler_lock_mutex();

				/*dstate.flags_thread2 &= ~(SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
				dstate.flags_thread2 &= ~(SBLE_STATE_EVENT);
				dstate.flags &= ~(SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
				dstate.flags &= ~(SBLE_STATE_EVENT);*/

			}
		}

		sble_scheduler_unlock_mutex();

		//if(SBLE_ATTCLIENT_SENDING_MODE == SBLE_ATTCLIENT_SENDING_MODE_UNCONFIRMED)
		if(dstate.sble_sending_mode == SBLE_ATTCLIENT_SENDING_MODE_UNCONFIRMED)
		{
				if(seqCnt < SBLE_PARAMETER_MAX_SEQUENCES - 1){
					if(!extThread)
					{
						sble_bgapi_call_internal_bl_init(ble_cmd_attclient_write_command_idx);
						ble_send_message(ble_cmd_attclient_write_command_idx,con,handle,data->len,data->data);
						success = sble_bgapi_call_internal_bl_delay();
						if(!success){
							SBLE_ERROR("ERROR attclient attribute write.");
							while(1){
								asm("nop");
							}
						}

						//sble_call_bl(ble_cmd_attclient_write_command_idx,con,handle,data->len,data->data);
					}
					else
					{

						sble_bgapi_call_internal_bl_init_ext_thread(ble_cmd_attclient_write_command_idx);
						ble_send_message(ble_cmd_attclient_write_command_idx,con,handle,data->len,data->data);
						success = sble_bgapi_call_internal_bl_delay_ext_thread();
						if(!success){
							SBLE_ERROR("ERROR attclient attribute write.");
							while(1){
								asm("nop");
							}
						}

						//sble_call_bl_ext_thread(ble_cmd_attclient_write_command_idx,con,handle,data->len,data->data);

					}
					sble_scheduler_lock_mutex();

					//seqCnt++;
					sble_scheduler_unlock_mutex();

				}else{
					sble_scheduler_lock_mutex();
					if(!extThread)
								{
									dstate.evt_clear_list &= ~(SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
									sble_scheduler_unlock_mutex();
									sble_bgapi_call_internal_bl_init(ble_cmd_attclient_attribute_write_idx);
									ble_send_message(ble_cmd_attclient_attribute_write_idx,con,handle,data->len,data->data);
									success = sble_bgapi_call_internal_bl_delay();
									if(!success){
										SBLE_ERROR("ERROR attclient attribute write.");
										while(1){
											asm("nop");
										}
									}

									//sble_call_bl(ble_cmd_attclient_attribute_write_idx,con,handle,data->len,data->data);
									SBLE_DEBUG("WAITING FOR PROCEDURE_COMPLETE...");
									sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
								}
								else
								{
									dstate.evt_clear_list_thread2 &= ~(SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT|SBLE_STATE_EVENT);
									sble_scheduler_unlock_mutex();

									sble_bgapi_call_internal_bl_init_ext_thread(ble_cmd_attclient_attribute_write_idx);
									ble_send_message(ble_cmd_attclient_attribute_write_idx,con,handle,data->len,data->data);
									success = sble_bgapi_call_internal_bl_delay_ext_thread();
									if(!success){
										SBLE_ERROR("ERROR attclient attribute write.");
										while(1){
											asm("nop");
										}
									}

									//sble_call_bl_ext_thread(ble_cmd_attclient_attribute_write_idx,con,handle,data->len,data->data);
									SBLE_DEBUG("WAITING FOR PROCEDURE_COMPLETE...");
									sble_scheduler_start_extThreadAccess();
									sble_scheduler_wait_for_event_ext_thread(SBLE_THREAD_EVENT_EXTERNAL,SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
									sble_scheduler_finish_extThreadAccess();

								}
					sble_scheduler_lock_mutex();
					seqCnt = 0;
					sble_scheduler_unlock_mutex();
				}


		}

		SBLE_DEBUG("PROCEDURE completed...");

	//data len > 20
	}else{
//		exit(1);		//2do removeme
		sble_scheduler_unlock_mutex();

		for(cnt = 0; cnt < data->len; cnt = cnt + 20){
			bytes_to_send = data->len - cnt;
			if(bytes_to_send > 20){
				bytes_to_send = 20;
			}
			sble_scheduler_lock_mutex();
			SBLE_DEBUG("prepare write %d...",cnt);
			if(!extThread){
				dstate.evt_clear_list &= ~(SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
				sble_scheduler_unlock_mutex();
				sble_call_bl(ble_cmd_attclient_prepare_write_idx,con,handle,cnt,bytes_to_send,(data->data + cnt));
				sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
			}else{
				dstate.evt_clear_list_thread2 &= ~(SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT|SBLE_STATE_EVENT);
				sble_scheduler_unlock_mutex();
				sble_call_bl_ext_thread(ble_cmd_attclient_prepare_write_idx,con,handle,cnt,bytes_to_send,(data->data + cnt));
				sble_scheduler_start_extThreadAccess();
				sble_scheduler_wait_for_event_ext_thread(SBLE_THREAD_EVENT_EXTERNAL,SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
				sble_scheduler_finish_extThreadAccess();

			}
		}
		SBLE_DEBUG("execute write...");
		sble_scheduler_lock_mutex();
		if(!extThread){

			dstate.evt_clear_list &= ~(SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
			sble_scheduler_unlock_mutex();
			sble_call_bl(ble_cmd_attclient_execute_write_idx,con,1);
			sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);

		}else{
			dstate.evt_clear_list_thread2 &= ~(SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT|SBLE_STATE_EVENT);
			sble_scheduler_unlock_mutex();
			sble_call_bl_ext_thread(ble_cmd_attclient_execute_write_idx,con,1);
			sble_scheduler_start_extThreadAccess();
			sble_scheduler_wait_for_event_ext_thread(SBLE_THREAD_EVENT_EXTERNAL,SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
			sble_scheduler_finish_extThreadAccess();

		}
	}
	//	SBLE_DEBUG("writing for procedure complete event");
//	SBLE_DEBUG("event received");
	//return SBLE_TRUE;

	return success;
}

sble_bool sble_attclient_write_by_attribute(uint8_t con, sble_attribute* att, sble_array* data){
	return sble_attclient_write_by_attribute_multithreaded(con,att,data,0);
}

sble_bool sble_attclient_write_by_attribute_multithreaded(uint8_t con, sble_attribute* att, sble_array* data, uint8_t extThread){
	sble_scheduler_lock_mutex();

	if(!(dstate.cons_activity_map & (1<<con))){
		sble_scheduler_unlock_mutex();
		SBLE_ERROR_CONTINUABLE("Invalid Connection: %d",con);
		return SBLE_FALSE;
	}
//	SBLE_DEBUG("looking for attribute:");
//	sble_print_hex_array(att->uuid->data,att->uuid->len);
	sble_scheduler_unlock_mutex();

	sble_attribute* rvh = sble_attclient_get_from_list(con,att);

	if(rvh == NULL){
		SBLE_DEBUG("Attribute not found.");
		return SBLE_FALSE;
	}else{

		//		SBLE_DEBUG("Found attribute:");
//		sble_print_hex_array(rvh->uuid->data,rvh->uuid->len);
		return sble_attclient_write_by_handle_multithreaded(con,rvh->handle,data, extThread);
	}
}

sble_bool sble_attclient_write_by_uuid(uint8_t con, sble_array* uuid, sble_array* data){
	sble_scheduler_lock_mutex();

	if(!(dstate.cons_activity_map & (1<<con))){

		SBLE_ERROR_CONTINUABLE("Invalid Connection: %d",con);
		return SBLE_FALSE;
	}
	sble_scheduler_unlock_mutex();
	sble_attribute tmp;
	tmp.uuid = uuid;
	sble_attribute* att = sble_attclient_get_from_list(con,&tmp);
	if(att == NULL){
		SBLE_DEBUG("Attribute not found.");
		return SBLE_FALSE;
	}else{
		return sble_attclient_write_by_handle(con,att->handle,data);
	}
}

sble_payload* sble_attclient_read_by_handle(uint8_t con, uint16_t handle){
	sble_bool success;
	sble_scheduler_lock_mutex();
	if(!(dstate.cons_activity_map & (1<<con))){
		sble_scheduler_unlock_mutex();
		SBLE_ERROR_CONTINUABLE("Invalid Connection: %d",con);
	}
	sble_scheduler_unlock_mutex();
	sble_scheduler_autoclear_prevent(SBLE_STATE_ATTRIBUTE_VALUE_EVENT|SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
	sble_call_bl(ble_cmd_attclient_read_by_handle_idx,con,handle);
	sble_scheduler_wait_for_event_no_reset(SBLE_THREAD_MAIN,SBLE_STATE_ATTRIBUTE_VALUE_EVENT|SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
	sble_scheduler_lock_mutex();
	if(dstate.flags & SBLE_STATE_ATTRIBUTE_VALUE_EVENT){
		success = SBLE_TRUE;
	}else{
		success = SBLE_FALSE;
	}
	sble_scheduler_unlock_mutex();
	sble_scheduler_events_clear(SBLE_STATE_ATTRIBUTE_VALUE_EVENT|SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
	sble_payload* pl;

	if(success){
		//[2do]: check attribute UUID of rcv-stack's top
		sble_scheduler_lock_mutex();
		pl = ((sble_payload*) sble_ll_pop(dstate.cons[con]->ll_rcvqueue));
		sble_scheduler_unlock_mutex();

	}else{
		pl = NULL;
	}

	return pl;
}


sble_payload* sble_attclient_wait_for_payload(uint8_t con){
	sble_scheduler_autoclear_prevent(SBLE_STATE_ATTRIBUTE_VALUE_EVENT);
	sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,SBLE_STATE_ATTRIBUTE_VALUE_EVENT);
	sble_scheduler_lock_mutex();
	sble_payload* rv = ((sble_payload*) sble_ll_pop(dstate.cons[con]->ll_rcvqueue));
	sble_scheduler_unlock_mutex();

	return rv;
}

sble_payload* sble_attclient_read_by_uuid(uint8_t con, sble_array* uuid){
	sble_bool success;
	sble_scheduler_lock_mutex();

	if(!(dstate.cons_activity_map & (1<<con))){
		sble_scheduler_unlock_mutex();
		SBLE_ERROR_CONTINUABLE("Invalid Connection: %d",con);
	}
	sble_scheduler_unlock_mutex();
	sble_scheduler_autoclear_prevent(SBLE_STATE_ATTRIBUTE_VALUE_EVENT|SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
	sble_call_bl(ble_cmd_attclient_read_by_type_idx,0,1,0xffff,uuid->len,uuid->data);
	sble_scheduler_wait_for_event_no_reset(SBLE_THREAD_MAIN,SBLE_STATE_ATTRIBUTE_VALUE_EVENT|SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
	sble_scheduler_lock_mutex();

	if(dstate.flags & SBLE_STATE_ATTRIBUTE_VALUE_EVENT){
		success = SBLE_TRUE;
	}else{
		success = SBLE_FALSE;
	}
	sble_scheduler_unlock_mutex();
	sble_scheduler_events_clear(SBLE_STATE_ATTRIBUTE_VALUE_EVENT|SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT);
	sble_payload* pl;
	if(success){
		//[2do]: check attribute UUID of rcv-stack's top
		sble_scheduler_lock_mutex();
		pl = ((sble_payload*) sble_ll_pop(dstate.cons[con]->ll_rcvqueue));
		sble_scheduler_unlock_mutex();
	}else{
		pl = NULL;
	}
	return pl;
}

sble_payload* sble_attclient_read_by_attribute(uint8_t con, sble_attribute* att){
	sble_scheduler_lock_mutex();

	if(!(dstate.cons_activity_map & (1<<con))){
		sble_scheduler_unlock_mutex();
		SBLE_ERROR_CONTINUABLE("Invalid Connection: %d",con);
	}

	sble_scheduler_unlock_mutex();
	sble_payload* pl = sble_attclient_read_by_uuid(con,att->uuid);
	return pl;
}

void sble_attclient_set_sending_mode(uint8_t sending_mode)
{
	sble_scheduler_lock_mutex();
	dstate.sble_sending_mode = sending_mode;
	sble_scheduler_unlock_mutex();
}
