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



//sensitivity multiplier: The output of the Accelerometer is multiplied with this to
//produce the right value in g (earth accelleration)
#define LIS302DL_SENSITIVITY_MULTIPLIER_SMALL_SCALE 0.018
#define LIS302DL_SENSITIVITY_MULTIPLIER_FULL_SCALE 0.072
#define LIS302DL_SENSITIVITY_MULTIPLIER LIS302DL_SENSITIVITY_MULTIPLIER_SMALL_SCALE
#define LSM9DS0_SENSITIVITY_MULTIPLIER_2G 0.000030518
#define LSM9DS0_SENSITIVITY_MULTIPLIER LSM9DS0_SENSITIVITY_MULTIPLIER_2G
#define STREAMINGTHREAD_LASTACC_BUFFER_LENGTH 10

streamingThread::streamingThread() :
sema(1)
{
	sema.acquire();
	terminate = false;
	streamingState = STATE_IDLE;
	mwi = NULL;
	activity = 0;
	sema.release();
	motDet = new processdata();
}
void streamingThread::stopStreaming(){
	QSemaphore shutDownSema(1);
	shutDownSema.acquire(1);
	cout<<"requsting streaming thread termination..."<<endl;
	sema.acquire();
	streamingState = STATE_DISCONNECTING;
	terminate = true;
	sema.release();
	sble_shutdown();
	shutDownSema.release(1);

	cout<<"waiting for streaming thread to terminate..."<<endl;


}
void streamingThread::setMainWindowImpl(mainWindowImpl* mwi){
	sema.acquire();
	this->mwi = mwi;
	sema.release();

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
	uint8_t char_read = 0;
	uint32_t errCnt = 0;


	dtp_payload_rssi *rssi;
	dtp_frame* frame;

	sble_init((uint8_t*)mwi->getDeviceNode());
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
	streamingState = STATE_CONNECTING;
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
	streamingState = STATE_STREAMING;






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
			streamingState = STATE_IDLE;

			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("disconnection established - reconnecting...");
			goto e1ch_connect;
		}
		dstate.flags &= ~(SBLE_STATE_ATTRIBUTE_VALUE_EVENT | SBLE_STATE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

		sble_scheduler_unlock_mutex();

		while(!sble_ll_isempty(dstate.cons[ccon]->ll_rcvqueue)){
			SBLE_DEBUG("GATT value Event\n");
			pl = (sble_payload*) sble_ll_pop_from_begin(dstate.cons[ccon]->ll_rcvqueue);

			clock_gettime(CLOCK_MONOTONIC,&tEnd);
			dataPeriod =(double)tEnd.tv_sec + (double) tEnd.tv_nsec / 1000000000.0 - (double) tStart.tv_sec - (double) tStart.tv_nsec / 1000000000.0;
			tStart = tEnd;
			if((pl != NULL)&&(pl->data != NULL) && (pl->data->data != NULL)){
				//get local RSSI
				sble_call_bl(ble_cmd_connection_get_rssi_idx,0);
				sble_scheduler_lock_mutex();
				if(dstate.cons[0]->param != NULL){
					local_rssi = *((int8_t*)dstate.cons[0]->param);
					free(dstate.cons[0]->param);
					dstate.cons[0]->param = NULL;
				}

				sble_scheduler_unlock_mutex();

				//--
				uint32_t nValues = pl->data->data[1];
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
					addToBuf((float) x * LSM9DS0_SENSITIVITY_MULTIPLIER,(float)y * LSM9DS0_SENSITIVITY_MULTIPLIER,(float)z * LSM9DS0_SENSITIVITY_MULTIPLIER);

				}


				sble_payload_free_whole(&pl);
				pl = NULL;
			}else{
				SBLE_DEBUG("No Payload received.");
			}
		}
		sema.acquire();

		if(terminate){
			streamingState = STATE_DISCONNECTING;
			sema.release();
			SBLE_DEBUG("BLE streaming thread: Termination request received.")
			return;
		}
		sema.release();

	}
	sble_io_disconnect();
	cout<<"shutdown sequence terminated.\n"<<endl;
	terminate = false;
	return;







}

