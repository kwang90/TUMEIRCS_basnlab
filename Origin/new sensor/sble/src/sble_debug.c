/*
 * sble_debug.h
 *
 *  Created on: 06.07.2012
 *      Author: kindt
 */

#include "sble_debug.h"

void sble_print_hex_array(const uint8_t *data, uint32_t len){
//#ifdef SBLE_DEUG_MESSAGES_FREERTOS
	uint32_t cnt;
	for(cnt = 0; cnt < len; cnt++){
	   SBLE_DEBUG_CON("%02x",data[cnt]);
	}
	SBLE_DEBUG_CON("\n");

//#endif
}

void sble_print_char_array(const uint8_t *data, uint32_t len){
//#ifdef SBLE_DEUG_MESSAGES_FREERTOS
	uint32_t cnt;
	for(cnt = 0; cnt < len; cnt++){
		printf("%c",data[cnt]);
	}
    printf("\n");
//#endif

}

void sble_print_bitfield(sble_unsigned_integer field, sble_unsigned_integer length){
//#ifdef SBLE_DEUG_MESSAGES_FREERTOS
	sble_unsigned_integer cnt = 0;
	SBLE_DEBUG_CON("0x%x: ",field);
	char ch[2];
	ch[1] = '\0';
	for(cnt = 0; cnt < length; cnt++){
		if(((cnt % 8) == 0)&&(cnt != 0)){
			SBLE_DEBUG_CON(" ");
		}

		ch[0] = ((field & 1<<cnt) ? '1' : '0');
		SBLE_DEBUG_CON(ch);
	}
	SBLE_DEBUG_CON("\n");
//#endif SBLE_DEUG_MESSAGES_FREERTOS
}


