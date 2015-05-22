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
 * *
 */
#include "sble.h"
#include <unistd.h>
#include <sys/time.h>
#include <data_transmission_protocol/dtp.h>
#include <data_transmission_protocol/payloads/dtp_payload_acceleration.h>
#include <data_transmission_protocol/payloads/dtp_payload_rssi.h>


int main(int argc, char* argv[]){
	if(argc != 2){
		printf("usage: ./sble_example_readLIS302DL device_node\n");
		exit(1);
	}	
	sble_init(argv[1]);

	connect:
		SBLE_DEBUG("Connecting to any node in range...");
		sble_connect_to_any(6,6,60,0);
		SBLE_DEBUG("connection established!");
		sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);



		usleep(300);

		int32_t local_rssi;
		int32_t remote_rssi;
		uint32_t cnt;

		sble_payload* pl;
		while(1){
			//now wait for a client writing to the GATT server via ATT
			//SBLE_DEBUG("waiting for incoming transfer.");
			sble_scheduler_wait_for_event_no_reset(SBLE_THREAD_MAIN,SBLE_STATE_GATT_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

			sble_scheduler_lock_mutex();
			if(dstate.flags & SBLE_STATE_GATT_VALUE_EVENT){
				dstate.flags &= ~(SBLE_STATE_GATT_VALUE_EVENT);
				sble_scheduler_unlock_mutex();
				SBLE_DEBUG("GATT value Event\n");
				pl = (sble_payload*) sble_ll_pop_from_begin(dstate.ll_gatt_values);
			}
			if(dstate.flags & SBLE_STATE_DISCONNECT_EVENT){
				dstate.flags &= ~(SBLE_STATE_DISCONNECT_EVENT);
				sble_scheduler_unlock_mutex();
				SBLE_DEBUG("disconnection established - reconnecting...");
				goto connect;
			}
			dstate.flags &= ~(SBLE_STATE_GATT_VALUE_EVENT | SBLE_STATE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

			sble_scheduler_unlock_mutex();


			if((pl != NULL)&&(pl->data->len == 19)){
				//SBLE_DEBUG_CON("read from remote node %d bytes: ",pl->data->len);
				//acc[0] = pl->data->data[3]<<24 | pl->data->data[2]<<16| pl->data->data[1]<<8|pl->data->data[0];
				//acc[1] = pl->data->data[7]<<24 | pl->data->data[6]<<16| pl->data->data[5]<<8|pl->data->data[4];
				//acc[2] = pl->data->data[11]<<24 | pl->data->data[10]<<16| pl->data->data[9]<<8|pl->data->data[8];
				//printf("accelleration: (%d, %d, %d)\n",acc[0],acc[1],acc[2]);
				//get local RSSI
				sble_call_bl(ble_cmd_connection_get_rssi_idx,0);
				sble_scheduler_lock_mutex();
				if(dstate.cons[0]->param != NULL){
					local_rssi = *((int8_t*)dstate.cons[0]->param);
					free(dstate.cons[0]->param);
					dstate.cons[0]->param = NULL;
				}

				sble_scheduler_unlock_mutex();

				for(cnt = 0;  cnt < 6; cnt++){
					printf("[%d] A=(%d,%d,%d)\n",cnt,(int8_t) pl->data->data[3* cnt],(int8_t) pl->data->data[3* cnt + 1],(int8_t) pl->data->data[3* cnt + 2]);


				}
				printf("RSSI: %d\n\n",(int8_t) pl->data->data[18]);

			}else{
				SBLE_DEBUG("\n!Warn - eiter no acc data or RSSI available!\n");
			}


				sble_payload_free_whole(&pl);



		}

		sble_io_disconnect();
		return 0;
}
