/**\file sble_example_read_e1ch.c
* \brief reading the acceleration data from the e1ch project
* \date 6.3.2015
* \author Philipp Kindt, Jonathan Klimt
*
* This Programm reads the acceleration from the e1ch project and and prints them to the comand line
* Additionaly it executes a very simple fall detection algorithm
*
* For compilation, you have to set the -lm flag for the gcc!
* 
* @param the device path of your ble reciever (eg /dev/ttyACM1)
*
*/

#include "sble.h"
#include "../bglib/cmd_def.h"
#include <inttypes.h>
#include <string.h>		//for memcpy
#include <mcheck.h>
#include <math.h>		//for sqrt
#include <stdlib.h>
#include <time.h>

//Calibration Values, NOT PRECISE! Quick and Dirty Measurements
#define LSM9DS0_SENSITIVITY_MULTIPLIER_X 0.000184672		
#define LSM9DS0_SENSITIVITY_MULTIPLIER_Y 0.000183486
#define LSM9DS0_SENSITIVITY_MULTIPLIER_Z 0.000181159
#define LSM9SS0_OFFSET_X -0.033
#define LSM9SS0_OFFSET_Y -0.01
#define LSM9SS0_OFFSET_Z 0.013

#define THRESHOLD_LYING 10 				// time for detection of a serious Accident
#define THRESHOLD_FALLING 0.5 			// duration of falling. 
#define THRESHOLD_ACC_FALLING 3.3 		// acceleration threshold of a serious fall accident, accelerations below threshold are (probably) ADLs
#define THRESHOLD_ACC_STANDING_UP 0.5 	// acceleration needet to stand up after a fall
#define true 1
#define false 0

enum Statemachine { NORMAL, DETECTION, WAIT_WHILE_FALLING, FALLEN, STILL_LYING};
enum Statemachine states;
time_t start, stop;
double A;			//Acc Values
double Xacc;
double Yacc;
double Zacc;		
double diff;				

int fallDetectionAlgorithm (){
 	switch (states){
		case NORMAL:
		if (A>THRESHOLD_ACC_FALLING){
			printf("FALL ALERT!!! Inform medical staff");
			start=time(NULL);					// save start time of fall
			states=WAIT_WHILE_FALLING;
		}
		break;

		case WAIT_WHILE_FALLING:
		if (difftime(stop=time(NULL),start)>THRESHOLD_FALLING){
			states=FALLEN;
		}
		break;

		case FALLEN:
		if (A>THRESHOLD_ACC_STANDING_UP){
			printf("PERSON STANDING UP");
			states = NORMAL;
		}
		else{
			if (difftime(stop=time(NULL),start)>THRESHOLD_LYING){
				states = STILL_LYING;
				printf ("WARNING!!! FALL DETECTED! PERSON CAN'T MOVE!!! call police, fire brigade and maybe a doctor... \n");
				return 0;
			}
		}
		break;

		case STILL_LYING:
		//space for future improvements
		break;

		default: printf("ERROR IN STATES");

	}
	return 1;
}


