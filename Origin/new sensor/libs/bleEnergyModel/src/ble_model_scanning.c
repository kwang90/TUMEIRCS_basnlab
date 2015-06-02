/*
 * ble_model_scanning.c
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


#include "ble_model_scanning.h"
#include "ble_model_params_scanning.h"

double ble_e_model_sc_getChargeScanEvent(double scanWindow, ble_e_model_sc_event_type_t eventType, ble_e_model_sc_scan_type scanType, uint8_t nBytesAdvInd, uint8_t nBytesTx, uint8_t nBytesRx, double receptionAfterTime){
	//Charge for pre- and postprocessing
	double charge = BLE_E_MOD_SCAN_DPRE * BLE_E_MOD_SCAN_IPRE + BLE_E_MOD_SCAN_DPOST * BLE_E_MOD_SCAN_IPOST;

	//continous scaning? => overwrite charge for pre-postprocessing with the charge for one channel-changing phase
	charge = BLE_E_MOD_SCAN_DCHCH * BLE_E_MOD_SCAN_ICHCH;

	switch(eventType){
		//SCANNING with no reception
		case SC_EVENT_TYPE_NO_RECEPTION:
			charge += (scanWindow  + BLE_E_MOD_SCAN_DWOFFSET)* BLE_E_MOD_SCAN_IRX;
			break;
		case SC_EVENT_TYPE_ABORTED:
			if(scanWindow < receptionAfterTime){
				charge += (scanWindow  + BLE_E_MOD_SCAN_DWOFFSET)* BLE_E_MOD_SCAN_IRX;
			}else{
				charge += (receptionAfterTime + BLE_E_MOD_SCAN_DWOFFSET)* BLE_E_MOD_SCAN_IRX;
			}
			break;

		//passive scanning with or without an ADV_IND received, but with no CON_REQ packet sent
		case SC_EVENT_TYPE_PASSIVE_SCANNING:
			charge += (scanWindow + BLE_E_MOD_SCAN_DWOFFSET) * BLE_E_MOD_SCAN_IRX;
			break;

		//active scanning
		case SC_EVENT_TYPE_ACTIVE_SCANNING:
			charge += (scanWindow + BLE_E_MOD_SCAN_DWOFFSET - BLE_E_MOD_SCAN_DRXTX - BLE_E_MOD_SCAN_DPRETX - 8e-6*nBytesTx - BLE_E_MOD_SCAN_DTXRX - BLE_E_MOD_SCAN_DPRERX - 8e-6*nBytesRx - BLE_E_MOD_SCAN_DRXRX ) * BLE_E_MOD_SCAN_IRX
					+ BLE_E_MOD_SCAN_DRXTX * BLE_E_MOD_SCAN_IRXTX + (BLE_E_MOD_SCAN_DPRETX + 8e-6*nBytesTx)*BLE_E_MOD_SCAN_ITX + BLE_E_MOD_SCAN_DTXRX * BLE_E_MOD_SCAN_ITXRX +  (BLE_E_MOD_SCAN_DPRERX + 8e-6*nBytesRx)*BLE_E_MOD_SCAN_IRXS + BLE_E_MOD_SCAN_DRXRX * BLE_E_MOD_SCAN_IRXRX + BLE_E_MOD_SCAN_QCRX + BLE_E_MOD_SCAN_QCTX;
			break;

		//scanning with con_req
		case SC_EVENT_TYPE_CON_REQ:
			charge += (scanWindow - receptionAfterTime) * BLE_E_MOD_SCAN_IRX +
				 BLE_E_MOD_SCAN_DRXTX * BLE_E_MOD_SCAN_IRXTX + (BLE_E_MOD_SCAN_DPRETX + 8e-6*nBytesTx)*BLE_E_MOD_SCAN_ITX + BLE_E_MOD_SCAN_QCTX;
			break;

		//scanning with con req as offset to the discovery energy model
		case SC_EVENT_TYPE_CON_REQ_OFFSET:
			//note: 'charge =' instead of 'charge +=' means: no pre- or postprocessing
			charge = (BLE_E_MOD_SCAN_DRXTX * BLE_E_MOD_SCAN_IRXTX + (BLE_E_MOD_SCAN_DPRETX + 8e-6*nBytesTx)*BLE_E_MOD_SCAN_ITX);
			break;

	}

	return charge;
};
