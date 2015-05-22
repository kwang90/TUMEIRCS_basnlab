/** \file sble_example_minimal_gatserver.c
 * \brief minimal demo application acting as advertiser & GATT-server.
 * * \date 29.8.2012
 * \author Philipp Kindt
 *
 * This example will communicate with the sble_example_minimal client.c- example.
 * This example assumes that the local gatt-DB has got the follwing attributes:
 * - Handle 16: 530accd9e7d490b957410ac5e52c656a, writable via ATT
 * - Handle 20: a1bad39994420d8e7a4e330340b895d1, readale via ATT
 * 
 * The last four bytes of each value received is expected to be a sequence counter.
 * This example will check if both local and remote sequence counter match to detect packet loss.
 *
 */

#include "sble.h"
#include <time.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("usage: ./sble_example_minimal_client device_node");
		exit(1);
	}
	sble_init(argv[1]);

	//make the device connectable by any node who wishes to
	//sble_make_connectable_by_any(20, 40);
	//make the device connectable by any node who wishes to
	sble_connect_to_any(6,6,70,0);

	//some variables for data rate measuement - measure time
	struct timespec tNow, tPrev, tStart;
	double duration, durationWhole;
	clock_gettime(CLOCK_MONOTONIC, &tPrev);

	//payload to receive
	sble_payload* pl;

	//a cycle counter. Is counts up for every packet received successfully.
	uint32_t cycleCnt = 0;
	uint32_t cycleCntWhole = 0;
	//a received sequence number.  Expect the last 2 bytes received in each packet to contain the cycle counter of the remote node and compare it to the lcoal cycleCnt to detect losses
	uint32_t recSeqNr;
	uint32_t tmp;

	uint32_t nPacketsInQueue;
	clock_gettime(CLOCK_MONOTONIC, &tStart);
	uint32_t rcvByteSum = 0;
	while (1) {
		//now wait for a client writing to the GATT server via ATT
		SBLE_DEBUG("waiting for incoming transfer.");


		//we now wait for a GATT-value event that indicates a new value has been received
		sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT);
		sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,
				SBLE_STATE_GATT_VALUE_EVENT);

		//now we look how many packets are in the reception queue
		nPacketsInQueue = sble_ll_get_nr_of_elements(dstate.ll_gatt_values);




		while (nPacketsInQueue > 0) {
			//get payload from packet
			pl = sble_ll_pop_from_begin(dstate.ll_gatt_values);
			nPacketsInQueue--;


			//do time profiling
			clock_gettime(CLOCK_MONOTONIC, &tNow);
			duration = (double) tPrev.tv_sec
					+ (double) tPrev.tv_nsec / 1000000000.0
					- (double) tNow.tv_sec
					- (double) tNow.tv_nsec / 1000000000.0;
			duration *= -1.0;
			durationWhole = (double) tStart.tv_sec
					+ (double) tStart.tv_nsec / 1000000000.0
					- (double) tNow.tv_sec
					- (double) tNow.tv_nsec / 1000000000.0;
			durationWhole *= -1.0;
			tPrev = tNow;

			//Print payload and check sequence counter
			if ((pl != NULL )&&(pl->data != NULL)){
				cycleCntWhole++;
				rcvByteSum += pl->data->len;
			//	printf("payload length: %d bytes\n",pl->data->len);
				if(cycleCntWhole % 100 == 0){
					printf("[Packet #%x, throughput = %.4f packets/sec, avg throughput = %.4f packets/se = %.4f bits/sec] read from remote node %d bytes: \n",cycleCnt,1.0/duration,(double) cycleCntWhole/durationWhole,(double) rcvByteSum*8.0 /durationWhole, pl->data->len);
				}
				sble_print_hex_array(pl->data->data,pl->data->len);
				recSeqNr = 0;

				for(tmp = 18; tmp < pl->data->len; tmp++) {
					recSeqNr <<=8;
					recSeqNr |= (*((uint8_t*)(pl->data->data + tmp)));
				}


				if(recSeqNr%10000 != cycleCnt%10000) {

					//ignore first sequence counter
					if(cycleCnt == 1) {
						cycleCnt = recSeqNr;
					}

					//check if local an remote counter are equal
					if(cycleCnt != 1) {
						printf("WARN: packet loss - received 0x%x, should be 0x%x\n",recSeqNr, cycleCnt);
						printf("current payload:\n");
						sble_print_hex_array(pl->data->data,pl->data->len);
						sble_payload_free_whole(&pl);
						cycleCnt = recSeqNr;
						exit(2);
					}

				}
				cycleCnt++;


			}
			//we have to free the payload's memory sble has allocated
			sble_payload_free_whole(&pl);

		}
	}
	//never reached
	return 0;


}
