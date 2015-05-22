/**\file sble_example_minimal client.c
 * \brief minimal demo application acting as scanner and simple att-cliet
 * \date 17.7.2012
 * \author Philipp Kindt
 *
 * This example will communicate with the sble_example_gattserver.c- example.
 * This example assumes that the remote GATT-DB has got the follwing attributes:
 * - Handle 16: 530accd9e7d490b957410ac5e52c656a, writable via ATT
 * - Handle 20: a1bad39994420d8e7a4e330340b895d1, readale via ATT
 * 
 * The client will send the value caffeeeexxxx, where xxxx is a sequence counter indicating
 * the packet number sent.
 *
 */

#include "sble.h"
#include "../bglib/cmd_def.h"
#include <inttypes.h>
#include <string.h>		//for memcpy
#include <mcheck.h>

int main(int argc, char* argv[]){


	uint16_t cycleCnt = 0;

	sble_array* data;

	sble_attribute* att;

	if(argc != 2){
		printf("usage: ./sble_example_minimal_client device_node");
		exit(1);
	}
	//define our attribute's UUIs. This would have been eaiser with ble_type_conversion_hexstring_to_binary(), but just to show you another way...
	uint8_t uuid_wr[16] = {0x53,0x0a,0xcc,0xd9,0xe7,0xd4,0x90,0xb9,0x57,0x41,0x0a,0xc5,0xe5,0x2c,0x65,0x6a};
	sble_init(argv[1]);


	//connect to any node in range with minimal connection interval
	SBLE_DEBUG("Connecting to any node in range...");
	sble_make_connectable_by_any(20,30);
//	sble_connect_to_any(400,400,100,0);
	sble_wait_for_new_connection();

	SBLE_DEBUG("connection established!");







	//-> create an attribute to write
	sble_attribute_malloc_whole(&att,16);		//UUID has 16 byte
	memcpy(att->uuid->data,uuid_wr,16);

	//and create the data to write
	sble_array_malloc_whole(&data,20);		//5 bytes to write
	uint8_t buf[20] = {0xca, 0xff, 0xee,0xee,0xca, 0xff, 0xee,0xee,0xca, 0xff, 0xee,0xee,0xca, 0xff, 0xee,0xee,0xca, 0x00,0x12,0x34};	//"caffee1234"
	memcpy(data->data, buf,20);			//write value
	data->len = 20;

	//retrive attribute list at remote. Only after doing this, sble_attclient_write_by_attribute() is available
	sble_attclient_getlist(0);

	//do it!
	while(1){	

		SBLE_DEBUG("writing attribute...");	
		//sble_attclient_write_by_attribute(0,att,data);
		sble_call_bl(ble_cmd_attclient_write_command_idx,0,16,data->len,data->data);
//		sble_call_bl(ble_cmd_attclient_write_command_idx,0,16,0,data2->len,data2->data);
//		sble_call_bl(ble_cmd_attclient_attribute_write_idx,0,16,0,data3->len,data3->data);
//		sble_call_bl(ble_cmd_attclient_execute_write_idx,0,1);
//		sble_gatt_write_by_handle(16,data);

		SBLE_DEBUG(" \nattribute written...\n");	
		printf("number of packets Sent: %u\n",cycleCnt);

		//increase cycle counter and copy value to next payload
		cycleCnt++;
		memcpy(data->data +18, ((void*) &cycleCnt + 1) ,1);			//write value
		memcpy(data->data +19, ((void*) &cycleCnt),1);			//write value
//usleep(1000000);

		//after 99 cycles, modify connection interval

		if(cycleCnt%100 == 0){
			if(cycleCnt%200 == 0){
			//	SBLE_DEBUG("Setting ConInt to 6..");
			//	sble_call_bl(ble_cmd_connection_update_idx, 0, 6,6,0,200);
			}else{
				SBLE_DEBUG("Setting ConInt to 100..");
			//	sble_call_bl(ble_cmd_connection_update_idx, 0, 200,200,0,800);
			}
		}
	}




	//never reached
	return 0;
}

