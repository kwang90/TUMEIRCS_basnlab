/*
 * ble_model_connected.c
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

#include "ble_model_params.h"
#include "ble_model_connected.h"

#include <string.h>			//for memset
#if BLE_MODEL_PLATTFORM == BLE_MODEL_PLATTFORM_POSIX
	#include <stdlib.h>
	#include <stdio.h>
#endif

double ble_e_model_c_getChargeConstantParts(){
	return BLE_E_MOD_C_DHEAD * BLE_E_MOD_C_IHEAD	+	BLE_E_MOD_C_DPRE * BLE_E_MOD_C_IPRE		+	BLE_E_MOD_C_DCPRE * BLE_E_MOD_C_ICPRE	+	BLE_E_MOD_C_DTRA * BLE_E_MOD_C_ITRA +
			+ BLE_E_MOD_C_DPOST * BLE_E_MOD_C_IPOST		+	BLE_E_MOD_C_DTAIL * BLE_E_MOD_C_ITAIL;

}

double ble_e_model_c_getDurationConstantParts(){
	return BLE_E_MOD_C_DHEAD + BLE_E_MOD_C_DPRE + BLE_E_MOD_C_DCPRE + BLE_E_MOD_C_DTRA + BLE_E_MOD_C_DPOST + BLE_E_MOD_C_DTAIL;

}

double ble_e_model_c_getChargeSequences(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx[], int8_t nTx[],uint8_t txPower, double constantWindowWidening){
	uint32_t cnt = 0;
	double charge = 0;

	double iTx = 0;

	//determine TX current drawn determined by the Tx Power level
	switch(txPower){
		case 3:
			iTx  = BLE_E_MOD_C_ITX;
			break;
		default:
#if BLE_MODEL_PLATTFORM == BLE_MODEL_PLATTFORM_POSIX
			printf("invalid tx power level: %d\n",txPower);
			exit(1);

#else
			return 0;
#endif
		break;
	}



	//go through all seqences
	for(cnt = 0; cnt < nSeq; cnt++){

		//charge cosumed by RX-phase overheads (dPreRx + windowWidening)
		if((cnt == 0)&&(!masterOrSlave)){


			//first rx - window widening  and longer dPreRx @ slave
			if(constantWindowWidening < 0){
				//window-widening depends on connection interval
				charge = charge + (BLE_E_MOD_C_DPRERX_SL1 + (BLE_E_MOD_G_SCA*2.0/1.0e6)*Tc)*BLE_E_MOD_C_IRX;
			}else{
				//a constant amount of window widening is assumed
				charge = charge + (BLE_E_MOD_C_DPRERX_SL1 + constantWindowWidening*BLE_E_MOD_C_IRX);
			}
		}else{
			//not the first rx phase or the first RX-phase of master - no window-widening and normal dPreRx
			charge = charge + BLE_E_MOD_C_DPRERX * BLE_E_MOD_C_IRX;
		}

		//charge consumed by recption
		charge = charge + 8.0e-6*nRx[cnt] * BLE_E_MOD_C_IRX;

		if(nTx[cnt] >= 0){
			//charge consumed by transmission + dPreTx
			charge = charge + (BLE_E_MOD_C_DPRETX + 8.0e-6*nTx[cnt]) * iTx;

			//rx2tx and tx2rx interframe spaces
			charge = charge + BLE_E_MOD_C_DRXTX * BLE_E_MOD_C_IRXTX;
			charge = charge + BLE_E_MOD_C_DTXRX * BLE_E_MOD_C_ITXRX;
		}
		//add sequence correction offset
		charge = charge + BLE_E_MOD_C_QTO;

	}

	//remove leftover IFS
	if(masterOrSlave){
		//master
		charge = charge -  BLE_E_MOD_C_DRXTX * BLE_E_MOD_C_IRXTX;
	}else{
		//slave
		if(nTx[nSeq - 1] >= 0){
			charge = charge -  BLE_E_MOD_C_DTXRX * BLE_E_MOD_C_ITXRX;
		}
	}
	return charge;
}





double ble_e_model_c_getChargeSequencesSamePayload(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx, int8_t nTx,uint8_t txPower, double constantWindowWidening){
	double charge = 0;
	uint8_t nRxA[BLE_E_MODE_INT_MAXSEQUENCES];
	int8_t nTxA[BLE_E_MODE_INT_MAXSEQUENCES];
	if(nSeq > BLE_E_MODE_INT_MAXSEQUENCES){
#if BLE_MODEL_PLATTFORM == BLE_MODEL_PLATTFORM_POSIX
		printf("ERROR: nSeq = %d > maximum sequence number which is %d. Adjust maximum sequence number in ble_model_connected.h by changing the BLE_E_MODE_INT_MAXSEQUENCES makro.\n",nSeq,BLE_E_MODE_INT_MAXSEQUENCES);
		exit(1);
#else
		return 0;

#endif
	}
    memset(nRxA,nRx,nSeq);
	memset(nTxA,nTx,nSeq);
	charge = ble_e_model_c_getChargeSequences(masterOrSlave,Tc,nSeq,nRxA,nTxA,txPower,constantWindowWidening);
	return charge;
}



double ble_e_model_c_getDurationSequences(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx[], int8_t nTx[],uint8_t txPower, double constantWindowWidening){
	uint32_t cnt = 0;
	double duration = 0;


	//go through all seqences
	for(cnt = 0; cnt < nSeq; cnt++){
		//time cosumed by RX-phase overheads (dPreRx + windowWidening)
		if((cnt == 0)&&(!masterOrSlave)){
			//first rx - window widening  and longer dPreRx @ slave

			if(constantWindowWidening < 0){
				//window widening depends on time since last anchor
				duration = duration + (BLE_E_MOD_C_DPRERX_SL1 + BLE_E_MOD_G_SCA*2.0/1.0e6*Tc);
			}else{
				//window widening is assumed to have a constant, predefined value
				duration = duration + (BLE_E_MOD_C_DPRERX_SL1 + constantWindowWidening);

			}
		}else{
			//not the first rx phase or the first RX-phase of master - no window-widening and normal dPreRx
			duration= duration + BLE_E_MOD_C_DPRERX;
		}

		//time consumed by recption
		duration = duration + 8.0e-6*nRx[cnt];
		if(nTx[cnt] >= 0){
			//time consumed by transmission + dPreTx
			duration = duration + (BLE_E_MOD_C_DPRETX + 8.0e-6*nTx[cnt]);

			//rx2tx and tx2rx interframe spaces
			duration = duration + BLE_E_MOD_C_DRXTX;
			duration = duration + BLE_E_MOD_C_DTXRX;
		}
	}
	//remove leftover IFS [2do: check with nTx < 0)
	if(masterOrSlave){
		//master
		duration =duration -  BLE_E_MOD_C_DRXTX;
	}else{
		//slave
		if(nTx[nSeq - 1] >=0){
			duration = duration -  BLE_E_MOD_C_DTXRX;
		}
	}
	return duration;
}




double ble_e_model_c_getDurationSequencesSamePayload(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx, int8_t nTx,uint8_t txPower, double constantWindowWidening){
	double duration = 0;
	uint8_t nRxA[BLE_E_MODE_INT_MAXSEQUENCES];
	int8_t nTxA[BLE_E_MODE_INT_MAXSEQUENCES];
	if(nSeq > BLE_E_MODE_INT_MAXSEQUENCES){
#if BLE_MODEL_PLATTFORM == BLE_MODEL_PLATTFORM_POSIX
		printf("ERROR: nSeq = %d > maximum sequence number which is %d. Adjust maximum sequence number in ble_model_connected.h by changing the BLE_E_MODE_INT_MAXSEQUENCES makro.\n",nSeq,BLE_E_MODE_INT_MAXSEQUENCES);
		exit(1);
#else
		return 0;
#endif
	}
	if(nSeq == 0){
	#if BLE_MODEL_PLATTFORM == BLE_MODEL_PLATTFORM_POSIX
			printf("ERROR: nSeq = %d > maximum sequence number which is %d. Adjust maximum sequence number in ble_model_connected.h by changing the BLE_E_MODE_INT_MAXSEQUENCES makro.\n",nSeq,BLE_E_MODE_INT_MAXSEQUENCES);
			exit(1);
	#else
			return 0;
	#endif
		}
    memset(nRxA,nRx,nSeq);
	memset(nTxA,nTx,nSeq);
	duration = ble_e_model_c_getDurationSequences(masterOrSlave,Tc,nSeq,nRxA,nTxA,txPower,constantWindowWidening);
	return duration;
}


double ble_e_model_c_getChargeEvent(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx[], int8_t nTx[],uint8_t txPower, double constantWindowWidening){
	double charge = ble_e_model_c_getChargeConstantParts();
	charge = charge + ble_e_model_c_getChargeSequences(masterOrSlave, Tc, nSeq,nRx,nTx,txPower,constantWindowWidening);
	return charge;

}

double ble_e_model_c_getChargeEventSamePayload(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx, int8_t nTx,uint8_t txPower,double constantWindowWidening){
	double charge = ble_e_model_c_getChargeConstantParts();
	charge = charge + ble_e_model_c_getChargeSequencesSamePayload(masterOrSlave, Tc, nSeq,nRx,nTx,txPower,constantWindowWidening);
	return charge;
}

double ble_e_model_c_getDurationEvent(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx[], int8_t nTx[],uint8_t txPower, double constantWindowWidening){
	double duration = ble_e_model_c_getDurationConstantParts();
	duration = duration + ble_e_model_c_getDurationSequences(masterOrSlave, Tc, nSeq,nRx,nTx,txPower,constantWindowWidening);
	return duration;

}

double ble_e_model_c_getDurationEventSamePayload(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx, int8_t nTx,uint8_t txPower, double constantWindowWidening){
	double duration = ble_e_model_c_getDurationConstantParts();
	duration = duration + ble_e_model_c_getDurationSequencesSamePayload(masterOrSlave, Tc, nSeq,nRx,nTx,txPower,constantWindowWidening);
	return duration;

}

double ble_e_model_c_getChargeConnectionInterval(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx[], int8_t nTx[],uint8_t txPower,double constantWindowWidening){
	double duration = ble_e_model_c_getDurationEvent(masterOrSlave, Tc, nSeq,nRx,nTx,txPower,constantWindowWidening);
	double charge = ble_e_model_c_getChargeEvent(masterOrSlave, Tc, nSeq,nRx,nTx,txPower,constantWindowWidening);
	charge = charge + (Tc - duration) * BLE_E_MOD_G_ISL;
	return charge;
}

double ble_e_model_c_getChargeConnectionIntervalSamePayload(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx, int8_t nTx,uint8_t txPower, double constantWindowWidening){
	double duration = ble_e_model_c_getDurationEventSamePayload(masterOrSlave, Tc, nSeq,nRx,nTx,txPower,constantWindowWidening);
	double charge = ble_e_model_c_getChargeEventSamePayload(masterOrSlave, Tc, nSeq,nRx,nTx,txPower,constantWindowWidening);
	charge = charge + (Tc - duration) * BLE_E_MOD_G_ISL;
	return charge;
}
