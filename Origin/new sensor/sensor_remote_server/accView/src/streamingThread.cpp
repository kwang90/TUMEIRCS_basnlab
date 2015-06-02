/*
 * streamingThread.cpp
 *
 *  Created on: 14.03.2013
 *      Author: kindt
 */

#include "streamingThread.h"
#include "mainWindowImpl.h"
#include <QObject>
#include <QThread>

#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <data_transmission_protocol/dtp.h>
#include <data_transmission_protocol/payloads/dtp_payload_acceleration.h>
#include <data_transmission_protocol/payloads/dtp_payload_rssi.h>
#include <stdio.h>
#include <sble.h>
#include <sys/time.h>
#include "libHDRHistory.h"
#include <external/iir/iir.h>
#include <rcs_dsp/rcs_dsp_iir.h>


//sensitivity multiplier: The output of the Accelerometer is multiplied with this to
//produce the right value in g (earth accelleration)
#define LIS302DL_SENSITIVITY_MULTIPLIER_SMALL_SCALE 0.018
#define LIS302DL_SENSITIVITY_MULTIPLIER_FULL_SCALE 0.072
#define LIS302DL_SENSITIVITY_MULTIPLIER LIS302DL_SENSITIVITY_MULTIPLIER_SMALL_SCALE
#define LSM9DS0_SENSITIVITY_MULTIPLIER_2G 0.000030518
#define LSM9DS0_SENSITIVITY_MULTIPLIER LSM9DS0_SENSITIVITY_MULTIPLIER_2G
#define RSSI_SMOOTHING_ALPHA 0.01
streamingThread::streamingThread() :
sema(1)
{
	terminate = false;
	mwi = NULL;
	recentRSSI = 0;
}
void streamingThread::stopStreaming(){
	terminate = true;
}
void streamingThread::setMainWindowImpl(MainWindowImpl* mwi){
	this->mwi = mwi;
}

