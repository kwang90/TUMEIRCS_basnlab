/*
 * ble_model_connection_establishment.c
 *
 * (c) 2013, Philipp Kindt
 * (c) 2013, Lehrstuhl für Realzeit-Computersysteme (RCS), Techinsche Universität München (TUM)
 *
 *	This file is part of bleemod.
 *
 *   bleemod is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   bleemod is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with bleemod.  If not, see <http://www.gnu.org/licenses/>

 */

#include "ble_model_connected.h"
#include "ble_model_scanning.h"
#include "ble_model_connection_establishment.h"
#include "ble_model_params_general.h"
#include "ble_model_params_connected.h"
#include "ble_model_params.h"
#include "ble_model_connected.h"

double ble_e_model_ce_getChargeForConnectionProcedure(uint8_t establishmentOrUpdate, ble_e_model_sc_scan_type scanType, uint8_t masterOrSlave, double TcOld, double TcNew, ble_e_model_ce_updateEventType eventType){
	double charge = 0;
	double durationEvent = 0;
	if(masterOrSlave){
		//master
		if(establishmentOrUpdate){
			//connection establishment procedure

			//event / packet
			charge = ble_e_model_sc_getChargeScanEvent(0,SC_EVENT_TYPE_CON_REQ_OFFSET,scanType, BLE_E_MOD_CE_ADV_IND_PKG_LEN,BLE_E_MOD_CE_CON_REQ_LEN,0,0);

			//sleep duratin for dtwo + dp + 1,25 ms
			charge = charge  + (0.00125 + BLE_E_MOD_CE_DTWO_CR(TcNew) +  BLE_E_MOD_CE_DP(TcNew)) *  BLE_E_MOD_G_ISL;
		}else{
			//connection update procedure

			//packet / event
			if(eventType == BLE_E_MODEL_CE_EVENT_TYPE_EMBEDDED){
				//the connection update packet is piggied-back on a regular connection update as pair of packets

//				durationEvent  = BLE_E_MOD_C_DRXTX + BLE_E_MOD_C_DPRETX + BLE_E_MOD_CE_CON_UP_LEN * 1e-6 + BLE_E_MOD_C_DTXRX + BLE_E_MOD_C_DPRERX + BLE_E_MOD_CE_CON_UP_SLRSP_LEN * 1e-6;
				charge  = BLE_E_MOD_C_DRXTX * BLE_E_MOD_C_IRXTX + (BLE_E_MOD_C_DPRETX + BLE_E_MOD_CE_CON_UP_LEN * 1e-6)*BLE_E_MOD_C_ITX + BLE_E_MOD_C_DTXRX * BLE_E_MOD_C_ITXRX + (BLE_E_MOD_C_DPRERX + BLE_E_MOD_CE_CON_UP_SLRSP_LEN * 1e-6) * BLE_E_MOD_C_IRX;

			}else if(eventType == BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY){
				//the update event replaces an empty event => 22by update packet instead of 10 byte empty packet
				charge  = ((BLE_E_MOD_CE_CON_UP_LEN - BLE_E_MOD_CE_EMPTY_PACKET_LEN)* 1e-6)*BLE_E_MOD_C_ITX;

			}else if(eventType == BLE_E_MODEL_CE_EVENT_TYPE_FULL){
				//a full, complete connection event contributes to the energy consumption of the connection parameter update
				charge = ble_e_model_c_getChargeEventSamePayload(1,0,1,BLE_E_MOD_CE_CON_UP_SLRSP_LEN,BLE_E_MOD_CE_CON_UP_LEN,BLE_E_MOD_CE_CON_UP_TXPOWER,-1);
				durationEvent = ble_e_model_c_getDurationEventSamePayload(1,0,1,BLE_E_MOD_CE_CON_UP_SLRSP_LEN,BLE_E_MOD_CE_CON_UP_LEN,BLE_E_MOD_CE_CON_UP_TXPOWER,-1);
				//sleep duration
				charge = charge + (TcOld + 0.0 + BLE_E_MOD_CE_DP(TcNew) - durationEvent) * BLE_E_MOD_G_ISL;
			}

		}

	}else{
		//slave
		if(establishmentOrUpdate){
			//connection establishment procedure

			//packet / event
			//The advertising event is already accounted for in the adv/ scanning model

			//sleep duration and idle rx duration during transmit window:
			//		  1,25ms		dtwo														dww																		dp							dww
			charge = (0.00125 + BLE_E_MOD_CE_DTWO_CR(TcNew) - (BLE_E_MOD_G_SCA * 2.0 / 1e6) * (0.00125 + BLE_E_MOD_CE_DTWO_CR(TcNew))) * BLE_E_MOD_G_ISL + (BLE_E_MOD_CE_DP(TcNew) + (BLE_E_MOD_G_SCA * 2.0 / 1e6) * (0.00125 + BLE_E_MOD_CE_DTWO_CR(TcNew))) * BLE_E_MOD_SCAN_IRX;
		}else{
			//packet / event
			if(eventType == BLE_E_MODEL_CE_EVENT_TYPE_EMBEDDED){

	//			durationEvent  = BLE_E_MOD_C_DTXRX + BLE_E_MOD_C_DPRERX + BLE_E_MOD_CE_CON_UP_LEN * 1e-6 + BLE_E_MOD_C_DRXTX + BLE_E_MOD_C_DPRETX + BLE_E_MOD_CE_CON_UP_SLRSP_LEN * 1e-6;
				charge  = BLE_E_MOD_C_DTXRX * BLE_E_MOD_C_ITXRX + (BLE_E_MOD_C_DPRERX + BLE_E_MOD_CE_CON_UP_LEN * 1e-6)*BLE_E_MOD_C_IRX + BLE_E_MOD_C_DRXTX * BLE_E_MOD_C_IRXTX + (BLE_E_MOD_C_DPRETX + BLE_E_MOD_CE_CON_UP_SLRSP_LEN * 1e-6) * BLE_E_MOD_C_ITX;


			}else if(eventType == BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY){
	//			durationEvent  = BLE_E_MOD_C_DTXRX + BLE_E_MOD_C_DPRERX + BLE_E_MOD_CE_CON_UP_LEN * 1e-6 + BLE_E_MOD_C_DRXTX + BLE_E_MOD_C_DPRETX + BLE_E_MOD_CE_CON_UP_SLRSP_LEN * 1e-6;
				charge  = ((BLE_E_MOD_CE_CON_UP_LEN - BLE_E_MOD_CE_EMPTY_PACKET_LEN) * 1e-6)* BLE_E_MOD_C_ITX;

			}else if(eventType == BLE_E_MODEL_CE_EVENT_TYPE_FULL){

				charge = ble_e_model_c_getChargeEventSamePayload(0,TcOld,1,BLE_E_MOD_CE_CON_UP_LEN,BLE_E_MOD_CE_CON_UP_SLRSP_LEN,BLE_E_MOD_CE_CON_UP_TXPOWER,-1);
				durationEvent = ble_e_model_c_getDurationEventSamePayload(0,TcOld,1,BLE_E_MOD_CE_CON_UP_LEN,BLE_E_MOD_CE_CON_UP_SLRSP_LEN,BLE_E_MOD_CE_CON_UP_TXPOWER,-1);
				//sleep period
				charge += (TcOld + 0.0 - (BLE_E_MOD_G_SCA * 2.0 / 1e6) * TcOld - durationEvent) * BLE_E_MOD_G_ISL;

			}

			//idle rx during transmit window
			charge += (BLE_E_MOD_CE_DP(TcNew) + ((double)BLE_E_MOD_G_SCA * 2.0 / 1e6) * TcOld) * BLE_E_MOD_SCAN_IRX;

		}
	}

	return charge;
};


