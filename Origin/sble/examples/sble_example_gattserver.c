/** \file sble_serverdemo.c
 * \brief demo application acting as advertiser & GATT-server
 * \date 17.7.2012
 * \author Philipp Kindt
 *
 * This example will communicate with the sble_example_client.c- example.
 * This example assumes that the local gatt-DB has got the follwing attributes:
 * - Handle 16: 530accd9e7d490b957410ac5e52c656a, writable via ATT
 * - Handle 20: a1bad39994420d8e7a4e330340b895d1, readale via ATT
 * 
 *
 *
 */

#include "sble.h"

int main(){
	sble_init("/dev/ttyACM0");

	//make the device connectable by any node who wishes to
	sble_make_connectable_by_any(400,400);


	//create an array having the data 0xcaffee1234
	sble_array* arr = sble_type_conversion_hexstring_to_binary("caffee1234");
	sble_print_hex_array(arr->data,arr->len);

	//write data array to gatt server
	sble_gatt_write_by_handle(20,arr);

	//read out what we have written
	sble_payload* pl = sble_gatt_read_by_handle(20);
	if(pl != NULL){
		SBLE_DEBUG_CON("H20 read from GATT server: ");
		sble_print_hex_array(pl->data->data,pl->data->len);
	}

	//now read out handle 16
	pl = sble_gatt_read_by_handle(16);
	if(pl != NULL){
		SBLE_DEBUG_CON("H16 read from GATT server: ");
		sble_print_char_array(pl->data->data,pl->data->len);

	}

	//now wait for a client writing to the GATT server via ATT
	SBLE_DEBUG("waiting for incoming transfer.");
	pl = sble_gatt_recieve();


	if(pl != NULL){
		SBLE_DEBUG_CON("read from remote node: ");
		sble_print_char_array(pl->data->data,pl->data->len);

	}else{
		SBLE_DEBUG("No Payload received.");
	}

	
	//Wait for some time and shutdown
	sleep(10);
	sble_shutdown();
	return 0;
}
