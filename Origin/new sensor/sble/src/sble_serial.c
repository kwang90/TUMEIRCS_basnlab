/**
 * sble_serial.c
 *
 *  Created on: 02.05.2013
 *      Author: Mathias Gopp <gopp@rcs.ei.tum.de>
 *
 *      See sble_serial.h for a detailed description.
 **/

#include "sble_serial.h"
#include <time.h>

//#define SBLE_DEBUG(...) printf(__VA_ARGS__); printf("\n");
//#define SBLE_DEBUG_CON(...) SBLE_DEBUG(__VA_ARGS__);

serialP serialParameters = { .mode = 0, .conInterval = 0, .actualSpeed = 0,
		.sleepRounds = 0, .thread_arg = 0 };

uint32_t packetNo = 0;				//only used for printf()

_serialBufAdr serialBufferAddress;

int8_t sble_serial_init(uint8_t serialConnectionMode)
{

	SBLE_DEBUG("SERIAL INIT");
	//set serial parameters
	serialParameters.actualSpeed = 10;
	serialParameters.conInterval = 50;

	serialParameters.mode = serialConnectionMode;

	return 1;
}

int8_t sble_serial_send(uint32_t bufsize, uint8_t* databuf,sble_attribute* attribute, uint8_t multithreading)
{

	uint16_t nPackets = 0;

	//calculate number of packets to send over ble
	//first calculate the amount of carryover bytes
	uint8_t carryover = (bufsize + SBLE_PACKAGE_BYTES) % PACKET_SIZE;
	//calculate the amount of full packets
	uint32_t bytesToSendInFullPackets = bufsize + SBLE_PACKAGE_BYTES - carryover;

	if (carryover == 0)
	{
		carryover = PACKET_SIZE;
	}

	//check if there is more than one packet
	if (bytesToSendInFullPackets == 0 && carryover == 0)
	{
		//no data, return with error
		return -1;
	}

	//check if more than one packet needs to be sent
	if (bufsize + SBLE_PACKAGE_BYTES > PACKET_SIZE)
	{
		//increase packet count + 1 for the carryover bytes
		nPackets = bytesToSendInFullPackets / PACKET_SIZE + 1;
	}
	else
	{
		nPackets = 1;
	}

	SBLE_DEBUG("Sending: %d bytes in %d packets",bufsize,nPackets);

	sble_array* data;
	sble_array_malloc_whole(&data, PACKET_SIZE);

	uint16_t cyclecount = 0;
	uint32_t bytecount = 0;

	uint8_t tmpbuf[PACKET_SIZE];
	uint8_t i = 0;

	while (nPackets != cyclecount)
	{
		//if only one packet is sent then add the protocol information and the data
		//else attach the first packet with protocol information and data
		//further packets will only be filled with data
		if (nPackets == 1)
		{
			for (i = 0; i < bufsize + SBLE_PACKAGE_BYTES; i++)
			{
				if ((i == 0) || ((i == 1) && (SBLE_PACKAGE_BYTES == 2)))
				{
					if (i == 0)
					{
						tmpbuf[i] = nPackets & 0xFF;
					}
					if ((i == 1) && (SBLE_PACKAGE_BYTES == 2))
					{
						tmpbuf[i] = (nPackets >> 8) & 0xFF;
					}
				}
				else
				{
					if (bytecount <= bufsize)
					{
						tmpbuf[i] = databuf[i - SBLE_PACKAGE_BYTES];
						bytecount++;
					}
				}
			}

			memcpy(data->data, tmpbuf, bufsize + SBLE_PACKAGE_BYTES);
			data->len = bufsize + SBLE_PACKAGE_BYTES;
		}
		else
		{
			for (i = 0; i < PACKET_SIZE; i++)
			{
				if (cyclecount == 0)
				{
					if (i == 0 || ((SBLE_PACKAGE_BYTES == 2) && (i == 1)))
					{
						if (i == 0)
						{
							tmpbuf[i] = nPackets & 0xFF;
						}
						else
						{
							tmpbuf[i] = (nPackets >> 8) & 0xFF;
						}
					}
					else
					{
						if (bytecount < bufsize)
						{
							tmpbuf[i] = databuf[i - SBLE_PACKAGE_BYTES];
							bytecount++;
						}
					}
				}
				else
				{
					if (bytecount < bufsize)
					{
						tmpbuf[i] = databuf[cyclecount * PACKET_SIZE + i - SBLE_PACKAGE_BYTES];
						bytecount++;
					}
				}
			}

			if (cyclecount + 1 < nPackets)
			{
				memcpy(data->data, tmpbuf, PACKET_SIZE);
				data->len = PACKET_SIZE;
			}
			else
			{
				memcpy(data->data, tmpbuf, carryover);
				data->len = carryover;
			}
		}

		//send packet

		sble_bool success = SBLE_FALSE;

		do{
			if (multithreading == 0)
			{
				SBLE_DEBUG("Sending data: %d bytes",data->len);
				//sble_print_char_array(data->data,data->len);
				sble_print_hex_array(data->data, data->len);
				//	printf("[serial] sending packet no %d\n",packetNo);
				packetNo++;
				success = sble_attclient_write_by_attribute(0, attribute, data);
			}
			else
			{
				SBLE_DEBUG("Sending data: %d bytes",data->len);
				//sble_print_char_array(data->data,data->len);
				sble_print_hex_array(data->data, data->len);
				SBLE_DEBUG("- end");
				//printf("[serial] sending packet no %d\n", packetNo);
				packetNo++;
				success = sble_attclient_write_by_attribute_multithreaded(0,attribute, data, 1);
			}

		} while (success != SBLE_TRUE);

		//increase counter for each sent packet
		cyclecount++;
	}

	//free memory
	sble_array_free_whole(&data);

	SBLE_DEBUG_CON("nPackets: %d\n",cyclecount);
	//return if success

	return 1;
}