void streamingThread::bleStreaming(){
	cout<<"Starting Bluetooth Low Energy streaming (RCS-DTP via GATT)"<<endl;
	struct timespec tStart, tEnd;
	dtp_payload_acceleration *acc;
	libHDRHistory<double> histRssi(50);



	dtp_payload_rssi *rssi;
	dtp_frame* frame;

	sble_init((uint8_t*)mwi->getDevNode());
	connect:
	SBLE_DEBUG("Connecting to any node in range...");
	sble_connect_to_any(6,6,60,0);
	SBLE_DEBUG("connection established!");
	sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);



	clock_gettime(CLOCK_MONOTONIC,&tStart);
	usleep(300);

	int32_t local_rssi;
	uint8_t char_read = 0;


	sble_payload* pl;
	while(1){
		acc = NULL;
		rssi = NULL;
		//now wait for a client writing to the GATT server via ATT
		//SBLE_DEBUG("waiting for incoming transfer.");
		sble_scheduler_wait_for_event_no_reset(SBLE_THREAD_MAIN,SBLE_STATE_GATT_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

		sble_scheduler_lock_mutex();
		if(dstate.flags & SBLE_STATE_GATT_VALUE_EVENT){
			dstate.flags &= ~(SBLE_STATE_GATT_VALUE_EVENT);
			SBLE_DEBUG("GATT value Event\n");
			pl = (sble_payload*) sble_ll_pop_from_begin(dstate.ll_gatt_values);
		}
		if(dstate.flags & SBLE_STATE_DISCONNECT_EVENT){
			dstate.flags &= ~(SBLE_STATE_DISCONNECT_EVENT);
			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("disconnection established - reconnecting...");
			goto connect;
		}
		dstate.flags &= ~(SBLE_STATE_GATT_VALUE_EVENT | SBLE_STATE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

		sble_scheduler_unlock_mutex();


		clock_gettime(CLOCK_MONOTONIC,&tEnd);
		dataPeriod =(double)tEnd.tv_sec + (double) tEnd.tv_nsec / 1000000000.0 - (double) tStart.tv_sec - (double) tStart.tv_nsec / 1000000000.0;
		tStart = tEnd;
		if(pl != NULL){
			//SBLE_DEBUG_CON("read from remote node %d bytes: ",pl->data->len);
			//acc[0] = pl->data->data[3]<<24 | pl->data->data[2]<<16| pl->data->data[1]<<8|pl->data->data[0];
			//acc[1] = pl->data->data[7]<<24 | pl->data->data[6]<<16| pl->data->data[5]<<8|pl->data->data[4];
			//acc[2] = pl->data->data[11]<<24 | pl->data->data[10]<<16| pl->data->data[9]<<8|pl->data->data[8];
			//printf("accelleration: (%d, %d, %d)\n",acc[0],acc[1],acc[2]);
			frame = dtp_frame_generate(pl->data->data);
			if(frame != NULL){
				if(dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_ACCELERATION) != NULL){
					acc = ((dtp_payload_acceleration*) dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_ACCELERATION)->data);
				}else{
					acc = NULL;
				}

				if(dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_RSSI) != NULL){
					rssi = ((dtp_payload_rssi*) dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_RSSI)->data);
				}else{
					SBLE_DEBUG("No RSSI received from remote!")
								rssi = NULL;
				}

			}
			//get local RSSI
			sble_call_bl(ble_cmd_connection_get_rssi_idx,0);
			sble_scheduler_lock_mutex();
			if(dstate.cons[0]->param != NULL){
				local_rssi = *((int8_t*)dstate.cons[0]->param);
				free(dstate.cons[0]->param);
				dstate.cons[0]->param = NULL;
			}

			sble_scheduler_unlock_mutex();



			if((acc != NULL)&&(rssi != NULL)){
				lockSema();
				mwi->dAx->addData(acc->aX);
				mwi->dAy->addData(acc->aY);
				mwi->dAz->addData(acc->aZ);
				mwi->sX.addData(acc->aX);
				mwi->sY.addData(acc->aY);
				mwi->sZ.addData(acc->aZ);

				//			histRssi.add(local_rssi);
				//			mwi->dRSSI->addData(-200 - 3 * local_rssi);
				//			mwi->sRSSI.addData(local_rssi - histRssi.getSimpleAverage(0,histRssi.getBiggestIndex()));


				histRssi.add(rssi->rssi);
				mwi->dRSSI->addData(-1 * (rssi->rssi));
				mwi->sRSSI.addData(rssi->rssi);
				releaseSema();
			}else{
				SBLE_DEBUG("\n!Warn - eiter no acc data or RSSI available!\n");

			}

			if((acc != NULL)||(rssi != NULL)){

				mwi->packetsReceived++;
			}

			dtp_frame_destroy(frame,1,0);
			sble_payload_free_whole(&pl);
		}else{
			SBLE_DEBUG("No Payload received.");
		}

		if(terminate){
			sble_io_disconnect();
			return;
		}

	}

	sble_io_disconnect();
	return;

}

