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

int main(int argc, char *argv[]){
	struct timespec tStart, tEnd;
	time_t random_seed;
	double duration;

	time(&random_seed);
	srand(random_seed);
	if(argc != 2){
		printf("please give BLE node device!\n");
		exit(1);
	}

	sble_init(argv[1]);
	uint32_t repCnt;
	uint32_t IaCnt;
	printf("Ia,\tRep,\tduration\n");

	for(IaCnt = SBLE_EXP_ADVSCANNING_IA_MIN; IaCnt < SBLE_EXP_ADVSCANNING_IA_MAX; IaCnt += SBLE_EXP_ADVSCANNING_IA_STEP){
		for(repCnt = 0; repCnt < SBLE_EXP_ADVSCANNING_NROFREPETITIONS; repCnt ++){
			//sleep for a random inteval < Is
			usleep((double)rand()/(double)RAND_MAX * 3.0* (double)SBLE_EXP_ADVSCANNING_SCAN_INTERVAL*626.0);
			
			SBLE_DEBUG("setting ADV parameters...");
			dstate.evt_clear_list &= ~(SBLE_STATE_CONNECTION_EVENT);
			sble_call_bl(ble_cmd_gap_set_adv_parameters_idx,IaCnt,IaCnt,SBLE_EXP_ADVSCANNING_CHANNEL_MAP);
			
			//read clock
			clock_gettime(CLOCK_MONOTONIC_RAW,&tStart);

			//start advertising
			sble_call_bl(ble_cmd_gap_set_mode_idx, gap_general_discoverable,gap_undirected_connectable);

			SBLE_DEBUG("waiting for connection...");
			dstate.evt_clear_list &= ~(SBLE_STATE_CONNECTION_EVENT);
			
			//wait for connection
			sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_CONNECTION_EVENT);

			//get time
			clock_gettime(CLOCK_MONOTONIC_RAW,&tEnd);
			sble_call_bl(ble_cmd_gap_end_procedure_idx);
			
			//calc duration
			duration =(double)tEnd.tv_sec * 1000000000.0 + (double) tEnd.tv_nsec - (double) tStart.tv_sec * 1000000000.0 - (double) tStart.tv_nsec;
			printf("%f,\t%d,\t\%f\n",IaCnt*625.0/1000000,repCnt,duration/1000000000.0);

			//disconnect and wait
			dstate.evt_clear_list &= ~(SBLE_STATE_DISCONNECT_EVENT);
			sble_call_bl(ble_cmd_connection_disconnect_idx,dstate.current_con);
			SBLE_DEBUG("waiting until connecation has been closed...");

			sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_DISCONNECT_EVENT);

			//usleep((double)SBLE_EXP_ADVSCANNING_SCAN_INTERVAL*625.0);
		}
	};
	sble_shutdown();
	return 0;
}