int8_t sble_serial_send_pseudo(uint32_t bufsize, uint8_t* databuf,
		sble_attribute* attribute, uint8_t multithreading) {
	SBLE_DEBUG_CON("SERIAL SEND - DATA PSEUDO:");
	sble_print_hex_array(databuf, bufsize);

	uint8_t packetsize = PACKET_SIZE;	//Number of Bytes that fit into a packet

	uint16_t nPackets = 0;//a packet counter (number of packet sent containing byte of this buffer)

	uint8_t carryover = bufsize % packetsize;	//Bytes left in last packet


	//if the last packet is empty, then fill it completely
	if (carryover == 0)
	{
		carryover = packetsize;
	}

	uint32_t bytesToSendInFullPackets = bufsize - carryover;//Bytes to be sent with full packets

	//check if there is more than one packet
	if (bytesToSendInFullPackets == 0 && carryover == 0) {
		//no data, return with error
		return -1;
	}

	if (bufsize > 20)
	{
		//increase packet count + 1 for the carryover bytes
		nPackets = bytesToSendInFullPackets / packetsize + 1;
	}
	else
	{
		nPackets = 1;
	}

	SBLE_DEBUG("Sending: %d bytes in %d packets",bufsize,nPackets);

	sble_array* data;
	sble_array_malloc_whole(&data, packetsize);

	uint16_t cyclecount = 0;
	uint32_t bytecount = 0;

	uint8_t tmpbuf[packetsize];
	uint8_t i = 0;

	while (nPackets > cyclecount) {

		//send only one packet
		if (nPackets == 1) {
			for (i = 0; i < bufsize; i++) {
				if (bytecount <= bufsize) {
					tmpbuf[i] = databuf[i];
					bytecount++;
				}
			}

			memcpy(data->data, tmpbuf, bufsize);
			data->len = bufsize;

		//send more than one packet
		} else {
			for (i = 0; i < packetsize; i++) {
				if (bytecount < bufsize) {
					tmpbuf[i] = databuf[cyclecount * packetsize + i];
					bytecount++;
				}
			}

			if (cyclecount < nPackets - 1) {		//all packets except last one (= all full-sized packets)
				memcpy(data->data, tmpbuf, packetsize);
				data->len = packetsize;
			} else {						//last, none-full-size packet
				//printf("\t ====last one!=======\n");
				memcpy(data->data, tmpbuf, carryover);
				data->len = carryover;
			}
		}

		//send packet

		sble_bool success = SBLE_FALSE;

		do {
			if (multithreading == 0)
			{
				SBLE_DEBUG("Sending data: %d bytes",data->len);
				//sble_print_char_array(data->data,data->len);
				sble_print_hex_array(data->data, data->len);
				success = sble_attclient_write_by_attribute(0, attribute, data);
			}
			else
			{
				SBLE_DEBUG("Sending data: %d bytes",data->len);
				//sble_print_char_array(data->data,data->len);
				//		sble_print_hex_array(data->data,data->len);
				//printf("[serial] sending packet no %d\n", packetNo);
				//write(1, data->data, data->len);
				packetNo++;

				success = sble_attclient_write_by_attribute_multithreaded(0, attribute, data, 1);
				if(!success)
				{
					printf("sble_attclient_write_by_attribute_multithreaded() returned with error!");
					exit(1);
				}
				SBLE_DEBUG("- sending done.");

			}

		} while (success != SBLE_TRUE);

		//increase counter for each sent packet
		cyclecount++;
	}

	//free memory
	sble_array_free_whole(&data);

	SBLE_DEBUG_CON("nPackets: %d\n",cyclecount);
	//return if success

	return 1;
}