void streamingThread::bleRawStreaming(){
	cout<<"Starting Bluetooth Low Energy streaming (Raw-Accel-6 via GATT)"<<endl;

	struct timespec tStart, tEnd;




	sema.acquire();
	terminate = false;
	streamingState = STATE_CONNECTING;
	uint8_t* rv = (uint8_t*)mwi->getDeviceNode();
	sema.release();

	sble_init(rv);
	if(terminate){
		cout<<"Terminating..."<<endl;
		return;
	}


	connect:
	SBLE_DEBUG("Connecting to any node in range...");
	sema.acquire();
	streamingState = STATE_CONNECTING;
	if(terminate){
		sema.release();
		cout<<"Terminating..."<<endl;
		return;
	}
	sema.release();
	SBLE_DEBUG("connecting...");
	sble_connect_to_any(6,6,600,0);
	SBLE_DEBUG("connection established!");

	//	sble_make_connectable_by_any(40,100);
	sema.acquire();
	streamingState = STATE_STREAMING;

	if(terminate){
		streamingState = STATE_DISCONNECTING;
		sema.release();
		cout<<"Terminating..."<<endl;
		return;
	}
	sema.release();
	sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);
	sema.acquire();
	if(terminate){
		streamingState = STATE_DISCONNECTING;
		sema.release();
		cout<<"Terminating..."<<endl;
		return;
	}
	sema.release();



	clock_gettime(CLOCK_MONOTONIC,&tStart);
	usleep(300);

	int32_t local_rssi;
	uint8_t char_read = 0;


	sble_payload* pl = NULL;
	while(1){
		//now wait for a client writing to the GATT server via ATT
		//SBLE_DEBUG("waiting for incoming transfer.");
		sble_scheduler_wait_for_event_no_reset(SBLE_THREAD_MAIN,SBLE_STATE_GATT_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);

		sble_scheduler_lock_mutex();
		if(dstate.flags & SBLE_STATE_GATT_VALUE_EVENT){
			dstate.flags &= ~(SBLE_STATE_GATT_VALUE_EVENT);
			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("GATT value Event\n");
			pl = (sble_payload*) sble_ll_pop_from_begin(dstate.ll_gatt_values);
			sble_scheduler_lock_mutex();

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
		if((pl != NULL)&&(pl->data != NULL) && (pl->data->data != NULL)){
			//get local RSSI
			sble_call_bl(ble_cmd_connection_get_rssi_idx,0);
			sble_scheduler_lock_mutex();
			if(dstate.cons[0]->param != NULL){
				local_rssi = *((int8_t*)dstate.cons[0]->param);
				free(dstate.cons[0]->param);
				dstate.cons[0]->param = NULL;
			}

			sble_scheduler_unlock_mutex();





			for(uint32_t cnt = 0; cnt < 6; cnt++){
				int8_t x = (int8_t) pl->data->data[3* cnt];
				int8_t y = (int8_t) pl->data->data[3* cnt + 1];
				int8_t z = (int8_t) pl->data->data[3* cnt + 2];
				float aX = LIS302DL_SENSITIVITY_MULTIPLIER * (float) x;
				float aY = LIS302DL_SENSITIVITY_MULTIPLIER * (float) y;
				float aZ = LIS302DL_SENSITIVITY_MULTIPLIER * (float) z;
				//	cout<<"Acceleration (RAW): ("<<(int32_t) x<<","<<(int32_t) y<<","<<(int32_t) z<<") - Phy: ("<<aX<<","<<aY<<","<<aZ<<")"<<endl;

				addToBuf(aX,aY,aZ);
			}
			sble_payload_free_whole(&pl);
			pl = NULL;
		}else{
			SBLE_DEBUG("No Payload received.");
		}
		sema.acquire();

		if(terminate){
			streamingState = STATE_DISCONNECTING;
			sema.release();
			SBLE_DEBUG("BLE streaming thread: Termination request received.")
			return;
		}
		sema.release();

	}
	if(sble_io_is_connected()){
		sble_io_disconnect();
	}
	return;

}
void streamingThread::bleStreaming(){
	cout<<"Starting Bluetooth Low Energy streaming (RCS-DTP via GATT)"<<endl;

	struct timespec tStart, tEnd;
	dtp_payload_acceleration *acc;
	libHDRHistory<double> histRssi(50);



	dtp_payload_rssi *rssi;
	dtp_frame* frame;

	sema.acquire();
	terminate = false;
	streamingState = STATE_CONNECTING;
	uint8_t* rv = (uint8_t*)mwi->getDeviceNode();
	sema.release();

	sble_init(rv);
	if(terminate){
		cout<<"Terminating..."<<endl;
		return;
	}


	connect:
	SBLE_DEBUG("Connecting to any node in range...");
	sema.acquire();
	streamingState = STATE_CONNECTING;
	if(terminate){
		sema.release();
		cout<<"Terminating..."<<endl;
		return;
	}
	sema.release();

	sble_connect_to_any(6,6,60,0);
	streamingState = STATE_STREAMING;

	sema.acquire();
	if(terminate){
		streamingState = STATE_DISCONNECTING;
		sema.release();
		cout<<"Terminating..."<<endl;
		return;
	}
	sema.release();
	SBLE_DEBUG("connection established!");
	sble_scheduler_autoclear_prevent(SBLE_STATE_GATT_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);
	sema.acquire();
	if(terminate){
		streamingState = STATE_DISCONNECTING;
		sema.release();
		cout<<"Terminating..."<<endl;
		return;
	}
	sema.release();



	clock_gettime(CLOCK_MONOTONIC,&tStart);
	usleep(300);

	int32_t local_rssi;
	uint8_t char_read = 0;


	sble_payload* pl = NULL;
	while(1){
		acc = NULL;
		rssi = NULL;
		//now wait for a client writing to the GATT server via ATT
		//SBLE_DEBUG("waiting for incoming transfer.");
		sble_scheduler_wait_for_event_no_reset(SBLE_THREAD_MAIN,SBLE_STATE_GATT_VALUE_EVENT|SBLE_STATE_DISCONNECT_EVENT);
		sble_scheduler_lock_mutex();
		if(dstate.flags & SBLE_STATE_GATT_VALUE_EVENT){
			dstate.flags &= ~(SBLE_STATE_GATT_VALUE_EVENT);
			sble_scheduler_unlock_mutex();
			SBLE_DEBUG("GATT value Event\n");
			pl = (sble_payload*) sble_ll_pop_from_begin(dstate.ll_gatt_values);
			sble_scheduler_lock_mutex();

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
		if((pl != NULL)&&(pl->data != NULL) && (pl->data->data != NULL)){
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
				/*
				mwi->dAx->addData(acc->aX);
				mwi->dAy->addData(acc->aY);
				mwi->dAz->addData(acc->aZ);
				mwi->sX.addData(acc->aX);
				mwi->sY.addData(acc->aY);
				mwi->sZ.addData(acc->aZ);


				 */
				sema.acquire();
				histRssi.add(rssi->rssi);
				sema.release();

				/*
				mwi->dRSSI->addData(-200 - 3 * rssi->rssi);
				mwi->sRSSI.addData(rssi->rssi);
				 */
			}else{
				SBLE_DEBUG("\n!Warn - eiter no acc data or RSSI available!\n");

			}
			if(acc != NULL){
				addToBuf(acc->aX,acc->aY, acc->aZ);
			}
			if((acc != NULL)||(rssi != NULL)){

				//mwi->packetsReceived++;
			}

			dtp_frame_destroy(frame,1,0);
			sble_payload_free_whole(&pl);
			pl = NULL;
		}else{
			SBLE_DEBUG("No Payload received.");
		}
		sema.acquire();

		if(terminate){
			streamingState = STATE_DISCONNECTING;
			sema.release();
			SBLE_DEBUG("BLE streaming thread: Termination request received.")
			return;
		}
		sema.release();

	}
	if(sble_io_is_connected()){
		sble_io_disconnect();
	}
	return;

}


