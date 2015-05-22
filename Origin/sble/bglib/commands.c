
/*****************************************************************************
 *
 *
 *
 *
 *
 ****************************************************************************/

#include "cmd_def.h"
#include "../include/sble_debug.h"
#include "../include/sble_config.h"

#include "../include/sble_ll.h"
#include "../include/sble_attclient.h"
#include "../include/sble_scheduler.h"
#include "../include/sble_state.h"
#include "../include/sble_event_handler_functions.h"
#include "../include/sble_types.h"
#ifdef SBLE_CFG_TIME_PROFILING
#include <time.h>
#endif

void ble_default(const void*v)
{
	SBLE_DEBUG("WARNING: default handler called - completely unhandled event eoccured.");
}

void ble_rsp_system_reset(const void* nul)
{
	SBLE_DEBUG("System reset message received.");
}

void ble_rsp_system_hello(const void* nul)
{
	SBLE_DEBUG("System hello message received.");
}

void ble_rsp_system_address_get(const struct ble_msg_system_address_get_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_system_reg_write(const struct ble_msg_system_reg_write_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_system_reg_read(const struct ble_msg_system_reg_read_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_system_get_counters(const struct ble_msg_system_get_counters_rsp_t *msg)
{
	sble_scheduler_lock_mutex();

	dstate.cons[0]->param = malloc(sizeof(struct ble_msg_system_get_counters_rsp_t));
	if(dstate.cons[0]->param != NULL){
		*(( struct ble_msg_system_get_counters_rsp_t*) dstate.cons[0]->param) = *msg;
	}
	sble_scheduler_unlock_mutex();
}

void ble_rsp_system_get_connections(const struct ble_msg_system_get_connections_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_system_read_memory(const struct ble_msg_system_read_memory_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_system_get_info(const struct ble_msg_system_get_info_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_system_endpoint_tx(const struct ble_msg_system_endpoint_tx_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
			if(msg->result == 0){
				sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
			}else{
				sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
			}
}

void ble_rsp_system_whitelist_append(const struct ble_msg_system_whitelist_append_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_system_whitelist_remove(const struct ble_msg_system_whitelist_remove_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
			if(msg->result == 0){
				sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
			}else{
				sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
			}
}

void ble_rsp_system_whitelist_clear(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_system_endpoint_rx(const struct ble_msg_system_endpoint_rx_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_system_endpoint_set_watermarks(const struct ble_msg_system_endpoint_set_watermarks_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		//printf("watermark set sucessfully.\n");
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		//printf("watermark set failed.\n");
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_flash_ps_defrag(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_flash_ps_dump(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_flash_ps_erase_all(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_flash_ps_save(const struct ble_msg_flash_ps_save_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
		if(msg->result == 0){
			sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
		}else{
			sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
		}
}

void ble_rsp_flash_ps_load(const struct ble_msg_flash_ps_load_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}

}

void ble_rsp_flash_ps_erase(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_flash_erase_page(const struct ble_msg_flash_erase_page_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
		if(msg->result == 0){
			sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
		}else{
			sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
		}
}

void ble_rsp_flash_write_words(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_attributes_write(const struct ble_msg_attributes_write_rsp_t *msg)
{
	SBLE_DEBUG("attributes write response. Result: 0x%x",msg->result);
		if(msg->result == 0){
			sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
		}else{
			sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
		}
}

void ble_rsp_attributes_read(const struct ble_msg_attributes_read_rsp_t *msg)
{
	SBLE_DEBUG("attributes read response. Result: 0x%x",msg->result);
	sble_scheduler_lock_mutex();
	if(dstate.pl_gatt_value != NULL){
		SBLE_DEBUG("Freeing pl...");
		sble_payload_free_whole(&(dstate.pl_gatt_value));
	}

	//	SBLE_DEBUG("malloc pl...");

	sble_payload_malloc_whole(&(dstate.pl_gatt_value),msg->value.len);
	memcpy(dstate.pl_gatt_value->data->data,msg->value.data, msg->value.len);
	dstate.flags |= SBLE_STATE_GATT_VALUE_RESPONSE;
	sble_scheduler_unlock_mutex();

	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}


void ble_rsp_attributes_read_type(const struct ble_msg_attributes_read_type_rsp_t *msg)
{
	SBLE_DEBUG("attributes read type response.");

	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_attributes_user_read_response(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_attributes_user_write_response(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_connection_disconnect(const struct ble_msg_connection_disconnect_rsp_t *msg)
{
	SBLE_DEBUG("Disconnect response.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_connection_get_rssi(const struct ble_msg_connection_get_rssi_rsp_t *msg)
{
	sble_scheduler_lock_mutex();

	dstate.cons[msg->connection]->param = malloc(sizeof(int8_t));
	*((int8_t*)dstate.cons[msg->connection]->param) = msg->rssi;
	SBLE_DEBUG("unknown response/event.");
	sble_scheduler_unlock_mutex();

}

void ble_rsp_connection_update(const struct ble_msg_connection_update_rsp_t *msg)
{
	SBLE_DEBUG("Connection Update Response @ con %d, result= 0x%x",msg->connection, msg->result);
	sble_scheduler_lock_mutex();
    dstate.cons[msg->connection]->secondary_flags &= ~(SBLE_STATE_SECONDARY_CONNECTION_UPDATE_OCCURED);
	sble_scheduler_unlock_mutex();

	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_connection_version_update(const struct ble_msg_connection_version_update_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
		if(msg->result == 0){
			sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
		}else{
			sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
		}
}

void ble_rsp_connection_channel_map_get(const struct ble_msg_connection_channel_map_get_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_connection_channel_map_set(const struct ble_msg_connection_channel_map_set_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_connection_features_get(const struct ble_msg_connection_features_get_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
		if(msg->result == 0){
			sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
		}else{
			sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
		}
}

void ble_rsp_connection_get_status(const struct ble_msg_connection_get_status_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_connection_raw_tx(const struct ble_msg_connection_raw_tx_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_attclient_find_by_type_value(const struct ble_msg_attclient_find_by_type_value_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_attclient_read_by_group_type(const struct ble_msg_attclient_read_by_group_type_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_attclient_read_by_type(const struct ble_msg_attclient_read_by_type_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_attclient_find_information(const struct ble_msg_attclient_find_information_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}


void ble_rsp_attclient_read_by_handle(const struct ble_msg_attclient_read_by_handle_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_attclient_attribute_write(const struct ble_msg_attclient_attribute_write_rsp_t *msg)
{
	SBLE_DEBUG("attribute write response. con = %d, result=0x%x",msg->connection, msg->result);
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_attclient_write_command(const struct ble_msg_attclient_write_command_rsp_t *msg)
{
	SBLE_DEBUG("attclient write response @ connection %x, result = 0x%x",msg->connection,msg->result);

	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_attclient_indicate_confirm(const struct ble_msg_attclient_indicate_confirm_rsp_t *msg)
{
	SBLE_DEBUG("attclent indicate confirm response  result = 0x%x",msg->result);
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_attclient_read_long(const struct ble_msg_attclient_read_long_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}

}

void ble_rsp_attclient_prepare_write(const struct ble_msg_attclient_prepare_write_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_attclient_execute_write(const struct ble_msg_attclient_execute_write_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_attclient_read_multiple(const struct ble_msg_attclient_read_multiple_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_sm_encrypt_start(const struct ble_msg_sm_encrypt_start_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_sm_set_bondable_mode(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_sm_delete_bonding(const struct ble_msg_sm_delete_bonding_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_sm_set_parameters(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_sm_passkey_entry(const struct ble_msg_sm_passkey_entry_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_sm_get_bonds(const struct ble_msg_sm_get_bonds_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_sm_set_oob_data(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_gap_set_privacy_flags(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_gap_set_mode(const struct ble_msg_gap_set_mode_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_gap_discover(const struct ble_msg_gap_discover_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_gap_connect_direct(const struct ble_msg_gap_connect_direct_rsp_t *msg)
{
	SBLE_DEBUG("Connect direct response - result: 0x%x",msg->result);
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_gap_end_procedure(const struct ble_msg_gap_end_procedure_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_gap_connect_selective(const struct ble_msg_gap_connect_selective_rsp_t *msg)
{
	SBLE_DEBUG("Connect selective result: 0x%x",msg->result);
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_gap_set_filtering(const struct ble_msg_gap_set_filtering_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
		if(msg->result == 0){
			sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
		}else{
			sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
		}
}

void ble_rsp_gap_set_scan_parameters(const struct ble_msg_gap_set_scan_parameters_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_gap_set_adv_parameters(const struct ble_msg_gap_set_adv_parameters_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_gap_set_adv_data(const struct ble_msg_gap_set_adv_data_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}

}

void ble_rsp_gap_set_directed_connectable_mode(const struct ble_msg_gap_set_directed_connectable_mode_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_io_port_config_irq(const struct ble_msg_hardware_io_port_config_irq_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_set_soft_timer(const struct ble_msg_hardware_set_soft_timer_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_adc_read(const struct ble_msg_hardware_adc_read_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_io_port_config_direction(const struct ble_msg_hardware_io_port_config_direction_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_io_port_config_function(const struct ble_msg_hardware_io_port_config_function_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_io_port_config_pull(const struct ble_msg_hardware_io_port_config_pull_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_io_port_write(const struct ble_msg_hardware_io_port_write_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_io_port_read(const struct ble_msg_hardware_io_port_read_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_spi_config(const struct ble_msg_hardware_spi_config_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_spi_transfer(const struct ble_msg_hardware_spi_transfer_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_i2c_read(const struct ble_msg_hardware_i2c_read_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_hardware_i2c_write(const struct ble_msg_hardware_i2c_write_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_hardware_set_txpower(const void* nul)
{
	SBLE_DEBUG("TX power set");
}

void ble_rsp_hardware_timer_comparator(const struct ble_msg_hardware_timer_comparator_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
	if(msg->result == 0){
		sble_scheduler_events_set(SBLE_STATE_CMD_SUCCESS);
	}else{
		sble_scheduler_events_clear(SBLE_STATE_CMD_SUCCESS);
	}
}

void ble_rsp_test_phy_tx(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_test_phy_rx(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_test_phy_end(const struct ble_msg_test_phy_end_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_test_phy_reset(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_test_get_channel_map(const struct ble_msg_test_get_channel_map_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_test_debug(const struct ble_msg_test_debug_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_dfu_reset(const void* nul)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_dfu_flash_set_address(const struct ble_msg_dfu_flash_set_address_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_dfu_flash_upload(const struct ble_msg_dfu_flash_upload_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_rsp_dfu_flash_upload_finish(const struct ble_msg_dfu_flash_upload_finish_rsp_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_system_boot(const struct ble_msg_system_boot_evt_t *msg)
{
	SBLE_DEBUG("BLE/BLED112 radio booted with api version %d",msg->protocol_version);
}

void ble_evt_system_debug(const struct ble_msg_system_debug_evt_t *msg)
{
	SBLE_DEBUG("System Debug event: %x", msg->data);
}

void ble_evt_system_endpoint_watermark_rx(const struct ble_msg_system_endpoint_watermark_rx_evt_t *msg)
{
	//printf("rx watermark.\n");

	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_system_endpoint_watermark_tx(const struct ble_msg_system_endpoint_watermark_tx_evt_t *msg)
{
	//printf("tx watermark.\n");
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_system_script_failure(const struct ble_msg_system_script_failure_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_system_no_license_key(const void* nul)
{
	SBLE_DEBUG("\n\n\nWARNING: NO LICENSE KEY FOUND!\n\n\n.");
}

void ble_evt_flash_ps_key(const struct ble_msg_flash_ps_key_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_attributes_value(const struct ble_msg_attributes_value_evt_t *msg)
{
	SBLE_DEBUG("GATT value event @con %d - Handle = %d, Offset = %d, Reason=%d, Value lenght: %d",msg->connection,msg->handle,msg->offset,msg->reason,msg->value.len);
//	SBLE_DEBUG("AGATT value event @con %d - Handle = %d, Offset = %d, Reason=%d, Value lenght: %d",msg->connection,msg->handle,msg->offset,msg->reason,msg->value.len);
//	SBLE_DEBUG("BGATT value event @con %d - Handle = %d, Offset = %d, Reason=%d, Value lenght: %d",msg->connection,msg->handle,msg->offset,msg->reason,msg->value.len);
//	SBLE_DEBUG("CGATT value event @con %d - Handle = %d, Offset = %d, Reason=%d, Value lenght: %d",msg->connection,msg->handle,msg->offset,msg->reason,msg->value.len);

	sble_scheduler_events_set(SBLE_STATE_EVENT | SBLE_STATE_GATT_VALUE_EVENT);
	sble_payload *pl;
	sble_ll_node* node;
	sble_payload pl_cmp;

	if(msg->offset == 0)
	{
		SBLE_DEBUG("Received new handle");
		//receive a new gatt packet
		sble_payload_malloc_whole(&pl,msg->value.len);
		pl->atthandle = msg->handle;
		pl->connection = msg->connection;
		memcpy(pl->data->data,msg->value.data,msg->value.len);
		sble_scheduler_lock_mutex();
		sble_ll_push(dstate.ll_gatt_values,pl);
		sble_scheduler_unlock_mutex();
	}
	else
	{
//			SBLE_DEBUG("Received additional data for existing handle. Lenght: %x -> %x", pl->data->len,msg->value.len + msg->offset);
		//receive a continued long attribute
		pl_cmp.atthandle = msg->handle,
		sble_scheduler_lock_mutex();
		node = sble_ll_get_element(dstate.ll_gatt_values,sble_payload_comparator_by_handle,&pl_cmp);
		sble_scheduler_unlock_mutex();

		if(node != NULL)
		{
			pl = (sble_payload*) node->data;
		}
		else
		{
			return;
		}
		if(pl != NULL)
		{
			if(pl->data != NULL)
			{
				sble_array_resize(pl->data,msg->offset + msg->value.len);
				memcpy(pl->data->data + msg->offset,msg->value.data,msg->value.len);
			}
		}
	}
}

void ble_evt_attributes_user_read_request(const struct ble_msg_attributes_user_read_request_evt_t *msg)
{
	SBLE_DEBUG("*** UNHANDLED EVENT OCCURED *** Attributes user_read-event @con %d - Handle = %d, Offset = %d",msg->connection,msg->handle,msg->offset);
}

void ble_evt_attributes_status(const struct ble_msg_attributes_status_evt_t *msg)
{
	SBLE_DEBUG("*** UNHANDLED EVENT OCCURED *** Attributes user_read-event - Handle = %d, Flags=%d",msg->handle,msg->flags);
}

void ble_evt_connection_status(const struct ble_msg_connection_status_evt_t *msg)
{
	SBLE_DEBUG("Connection Status Event");
	sble_scheduler_lock_mutex();

	dstate.flags |= SBLE_STATE_EVENT;
	dstate.flags |= SBLE_STATE_CONNECTION_EVENT;
	dstate.current_con = msg->connection;
	sble_scheduler_unlock_mutex();

		if(msg->flags & connection_completed){
	        SBLE_DEBUG("Connection Status: connected @ con %d\n",msg->connection);

	        SBLE_DEBUG_CON("Remote device address: ");
	        //sble_print_hex_array(msg->address.addr, 6);
	        sble_evth_connection_established(msg);

	        //remove all data from the linked list
	        sble_ll_remove_all_elements(dstate.ll_gatt_values);
	   }else if(msg->flags & connection_parameters_change){
		   SBLE_DEBUG("Connection parameter update - new connection interval: Tc = %d / %f s",msg->conn_interval ,msg->conn_interval * 0.000625);
		   sble_scheduler_lock_mutex();
		   dstate.cons[msg->connection]->secondary_flags |= SBLE_STATE_SECONDARY_CONNECTION_UPDATE_OCCURED;
		   if(dstate.connection_update_callback != NULL){
			   dstate.connection_update_callback(msg->connection, msg->conn_interval,msg->latency,msg->timeout);
			   sble_scheduler_unlock_mutex();

		   }else{
			   sble_scheduler_unlock_mutex();
		   }
	   }
}

void ble_evt_connection_version_ind(const struct ble_msg_connection_version_ind_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_connection_feature_ind(const struct ble_msg_connection_feature_ind_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_connection_raw_rx(const struct ble_msg_connection_raw_rx_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_connection_disconnected(const struct ble_msg_connection_disconnected_evt_t *msg)
{
	SBLE_DEBUG("disconnected event: connection: %d, reason: 0x%x\n",msg->connection,msg->reason);
		sble_evth_disconnected(msg);
		sble_scheduler_lock_mutex();
		uint32_t cnt = 0;
		dstate.flags |= SBLE_STATE_DISCONNECT_EVENT;
		dstate.flags |= SBLE_STATE_EVENT;
		if(dstate.callback_disconnect != NULL){
			sble_scheduler_unlock_mutex();

			dstate.callback_disconnect(msg->connection, msg->reason);
		}else{
			sble_scheduler_unlock_mutex();
		}


}

void ble_evt_attclient_indicated(const struct ble_msg_attclient_indicated_evt_t *msg)
{
	sble_scheduler_lock_mutex();
	SBLE_DEBUG("Indication confirmation arrived");
	dstate.flags |= SBLE_STATE_INDICATION_CONFIRM_EVENT;
	dstate.flags |= SBLE_STATE_EVENT;
	sble_scheduler_unlock_mutex();

}

void ble_evt_attclient_procedure_completed(const struct ble_msg_attclient_procedure_completed_evt_t *msg)
{
	sble_scheduler_lock_mutex();
	SBLE_DEBUG("procedure complete event @ connection %d, handle %d. Return value: %x",msg->connection, msg->chrhandle,msg->result);
	dstate.flags |= SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT;
	dstate.flags |= SBLE_STATE_EVENT;
	sble_scheduler_unlock_mutex();
}

void ble_evt_attclient_group_found(const struct ble_msg_attclient_group_found_evt_t *msg)
{
	SBLE_DEBUG("**UNSUPPORTED EVENT OCCURED** : attlient group found event.");
}

void ble_evt_attclient_attribute_found(const struct ble_msg_attclient_attribute_found_evt_t *msg)
{
	SBLE_DEBUG("**UNSUPPORTED EVENT OCCURED** : attlient attribute found event.");
	//	SBLE_DEBUG("Attclient find information found:");
		//	sble_print_hex_array(msg->uuid.data, msg->uuid.len);
//	sble_scheduler_lock_mutex();

	//	dstate.flags |= SBLE_STATE_EVENT;
	//	dstate.flags |= SBLE_STATE_ATTRIBUTE_FOUND_EVENT;
	/*
		sble_scheduler_unlock_mutex();

		sble_attribute *att;
		sble_attribute_malloc_whole(&att,msg->uuid.len);
		memcpy(arr_uuid->data,msg->uuid.data,msg->uuid.len);
		att->chrhandle = msg->chrhandle;
		att->handle = msg->
			sble_scheduler_lock_mutex();
		sble_ll_push(dstate.cons[msg->connection]->ll_attributes,arr_uuid);
			sble_scheduler_unlock_mutex();

	*/
}

void ble_evt_attclient_find_information_found(const struct ble_msg_attclient_find_information_found_evt_t *msg)
{
	SBLE_DEBUG("Attclient find information found event");
			sble_print_hex_array(msg->uuid.data, msg->uuid.len);
			SBLE_DEBUG("-> handle %d", msg->chrhandle);
				SBLE_DEBUG_CON("----------------\n");
			sble_scheduler_lock_mutex();
			dstate.flags |= SBLE_STATE_EVENT;
			dstate.flags |= SBLE_STATE_ATTRIBUTE_INFORMATION_FOUND_EVENT;
			sble_scheduler_unlock_mutex();

			sble_attribute *att;
			sble_attribute_malloc_whole(&att,msg->uuid.len);
			if(att != NULL){
				memcpy(att->uuid->data,msg->uuid.data,msg->uuid.len);
				att->handle = msg->chrhandle;
				sble_scheduler_lock_mutex();
				sble_ll_push_unique(dstate.cons[msg->connection]->ll_attributes,att,sble_attribute_uuid_comparator);

				sble_scheduler_unlock_mutex();


			}else{
				SBLE_DEBUG("Could not allocc attribute.");
			}
}

void ble_evt_attclient_attribute_value(const struct ble_msg_attclient_attribute_value_evt_t *msg)
{
		SBLE_DEBUG("event occured: attribute value event");
		SBLE_DEBUG_CON("Data received: ");
		sble_print_hex_array(msg->value.data,msg->value.len);

		sble_scheduler_lock_mutex();

		dstate.flags |= SBLE_STATE_EVENT;
		dstate.flags |= SBLE_STATE_ATTRIBUTE_VALUE_EVENT;
		sble_scheduler_unlock_mutex();

		sble_payload* pl;
		sble_payload_malloc_whole(&pl,msg->value.len);
		if(pl != NULL){
			memcpy(pl->data->data,msg->value.data,msg->value.len);
			pl->atthandle = msg->atthandle;
			pl->connection = msg->connection;
			sble_scheduler_lock_mutex();
			sble_ll_push(dstate.cons[msg->connection]->ll_rcvqueue,pl);
			sble_scheduler_unlock_mutex();

		}
}

void ble_evt_attclient_read_multiple_response(const struct ble_msg_attclient_read_multiple_response_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_sm_smp_data(const struct ble_msg_sm_smp_data_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_sm_bonding_fail(const struct ble_msg_sm_bonding_fail_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_sm_passkey_display(const struct ble_msg_sm_passkey_display_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_sm_passkey_request(const struct ble_msg_sm_passkey_request_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_sm_bond_status(const struct ble_msg_sm_bond_status_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_gap_scan_response(const struct ble_msg_gap_scan_response_evt_t *msg)
{

#ifdef SBLE_CFG_TIME_PROFILING
	sble_scheduler_lock_mutex();

	if(dstate.tScanSet == 0){
		clock_gettime(CLOCK_MONOTONIC,&dstate.tScan);
		dstate.tScanSet = 1;
		SBLE_DEBUG("Time set.");;
	}else{
		SBLE_DEBUG("Time not set.");;

	}
	sble_scheduler_unlock_mutex();

#endif

		SBLE_DEBUG("GAP Scan Response - Addr:");
		sble_print_hex_array(msg->sender.addr,6);
		sble_scheduler_lock_mutex();
		dstate.flags |= SBLE_STATE_SCAN_RESPONSE_EVENT;
		dstate.flags |= SBLE_STATE_EVENT;
		memcpy(dstate.addr,msg->sender.addr,6);
		dstate.addr_type = msg->address_type;
		sble_scheduler_unlock_mutex();

		//advertising example:
		//receiving data from advertising packet
		//enable code below for sble_experiment_adv_data_*.c examples
		
		if(msg->data.len > 0)

		{
			sble_payload* pl;
			sble_payload_malloc_whole(&pl,msg->data.len);
			memcpy(pl->data->data,msg->data.data,msg->data.len);
			//sble_print_hex_array(msg->data.data,msg->data.len);
			sble_ll_push(dstate.ll_gatt_values,pl);
		}
		

}

void ble_evt_gap_mode_changed(const struct ble_msg_gap_mode_changed_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_hardware_io_port_status(const struct ble_msg_hardware_io_port_status_evt_t *msg)
{

	SBLE_DEBUG("I/O port status event at port: %u",msg->port);

	if(msg->port == 0)
	{

		//printf("*W*\n");
		SBLE_DEBUG("SBLE_STATE_IO_PORT_STATUS set");

		sble_scheduler_lock_mutex();

		dstate.flags |= SBLE_STATE_IO_PORT_STATUS;
		dstate.flags |= SBLE_STATE_EVENT;

		sble_scheduler_unlock_mutex();
#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
		sble_schedulerFRT_trigger_hw_wakeup();
#endif
	}

}

void ble_evt_hardware_soft_timer(const struct ble_msg_hardware_soft_timer_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_hardware_adc_result(const struct ble_msg_hardware_adc_result_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

void ble_evt_dfu_boot(const struct ble_msg_dfu_boot_evt_t *msg)
{
	SBLE_DEBUG("unknown response/event.");
}