uint32_t sble_serial_receive(uint8_t* buf)
{
	return sble_serial_receive_autoReconnect(buf, 0, NULL);
}

uint32_t sble_serial_receive_autoReconnect(uint8_t* buf, uint8_t reconnect, sble_reconnect_callback cb)
{
	uint32_t bytecount = 0;
	uint32_t nPacketsInQueue;

	//uint16_t counter = 1;
	uint16_t packetnumber = 0;
	uint16_t nPacketser = 0;

	uint8_t packetsize = PACKET_SIZE;
	uint8_t* data;

	static uint32_t packetsReceived = 0;
	static struct timespec tRcv1 = { 0, 0 };
	static struct timespec tRcv = { 0, 0 };
	double duration;

	sble_payload* pl;
	//fixme write this nicer without goto..
	while (1)
	{
		loop: sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT | SBLE_STATE_DISCONNECT_EVENT);
		sble_scheduler_wait_for_event_no_reset(SBLE_THREAD_MAIN,SBLE_STATE_GATT_VALUE_EVENT | SBLE_STATE_DISCONNECT_EVENT);
		sble_scheduler_lock_mutex();

		if (dstate.flags & SBLE_STATE_DISCONNECT_EVENT)
		{
			dstate.flags &= ~(SBLE_STATE_DISCONNECT_EVENT | SBLE_STATE_EVENT);
			sble_scheduler_unlock_mutex();

			if (reconnect != 0)
			{
				if (cb != NULL)
				{
					cb();
					bytecount = 0;
					packetnumber = 0;
					nPacketser = 0;
					continue;
				}
			}
		}

		dstate.flags &= ~(SBLE_STATE_EVENT | SBLE_STATE_GATT_VALUE_EVENT | SBLE_STATE_DISCONNECT_EVENT);
		nPacketsInQueue = sble_ll_get_nr_of_elements(dstate.ll_gatt_values);
		sble_scheduler_unlock_mutex();

		while (nPacketsInQueue > 0)
		{
			//get data from list
			sble_scheduler_lock_mutex();
			pl = sble_ll_pop_from_begin(dstate.ll_gatt_values);
			sble_scheduler_unlock_mutex();

			nPacketsInQueue--;

			if ((pl != NULL) && (pl->data != NULL))
			{
			#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_POSIX
				clock_gettime(CLOCK_MONOTONIC, &tRcv);
				if (tRcv1.tv_sec == 0)
				{
					tRcv1 = tRcv;
				}
				duration = (double) tRcv.tv_sec - (double) tRcv1.tv_sec + ((double) tRcv.tv_nsec - (double) tRcv1.tv_nsec) / 1000000000.0;
			#endif

				SBLE_DEBUG_CON("RWA read from remote node %d bytes: ",pl->data->len);
				//sble_print_char_array(pl->data->data,pl->data->len);
				sble_print_hex_array(pl->data->data, pl->data->len);
				SBLE_DEBUG_CON(" - end");
				data = pl->data->data;

				if (nPacketser == 0)
				{
					if (SBLE_PACKAGE_BYTES == 1)
					{
						packetnumber = data[0];
					}
					else
					{
						packetnumber = (data[1] << 8) | data[0];
						if (packetnumber == 0)
						{
							exit(2);
						}
					}
					buf = (uint8_t*) malloc(packetnumber * packetsize);
				}

				SBLE_DEBUG_CON("Number of packets should be received: %d\n",packetnumber);

				uint32_t i = 0;

				for (i = 0; i < pl->data->len; i++)
				{
					if (bytecount == 0)
					{
						i = SBLE_PACKAGE_BYTES;
					}

					//add data to the buffer
					buf[bytecount] = data[i];
					bytecount++;
				}

				sble_payload_free_whole(&pl);

				nPacketser++;
				packetsReceived++;
				SBLE_DEBUG("packets received: %d @ %f => avg Throughput = %f by/s\n",packetsReceived, duration,((double) packetsReceived - 1.0) / duration * 20.0);
				SBLE_DEBUG_CON("nPacketser: %d \n",nPacketser);

				if (packetnumber == nPacketser)
				{
					SBLE_DEBUG_CON("Bytes received: %d \n",bytecount);

					//set the buffer pointer
					sble_serial_setBufAdr(buf);
					return bytecount;
				}

			}
			else
			{
				SBLE_DEBUG_CON("No Payload received.");
			}
		}
	}
	return bytecount;
}