void streamingThread::e1chStreaming(){

	cout<<"Starting Bluetooth Low Energy streaming to RCS E1CH"<<endl;
	dtp_payload_acceleration *acc;
	libHDRHistory<double> histRssi(50);
	struct timespec tStart, tEnd, tConnect;
	ble_msg_system_get_counters_rsp_t* cntrs;

	double errRate;
	double runTime;
	int32_t local_rssi;
	int32_t remote_rssi;
	double errorRate;
	uint8_t char_read = 0;
	uint32_t errCnt = 0;


	dtp_payload_rssi *rssi;
	dtp_frame* frame;

	sble_init((uint8_t*)mwi->getDevNode());
	uint16_t cycleCnt = 0;

	sble_array* data;
	sble_attribute* att_notify;
	sble_attribute* att_cfg;
	//payload to receive
	sble_payload* pl;

	uint32_t nPacketsInQueue;
	//define our attribute's UUIs. This would have been eaiser with ble_type_conversion_hexstring_to_binary(), but just to show you another way...
	uint8_t uuid_wr_notify[2] = {0x02,0x29};
	uint8_t uuid_wr_cfg[16] = {0x07,0x29,0x5d,0xf2,0xb0,0x5c,0x11,0xe4,0xad,0x44,0x3c,0x97,0x02,0xae,0x00,0x00};


	//connect to any node in range with minimal connection interval
	printf("waiting...\n");
	SBLE_DEBUG("Connecting to any node in range...");

	e1ch_connect:
	usleep(500000);
	sble_scheduler_lock_mutex();
	dstate.evt_clear_list &= ~(SBLE_STATE_SCAN_RESPONSE_EVENT);
	sble_scheduler_unlock_mutex();

	sble_call_bl(ble_cmd_gap_discover_idx,2);
	//printf("Waiting for scan response event...\n");
	sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_SCAN_RESPONSE_EVENT);

	//printf("Found remote device. Connecting...\n");
	SBLE_DEBUG("Found remote device. Connecting...");
	sble_scheduler_lock_mutex();

	dstate.evt_clear_list &= ~(SBLE_STATE_CONNECTION_EVENT);
	sble_scheduler_unlock_mutex();
	sble_print_hex_array(dstate.addr,16);
	sble_call_bl(ble_cmd_gap_connect_direct_idx,dstate.addr, gap_address_type_random, 7,7,90,10);
	//	sble_call_bl(ble_cmd_gap_connect_direct_idx,dstate.addr, gap_address_type_public, con_int_min,con_int_max,timeout,latency);
	sble_scheduler_wait_for_event(SBLE_THREAD_MAIN, SBLE_STATE_CONNECTION_EVENT);
	SBLE_DEBUG("Connected.");
	//	sble_call_bl(ble_cmd_gap_end_procedure_idx,245);

	uint8_t ccon;
	sble_scheduler_lock_mutex();
	ccon = dstate.current_con;
	sble_scheduler_unlock_mutex();

	/*
	sble_make_connectable_by_any(40,50);
	sble_scheduler_autoclear_prevent(SBLE_STATE_CONNECTION_EVENT);
	sble_scheduler_wait_for_event(SBLE_THREAD_MAIN,
				SBLE_STATE_CONNECTION_EVENT);
	 */
	SBLE_DEBUG("connection established!");






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
	usleep(500000);
	//	sble_attclient_write_by_handle(0,15,data);
	sble_attclient_write_by_attribute(0,att_notify,data);
	data->data[0] = 0x64;
	data->data[1] = 0x00;
	sble_attclient_write_by_attribute(0,att_cfg,data);


	//do it!
	sble_scheduler_autoclear_prevent(SBLE_STATE_ATTRIBUTE_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

	while(1){	
		sble_scheduler_wait_for_event_no_reset(SBLE_THREAD_MAIN,SBLE_STATE_ATTRIBUTE_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

		sble_scheduler_lock_mutex();
		if(dstate.flags & SBLE_STATE_ATTRIBUTE_VALUE_EVENT){
			dstate.flags &= ~(SBLE_STATE_ATTRIBUTE_VALUE_EVENT);
		}
		if(dstate.flags & SBLE_STATE_DISCONNECT_EVENT){
			dstate.flags &= ~(SBLE_STATE_ATTRIBUTE_VALUE_EVENT | SBLE_STATE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

			sble_ll_remove_all_elements(dstate.cons[0]->ll_rcvqueue);
			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("disconnection established - reconnecting...");
			goto e1ch_connect;
		}
		dstate.flags &= ~(SBLE_STATE_ATTRIBUTE_VALUE_EVENT | SBLE_STATE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

		sble_scheduler_unlock_mutex();


		clock_gettime(CLOCK_MONOTONIC,&tEnd);
		dataPeriod =(double)tEnd.tv_sec + (double) tEnd.tv_nsec / 1000000000.0 - (double) tStart.tv_sec - (double) tStart.tv_nsec / 1000000000.0;
		runTime =(double)tEnd.tv_sec + (double) tEnd.tv_nsec / 1000000000.0 - (double) tConnect.tv_sec - (double) tConnect.tv_nsec / 1000000000.0;

		tStart = tEnd;
		while(!sble_ll_isempty(dstate.cons[ccon]->ll_rcvqueue)){
			SBLE_DEBUG("GATT value Event\n");
			pl = (sble_payload*) sble_ll_pop_from_begin(dstate.cons[ccon]->ll_rcvqueue);


			if((pl != NULL)){
				printf("read from remote node %d bytes: \n",pl->data->len);
				lockSema();

				sble_call_bl(ble_cmd_connection_get_rssi_idx,ccon);
				sble_scheduler_lock_mutex();
				if(dstate.cons[ccon]->param != NULL){
					local_rssi = *((int8_t*)dstate.cons[ccon]->param);
					//cout<<"local RSSI is "<<(double) local_rssi<<endl;
					free(dstate.cons[ccon]->param);
					dstate.cons[ccon]->param = NULL;
				}else{
					local_rssi = 0;
				}
				releaseSema();
				sble_scheduler_unlock_mutex();

				lockSema();
				uint32_t nValues = pl->data->data[1];
				mwi->addTimeStamp(runTime,nValues);
				int16_t x = 0;
				int16_t y = 0;
				int16_t z = 0;
				for(uint32_t cnt = 0;  cnt < nValues; cnt++){
					x = (int16_t) pl->data->data[6* cnt + 2];
					x |= ((int16_t) pl->data->data[6* cnt + 3])<<8;

					y = (int16_t) pl->data->data[6* cnt + 4];
					y |= ((int16_t) pl->data->data[6* cnt + 5])<<8;
					z = (int16_t) pl->data->data[6* cnt + 6];
					z |= ((int16_t) pl->data->data[6* cnt + 7])<<8;

					//	cout<<"("<<(int32_t) x<<","<<(int32_t)y<<","<<(int32_t)z<<")"<<endl;
					mwi->dAx->addData(LSM9DS0_SENSITIVITY_MULTIPLIER * 1000.0 * (double) x);
					mwi->dAy->addData(LSM9DS0_SENSITIVITY_MULTIPLIER * 1000.0 * (double) y);
					mwi->dAz->addData(LSM9DS0_SENSITIVITY_MULTIPLIER * 1000.0 * (double) z);
					if(dataPeriod != 0){
						mwi->dGoodPut->addData(20.0/dataPeriod);
					}
					mwi->sX.addData(LSM9DS0_SENSITIVITY_MULTIPLIER *  (double) x);
					mwi->sY.addData(LSM9DS0_SENSITIVITY_MULTIPLIER  * (double) y);
					mwi->sZ.addData(LSM9DS0_SENSITIVITY_MULTIPLIER  * (double) z);
					mwi->dRSSI->addData((local_rssi + 70.0) * -1.0);
					recentRSSI = recentRSSI * (1.0-RSSI_SMOOTHING_ALPHA) + RSSI_SMOOTHING_ALPHA * (double) local_rssi;
					mwi->sRSSI.addData(local_rssi * -1);
					releaseSema();
				}
				mwi->addThroughPut(20.0/dataPeriod,nValues);

				//	cout<<"getting error rate..."<<endl;
				sble_call_bl(ble_cmd_system_get_counters_idx);
				//	cout<<"done."<<endl;

				sble_scheduler_lock_mutex();
				if(dstate.cons[0]->param != NULL){
					lockSema();
						cntrs = ((ble_msg_system_get_counters_rsp_t*)dstate.cons[0]->param);
			//
						mwi->addPacketStats(cntrs->rxok, cntrs->rxfail,nValues);
						for(uint32_t cnt = 0; cnt < nValues; cnt++){
							mwi->dErrRate->addData(errorRate*1000.0);
						}
						if(cntrs->rxfail + cntrs->rxok > 0){

								errorRate = (double) cntrs->rxfail/((double) cntrs->rxfail + (double) cntrs->rxok);
						}
			//			cout<<"--error Rate is "<<(double) errorRate<<"\t rxFail = "<<(uint32_t) cntrs->rxfail<<"\t rxOk = "<<(uint32_t) cntrs->rxok<<endl;
						free(dstate.cons[0]->param);
						dstate.cons[0]->param = NULL;
						releaseSema();

				}else{
					mwi->addPacketStats(0, 0,nValues);
					mwi->dErrRate->addData(0);

						errorRate = 0;
					}
				sble_scheduler_unlock_mutex();

			}else{
				SBLE_DEBUG("\n!Warn - either no acc data or RSSI available!\n");
			}
		}
		lockSema();
		mwi->packetsReceived += 1;
		releaseSema();
		sble_payload_free_whole(&pl);


		if(terminate){
			if(!mwi->getContErrorRate()){
				cout<<"getting error rate..."<<endl;
				sble_call_bl(ble_cmd_system_get_counters_idx);
				cout<<"done."<<endl;
				sble_scheduler_lock_mutex();
				if(dstate.cons[0]->param != NULL){

					cntrs = ((ble_msg_system_get_counters_rsp_t*)dstate.cons[0]->param);
					if(cntrs->rxfail + cntrs->rxok > 0){
						errorRate = (double) cntrs->rxfail/((double) cntrs->rxfail + (double) cntrs->rxok);
					}else{
						errorRate = 0;
					}
					cout<<"--error Rate is "<<(double) errorRate<<"\t rxFail = "<<(uint32_t) cntrs->rxfail<<"\t rxOk = "<<(uint32_t) cntrs->rxok<<endl;
					dstate.cons[0]->param = NULL;

					emit indicateErrorRate(QString().sprintf("Error rate has been %f with %d packets ok, %d packets failed.\n",errorRate,cntrs->rxok,cntrs->rxfail));
					sble_scheduler_unlock_mutex();
					free(dstate.cons[0]->param);
				}else{
					sble_scheduler_unlock_mutex();

				}
			}
			sble_io_disconnect();
			cout<<"shutdown sequence terminated.\n"<<endl;
			terminate = false;
			return;
		}
	}




	//never reached
	return;


}
void streamingThread::bleRawStreaming(){
	mwi->resetPacketStats();
	dataPeriod = 0;
	recentRSSI = 0;
	cout<<"Starting Bluetooth Low Energy streaming (Raw Accelerometer data streaming via GATT)"<<endl;
	struct timespec tStart, tEnd, tConnect;
	ble_msg_system_get_counters_rsp_t* cntrs;

	double errRate;
	double runTime;
	int32_t local_rssi;
	int32_t remote_rssi;
	double errorRate;
	uint8_t char_read = 0;
	uint32_t errCnt = 0;

	sble_init((uint8_t*)mwi->getDevNode());
	connect:
	SBLE_DEBUG("Connecting to any node in range...");
	sble_connect_to_any(6,6,100,0);
	SBLE_DEBUG("connection established!");
	sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);
	clock_gettime(CLOCK_MONOTONIC,&tConnect);



	//	cout<<"getting error rate..."<<endl;
	sble_call_bl(ble_cmd_system_get_counters_idx);
	//	cout<<"done."<<endl;

	sble_scheduler_lock_mutex();
	if(dstate.cons[0]->param != NULL){
		lockSema();
			cntrs = ((ble_msg_system_get_counters_rsp_t*)dstate.cons[0]->param);
//
			mwi->addPacketStats(cntrs->rxok, cntrs->rxfail,6);
			mwi->dErrRate->addData(errorRate*1000.0);
			mwi->dErrRate->addData(errorRate*1000.0);
			mwi->dErrRate->addData(errorRate*1000.0);
			mwi->dErrRate->addData(errorRate*1000.0);
			mwi->dErrRate->addData(errorRate*1000.0);
			mwi->dErrRate->addData(errorRate*1000.0);
//
			if(cntrs->rxfail + cntrs->rxok > 0){
					errorRate = (double) cntrs->rxfail/((double) cntrs->rxfail + (double) cntrs->rxok);
			}
//			cout<<"--error Rate is "<<(double) errorRate<<"\t rxFail = "<<(uint32_t) cntrs->rxfail<<"\t rxOk = "<<(uint32_t) cntrs->rxok<<endl;
			free(dstate.cons[0]->param);
			dstate.cons[0]->param = NULL;
			releaseSema();

	}else{
			errorRate = 0;
		}
	sble_scheduler_unlock_mutex();

	clock_gettime(CLOCK_MONOTONIC,&tStart);
	usleep(300);



	sble_payload* pl;
	while(1){
		//now wait for a client writing to the GATT server via ATT
		//SBLE_DEBUG("waiting for incoming transfer.");
		sble_scheduler_wait_for_event_no_reset(SBLE_THREAD_MAIN,SBLE_STATE_GATT_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

		sble_scheduler_lock_mutex();
		if(dstate.flags & SBLE_STATE_GATT_VALUE_EVENT){
			dstate.flags &= ~(SBLE_STATE_GATT_VALUE_EVENT);

			SBLE_DEBUG("GATT value Event\n");
			pl = (sble_payload*) sble_ll_pop_from_begin(dstate.ll_gatt_values);
		}
		if(dstate.flags & SBLE_STATE_DISCONNECT_EVENT){
			dstate.flags &= ~(SBLE_STATE_DISCONNECT_EVENT);
			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("disconnection established - reconnecting...");
			goto connect;
		}
		dstate.flags &= ~(SBLE_STATE_GATT_VALUE_EVENT | SBLE_STATE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

		sble_scheduler_unlock_mutex();


		clock_gettime(CLOCK_MONOTONIC,&tEnd);
		dataPeriod =(double)tEnd.tv_sec + (double) tEnd.tv_nsec / 1000000000.0 - (double) tStart.tv_sec - (double) tStart.tv_nsec / 1000000000.0;
		runTime =(double)tEnd.tv_sec + (double) tEnd.tv_nsec / 1000000000.0 - (double) tConnect.tv_sec - (double) tConnect.tv_nsec / 1000000000.0;

		tStart = tEnd;
		if((pl != NULL)){
			//SBLE_DEBUG_CON("read from remote node %d bytes: ",pl->data->len);
			//acc[0] = pl->data->data[3]<<24 | pl->data->data[2]<<16| pl->data->data[1]<<8|pl->data->data[0];
			//acc[1] = pl->data->data[7]<<24 | pl->data->data[6]<<16| pl->data->data[5]<<8|pl->data->data[4];
			//acc[2] = pl->data->data[11]<<24 | pl->data->data[10]<<16| pl->data->data[9]<<8|pl->data->data[8];
			//printf("accelleration: (%d, %d, %d)\n",acc[0],acc[1],acc[2]);
			//get local RSSI
			lockSema();

			sble_call_bl(ble_cmd_connection_get_rssi_idx,0);
			sble_scheduler_lock_mutex();
			if(dstate.cons[0]->param != NULL){
				local_rssi = *((int8_t*)dstate.cons[0]->param);
				//cout<<"local RSSI is "<<(double) local_rssi<<endl;
				free(dstate.cons[0]->param);
				dstate.cons[0]->param = NULL;
			}else{
				local_rssi = 0;
			}
			releaseSema();
			sble_scheduler_unlock_mutex();


			if(mwi->getContErrorRate()){
				//cout<<"getting error rate..."<<endl;
				sble_call_bl(ble_cmd_system_get_counters_idx);
				//cout<<"done."<<endl;
				sble_scheduler_lock_mutex();
				lockSema();
				if(dstate.cons[0]->param != NULL){
					cntrs = ((ble_msg_system_get_counters_rsp_t*)dstate.cons[0]->param);
						mwi->addPacketStats(cntrs->rxok, cntrs->rxfail,6);

						if(cntrs->rxfail + cntrs->rxok > 0){
						errorRate = (double) cntrs->rxfail/((double) cntrs->rxfail + (double) cntrs->rxok);
					}
				//	cout<<"--error Rate is "<<(double) errorRate<<"\t rxFail = "<<(uint32_t) cntrs->rxfail<<"\t rxOk = "<<(uint32_t) cntrs->rxok<<endl;
					free(dstate.cons[0]->param);
					dstate.cons[0]->param = NULL;
				}else{
					errorRate = 0;
				}
				releaseSema();
				sble_scheduler_unlock_mutex();
			}
			lockSema();

			if(dataPeriod != 0){
				mwi->addThroughPut(20.0/dataPeriod,6);
			}

			mwi->addTimeStamp(runTime,6);
			for(uint32_t cnt = 0;  cnt < 6; cnt++){
				int8_t x = (int8_t) pl->data->data[3* cnt];
				int8_t y = (int8_t) pl->data->data[3* cnt + 1];
				int8_t z = (int8_t) pl->data->data[3* cnt + 2];
				//	cout<<"("<<(int32_t) x<<","<<(int32_t)y<<","<<(int32_t)z<<")"<<endl;
				mwi->dAx->addData(LIS302DL_SENSITIVITY_MULTIPLIER * 1000.0 * (double) x);
				mwi->dAy->addData(LIS302DL_SENSITIVITY_MULTIPLIER * 1000.0 * (double) y);
				mwi->dAz->addData(LIS302DL_SENSITIVITY_MULTIPLIER * 1000.0 * (double) z);
				if(dataPeriod != 0){
					mwi->dGoodPut->addData(20.0/dataPeriod);
				}
				mwi->dErrRate->addData(errorRate*1000.0);
				mwi->sX.addData(LIS302DL_SENSITIVITY_MULTIPLIER *  (double) x);
				mwi->sY.addData(LIS302DL_SENSITIVITY_MULTIPLIER  * (double) y);
				mwi->sZ.addData(LIS302DL_SENSITIVITY_MULTIPLIER  * (double) z);
				mwi->dRSSI->addData((local_rssi + 70.0) * -1.0);
				recentRSSI = recentRSSI * (1.0-RSSI_SMOOTHING_ALPHA) + RSSI_SMOOTHING_ALPHA * (double) local_rssi;
				mwi->sRSSI.addData(local_rssi * -1);
				releaseSema();
			}


		}else{
			SBLE_DEBUG("\n!Warn - either no acc data or RSSI available!\n");
		}
		lockSema();
		mwi->packetsReceived += 1;
		releaseSema();
		sble_payload_free_whole(&pl);


		if(terminate){
			if(!mwi->getContErrorRate()){
				cout<<"getting error rate..."<<endl;
				sble_call_bl(ble_cmd_system_get_counters_idx);
				cout<<"done."<<endl;
				sble_scheduler_lock_mutex();
				if(dstate.cons[0]->param != NULL){

					cntrs = ((ble_msg_system_get_counters_rsp_t*)dstate.cons[0]->param);
					if(cntrs->rxfail + cntrs->rxok > 0){
						errorRate = (double) cntrs->rxfail/((double) cntrs->rxfail + (double) cntrs->rxok);
					}else{
						errorRate = 0;
					}
					cout<<"--error Rate is "<<(double) errorRate<<"\t rxFail = "<<(uint32_t) cntrs->rxfail<<"\t rxOk = "<<(uint32_t) cntrs->rxok<<endl;
					dstate.cons[0]->param = NULL;

					emit indicateErrorRate(QString().sprintf("Error rate has been %f with %d packets ok, %d packets failed.\n",errorRate,cntrs->rxok,cntrs->rxfail));
					sble_scheduler_unlock_mutex();
					free(dstate.cons[0]->param);
				}else{
					sble_scheduler_unlock_mutex();

				}
			}
			sble_io_disconnect();
			cout<<"shutdown sequence terminated.\n"<<endl;
			terminate = false;
			return;
		}

	}

	sble_io_disconnect();
	return;

}


void streamingThread::acmStreaming(){
	cout<<"Starting Abstract Control Mode (ACM) streaming..."<<endl;


	double coeff_Fwd[21];
	double* coeff_Backwd = dcof_bwlp(20,0.25);
	int* coeff_FwdTmp = ccof_bwlp(20);
	double scale = sf_bwlp(20,0.25);
	uint32_t ccnt;
	for(ccnt = 0; ccnt < 21; ccnt++){
		coeff_Fwd[ccnt] = ((double)coeff_FwdTmp[ccnt]) * scale;
		cout<<"Coeff fwd["<<ccnt<<"]="<<coeff_Fwd[ccnt]<<"     backwd["<<ccnt<<"]="<<coeff_Backwd[ccnt]<<endl;
	}
	coeff_Backwd[0] = 0.0;
	rcs_dsp_iir fiir;
	rcs_dsp_iir_init(&fiir);
	rcs_dsp_iir_setFeedForward(&fiir, 21,  coeff_Fwd);
	rcs_dsp_iir_setFeedBack(&fiir, 21, coeff_Backwd);



	struct timespec tStart, tEnd;
	dtp_frame* frame;
	dtp_payload_acceleration *acc;
	dtp_payload_rssi *rssi;
	int32_t bytesRead;
	uint8_t inbuf[1000];
	clock_gettime(CLOCK_MONOTONIC,&tStart);
	char buf[256];
	sprintf(buf,"stty -F %s 1:0:1cb7:0:3:1c:7f:15:1:1:3c:0:11:13:1a:0:12:f:17:16:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0",mwi->getDeviceNode());
	system(buf);
	fd = open(mwi->getDevNode(), O_RDONLY|O_SYNC|O_NOCTTY);
	if(fd < 0){
		cout<<"invalid device node: "<<mwi->getDeviceNode()<<".Streaming thread terminating..."<<endl;
		this->thread()->quit();
		return;
	}
	terminate = false;
	uint32_t bytesProcessed = 0;
	while ((frame = dtp_frame_generate_from_filedescriptor(fd)) != NULL) {
		clock_gettime(CLOCK_MONOTONIC,&tEnd);
		dataPeriod =(double)tEnd.tv_sec + (double) tEnd.tv_nsec / 1000000000.0 - (double) tStart.tv_sec - (double) tStart.tv_nsec / 1000000000.0;
		tStart = tEnd;

		if(dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_ACCELERATION) != NULL){
			acc = ((dtp_payload_acceleration*) dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_ACCELERATION)->data);
			//	acc->aX = rcs_dsp_iir_apply(&fiir, acc->aX);
			//	acc->aX = acc->aX;

		}else{
			acc = NULL;
		}

		if(dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_RSSI) != NULL){
			rssi = ((dtp_payload_rssi*) dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_RSSI)->data);
		}else{
			rssi = NULL;
		}
		if(acc != NULL){
			lockSema();
			mwi->dAx->addData(acc->aX);
			mwi->dAy->addData(acc->aY);
			mwi->dAz->addData(acc->aZ);
			mwi->sX.addData(acc->aX);
			mwi->sY.addData(acc->aY);
			mwi->sZ.addData(acc->aZ);
			releaseSema();
		}

		if(rssi != NULL){
			mwi->dRSSI->addData(-1 * rssi->rssi);
			mwi->sRSSI.addData(rssi->rssi);
		}
		//			printf("acceleration: %d %d %d, bytesRead: %d\n", acc->aX, acc->aY, acc->aZ,bytesRead);
		dtp_frame_destroy(frame, 1, 1);

		if (terminate) {
			break;
		}
	}
	close(fd);
}



