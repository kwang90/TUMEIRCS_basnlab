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
//#include "../bglib/cmd_def.h"
#include <inttypes.h>
#include <string.h>		//for memcpy
#include <mcheck.h>
#include <stdlib.h>


int main(int argc, char* argv[]){
	
	sble_attribute* att;
	
	//timer
	time_t start,end;
	int t;

	if(argc != 2)
	{
		printf("usage: ./sble_example_minimal_client device_node");
		exit(1);
	}
	//define our attribute's UUIs. This would have been eaiser with ble_type_conversion_hexstring_to_binary(), but just to show you another way...
	uint8_t uuid_wr[16] = {0x53,0x0a,0xcc,0xd9,0xe7,0xd4,0x90,0xb9,0x57,0x41,0x0a,0xc5,0xe5,0x2c,0x65,0x6a};

	sble_init(argv[1]);

	SBLE_DEBUG("Connecting to any node in range...");
	
	//the testtime was done by sending 10 times 512bytes
	//sble_connect_to_any(15,300,10,5); //random testing values (15,300,10,5) -> 15sec
	
	//sble_connect_to_any(6,6,10,0);
	uint8_t con = 0;
	
	con = sble_connect_to_any(6,6,10,0);


	SBLE_DEBUG("connection established with connectionnumber:%d!",dstate.current_con);


	//now write the value "test12" to the remote's gatt server, to attribute uuid_wr

	//-> create an attribute type to write
	sble_attribute_malloc_whole(&att,16);		//UUID has 16 byte
	memcpy(att->uuid->data,uuid_wr,16);

	//uint32_t y = (rand()%100) + 1;
	
	//maximum number of bytes = 5099!!!
	uint32_t numOfBytes = 62;
	uint8_t* data;
	data = (uint8_t*) malloc(numOfBytes);
	
	uint32_t i = 0;
	for(i = 0; i < numOfBytes; i++)
	{
		data[i] = i;
	}

	sble_attclient_getlist(0);

	int8_t success = 0;
	uint8_t numOfLoops = 1;

	sble_serial_init(0);
	uint8_t multithreading = 0;

	//start timer
	start=time(NULL);//predefined  function in c
	
	
	//for(i = 0; i < numOfLoops; i++)
	for(;;)
	{
		success = sble_serial_send(numOfBytes, data, att, multithreading);
		/*sleep(20);
		success = sble_serial_send(numOfBytes, data, att);
		sleep(3);
		success = sble_serial_send(numOfBytes, data, att);
		//sleep(5);
	*/
	}

	//while(1);
	//stop timer
	end=time(NULL);
	
	if(success == 1)
	{
		SBLE_DEBUG("sending buffer finished successfully");
	}
	else
	{
		SBLE_DEBUG("sending buffer error");
	}
	
	//disconnect bluetooth
	sble_disconnect(dstate.current_con);
	//sble_shutdown();
	
	
	//t=(end-start)/CLOCKS_PER_SEC;
	t = end - start;
	
	double te = end - start;

	double nob = numOfBytes;
	SBLE_DEBUG("Time: %d",t);
	SBLE_DEBUG("Datarate: %lf kBit/s",nob * numOfLoops *8/(1000 * te));
	//while(1);
	//reached when ready
	return 0;
}