uint32_t sble_serial_receive_pseudo(uint8_t* buf){
	return sble_serial_receive_pseudo_autoReconnect(buf,0,NULL);
}
uint32_t sble_serial_receive_pseudo_autoReconnect(uint8_t* buf, uint8_t reconnect, sble_reconnect_callback cb)

{
	uint32_t nPacketsInQueue;
	uint32_t nPacketser = 0;
	sble_payload* pl;
	static uint32_t packetsReceived = 0;
	static struct timespec tRcv1 = { 0, 0 };
	static struct timespec tRcv = { 0, 0 };
	double duration;
	uint32_t bytesReceived = 0;
	uint32_t i = 0;
	while (1)
	{
		loopPseudo: sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT | SBLE_STATE_DISCONNECT_EVENT);
				sble_scheduler_wait_for_event_no_reset(SBLE_THREAD_MAIN,SBLE_STATE_GATT_VALUE_EVENT | SBLE_STATE_DISCONNECT_EVENT);
				sble_scheduler_lock_mutex();

				if (dstate.flags & SBLE_STATE_DISCONNECT_EVENT)
				{
					dstate.flags &= ~(SBLE_STATE_DISCONNECT_EVENT | SBLE_STATE_EVENT);
					sble_scheduler_unlock_mutex();

					if (reconnect != 0)
					{
						if (cb != NULL)
						{
							cb();
							nPacketser = 0;
							continue;
						}
					}
				}


#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_POSIX
				clock_gettime(CLOCK_MONOTONIC, &tRcv);
				if (tRcv1.tv_sec == 0)
				{
					tRcv1 = tRcv;
				}
				duration = (double) tRcv.tv_sec - (double) tRcv1.tv_sec + ((double) tRcv.tv_nsec - (double) tRcv1.tv_nsec) / 1000000000.0;

#endif

		dstate.flags &= ~(SBLE_STATE_EVENT | SBLE_STATE_GATT_VALUE_EVENT | SBLE_STATE_DISCONNECT_EVENT);

		nPacketsInQueue = sble_ll_get_nr_of_elements(dstate.ll_gatt_values);
		buf = (uint8_t*) malloc(nPacketsInQueue * 20);
		if(buf == NULL){
			SBLE_ERROR("Malloc failed.");
			exit(1);
		}
		while (nPacketsInQueue > 0)
		{
			pl = sble_ll_pop_from_begin(dstate.ll_gatt_values);
			nPacketsInQueue--;

			if ((pl != NULL) && (pl->data != NULL))
			{

				SBLE_DEBUG_CON("RWA read from remote node %d bytes: ",pl->data->len);
				sble_print_hex_array(pl->data->data, pl->data->len);

				for (i = 0; i < pl->data->len; i++)
					{
						//add data to the buffer
						buf[bytesReceived] = pl->data->data[i];
						bytesReceived++;
					}

				nPacketser++;
#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_POSIX
				packetsReceived++;
				SBLE_DEBUG("packets received: %d @ %f => avg Throughput = %f by/s \t currentLen = %d\n",packetsReceived, duration,((double) bytesReceived) / duration, pl->data->len );
#endif
				SBLE_DEBUG_CON("nPacketser: %d \n",nPacketser);
				sble_payload_free_whole(&pl);

			}
		}
		sble_serial_setBufAdr(buf);

		sble_scheduler_unlock_mutex();
		return bytesReceived;

	}
}


