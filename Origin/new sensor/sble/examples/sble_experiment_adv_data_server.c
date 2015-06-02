#include "sble.h"
#include "../bglib/cmd_def.h"
#include <time.h>
#include <stdlib.h>

/*void sble_serial_advertising(uint16_t adv_int_min,uint16_t adv_int_max)
{
		sble_call_bl(ble_cmd_gap_set_adv_parameters_idx,adv_int_min,adv_int_max,0x01);		//better: 0x07 for all channels
		sble_call_bl(ble_cmd_gap_set_mode_idx,2);
		SBLE_DEBUG_CON("Starting BLE advertising sequence...");
}*/

int8_t sble_serial_set_adv_data(uint32_t bufsize, uint8_t* buf)
{
	SBLE_DEBUG_CON("enterd adv data function");
	uint8_t packetsize = 31;
	uint16_t packetcount = 0;
	uint16_t count = 0;

	//calculate number of packets
	uint8_t carryover = (bufsize) % packetsize;
	uint32_t bufsizetemp = bufsize - carryover;
	uint32_t bytecount = 0;

	if(carryover == 0)
	{
		carryover = packetsize;
	}

	//check if there is more than one packet
	if(bufsizetemp == 0 && carryover == 0)
	{
		//no data, return with error
		return -1;
	}

	if(bufsize > 31)
	{
		//increase packet count + 1 for the carryover bytes
		packetcount = bufsizetemp/packetsize + 1;
	}
	else
	{
		packetcount = 1;
	}

	uint8_t tmpbuf[packetsize];

	while(packetcount != count)
	{
		uint8_t i = 0;
		for(i = 0; i < packetsize; i++)
		{
			if(bytecount < bufsize)
			{
				tmpbuf[i] = buf[count * packetsize + i];
				bytecount++;
			}
		}

		ble_cmd_gap_set_adv_data(1,packetsize,tmpbuf);
		count++;
	}
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("usage: ./sble_example_minimal_client device_node");
		exit(1);
	}
	
	sble_init(argv[1]);

	sble_payload* pl;
	sble_payload* pl2;

	uint8_t* data;
	uint8_t* data2;
	//time_t start,end;
	//start=time(NULL);
	
	//maximum amount of bytes is 31 databytes for advertising 
	uint32_t numOfBytes = 31;
	
	//sble_array_malloc_whole();
	data = (uint8_t*) malloc(numOfBytes);
	data2 = (uint8_t*) malloc(numOfBytes);
	
	uint8_t i = 0;
	for(i = 0; i < numOfBytes; i++)
	{
		data[i] = i;
		data2[i] = i+2;
	}
	
	//uint8_t packetsize = 31;
	//uint16_t packetcount = 0;

	//calculate number of packets
	//uint8_t carryover = (numOfBytes) % packetsize;
	//uint32_t bufsizetemp = numOfBytes - carryover;

	pl->data->len = numOfBytes;
	//pl2->data->len = numOfBytes;

	/*uint8_t tmp[packetsize];
	for(i = 0; i < packetsize; i++)
	{
		tmp[i] = data[i];
	}*/
	pl->data->data = data;
	//pl2->data->data = data2;
	//make the device connectable by any node who wishes to
	//sble_make_connectable_by_any(20,40);
	sble_serial_advertising(10,10);
	
	//need to wait for something like scan response...

	SBLE_DEBUG("SET ADV DATA");
	ble_cmd_gap_set_adv_data(0,pl->data->len,pl->data->data);
	
	//sble_serial_set_adv_data(62, data);
	//while(1);
	//ble_cmd_gap_end_procedure();
	uint8_t k = 0;
	/*struct timespec *tim;
	struct timespec *tim2;
	tim->tv_sec = 0;
	tim->tv_nsec = 40000;*/

	uint8_t cntr = 0;
	uint8_t loop = 0;

	while(1)
	{
		//SBLE_DEBUG("IN THE LOOP")
		//dstate.evt_clear_list &= ~(SBLE_STATE_SCAN_RESPONSE_EVENT);
		//sble_call_bl(ble_cmd_gap_discover_idx,2);
		//sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_SCAN_RESPONSE_EVENT);
		//SBLE_DEBUG("RECEIVED SCAN RESPONSE");
		//nanosleep(tim,tim2);
		usleep(200000);
		/*
		if(k == 0)
		{
			pl->data->data = data2;
			ble_cmd_gap_set_adv_data(0,pl->data->len,pl->data->data);
			k=1;
			SBLE_DEBUG("DATASET 2");
		}
		else
		{
			pl->data->data = data;
			ble_cmd_gap_set_adv_data(0,pl->data->len,pl->data->data);
			k=0;
			SBLE_DEBUG("DATASET 1");
		}
		*/

		data[0] = cntr;
		pl->data->data = data;
		ble_cmd_gap_set_adv_data(0,pl->data->len,pl->data->data);
		cntr++;

		if(cntr == 255)
		{
			loop++;
			cntr = 0;
		}

		if(loop == 100)
		{
			break;
		}
	}
	//sble_disconnect(dstate.current_con);
	sble_shutdown();
}
	
