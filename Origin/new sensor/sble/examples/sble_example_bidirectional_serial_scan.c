/**\file sble_example_bidirectional_serial_scan.c
 * \brief minimal demo application which starts as scanner
 * \date 29.08.2013
 * \author Mathias Gopp
 *
 * After setting up a connection, the device works as a master
 * This example will communicate with the sble_example_bidirectional_serial_adv.c- example.
 * This example assumes that the remote GATT-DB has got the following attributes:
 * - Handle 16: 530accd9e7d490b957410ac5e52c656a, writable via ATT
 * - Handle 20: a1bad39994420d8e7a4e330340b895d1, readable via ATT
 */

/*********************************************************************************
 * For a more detailed description see sble_example_bidirectional_serial_adv.c
 *********************************************************************************/

#include "sble.h"
#include <inttypes.h>
#include <string.h>		//for memcpy
#include <mcheck.h>
#include <stdlib.h>

sble_attribute* att;

void* send_thread(void* param)
{
	uint8_t multithreading = 1;
	int8_t success = 0;
	uint8_t numOfLoops = 10;
	uint32_t numOfBytes = 6000;
	uint8_t* data;
	data = (uint8_t*) malloc(numOfBytes);
	
	uint32_t i = 0;
	for(i = 0; i < numOfBytes; i++)
	{
		data[i] = i+1;
	}
	
	while(1)
	{
		success = sble_serial_send(numOfBytes, data, att, multithreading);
		//sleep(2);
	}
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("usage: ./sble_example_minimal_client device_node");
		exit(1);
	}
	//define our attribute's UUIs. This would have been eaiser with ble_type_conversion_hexstring_to_binary(), but just to show you another way...
	uint8_t uuid_wr[16] = {0x53,0x0a,0xcc,0xd9,0xe7,0xd4,0x90,0xb9,0x57,0x41,0x0a,0xc5,0xe5,0x2c,0x65,0x6a};

	sble_init(argv[1]);

	SBLE_DEBUG("Connecting to any node in range...");
	
	uint8_t con = 0;
	con = sble_connect_to_any(6,6,10,0);


	SBLE_DEBUG("connected");

	sble_attribute_malloc_whole(&att,16);		//UUID has 16 byte
	memcpy(att->uuid->data,uuid_wr,16);

	sble_attclient_getlist(0);

	sble_serial_init(0);
	
	pthread_t sendThread;
	pthread_create(&sendThread,NULL,send_thread,NULL);
	
	uint8_t* buf;
	uint32_t numOfBytes = 0;

	while(1)
	{
		numOfBytes = sble_serial_receive(buf);
		SBLE_DEBUG_CON("number of bytes received: %d\n",numOfBytes);
		buf = sble_serial_getBufAdr();
		free(buf);
	}

	sble_disconnect(dstate.current_con);

	return 0;
}

