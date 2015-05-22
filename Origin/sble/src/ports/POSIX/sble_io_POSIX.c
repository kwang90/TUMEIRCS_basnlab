#include "sble_io.h"
#include "sble_config.h"
#include "sble_state.h"
#include "sble_debug.h"

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "../bglib/cmd_def.h"
#include "sble_scheduler.h"
#include "sble_bgapi_call.h"
#include <termios.h>

pthread_t sble_dispatcher_thread;

void sble_io_reset(char* device){
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
	char cmdbuf[255];

	sprintf(cmdbuf,"stty -F %s 1:0:cbd:0:3:1c:7f:15:1:1:3c:0:11:13:1a:0:12:f:17:16:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0",param);
	//printf("%s\n",cmdbuf);
	system(cmdbuf);
	dstate.filedescriptor = open(param, O_RDWR|O_SYNC);
	SBLE_DEBUG("Temorarily opened %s on file-descriptor %d",param,dstate.filedescriptor);



	if (dstate.filedescriptor < 0) {
		if(!(dstate.flags & SBLE_STATE_TERMINATE)){
		SBLE_ERROR_CONTINUABLE(
				"Failed to Open device: %s. Error Code from kernel: %d", param, errno);
				perror("open");
//				exit(1);
		}
	}

	SBLE_DEBUG("Performing BLE112 system Reset...");
	//if(!(dstate.flags & SBLE_STATE_TERMINATE)){
		sble_call_nb(ble_cmd_system_reset_idx);
	//}
	usleep(500000);
	sble_scheduler_lock_mutex();
	if(close(dstate.filedescriptor) < 0){
		sble_scheduler_unlock_mutex();

			SBLE_ERROR_CONTINUABLE("close-after-reset failed. Kernel message:");
			perror("close(): ");
	}

	usleep(500000);
	system(cmdbuf);

	dstate.filedescriptor = open(param, O_RDWR|O_SYNC);
	if (dstate.filedescriptor < 0) {
		sble_scheduler_unlock_mutex();
			SBLE_ERROR_CONTINUABLE("Failed to Open device: %s. Error Code from kernel: %d", param, errno);
					perror("open");
					exit(1);
		}
	sble_scheduler_unlock_mutex();

	SBLE_DEBUG("SBLE BLE connected to %s; file-descriptor assigned by kernel: %d ", param, dstate.filedescriptor);



	  struct termios attribs;
		    speed_t speed;
		    if(tcgetattr(dstate.filedescriptor, &attribs) < 0) {
		        perror("stdin");
		        return;
		    }
		       attribs.c_cflag |= CRTSCTS;
		    if(cfsetispeed(&attribs, B115200) < 0)
		      {
		          perror("invalid baud rate");
		          return;
		      }
		    speed = cfgetispeed(&attribs);
	    //printf("input speed: %lu\n", (unsigned long) speed);
	 //   tcsetattr(dstate.filedescriptor,TCSANOW,&attribs);
	//set output function of bglib to this sble library's posix output function
	bglib_output = sble_io_out;



	struct ble_header hdr;
	//	sble_call_nb(ble_cmd_system_hello_idx);

	sble_signed_integer bytes_read = 0;
	sble_call_nb(ble_cmd_system_hello_idx);

	while(1){
			SBLE_DEBUG("waiting for hello message from BLE...");
			//do{
				SBLE_DEBUG("trying to read hello response...");

				bytes_read = _sble_io_read_raw_internal((unsigned char*) &hdr, 4);
				;
			//}while(bytes_read <= 0);

			sble_signed_integer payload_length = hdr.lolen;

			_sble_io_read_raw_internal(NULL,payload_length);

			SBLE_DEBUG("Response Class is: %x",hdr.cls);
			SBLE_DEBUG("Response Command is: %x",hdr.command);
			SBLE_DEBUG("Response type_hilen is: %x",hdr.type_hilen);

			if(dstate.flags & SBLE_STATE_TERMINATE){
				SBLE_DEBUG("Termination requested...");
				return;
			}
			if((bytes_read == 4)&&(hdr.cls == ble_cls_system)){
				if((hdr.command == ble_rsp_system_reset_id) ||
					(hdr.command == ble_rsp_system_hello_id) ||
					(hdr.command = ble_evt_system_boot_id)){

					break;
				}
			}
	}

	SBLE_DEBUG("System initialized.");

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

void sble_io_disconnect() {
#ifndef SBLE_CONFIG_FASTMODE
	sble_scheduler_lock_mutex();
	if (dstate.filedescriptor < 0) {
		sble_scheduler_unlock_mutex();

		SBLE_DEBUG("Warning: Not Connected");
	}
	sble_scheduler_unlock_mutex();

#endif
	sble_scheduler_dispatcher_shutdown(SBLE_THREAD_DISPATCHER);


	SBLE_DEBUG("Disconnecting from BLE112 device...");


	if(dstate.filedescriptor >= 0){
	//	fcntl(dstate.filedescriptor, O_NONBLOCK);
		SBLE_DEBUG("Closing filedescriptor %d", dstate.filedescriptor);
		if (close(dstate.filedescriptor) < 0) {

			SBLE_ERROR("Failed to close device.");
		}
	}else{
		SBLE_ERROR_CONTINUABLE("invalid filedescriptor");
	}
	SBLE_DEBUG("SBLE BLE disconnected, file descriptor %d closed.", dstate.filedescriptor);
	dstate.filedescriptor = -1;
	sble_state_finalize();
}

void sble_io_out(uint8_t len1, uint8_t* dbuf1, uint16_t len2, uint8_t* dbuf2){
	#ifndef SBLE_CONFIG_FASTMODE
	sble_scheduler_lock_mutex();
	if (dstate.filedescriptor < 0) {
		if((dstate.flags & SBLE_STATE_TERMINATE)){
			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("Not Connected");

		}else{

			sble_scheduler_unlock_mutex();
			SBLE_ERROR("Not Connected");

		}
	}
	#endif
	sble_scheduler_unlock_mutex();

	if (write(dstate.filedescriptor, dbuf1, len1) < 0) {
		SBLE_ERROR_CONTINUABLE("Failed to write dbuf1. Kernel error message: ");
		perror("write: ");
		if(!(dstate.flags & SBLE_STATE_TERMINATE)){
			exit(1);
		}
	}

	if (write(dstate.filedescriptor, dbuf2, len2) < 0) {
		SBLE_ERROR_CONTINUABLE("Failed to write dbuf2");
		perror("write: ");
		if(!(dstate.flags & SBLE_STATE_TERMINATE)){
			exit(1);
		}

	}


}
sble_signed_integer _sble_io_read_raw_internal(uint8_t* buf, uint32_t len){
	int32_t bytes_read = 0;
	//SBLE_DEBUG("_sble_io_read_raw_internal entry");
	while(bytes_read < len)
	{
		//SBLE_DEBUG("_sble_io_read_raw_internal begin while loop");
		if(buf == NULL)
		{
			SBLE_DEBUG("problem");
		}
		//SBLE_DEBUG("filedescriptor: %u", dstate.filedescriptor);
		bytes_read += read(dstate.filedescriptor, buf, len - bytes_read);
		//SBLE_DEBUG("_sble_io_read_raw_internal middle of while loop");
		if(bytes_read <= 0)
		{
			perror("read(): ");
			SBLE_ERROR_CONTINUABLE("Could not read. Bytes_read is %d",bytes_read);
			return -1;
		}
		//SBLE_DEBUG("_sble_io_read_raw_internal end of while loop");
	}
	//SBLE_DEBUG("_sble_io_read_raw_internal return");
	return bytes_read;
}



sble_signed_integer _sble_io_read_raw(uint8_t* buf, uint32_t len){
	//uint32_t bytes_read = 0;
	//while(bytes_read < len){
	//	bytes_read += read(dstate.filedescriptor, buf, len - bytes_read);
	//	if(bytes_read <= 0){
	//		SBLE_ERROR("Could not read.");
	//	}
	//}
	return _sble_io_read_raw_internal(buf,len);
//	return sble_scheduler_virtio_rd(buf,len);
}

sble_signed_integer sble_io_read() {

	//SBLE_DEBUG("entered sble_io_read");
	const struct ble_msg *msg;
	struct ble_header hdr;
	unsigned char data[256]; //buffer size 256 as message'headers lolen, which specifies the class id, command id and payload that is to come with the api-message is 8 bits long.
	uint32_t bytes_read;
	uint32_t payload_length;
#ifndef SBLE_CONFIG_FASTMODE
	if (dstate.filedescriptor < 0) {
		SBLE_ERROR("Not Connected");
	}
#endif
//	SBLE_DEBUG("RD1...");
	//SBLE_DEBUG("entered sble_io_read: before read raw");
	bytes_read = _sble_io_read_raw((unsigned char*) &hdr, 4);
	//SBLE_DEBUG("entered sble_io_read: after read raw");

	if (bytes_read != 4) {
		SBLE_ERROR_CONTINUABLE("Failed to read header from BLE device at file-descriptor %d. Actually read %d bytes, but should have been 4 bytes. Errno from kernel is %d.",dstate.filedescriptor,bytes_read,errno);
		perror("POSIX-Kernel: ");
		return -1;
	}
//	SBLE_DEBUG("done.");
	payload_length = hdr.lolen;

	//check how many body bytes are to come...
	if (payload_length > 0) {
//		SBLE_DEBUG("rd 2...");
		if ((bytes_read = _sble_io_read_raw(data, payload_length))
				<= 0) {
			SBLE_ERROR_CONTINUABLE(
					"Failed to read api message body from BLE device. Read %d bytes, should have been %d bytes.",bytes_read, payload_length);
			return -1;
		}
//		SBLE_DEBUG("rd done.");

		msg = ble_get_msg_hdr(hdr);
		if (msg == NULL) {
			SBLE_ERROR_CONTINUABLE("Could not get api msg from BLE device. Class: %d - Command: %d", hdr.cls, hdr.command);
			return 0;
		}
		//call callback
//		sble_scheduler_lock_mutex();
		msg->handler(data);
//		sble_scheduler_unlock_mutex();

		/*
		SBLE_DEBUG("CLASS: %d",hdr.cls);
		SBLE_DEBUG("CMD: %d",hdr.command);
		SBLE_DEBUG("hilen: %d",hdr.type_hilen);
		SBLE_DEBUG("lolen: %d",hdr.lolen);
*/
		if((hdr.type_hilen & 1<<7)==0){
			SBLE_DEBUG("{ Response type. Class: %d, Command: %d}",msg->hdr.cls, msg->hdr.command);
			sble_scheduler_lock_mutex();

			dstate.flags |= SBLE_STATE_RESPONSE_RECEIVED;
			sble_scheduler_unlock_mutex();

		}else{
			SBLE_DEBUG("{ no response: %d}\n",hdr.type_hilen);
		}
	//	sble_scheduler_unlock_mutex();

	}

	return bytes_read;
}


#endif
