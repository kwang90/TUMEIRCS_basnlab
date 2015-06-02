/**
 * \file examples.c
 * \brief some examples for the BLE energy model.
 * Compile them unter linux as the main file to get an executable programm.
 *
 *  30.09.2013 - 2.10.2013, Philipp Kindt <kindt@rcs.ei.tum.de>
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


#include "../src/ble_model.h"
#include <stdio.h>
int main(){


	///Some examples in connected mode:
	double dc, dseq, dwhole;

	//get the duration of all constant parts of a connection event. (Preprocessing, Postprocessing,...)
	dc = ble_e_model_c_getDurationConstantParts();

	//get the duration of the connection sequence (all non-constant parts of the model)
	dseq = ble_e_model_c_getDurationSequencesSamePayload(1,0.1,5,10,22,3,-1);

	//get the duration of the whole event, this should be the sum of dc and dseq
	dwhole = ble_e_model_c_getDurationEventSamePayload(1,0.1,5,10,22,3,-1);

	//now do the same with the charge of these phases
	double chargeC, chargeSeq, chargeWhole;
	chargeC = ble_e_model_c_getChargeConstantParts();
	chargeSeq = ble_e_model_c_getChargeSequencesSamePayload(1,0.1,5,10,22,3,-1);
	chargeWhole = ble_e_model_c_getChargeEventSamePayload(1,0.1,5,10,22,3,-1);
	printf("Duration: const %.15f - seq: %.15f, whole: %.15f\n",dc,dseq,dwhole);
	printf("Charge: const %.15f - seq: %.15f, whole: %.15f\n",chargeC,chargeSeq,chargeWhole);


	//now we're not interested in the events only, but also in the sleeping phase of a connection event
	double chargeInterval;
	chargeInterval = ble_e_model_c_getChargeConnectionIntervalSamePayload(0,0.1,5,10,22,3,-1);
	printf("Charge for connection interval: %.15f\n",chargeInterval);


	//now get the chrage for a master establishing a connection. We are only interested in the additional energy that is not accounted for in the energy model for device discovery as described in the paper, and not the whole model.
	//Therefore, we use \ref SC_EVENT_TYPE_CON_REQ_OFFSET
	double croCharge = ble_e_model_sc_getChargeScanEvent(0.025,SC_EVENT_TYPE_CON_REQ_OFFSET,SC_SCAN_TYPE_PERIODIC, BLE_E_MOD_CE_ADV_IND_PKG_LEN,BLE_E_MOD_CE_CON_REQ_LEN,0,0.125);
	printf("Connection Request offset charge: %.15f\n",croCharge);

	//now we calculate the energy consumption of a scan event with active scanning that receives and replies to an advertising event
	double croActScan = ble_e_model_sc_getChargeScanEvent(0.025,SC_EVENT_TYPE_ACTIVE_SCANNING,SC_SCAN_TYPE_PERIODIC,28,22,34,0.125);
	printf("Active Scanning Event: %.15f\n",croActScan);
	_ble_model_discovery_gausscdf(33.0);

	//Calculate the latency and energy consumption of device discovery for Ta = 2.55s, Ts = 2.56s and ds = 64ms.
	discoveryModelResult_t result =  ble_model_discovery_getResult(100,0.9999,2.55,2.56,0.064,0.01,1000);
	printf("discovery latency: %.15f\tdiscovery energy: advertiser: %.15f, scanner: %.15f\n",result.discoveryLatency,result.chargeAdv, result.chargeScan);

	return 0;

}
