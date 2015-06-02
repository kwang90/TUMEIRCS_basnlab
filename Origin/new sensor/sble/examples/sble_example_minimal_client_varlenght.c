/**\file sble_example_minimal client.c
 * \brief minimal demo application acting as scanner and simple att-client for attributes with variable lenght.
 * \date 17.7.2012
 * \author Philipp Kindt
 *
 * This example will communicate with the sble_example_gattserver.c- example.
 * This example assumes that the remote GATT-DB has got the follwing attributes:
 * - Handle 16: 530accd9e7d490b957410ac5e52c656a, writable via ATT
 * - Handle 20: a1bad39994420d8e7a4e330340b895d1, readale via ATT
 * 
 * Attribute values from one to 250 bytes lenght are transmitted. The payload is generated having the folowing scheme:
 * lenght 1 => payload 01
 * lenght 2 => payload 0102
 * lenght 3 => payload 010203
 * lenght 4 => payload 01020304...
 */

#include "sble.h"
#include "../bglib/cmd_def.h"
#include <inttypes.h>
#include <string.h>		//for memcpy
#include <mcheck.h>


//create payload buffer.
void fill_buf(uint8_t* buf, uint32_t len){
	uint32_t cnt = 0;
	for(cnt = 0; cnt < len; cnt++){
		buf[cnt] = cnt;
	}
}

int main(int argc, char* argv[]){

	uint16_t cycleCnt = 0;

	sble_array* data;
	sble_attribute* att;

	//define our attribute's UUIs. This would have been eaiser with ble_type_conversion_hexstring_to_binary(), but just to show you another way...
	uint8_t uuid_wr[16] = {0x53,0x0a,0xcc,0xd9,0xe7,0xd4,0x90,0xb9,0x57,0x41,0x0a,0xc5,0xe5,0x2c,0x65,0x6a};

	//Initialize SBLE. The file-descriptor it is attached to is 5
	sble_init(argv[1]);



	SBLE_DEBUG("Connecting to any node in range...");
	sble_connect_to_any(20,20,400,0);
	SBLE_DEBUG("connection established!");


	//retrive attribute list. It is needed for sble_attclient_write_by_attribute(),
	//as this function will look up the attribute handle via the UUID from this list, internally
	sble_attclient_getlist(0);


	//now write the value "caffe[0-9]{1-4}" to the remote's gatt server, to attribute uuid_wr

	//-> create an attribute type to write
	sble_attribute_malloc_whole(&att,16);		//UUID has 16 byte
	memcpy(att->uuid->data,uuid_wr,16);

	cycleCnt = 1;


	//do it!
	while(1){
		//and create the data to write
		sble_array_malloc_whole(&data,cycleCnt);				//5 bytes to write
		fill_buf(data->data,cycleCnt);
		SBLE_DEBUG("writing attribute...");
		sble_attclient_write_by_attribute(0,att,data);
		sble_array_free_whole(&data);
		//an alternative whould be: sble_attclient_write_by_handle(0,16,data) - in this case, the attribute list would not be needed

		SBLE_DEBUG(" \nattribute  length %d written...\n",cycleCnt);


		//Increase the number tat is sent along with the  caffee - value
		cycleCnt++;
		if(cycleCnt > 250){
			cycleCnt = 1;
		}
		volatile uint32_t cnt = 129999999;
		while(cnt > 0){
			cnt--;
		}
	}




	//never reached
	return 0;
}
