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

int main(int argc, char* argv[]){
	if(argc != 2){
		printf("usage: ./sble_example_readLIS302DL device_node");
		exit(1);
	}	
	sble_init(argv[1]);

	//make the device connectable by any node who wishes to
	//sble_make_connectable_by_any(40,40);
		sble_connect_to_any(6,6,60,0);

	int32_t acc[3];


	sble_payload* pl; 
	while(1){
	//now wait for a client writing to the GATT server via ATT
	//SBLE_DEBUG("waiting for incoming transfer.");
	//	pl = sble_gatt_receive();
		pl = sble_gatt_receive_until_next();

		if(pl != NULL){
			//SBLE_DEBUG_CON("read from remote node %d bytes: ",pl->data->len);
			acc[0] = pl->data->data[3]<<24 | pl->data->data[2]<<16| pl->data->data[1]<<8|pl->data->data[0];
			acc[1] = pl->data->data[7]<<24 | pl->data->data[6]<<16| pl->data->data[5]<<8|pl->data->data[4];
			acc[2] = pl->data->data[11]<<24 | pl->data->data[10]<<16| pl->data->data[9]<<8|pl->data->data[8];
			printf("accelleration: (%d, %d, %d)\n",acc[0],acc[1],acc[2]);
			sble_payload_free_whole(&pl);

		}else{
			SBLE_DEBUG("No Payload received.");
		}
	}
	
	//never reached	
	return 0;
}
