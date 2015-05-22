/**\file sble_example_bidirectional_serial_adv.c
 * \brief minimal demo application which starts as advertiser
 * \date 29.08.2013
 * \author Mathias Gopp
 *
 * After setting up a connection, the device works as a slave
 * This example will communicate with the sble_example_bidirectional_serial_scan.c- example.
 * This example assumes that the remote GATT-DB has got the following attributes:
 * - Handle 16: 530accd9e7d490b957410ac5e52c656a, writable via ATT
 * - Handle 20: a1bad39994420d8e7a4e330340b895d1, readable via ATT
 */

#include "sble.h"
#include <inttypes.h>
#include <string.h>		//for memcpy
#include <mcheck.h>
#include <stdlib.h>

//global
sble_attribute* att;

/* In the following there is the sending thread. This thread contains the code for sending data.
 * It is necessary that always the sending part runs in a thread.
 * There are two possibilities for transmitting data:
 * 		-sble_serial_send(...)
 * 		-sble_attclient_write_by_attribute_multi_threading(...)
 */
void* send_thread(void* param)
{
	int8_t success = 0;
	uint8_t multithreading = 1;
	uint8_t numOfLoops = 0;
	uint32_t numOfBytes = 6000;
	uint8_t* data;
	data = (uint8_t*) malloc(numOfBytes);
	

	//sample data
	uint32_t i = 0;
	for(i = 0; i < numOfBytes; i++)
	{
		data[i] = i;
	}
	
	
	while(1)
	{
		success = sble_serial_send(numOfBytes, data, att, multithreading);
		//numOfLoops++;
		//or as described before there can be used the following command for data transmission:
		//sble_attclient_write_by_attribute_multi_threading(...)
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
	sble_make_connectable_by_any(20,40);
	sble_wait_for_new_connection();

	SBLE_DEBUG("connected");

	sble_attribute_malloc_whole(&att,16);		//UUID has 16 byte
	memcpy(att->uuid->data,uuid_wr,16);

	sble_attclient_getlist(0);

	//The serial init is only necessary when sble_serial_send(...) or sble_serial_receive(...) are used
	sble_serial_init(0);
	
	//Here the thread for the sending part is initialized and started
	pthread_t sendThread;
	pthread_create(&sendThread,NULL,send_thread,NULL);
	
	uint8_t* buf;
	uint32_t numOfBytes = 0;
	uint32_t numOfBytesTotal = 0;

	while(1)
	{
		//sble_serial_receive(...) is used for receiving the data
		//as an alternative the code from sble_example_minimal_gatserver.c for receiving data can be used
		numOfBytes = sble_serial_receive(buf);
		SBLE_DEBUG_CON("number of bytes received: %d\n",numOfBytes);
		buf = sble_serial_getBufAdr();
		free(buf);
	}
	SBLE_DEBUG("Program closes");
	sble_disconnect(dstate.current_con);

	return 0;
}

