/*
 * sble_type_conversion.c
 *
 *  Created on: 12.07.2012
 *      Author: kindt
 */

#include "sble_type_conversion.h"

sble_array* sble_type_conversion_hexstring_to_binary(char* str_hex) {
	sble_unsigned_integer cnt = 0;
	sble_array* arr;
	sble_array_malloc_whole(&arr, 16);

	if (str_hex == NULL) {
		return NULL;
	}

	while (str_hex[cnt] != '\0') {
		if (cnt > 32) {
			SBLE_ERROR("String longer than 32 characters. UUIDs have max 16 bytes length => max. 32 chars. Please check your UUID-string, fix it and recompile your application. Mutch fun and have a nice day.");
		}

		*(arr->data + cnt/2) |= sble_type_conversion_char_2_numeric(str_hex[cnt])<<(4*(!(cnt%2)));
		cnt++;
	}
	if ((cnt % 2) != 0) {
		SBLE_ERROR("Uneven number of characters in UUID-string. 2 chars, one byte and UUID cannot be half-bytes long. Fix your code!");
	}

	arr->len = cnt/2;
	return arr;
}


uint8_t sble_type_conversion_char_2_numeric(char ch){
	if(ch - '0' <= 9){
		return ch - '0';
	}

	if(ch - 'a'  <= 5){
		return 10 + (ch - 'a');
	}

	SBLE_ERROR("Invalid cahracter! Only supperted characters: [0-9a-z] (all lowercase)");
	return 0;

}

char* sble_type_conversion_buf_to_hexstring(const uint8_t *data, uint32_t len){
	uint32_t cnt;
	char* rv = malloc((2*len + 1) * sizeof(char));
	char charbuf[3];
	for(cnt = 0; cnt < len; cnt++){
		sprintf(charbuf,"%02x",data[cnt]);
		rv[cnt * 2] = charbuf[0];
		rv[cnt * 2 + 1] = charbuf[1];
	}
	rv[2*len] = '\0';
	return rv;
}
