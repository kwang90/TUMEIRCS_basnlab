#include "sble_io.h"
#include "sble_config.h"
#include "sble_state.h"
#include "sble_debug.h"

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#include "../bglib/cmd_def.h"
#include "sble_scheduler.h"
#include "sble_bgapi_call.h"

void sble_io_reset(char* device) {
	/*
	 #ifndef SBLE_CONFIG_FASTMODE
	 if (sble_status.flags & sble_STATUS_BIT_ATTACHED) {
	 sble_io_disconnect();
	 }

	 sble_status.filedescriptor = open(device, O_RDWR|O_SYNC);
	 if (sble_status.filedescriptor < 0) {
	 SBLE_ERROR_CONTINUABLE(
	 "Failed to Open device: %s. Error Code from kernel: %d", device, errno);
	 perror("open");
	 exit(1);
	 }
	 bglib_output = sble_io_out;

	 ble_cmd_system_reset(0);
	 close(sble_status.filedescripto);
	 #endif
	 */

}
//single-threaded function => no measures for thread safety have to be taken

void sble_io_init(char* param) {

#ifndef SBLE_CONFIG_FASTMODE
	if (dstate.filedescriptor >= 0) {
		SBLE_ERROR("Allready Connected");
	}
#endif
	dstate.flags = 0;
	int32_t fd;
	sscanf(param, "%d", &fd);
	if (fd < 0) {
		SBLE_ERROR("Wrong filedescriptor: %d", fd);
	}
	char buf[16];
	dstate.filedescriptor = fd;

	//set output function of bglib to this sble library's posix output function
	bglib_output = sble_io_out;

	SBLE_DEBUG("Performing BLE112 system Reset...");

		struct ble_header hdr;
		//sble_call_nb(ble_cmd_system_hello_idx);
		/*
		if(initState.wufinit != NULL)
		{
			initState.wufinit(1);
		}*/
		if(dstate.reset == NULL){
			if(dstate.wuf != NULL){
				dstate.wuf(1);
			}

		//sble_call_nb(ble_cmd_system_reset_idx);
		//	vTaskDelay(500);

			if(dstate.wuf != NULL)
			{
				dstate.wuf(0);
			}
		}else{
			if(dstate.wuf != NULL){
				dstate.wuf(1);
				hhd_delay(200);
				dstate.reset(1);
			}
		}
		sble_signed_integer bytes_read;
		while(1){
//			sble_call_nb(ble_cmd_system_reset_idx);
			if(dstate.reset == NULL){

				if(dstate.wuf != NULL)
				{
					dstate.wuf(1);
				}

			}else{
				if(dstate.wuf == NULL){

					dstate.reset(1);
				}
			}
			sble_call_nb(ble_cmd_system_hello_idx);

				SBLE_DEBUG("waiting for hello message from BLE...");
				bytes_read = _sble_io_read_raw((unsigned char*) &hdr, 4);

				sble_signed_integer payload_length = hdr.lolen;

				//_sble_io_read_raw(NULL,payload_length);

				//SBLE_DEBUG("Response Class is: %x",hdr.cls);
				//SBLE_DEBUG("Response Command is: %x",hdr.command);
				//SBLE_DEBUG("Response type_hilen is: %x",hdr.type_hilen);

				if(dstate.flags & SBLE_STATE_TERMINATE){
					SBLE_DEBUG("Termination requested...");
					return;
				}

				if((bytes_read == 4)&&(hdr.cls == ble_cls_system))
				{
					if((hdr.command == ble_rsp_system_reset_id) || (hdr.command == ble_rsp_system_hello_id) || (hdr.command == ble_evt_system_boot_id))
					{
						_sble_io_read_raw(NULL,payload_length);



						break;
					}
				}



		}
		if(dstate.wuf != NULL){
			dstate.wuf(0);
		}
		SBLE_DEBUG("System initialized.");

}

void sble_io_disconnect() {
#ifndef SBLE_CONFIG_FASTMODE
	sble_scheduler_lock_mutex();
	if (dstate.filedescriptor < 0) {
		sble_scheduler_unlock_mutex();
		SBLE_ERROR("Not Connected");
	}

#endif
	SBLE_DEBUG("SBLE BLE disconnected, file descriptor %d closed.", dstate.filedescriptor);
	dstate.filedescriptor = -1;
	sble_scheduler_unlock_mutex();

}

