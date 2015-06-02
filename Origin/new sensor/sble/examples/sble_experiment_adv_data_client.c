#include "sble.h"
#include "../bglib/cmd_def.h"
//#include <time.h>
#include <stdlib.h>

uint8_t sble_serial_adv_data_receive()
{
	time_t start,end;
	int t;
	uint8_t* data;
	uint32_t nPacketsInQueue;
	sble_payload* pl;
	uint8_t count = 0;
	uint8_t k = 0;

	while(count < 255)
	{
		//printf("ITS SOMETHING COMING SOON\n");
		dstate.evt_clear_list &= ~(SBLE_STATE_SCAN_RESPONSE_EVENT);
		sble_call_bl(ble_cmd_gap_discover_idx,2);
		sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_SCAN_RESPONSE_EVENT);

		nPacketsInQueue = sble_ll_get_nr_of_elements(dstate.ll_gatt_values);
		while (nPacketsInQueue > 0 && count < 255)
		{
			pl = sble_ll_pop_from_begin(dstate.ll_gatt_values);
			nPacketsInQueue--;

			data = (uint8_t*) malloc((size_t)pl->data->len);
			data = pl->data->data;

			if(data[0] == count)
			{
				if(count == 0)
				{
					start=time(NULL);
				}
				SBLE_DEBUG("Count: %d", count);
				count++;
			}
			/*
			if(data[0] == 0x00 && k == 0)
			{
				k = 1;
				count++;
			}
			if(data[0] == 0x02 && k == 1)
			{
				k = 0;
				count++;
			}*/
			//count++;
			sble_print_hex_array(pl->data->data,pl->data->len);
		}
		//printf("ITS SOMETHING\n");
	}
	end=time(NULL);

	t = end - start;

	double nob = 255 * 31;
	SBLE_DEBUG("Time: %d",t);
	SBLE_DEBUG("Datarate: %lf kBit/s",nob * 1 *8/(1000 * t));

	return 1;
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("usage: ./sble_example_minimal_client device_node");
		exit(1);
	}
	
	sble_init(argv[1]);
	
	uint8_t uuid_wr[16] = {0x53,0x0a,0xcc,0xd9,0xe7,0xd4,0x90,0xb9,0x57,0x41,0x0a,0xc5,0xe5,0x2c,0x65,0x6a};

	//scan interval, scan window, active
	//ble_cmd_gap_set_scan_parameters(30,30,0);
	//sble_call_bl(ble_cmd_gap_set_scan_parameters_idx, 30, 30, 0);
	sble_serial_scanning(30,30,0);

	sble_serial_adv_data_receive();

	sble_shutdown();
}
	