void streamingThread::performActivityDetection(QList<float> aX,QList<float> aY,QList<float> aZ){



	QList<float> laX, laY, laZ;
	streamingThread::streamingState_t state = getStreamingState();

	if(state == streamingThread::STATE_STREAMING){
		/*
		aX = sThread->getAx();
		aY = sThread->getAy();
		aZ = sThread->getAz();
		 */
		sema.acquire();
		laX = aX;
		laY = aY;
		laZ = aZ;
		aX.clear();
		aY.clear();
		aZ.clear();
		sema.release();


		if(mwi->getCbActivityDetection()->isChecked()){
			activity = motDet->startDetection(&laX,&laY,&laZ);
			/*
			if(motDet.getWalking().length() > 0){
				cout<<"Walking list: "<<motDet.getWalking().at(0)<<endl;
			}
			if(motDet.getRunning().length() > 0){
				cout<<"Running list: "<<motDet.getRunning().at(0)<<endl;
			}
			cout<<"StepsWalking: "<<motDet.getStepsW().length()<<" - StepsRunning: "<<motDet.getStepsR().length()<<endl;;
			 */
		}

	}
}
void streamingThread::acmStreaming(){
	cout<<"Starting Abstract Control Mode (ACM) streaming..."<<endl;




	struct timespec tStart, tEnd;
	dtp_frame* frame;
	dtp_payload_acceleration *acc;
	dtp_payload_rssi *rssi;
	int32_t bytesRead;
	uint8_t inbuf[1000];
	clock_gettime(CLOCK_MONOTONIC,&tStart);
	char buf[256];
	sema.acquire();
	sprintf(buf,"stty -F %s 1:0:1cb7:0:3:1c:7f:15:1:1:3c:0:11:13:1a:0:12:f:17:16:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0",mwi->getDeviceNode());
	sema.release();
	system(buf);
	sema.acquire();
	fd = open(mwi->getDeviceNode(), O_RDONLY|O_SYNC|O_NOCTTY);
	sema.release();

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
			cout<<"Accelleration ("<<acc->aX<<","<<acc->aY<<","<<acc->aZ<<" received "<<endl;

		}else{
			acc = NULL;
			cout<<"Accelleration not received..."<<endl;
		}

		if(dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_RSSI) != NULL){
			rssi = ((dtp_payload_rssi*) dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_RSSI)->data);
		}else{
			rssi = NULL;
		}
		if(acc != NULL){
			addToBuf(acc->aX,acc->aY, acc->aZ);
		}

		if(rssi != NULL){
			/*
			mwi->dRSSI->addData(-1 * rssi->rssi);
			mwi->sRSSI.addData(rssi->rssi);
			 */
		}
		//			printf("acceleration: %d %d %d, bytesRead: %d\n", acc->aX, acc->aY, acc->aZ,bytesRead);
		dtp_frame_destroy(frame, 1, 1);

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
	sema.acquire();
	if(mwi->getProtocol() == mainWindowImpl::PROTOCOL_ACM){
		sema.release();
		acmStreaming();
	}else if(mwi->getProtocol() == mainWindowImpl::PROTOCOL_BLE){
		sema.release();
		bleStreaming();
	}else if(mwi->getProtocol() == mainWindowImpl::PROTOCOL_BLE_RAW){
		sema.release();
		bleRawStreaming();
	}else if(mwi->getProtocol() == mainWindowImpl::PROTOCOL_BLE_E1CH){
		sema.release();
		e1chStreaming();

	}else{
		sema.release();
	}
	terminate = false;
	printf("Streaming thread terminating...\n");
	this->thread()->quit();
	streamingState = STATE_IDLE;

};

