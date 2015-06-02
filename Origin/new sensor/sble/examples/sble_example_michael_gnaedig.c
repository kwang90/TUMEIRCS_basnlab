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
	uint8_t uuid_wr[2] = {0x02,0x29};
	sble_init(argv[1]);


	//connect to any node in range with minimal connection interval
	printf("waiting...\n");
	SBLE_DEBUG("Connecting to any node in range...");
	sble_connect_to_any(10,100,500,50);
	SBLE_DEBUG("connection established!");







	//-> create an attribute to write
	sble_attribute_malloc_whole(&att,2);		//UUID has 2 byte
	memcpy(att->uuid->data,uuid_wr,2);

	//and create the data to write
	sble_array_malloc_whole(&data,2);		//1 bytes to write
	data->len = 2;
	data->data[0] = 0x01;
	data->data[1] = 0x00;

	//retrive attribute list at remote. Only after doing this, sble_attclient_write_by_attribute() is available
	sble_attclient_getlist(0);
//	sble_attclient_write_by_handle(0,15,data);
	sble_attclient_write_by_attribute(0,att,data);

	//payload to receive
	sble_payload* pl;

	uint32_t nPacketsInQueue;
	//do it!
	while(1){	

		SBLE_DEBUG("waiting for data...");

//we now wait for a GATT-value event that indicates a new value has been received
		sble_scheduler_autoclear_prevent(SBLE_STATE_ATTRIBUTE_VALUE_EVENT);
		sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,
				SBLE_STATE_ATTRIBUTE_VALUE_EVENT);
		sble_scheduler_events_clear(SBLE_STATE_ATTRIBUTE_VALUE_EVENT);
		//now we look how many packets are in the reception queue
		nPacketsInQueue = sble_ll_get_nr_of_elements(dstate.cons[0]->ll_rcvqueue);




		while (nPacketsInQueue > 0) {
			//get payload from packet
			pl = sble_ll_pop_from_begin(dstate.cons[0]->ll_rcvqueue);
			nPacketsInQueue--;

			//Print payload and check sequence counter
			if ((pl != NULL )&&(pl->data != NULL)){
				printf("payload length: %d bytes\n",pl->data->len);

				//print value
				sble_print_hex_array(pl->data->data,pl->data->len);

				//we have to free the payload's memory sble has allocated
				sble_payload_free_whole(&pl);
			}

		}	
	}




	//never reached
	return 0;
}

