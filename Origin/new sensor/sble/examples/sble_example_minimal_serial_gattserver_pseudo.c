/** \file sble_example_minimal_gatserver.c
 * \brief minimal demo application acting as advertiser & GATT-server
 * \date 29.8.2012
 * \author Philipp Kindt
 *
 * This example will communicate with the sble_example_minimal client.c- example.
 * This example assumes that the local gatt-DB has got the follwing attributes:
 * - Handle 16: 530accd9e7d490b957410ac5e52c656a, writable via ATT
 * - Handle 20: a1bad39994420d8e7a4e330340b895d1, readale via ATT
 * 
 *
 *
 */

#include "sble.h"
#include <stdlib.h>


int main(int argc, char* argv[]){
	if(argc != 2){
		printf("usage: ./sble_example_minimal_client device_node");
		exit(1);
	}
	sble_init(argv[1]);

	//make the device connectable by any node who wishes to
	sble_make_connectable_by_any(20,40);

	SBLE_DEBUG_CON("connected");
	uint8_t* buf;
	uint32_t numOfBytes = 0;
	uint32_t sum = 0;

	uint32_t loops = 0;

	//timer
	time_t start,end;
	int t;
	double te;


	uint32_t rec = sble_serial_receive_pseudo();
/*	uint32_t nPacketsInQueue;
	uint32_t packetcounter = 0;
	sble_payload* pl;


	while(1)
	{
		sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT);
		sble_scheduler_wait_for_event_no_reset(SBLE_THREAD_MAIN,SBLE_STATE_GATT_VALUE_EVENT);

		sble_scheduler_lock_mutex();
		dstate.flags &= ~(SBLE_STATE_EVENT|SBLE_STATE_GATT_VALUE_EVENT);
		sble_scheduler_unlock_mutex();


		nPacketsInQueue = sble_ll_get_nr_of_elements(dstate.ll_gatt_values);

		while(nPacketsInQueue > 0)
		{
			sble_scheduler_lock_mutex();
			pl = sble_ll_pop_from_begin(dstate.ll_gatt_values);
			sble_scheduler_unlock_mutex();
			nPacketsInQueue--;

			if((pl != NULL) && (pl->data != NULL))
			{
				SBLE_DEBUG_CON("RWA read from remote node %d bytes: ",pl->data->len);
				sble_print_hex_array(pl->data->data,pl->data->len);

				//sble_payload_free_whole(&pl);

				packetcounter++;

				SBLE_DEBUG_CON("Packetcounter: %d \n",packetcounter);
			}
		}
	}*/

	return 0;
}
