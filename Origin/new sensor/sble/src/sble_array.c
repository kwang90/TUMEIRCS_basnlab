/*
 * sble_array.c
 *
 *  Created on: 09.07.2012
 *      Author: kindt
 */

#include "sble_array.h"
#include "sble_debug.h"


void sble_array_malloc_data(sble_array* array, sble_unsigned_integer data_len){
	array->data = malloc(data_len);
	array->len = data_len;
}

void sble_array_free_data(sble_array* array){
	if(array->data != NULL){
		free(array->data);
		array->data = NULL;
	}

}

void sble_array_malloc_whole(sble_array** array, sble_unsigned_integer data_len){
	*array = malloc(sizeof(sble_array));
	if((*array != NULL)){
		(*array)->data = malloc(data_len);
		(*array)->len = data_len;
	}
}

void sble_array_resize(sble_array* array, sble_unsigned_integer data_len){
	if((array != NULL)){
		(array)->data = realloc((array)->data, data_len);
		(array)->len = data_len;
	}

}

void sble_array_free_whole(sble_array** array){
	if(*array == NULL){
		return;
	}
	if((*array)->data != NULL){
		free((*array)->data);
	}
	free((*array));
	*array = NULL;
}

sble_bool sble_array_comparator(void* a, void*b){
	if ((a==NULL)||(b==NULL)){
		SBLE_DEBUG("At least one operand is NULL");
		return 0;
	}
	sble_array* aa = a;
	sble_array* ab = b;
	if((aa->data == NULL)||(ab->data == NULL)){
	//	SBLE_DEBUG("0 (no data)");
		return 0;
	}
	uint8_t da, db;
	if(aa->len != ab->len){
	//	SBLE_DEBUG("0 (lenght missmatch)");
		return 0;
	}

	sble_unsigned_integer cnt;
	for(cnt = 0; cnt < aa->len; cnt++){

		if(((uint8_t*)aa->data)[cnt] != ((uint8_t*)ab->data)[cnt]){
//			SBLE_DEBUG("0 (byte missmatch @ byte %d - [%x vs %x])",cnt,((uint8_t*)aa->data)[cnt],((uint8_t*)ab->data)[cnt]);
			return 0;
		}
	}
	//SBLE_DEBUG("1");

	return 1;
}

