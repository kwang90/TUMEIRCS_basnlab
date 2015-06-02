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

	while(1)
	{
		start=time(NULL);
		numOfBytes = sble_serial_receive(buf);
		end=time(NULL);
		te = end - start;
		SBLE_DEBUG("Datarate: %lf kBit/s",numOfBytes *8/(1000 * te));
		SBLE_DEBUG_CON("number of bytes received: %d\n",numOfBytes);
		//buf = sble_serial_getBufAdr();
		sum = sum + numOfBytes;
		loops++;
		SBLE_DEBUG("Total number of Bytes: %u", sum);
		SBLE_DEBUG("Total number of Loops: %u", loops);
	}

	return 0;
}
