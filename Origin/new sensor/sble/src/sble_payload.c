/*
 * sble_payload.c
 *
 *  Created on: 11.07.2012
 *      Author: kindt
 */
#include "sble_payload.h"
#include "sble_debug.h"
#include "sble_ll.h"

void sble_payload_malloc_whole(sble_payload** pl, sble_unsigned_integer data_length){
	if(pl == NULL){
		SBLE_ERROR("Could not allcoate data. Payload pointer is NULL");
	}

	(*pl) = malloc(sizeof(sble_payload));

	sble_array_malloc_whole(&((*pl)->data),data_length);
	(*pl)->atthandle = 0;
	(*pl)->connection = 0;


}

void sble_payload_free_whole(sble_payload** pl){
	if(pl == NULL){
		SBLE_ERROR_CONTINUABLE("got NULL pointer -> nothing to free");
		return;
	}
	if(*pl == NULL){
		SBLE_ERROR_CONTINUABLE("payload not allocated -> nothing to free");
		return;

	}
	if(((*pl)->data) != NULL){
		sble_array_free_whole(&((*pl)->data));
	}

	free(*pl);

	*pl = NULL;
}

void sble_payload_malloc_data(sble_payload* pl, sble_unsigned_integer data_length){
	if(pl == NULL){
		SBLE_ERROR("Could not allcoate data. Payload pointer is NULL");
	}
	sble_array_malloc_whole(&((pl)->data),data_length);
	pl->atthandle = 0;
	pl->connection = 0;

}

void sble_payload_free_data(sble_payload* pl){
	if(pl == NULL){
		SBLE_ERROR("Could not allcoate data. Payload pointer is NULL");
	}
	sble_array_free_whole(&((pl)->data));
}

sble_bool sble_payload_comparator_by_handle(sble_payload *a, sble_payload* b){
	if(a->atthandle == b->atthandle){
		return SBLE_TRUE;
	}else{
		return SBLE_FALSE;
	}
}

void sble_payload_print_ll(sble_ll* ll){
	if(ll == NULL){;
		SBLE_DEBUG("parameter ll is NULL-pointer");
		return;
	}
	sble_ll_node *node;
	sble_unsigned_integer cnt = 0;
	if (ll->root == NULL) {
		SBLE_DEBUG("parameter ll is NULL-pointer");
		return;
	}

	node = ll->root;
	SBLE_DEBUG("####### [ll payload dump begin] ##########\n");
	while (node != NULL) {
		SBLE_DEBUG("[ll element #%d / 0x%x]:",cnt, (uint32_t) node);
		sble_print_hex_array(((sble_payload*) node->data)->data->data,((sble_payload*)node->data)->data->len);
		cnt++;
		node = node->next;
	}
	SBLE_DEBUG("####### [ll payload dump end] ##########\n");
}
