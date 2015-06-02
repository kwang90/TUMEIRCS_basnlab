#include "sble.h"
#include "sble_debug.h"


int main(){
	sble_attribute* att;
	sble_init("/dev/ttyACM0");
	uint8_t con = sble_connect_to_any(40,40);		//connect to any node in range.
		
	sble_attribute_malloc_whole(&att,2);			//make space for an attribute having a 2 bytes UUID
	att->uuid->data[1] = 0x28;
	att->uuid->data[0] = 0x03;

	//retrive attribute list for connection 0
	sble_attclient_getlist(con);

	//check if attribute exist at remote
	if(sble_attclient_is_in_list(con,att)){
		SBLE_DEBUG("Attribute found.");
	}else{
		SBLE_DEBUG("Attribute not found.");
	}

	//tidy up...
	sble_attribute_free_whole(&att);
	sble_disconnect(con);
	sble_shutdown();	
	
	 return 0;
}
