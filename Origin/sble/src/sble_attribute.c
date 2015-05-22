/*
 * sble_attribute.c
 *
 *  Created on: 12.07.2012
 *      Author: Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 *      See sble_attribute.h for a detailed description.
 */

#include "sble_attribute.h"
#include "sble_debug.h"
#include "sble_config.h"

sble_bool sble_attribute_uuid_comparator(void* a, void*b){
	//SBLE_DEBUG_CON("cmp: a=");
	//sble_print_hex_array(((sble_attribute*) a)->uuid->data,((sble_attribute*) a)->uuid->len);
	//SBLE_DEBUG_CON("cmp: b=");
	//sble_print_hex_array(((sble_attribute*) b)->uuid->data,((sble_attribute*) b)->uuid->len);
	if((((sble_attribute*) a)->uuid == NULL)||(((sble_attribute*) b)->uuid == NULL)){
		return SBLE_FALSE;
	}
	return sble_array_comparator(((sble_attribute*) a)->uuid, ((sble_attribute*) b)->uuid);
}

void sble_attribute_malloc_whole(sble_attribute** att, sble_unsigned_integer uuid_length){

	(*att) = malloc(sizeof(sble_attribute));
	if((*att) != NULL){
		sble_array_malloc_whole(&((*att)->uuid),uuid_length);
	}

}

void sble_attribute_free_whole(sble_attribute** att){
	sble_array_free_whole(&((*att)->uuid));
	free(*att);
	*att = NULL;
}

void sble_attribute_malloc_data(sble_attribute* att, sble_unsigned_integer uuid_length){
	sble_array_malloc_whole(&((att)->uuid),uuid_length);
}

void sble_attribute_free_data(sble_attribute* att){
	sble_array_free_whole(&((att)->uuid));
}
