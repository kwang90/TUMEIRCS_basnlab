/** \file sble_example_client.c
 * \brief demo application acting as scanner and simple att-cliet
 * \date 17.7.2012
 * \author Philipp Kindt
 *
 * This example will communicate with the sble_example_gattserver.c- example.
 * This example assumes that the remote GATT-DB has got the follwing attributes:
 * - Handle 16: 530accd9e7d490b957410ac5e52c656a, writable via ATT
 * - Handle 20: a1bad39994420d8e7a4e330340b895d1, readale via ATT
 * 
 *
 *
 */

#include "sble.h"
#include "../bglib/cmd_def.h"
#include <inttypes.h>
#include <string.h>		//for memcpy

int main(){
	uint32_t cnt;
	sble_array* data;
	sble_attribute* att;

	//define our attribute's UUIDs. This would have been eaiser with ble_type_conversion_hexstring_to_binary(), but just to show you another way...
	uint8_t uuid_rd[16] = {0xa1,0xba,0xd3,0x99,0x94,0x42,0x0d,0x8e,0x7a,0x4e,0x33,0x03,0x40,0xb8,0x95,0xd1};
	uint8_t uuid_wr[16] = {0x53,0x0a,0xcc,0xd9,0xe7,0xd4,0x90,0xb9,0x57,0x41,0x0a,0xc5,0xe5,0x2c,0x65,0x6a};

	sble_init("/dev/ttyACM1");

	SBLE_DEBUG("Connecting to any node in range...");
	sble_connect_to_any(40,40,400,0);

	//Or, if you want to connect to a specific node
	//sble_connect_to(sble_type_conversion_hexstring_to_binary("dc934c800700"), SBLE_ADDRESS_PUBLIC, 40,40,100,0);

	SBLE_DEBUG("Connected. Retriving attribute list...");
	//retrive the attribute list at the remote's GATT server
	sble_attclient_getlist(0);
	SBLE_DEBUG("retrived list.");




	//create an attribute and check if this uuid is in attribute list
	sble_attribute_malloc_whole(&att,2);
	att->uuid->data[1] = 0x28;
	att->uuid->data[0] = 0x03;
	if(sble_attclient_is_in_list(0,att)){
		SBLE_DEBUG("Attribute exists at remote.");
	}else{
		SBLE_DEBUG("Attribute does not exist at the remote.");
	}
	sble_attribute_free_whole(&att);		//free our "test attribute"






	//Now lets print out the whole attribute list
	sble_ll_node* n;
	n = (sble_ll_node*) dstate.cons[0]->ll_attributes->root;
	while((n = sble_ll_get_next(n)) != NULL){
		att = ((sble_attribute*) n->data);	
		SBLE_DEBUG_CON("Atthandle %d: ",att->handle);
		sble_print_hex_array(att->uuid->data,att->uuid->len);
	};







	//now write the value "knorke" to the remote's gatt server, to attribute uuid_wr

	//-> create an attribute type to write
	sble_attribute_malloc_whole(&att,16);
	memcpy(att->uuid->data,uuid_wr,16);

	//and create the data to write
	sble_array_malloc_whole(&data,5);
	memcpy(data->data,"knorke",5);

	//do it and tidy up
	sble_attclient_write_by_attribute(0,att,data);
	sble_array_free_whole(&data);








	//now read out uuid_rd. If the sble_example_gattserver.c - demo is running on the remote,
	//this will be our knorke...

	sble_payload *pl;
	
	//generate uuid...
	sble_array* uuid;
	uuid = sble_type_conversion_hexstring_to_binary("a1bad39994420d8e7a4e330340b895d1");
	SBLE_DEBUG_CON("Converted UUID :");
	sble_print_hex_array(uuid->data,uuid->len);
	
	//read out	
	pl = sble_attclient_read_by_uuid(0,uuid);
	
	//tidy up
	sble_array_free_whole(&uuid);
	if(pl != NULL){
		SBLE_DEBUG_CON("Data received (main): ");
		sble_print_hex_array(pl->data->data,pl->data->len);
	}






	//now we want to pick one particular attribute from the list
	
	//prepare attribute structure to retrice	
	sble_attribute* atr;
	sble_attribute_malloc_whole(&att,16);
	memcpy(att->uuid->data,uuid_rd,16);
	att->uuid->len=16;

	//retrive from list
	atr = sble_attclient_get_from_list(0,att);

	if(atr != NULL){
		SBLE_DEBUG_CON("attribute found");
	}else{
		SBLE_DEBUG("atr is NULL - not reading attribute");
	}	
	sble_attribute_free_whole(&att);
	

	//disconnect after a few seconds
	sleep(10);
	sble_call_bl(ble_cmd_connection_disconnect_idx,0);
	sleep(1);
	sble_shutdown();
	return 0;
}