void sble_io_out(uint8_t len1, uint8_t* dbuf1, uint16_t len2, uint8_t* dbuf2) {
	#ifndef SBLE_CONFIG_FASTMODE
	if (dstate.filedescriptor < 0) {
		SBLE_ERROR("Not Connected");
	}
#endif

	sble_signed_integer fd;
	fd = dstate.filedescriptor;

	if (write(fd, dbuf1, len1) < 0) {
		SBLE_ERROR_CONTINUABLE("Failed to write dbuf1");
	}

	if (write(fd, dbuf2, len2) < 0) {
		SBLE_ERROR_CONTINUABLE("Failed to write dbuf2");
	}

//	SBLE_DEBUG("Written %d + %d bytes.",len1,len2);
}


sble_signed_integer _sble_io_read_raw(uint8_t* buf, uint32_t len){
	uint32_t bytes_read = 0;
	bytes_read += read(dstate.filedescriptor, buf, len);
	if(bytes_read != len){
		SBLE_ERROR("Could not read - wrong number of bytes.");
	}

	return bytes_read;
}

sble_signed_integer sble_io_read() {

	const struct ble_msg *msg;
	struct ble_header hdr;
	unsigned char data[256]; //buffer size 256 as message'headers lolen, which specifies the class id, command id and payload that is to come with the api-message is 8 bits long.
	sble_signed_integer bytes_read;
	sble_signed_integer payload_length;
#ifndef SBLE_CONFIG_FASTMODE
	sble_scheduler_lock_mutex();
	if (dstate.filedescriptor < 0) {
		sble_scheduler_unlock_mutex();
		SBLE_ERROR("Not Connected");
	}
	sble_scheduler_unlock_mutex();

#endif
//	SBLE_DEBUG("READ1 @ fd %d ...", dstate.filedescriptor);
	bytes_read = _sble_io_read_raw((unsigned char*) &hdr, 4);
//	SBLE_DEBUG("read1 done.");

	if (bytes_read != 4) {
		SBLE_ERROR_CONTINUABLE(
				"Failed to read header from BLE device at file-descriptor %d. Actually read %d bytes, but should have been 4 bytes. ", dstate.filedescriptor, bytes_read);
		return -1;
	}
	payload_length = hdr.lolen;

	//printf("Payload received: %i\n",payload_length);
	//check how many body bytes are to come...
	if (payload_length > 0) {
//		SBLE_DEBUG("READ2 (%d bytes)...", payload_length);

		if ((bytes_read = _sble_io_read_raw(data, payload_length))
				<= 0) {
			SBLE_ERROR_CONTINUABLE(
					"Failed to read api message body from BLE device. Read %d bytes, should have been %d bytes.", bytes_read, payload_length);
			return -1;
		}
//		SBLE_DEBUG("read2 done.");

		msg = ble_get_msg_hdr(hdr);
		if (msg == NULL) {
			SBLE_ERROR_CONTINUABLE(
					"Could not get api msg from BLE device. type_hilen: %d, Class: %d - Command: %d", hdr.type_hilen, hdr.cls, hdr.command);
			return 0;
		}
		//	sble_scheduler_lock_mutex();
		//call callback
		msg->handler(data);

		if ((hdr.type_hilen & 1<<7) == 0) {
			SBLE_DEBUG("{ Response type. Class: %d, Command: %d}",msg->hdr.cls, msg->hdr.command);
			sble_scheduler_lock_mutex();
			dstate.flags |= SBLE_STATE_RESPONSE_RECEIVED;
			sble_scheduler_unlock_mutex();

		}else{
//			SBLE_DEBUG("{Something different than response.}");
//			SBLE_DEBUG("CLASS: %d",hdr.cls);
//			SBLE_DEBUG("CMD: %d",hdr.command);
//			SBLE_DEBUG("hilen: %d",hdr.type_hilen);
//			SBLE_DEBUG("lolen: %d",hdr.lolen);
		}
		//	sble_scheduler_unlock_mutex();
	} else if (payload_length == 0) {
//		SBLE_DEBUG("no payload...");
//		SBLE_DEBUG("CLASS: %d",hdr.cls);
//		SBLE_DEBUG("CMD: %d",hdr.command);
//		SBLE_DEBUG("hilen: %d",hdr.type_hilen);
//		SBLE_DEBUG("lolen: %d",hdr.lolen);
	} else if (payload_length < 0) {

		SBLE_ERROR("Wrong payload length: %d bytes. Exiting!", payload_length);
	}

	return bytes_read;
}

sble_bool sble_io_is_connected(){
	sble_bool rv;
	sble_scheduler_lock_mutex();
	if(dstate.filedescriptor >= 0){
		rv = SBLE_TRUE;
	}else{
		rv = SBLE_FALSE;
	}
	sble_scheduler_unlock_mutex();
	return rv;
}

#endif
