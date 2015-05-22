/** \file sble_example_client.c
 * \brief demo application acting as scanner and simple att-cliet
 * \date 17.7.2012
 * \author Philipp Kindt
 *
 * This example will communicate with the sble_example_gattserver.c- example.
 * This example assumes that the remote GATT-DB has got the follwing attributes:
 * - Handle 16: 530accd9e7d490b957410ac5e52c656a, writable via ATT
 * - Handle 20: a1bad39994420d8e7a4e330340b895d1, readale via ATT
 * 
 *
 *
 */

#include "sble.h"
#include "../bglib/cmd_def.h"
#include <inttypes.h>
#include <string.h>		//for memcpy
#include "sble_experiment_advScanning_params.h"
#include <time.h>

int main(int argc, char* argv[]){
	struct timespec tStart, tEnd;
	time_t random_seed;
	double duration;
	if(argc != 2){
		printf("Usage: sble_experiment_advScanning_scanner devnode\n");
		exit(1);
	}
	time(&random_seed);
	srand(random_seed);

	printf("init...\n");
	sble_init(argv[1]);
	printf("bla...\n");
	uint32_t repCnt = 0;
	uint32_t IaCnt = SBLE_EXP_ADVSCANNING_IA_MIN;
	printf("Ia,\tRep,\tduration\n");


	while(1){
		if(repCnt >= SBLE_EXP_ADVSCANNING_NROFREPETITIONS){
			repCnt = 0;
			IaCnt = IaCnt + SBLE_EXP_ADVSCANNING_IA_STEP;
		}else{
			repCnt++;
		};

		dstate.evt_clear_list &= ~(SBLE_STATE_SCAN_RESPONSE_EVENT);
		sble_call_bl(ble_cmd_gap_set_scan_parameters_idx, SBLE_EXP_ADVSCANNING_SCAN_INTERVAL, SBLE_EXP_ADVSCANNING_SCAN_WINDOW,0);
		sble_call_bl(ble_cmd_gap_discover_idx,1);
		clock_gettime(CLOCK_MONOTONIC_RAW,&tStart);
		SBLE_DEBUG("waiting for scan response...");

		sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_SCAN_RESPONSE_EVENT);
		clock_gettime(CLOCK_MONOTONIC_RAW,&tEnd);


		dstate.evt_clear_list &= ~(SBLE_STATE_CONNECTION_EVENT);
		sble_call_bl(ble_cmd_gap_connect_direct_idx,dstate.addr, gap_address_type_public, 40,40,200,200);
		SBLE_DEBUG("Found remote device. Connecting...");
		sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_CONNECTION_EVENT);
		SBLE_DEBUG("Connected.");


		dstate.evt_clear_list &= ~(SBLE_STATE_DISCONNECT_EVENT);
		sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_DISCONNECT_EVENT);
		duration = tEnd.tv_sec * 1000000000l + tEnd.tv_nsec - tStart.tv_sec * 1000000000l - tStart.tv_nsec;
		printf("%f,\t%d,\t\%f\t End: \%u, %lu - start: %u, %lu\n",IaCnt*625.0/1000000,repCnt,duration/1000000000.0,tStart.tv_sec, tStart.tv_nsec, tEnd.tv_sec, tEnd.tv_nsec);

	}
/*
	for(IaCnt = SBLE_EXP_ADVSCANNING_IA_MIN; IaCnt < SBLE_EXP_ADVSCANNING_IA_MAX; IaCnt += SBLE_EXP_ADVSCANNING_IA_STEP){
		for(repCnt = 0; repCnt < SBLE_EXP_ADVSCANNING_NROFREPETITIONS; repCnt ++){
			usleep((double)rand()/(double)RAND_MAX * 3.0* (double)SBLE_EXP_ADVSCANNING_SCAN_INTERVAL*626.0);
			clock_gettime(CLOCK_MONOTONIC,&tStart);
			//	SBLE_DEBUG("setting ADV parameters...");
			dstate.evt_clear_list &= ~(SBLE_STATE_CONNECTION_EVENT);
			sble_call_bl(ble_cmd_gap_set_adv_parameters_idx,IaCnt,IaCnt,SBLE_EXP_ADVSCANNING_CHANNEL_MAP);		//better: 0x07 for all channels
			sble_call_bl(ble_cmd_gap_set_mode_idx, gap_general_discoverable,gap_undirected_connectable);

			//SBLE_DEBUG("waiting for connection...");
			dstate.evt_clear_list &= ~(SBLE_STATE_CONNECTION_EVENT);
			sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_CONNECTION_EVENT);
			clock_gettime(CLOCK_MONOTONIC,&tEnd);
			duration = tEnd.tv_sec * 1000000000 + tEnd.tv_nsec - tStart.tv_sec * 1000000000 - tStart.tv_nsec;
			printf("%d,\t%d,\t\%f\n",IaCnt,repCnt,duration);
			sble_call_bl(ble_cmd_connection_disconnect_idx,0);
			sble_call_bl(ble_cmd_system_reset_idx);
		}
	};
	*/
	sble_shutdown();
	return 0;
}