int main(int argc, char* argv[]){

	uint16_t cycleCnt = 0;

	sble_array* data;
	sble_attribute* att_notify;
	sble_attribute* att_cfg;
	int fall_alert_detected = false;
	
	// device name has to be an argument
	if(argc != 2){				
		printf("usage: ./sble_example_minimal_client device_node");
		exit(1);
	}
	//define our attribute's UUIs. This would have been eaiser with ble_type_conversion_hexstring_to_binary(), but just to show you another way...
	uint8_t uuid_wr_notify[2] = {0x02,0x29};
	uint8_t uuid_wr_cfg[16] = {0x07,0x29,0x5d,0xf2,0xb0,0x5c,0x11,0xe4,0xad,0x44,0x3c,0x97,0x02,0xae,0x00,0x00};

	sble_init(argv[1]);


	//connect to any node in range with minimal connection interval
	printf("waiting...\n");
	SBLE_DEBUG("Connecting to any node in range...");


	sble_scheduler_lock_mutex();
	dstate.evt_clear_list &= ~(SBLE_STATE_SCAN_RESPONSE_EVENT);
	sble_scheduler_unlock_mutex();

	sble_call_bl(ble_cmd_gap_discover_idx,2);
	//printf("Waiting for scan response event...\n");
	sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_SCAN_RESPONSE_EVENT);
 
	SBLE_DEBUG("Found remote device. Connecting...");

	sble_scheduler_lock_mutex();
	dstate.evt_clear_list &= ~(SBLE_STATE_CONNECTION_EVENT);
	sble_scheduler_unlock_mutex();

	sble_print_hex_array(dstate.addr,16);		//debug
	sble_call_bl(ble_cmd_gap_connect_direct_idx,dstate.addr, gap_address_type_random, 7,7,200,100);
	//	sble_call_bl(ble_cmd_gap_connect_direct_idx,dstate.addr, gap_address_type_public, con_int_min,con_int_max,timeout,latency);
	sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_CONNECTION_EVENT);
	SBLE_DEBUG("Connected.");
	//	sble_call_bl(ble_cmd_gap_end_procedure_idx,245);

	uint8_t ccon;		//current connection

	sble_scheduler_lock_mutex();
	ccon = dstate.current_con;
	sble_scheduler_unlock_mutex();

	/*
	sble_make_connectable_by_any(40,50);
	sble_scheduler_autoclear_prevent(SBLE_STATE_CONNECTION_EVENT);
	sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,	SBLE_STATE_CONNECTION_EVENT);
	*/
	
	SBLE_DEBUG("connection established!");

	printf("con!\n");


	//-> create an attribute to write
	sble_attribute_malloc_whole(&att_notify,2);		//UUID has 2 byte
	memcpy(att_notify->uuid->data,uuid_wr_notify,2);
	sble_attribute_malloc_whole(&att_cfg,16);		//UUID has 16 byte
	memcpy(att_cfg->uuid->data,uuid_wr_cfg,16);

	//and create the data to write
	sble_array_malloc_whole(&data,2);		//1 bytes to write
	data->len = 2;
	data->data[0] = 0x01;
	data->data[1] = 0x00;

	//retrive attribute list at remote. Only after doing this, sble_attclient_write_by_attribute() is available
	sble_attclient_getlist(0);

	printf("RCV list\n");
	//sble_attclient_write_by_handle(0,15,data);
	sble_attclient_write_by_attribute(0,att_notify,data);
	data->data[0] = 0x64;
	data->data[1] = 0x02;			//Scale sensitivity
	sble_attclient_write_by_attribute(0,att_cfg,data);
	//payload to receive
	sble_payload* pl;

	uint32_t nPacketsInQueue;

	
	int16_t x = 0, y=0, z=0;

	//do it!
	while(1){	

		SBLE_DEBUG("waiting for data...");

		//we now wait for a GATT-value event that indicates a new value has been received
		sble_scheduler_autoclear_prevent(SBLE_STATE_ATTRIBUTE_VALUE_EVENT);
		sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,	SBLE_STATE_ATTRIBUTE_VALUE_EVENT);
		sble_scheduler_events_clear(SBLE_STATE_ATTRIBUTE_VALUE_EVENT);
		//now we look how many packets are in the reception queue
		nPacketsInQueue = sble_ll_get_nr_of_elements(dstate.cons[0]->ll_rcvqueue);




		while (nPacketsInQueue > 0) {
			//get payload from packet
			pl = sble_ll_pop_from_begin(dstate.cons[0]->ll_rcvqueue);
			nPacketsInQueue--;

			//Print payload and check sequence counter
			if ((pl != NULL )&&(pl->data != NULL)){
				//printf("payload length: %d bytes\n",pl->data->len);

				//print value
				sble_print_hex_array(pl->data->data,pl->data->len);
				
				x = (int16_t) pl->data->data[2];
				x |= ((int16_t) pl->data->data[3])<<8;

				y = (int16_t) pl->data->data[4];
				y |= ((int16_t) pl->data->data[5])<<8;
				
				z = (int16_t) pl->data->data[6];
				z |= ((int16_t) pl->data->data[7])<<8;
				
				Xacc = LSM9DS0_SENSITIVITY_MULTIPLIER_X * (double) x + LSM9SS0_OFFSET_X;							//Converts to real g-values
				Yacc = LSM9DS0_SENSITIVITY_MULTIPLIER_Y * (double) y + LSM9SS0_OFFSET_Y;
				Zacc = LSM9DS0_SENSITIVITY_MULTIPLIER_Z * (double) z + LSM9SS0_OFFSET_Z;

				//Creates the vector sum, representing the overall acceleration minus normal earth acceleration
				A = sqrt( Xacc*Xacc+Yacc*Yacc+Zacc*Zacc)-1;	

				//Prints out the acceleration in collums    |total |X acc |Y acc |Z acc |
				printf("\nTotal Acc: %.3f \tX: %.3f \tY: %.3f \tZ: %.3f\tDEBUG:%i",A, Xacc, Yacc, Zacc, fall_alert_detected);	 
				
				if (fallDetectionAlgorithm()==0){
					return 0;
				}
				//we have to free the payload's memory sble has allocated
				sble_payload_free_whole(&pl);
			}

		}	
	}




	//never reached
	return 0;
}

