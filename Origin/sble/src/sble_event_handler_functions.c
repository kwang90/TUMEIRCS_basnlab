/*
 * sble_event_handler_functions.c
 *
 *  Created on: 11.07.2012
 *      Author: kindt
 */

#include "sble_state.h"
#include "sble_scheduler.h"
#include <string.h>
#include "sble_event_handler_functions.h"

void sble_evth_connection_established(const struct ble_msg_connection_status_evt_t *msg){
	sble_scheduler_lock_mutex();
	if(dstate.cons_length == 0){
		dstate.cons_length = 1;
		dstate.cons = malloc(sizeof(sble_state));
	}else if(msg->connection > dstate.cons_length + 1){
		dstate.cons_length = msg->connection;
		dstate.cons = realloc((void*)dstate.cons,dstate.cons_length * sizeof(sble_state*));
	}
	dstate.cons[msg->connection] = malloc(sizeof(sble_state));
	dstate.cons_activity_map |= 1<<msg->connection;
	SBLE_DEBUG("Adding connection to activity map: %d",msg->connection);
	dstate.cons[msg->connection]->addr_type = msg->address_type;
	dstate.cons[msg->connection]->con = msg->connection;
	memcpy(dstate.cons[msg->connection]->addr,msg->address.addr,6);
	dstate.cons[msg->connection]->ll_rcvqueue = (sble_ll*) malloc(sizeof(sble_ll));
	dstate.cons[msg->connection]->ll_attributes = (sble_ll*) malloc(sizeof(sble_ll));
	sble_ll_init(dstate.cons[msg->connection]->ll_rcvqueue);
	sble_ll_init(dstate.cons[msg->connection]->ll_attributes);
	sble_scheduler_unlock_mutex();

}
void sble_evth_disconnected(const struct ble_msg_connection_disconnected_evt_t *msg){
	sble_scheduler_lock_mutex();
	if(!(dstate.cons_activity_map  & (1<<msg->connection))){
		sble_scheduler_unlock_mutex();
		SBLE_ERROR_CONTINUABLE("Connection does not exist. Maybe the device has been connected by external software or had been connected previously to this instance?");
		return;
	}
	dstate.cons_activity_map  &= ~(1<<msg->connection);
	if(msg->connection == dstate.cons_length-1){
		dstate.cons_length--;
	}
	sble_ll_free_nodes(dstate.cons[msg->connection]->ll_rcvqueue,SBLE_TRUE);
	sble_ll_free_nodes(dstate.cons[msg->connection]->ll_attributes,SBLE_TRUE);
	free(dstate.cons[msg->connection]->ll_rcvqueue);
	free(dstate.cons[msg->connection]->ll_attributes);
	free(dstate.cons[msg->connection]);
	sble_scheduler_unlock_mutex();

}