uint32_t sble_serial_calc_speed_adjustment(double bytesPerSecond, uint32_t buffersize, double period)
{
	uint16_t bps = (uint16_t) bytesPerSecond;
	if (bps < 1)
	{
		bps = 1;
	}

	//uint32_t conInterval = (uint32_t)(1000.0 * 10.0 / (1.25 * (bps + 2.0 * buffersize / period)));
	uint32_t conInterval = (uint32_t)(1000.0 * 47.0 / (1.25 * (bps + 4.0 * buffersize / period))); // for fast ble

	if (conInterval < 6)
	{
		conInterval = 6;
	}

	if (conInterval > 3200)
	{
		conInterval = 3200;
	}

	return conInterval;
}

uint8_t sble_serial_change_speed_specific(uint16_t conIntervalMin, uint16_t conIntervalMax, uint8_t blocking)
{
	uint16_t timeout = 2400;

	//check if connection interval is in range
	if (conIntervalMin < 6)
	{
		conIntervalMin = 6;
	}
	if (conIntervalMin > 3200)
	{
		conIntervalMin = 3200;
	}

	//do some timeout settings which are in range
	if (conIntervalMin > 50 && conIntervalMin < 200)
	{
		timeout = 2400;
	}
	if (conIntervalMin >= 200 && conIntervalMin < 600)
	{
		timeout = 2400;
	}
	if (conIntervalMin >= 600 && conIntervalMin < 1200)
	{
		timeout = 2400;
	}
	if (conIntervalMin >= 1200 && conIntervalMin < 2400)
	{
		timeout = 2400;
	}
	if (conIntervalMin >= 2400 && conIntervalMin <= 3200)
	{
		timeout = 2400;
	}

	serialParameters.conInterval = conIntervalMin;

	uint8_t rv;
	if (blocking == 0)
	{
		sble_call_nb(ble_cmd_connection_update_idx, dstate.current_con,conIntervalMin, conIntervalMin, 0, timeout);
		return 0;
	}
	else
	{
		sble_call_bl_ext_thread(ble_cmd_connection_update_idx,dstate.current_con, conIntervalMin, conIntervalMax, 0, timeout);
		sble_scheduler_lock_mutex();
		rv = ((dstate.flags & SBLE_STATE_CMD_SUCCESS) != 0);
		dstate.flags &= ~(SBLE_STATE_CMD_SUCCESS);
		sble_scheduler_unlock_mutex();
		return rv;
	}
}

//#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX

void sble_serial_advertising(uint16_t adv_int_min,uint16_t adv_int_max)
{
	sble_call_bl(ble_cmd_gap_set_adv_parameters_idx,adv_int_min,adv_int_max,0x01);//better: 0x07 for all channels
	sble_call_bl(ble_cmd_gap_set_mode_idx,2);
	SBLE_DEBUG_CON("Starting BLE advertising sequence...");
}

void sble_serial_scanning(uint16_t scan_interval,uint16_t scan_window,uint8_t active)
{
	sble_call_bl(ble_cmd_gap_set_scan_parameters_idx, scan_interval, scan_window, active);
}
//#endif

void sble_serial_setBufAdr(uint8_t* bufAdr) {
	serialBufferAddress = bufAdr;
}

uint8_t* sble_serial_getBufAdr() {
	return serialBufferAddress;
}