double streamingThread::getDataPeriod(){
	sema.acquire();
	double rv = dataPeriod;
	sema.release();
	return rv;
};


streamingThread::streamingState_t streamingThread::getStreamingState(){
	streamingState_t rv;
	sema.acquire();
	rv = streamingState;
	sema.release();
	return rv;
}
char* streamingThread::getRemoteAddress(){
	char* rv;
	sema.acquire();
	if(streamingState == STATE_IDLE){
		sema.release();
		return NULL;
	}

	rv = sble_type_conversion_buf_to_hexstring(dstate.cons[dstate.current_con]->addr,6);
	sema.release();

	return rv;
}


void streamingThread::addToBuf(float aX, float aY, float aZ){
	sema.acquire();
	if(mwi->getCbHe2mtActivateRaw()->isChecked()){
		mwi->getHe2mtSDIThread()->setNextAcceleration(aX,aY,aZ);
	}
	this->aX.append(aX);
	this->aY.append(aY);
	this->aZ.append(aZ);

	//keep max. lenght constant. Assumtion: all thre lists have the same length
	if(this->aX.length() > 64){
		sema.release();
		performActivityDetection(this->aX,this->aY,this->aZ);
		sema.acquire();
		this->aX.clear();
		this->aY.clear();
		this->aZ.clear();

	}
	//remove last element if there is a buffer overrun at the last* - buffers
	if(this->lastAx.length() >= STREAMINGTHREAD_LASTACC_BUFFER_LENGTH){
		if(lastAx.length() > 0){
			lastAx.removeLast();
		}
		if(lastAy.length() > 0){
			lastAy.removeLast();
		}
		if(lastAz.length() > 0){
			lastAz.removeLast();
		}
	}
	lastAx.push_front(aX);
	lastAy.push_front(aY);
	lastAz.push_front(aZ);

	//cout<<"lenght = "<<this->aX.length()<<" first: "<<this->aX.first()<<" - last: "<<this->aX.last()<<endl;

	sema.release();

}

QList<float>* streamingThread::getLastAx(){
	return &lastAx;
}
QList<float>* streamingThread::getLastAy(){
	return &lastAy;
}
QList<float>* streamingThread::getLastAz(){

	return &lastAz;
}

QList<float>* streamingThread::getAx(){
	QList<float>* copy = new QList<float>;
	sema.acquire();
	*copy = aX;
	sema.release();
	return copy;
}


QList<float>* streamingThread::getAy(){
	QList<float>* copy = new QList<float>;
	sema.acquire();
	*copy = aY;
	sema.release();
	return copy;
}

QList<float>* streamingThread::getAz(){
	QList<float>* copy = new QList<float>;
	sema.acquire();
	*copy = aZ;
	sema.release();
	return copy;
}

uint32_t streamingThread::getActivity(){
	uint32_t rv;
	sema.acquire();
	rv = activity;
	sema.release();
	return rv;
}

processdata* streamingThread::getMotDet(){
	return motDet;
}


void streamingThread::acquireSema(){
	sema.acquire();
}

void streamingThread::releaseSema(){
	sema.release();
}

streamingThread::~streamingThread(){

}