void streamingThread::usbRawStreaming(){
	cout<<"Starting USB Raw streaming..."<<endl;




	struct timespec tStart, tEnd;
	int32_t bytesRead;
	uint8_t inbuf[1000];
	clock_gettime(CLOCK_MONOTONIC,&tStart);
	char buf[256];
	sprintf(buf,"stty -F %s 1:0:1cb7:0:3:1c:7f:15:1:1:3c:0:11:13:1a:0:12:f:17:16:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0",mwi->getDeviceNode());
	system(buf);
	fd = open(mwi->getDevNode(), O_RDONLY|O_SYNC|O_NOCTTY);
	if(fd < 0){
		cout<<"invalid device node: "<<mwi->getDeviceNode()<<".Streaming thread terminating..."<<endl;
		this->thread()->quit();
		return;
	}
	terminate = false;
	uint32_t bytesProcessed = 0;
	uint32_t nBytesRead;
	double acc[20];
	while ((nBytesRead = read(fd,acc,sizeof(double)*3)) != NULL) {
		clock_gettime(CLOCK_MONOTONIC,&tEnd);
		printf("%f,%f,%f\n",acc[0],acc[1],acc[2]);
		dataPeriod =(double)tEnd.tv_sec + (double) tEnd.tv_nsec / 1000000000.0 - (double) tStart.tv_sec - (double) tStart.tv_nsec / 1000000000.0;
		tStart = tEnd;
		lockSema();
		mwi->dAx->addData(acc[0]*1000.0);
		mwi->dAy->addData(acc[1]*1000.0);
		mwi->dAz->addData(acc[2]*1000.0);
		mwi->sX.addData(acc[0]*1000.0);
		mwi->sY.addData(acc[1]*1000.0);
		mwi->sZ.addData(acc[2]*1000.0);
		releaseSema();

		if (terminate) {
			break;
		}
	}
	close(fd);
}

void streamingThread::doStreaming() {
	if(mwi == NULL){
		cout<<"mwi not set. terminating"<<endl;
		exit(1);
	}

	char cmdbuf[255];
	sprintf(cmdbuf,"stty -F %s 1:0:cbd:0:3:1c:7f:15:1:1:3c:0:11:13:1a:0:12:f:17:16:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0",mwi->getDeviceNode());
	printf("configuring device node: %s\n",cmdbuf);
	system(cmdbuf);
	if(mwi->getProtocol() == MainWindowImpl::PROTOCOL_ACM){
		acmStreaming();
	}
	if(mwi->getProtocol() == MainWindowImpl::PROTOCOL_BLE){
		bleStreaming();
	}
	if(mwi->getProtocol() == MainWindowImpl::PROTOCOL_BLE_RAW){
		bleRawStreaming();
	}
	if(mwi->getProtocol() == MainWindowImpl::PROTOCOL_USB_RAW){
		usbRawStreaming();
	}
	if(mwi->getProtocol() == MainWindowImpl::PROTOCOL_BLE_E1CH){
		e1chStreaming();
	}

	printf("Streaming thread terminating...\n");
	this->thread()->quit();
};

double streamingThread::getDataPeriod(){
	return dataPeriod;
};
double streamingThread::getRecentRSSI(){
	return recentRSSI;
};

void streamingThread::lockSema(){
	sema.acquire();
}

void streamingThread::releaseSema(){
	sema.release();
}


