/*
 * ble_model_discovery.c
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
#include "ble_model_params_connection_establishment.h"
#include "ble_model_params_general.h"
#include "ble_model.h"
#include "ble_model_discovery.h"

#if BLE_MODEL_PLATTFORM == BLE_MODEL_PLATTFORM_POSIX
#include <stdio.h>
#include <stdlib.h>
#endif

//#define DEBUG(...) printf(__VA_ARGS__);
#define DEBUG(...)

#include <tgmath.h>
#include <math.h>
/**
 * \brief Calculates cumulative density function (CDF) of a (mu, sigma)-normal distribution by transforming the Gaussian CDF
 * For the math behind this transformation, see http://en.wikipedia.org/wiki/Normal_distribution
 * @param x Value to evaluate
 * @param mu Mean value of the distribution
 * @param sigma Standard deviation of the distribution
 * @return Value of the CDF evaluated at x
 */
double _ble_model_discovery_normcdf(double x, double mu, double sigma)
{
	return _ble_model_discovery_gausscdf((x - mu)/sigma);
}

/**
 * \brief Gets cumulative density function (CDF) of a Gaussian distribution.
 * Code is taken from http://www.johndcook.com/cpp_phi.html
 * It is released in public domain according to the website mentioned above:
 * "This code is in the public domain. Do whatever you want with it, no strings attached."
 *
 * @param x value of the random variable.
 * @return Value of Gaussian CDF for x
 */
double _ble_model_discovery_gausscdf(double x)
{

    // constants
    double a1 =  0.254829592;
    double a2 = -0.284496736;
    double a3 =  1.421413741;
    double a4 = -1.453152027;
    double a5 =  1.061405429;
    double p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = fabs(x)/sqrt(2.0);

    // A&S formula 7.1.26
    double t = 1.0/(1.0 + p*x);
    double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);
    return 0.5*(1.0 + sign*y);
}

//probability that an advertising event starts at time T
double _ble_model_discovery_getApproxProbab(double mu, uint32_t n, double sigma, double t, double TaIdeal, double rhoMax){
	//the first advertising event is starting at TaIdeal = phi by definition
	if(n == 0){
		if(t < TaIdeal){
			return 0;
		}else{
			return 1;
		}

	// n = 1 -> First adv event after one adv interval  => uniform (rectangular) distribution between [Ta, Ta + rhoMax], height: 1/rhoMax
	}else if(n == 1){
		if((t > TaIdeal)&&t < TaIdeal + rhoMax){
			return (t - TaIdeal)/rhoMax;
		}else if(t < TaIdeal){
			return 0;

		}else{
			return 1;
		}

		// n = 2 => triangular distribution between Ta and Ta + 2*rhoMax
	}else if(n == 2){
		if(t >= TaIdeal){
			if(t < TaIdeal + rhoMax){
				return (t - TaIdeal) * (t - TaIdeal)/(2.0*rhoMax*rhoMax);
			}else if(t < TaIdeal + 2.0 * rhoMax){
				return 1 - (TaIdeal + 2.0*rhoMax - t)*(TaIdeal + 2.0*rhoMax - t)/(2.0 * rhoMax * rhoMax);
			}else{
				return 1;
			}
		}else{
			return 0;
		}
		//normal distribution
	}else{
		return _ble_model_discovery_normcdf(t,mu,sigma);
	}


}
discoveryModelResult_t _ble_model_discovery_getResultOnePhi(double epsilonHit, double Ta, double Ts, double ds, double phi,double rhoMax, double maxTime){

	discoveryModelResult_t result;	//structure for the result => initialize to zero
	result.chargeAdv = 0;
	result.chargeScan = 0;
	result.discoveryLatency = 0;

	uint32_t n = 0;					// Index of advertising event
	double pHit = 0;				// Hit probability for an advertising event n
	double pCummMiss = 1;			// Cumulative miss probability: The probability that all advertising events until so far could not be received
	double tExp = 0;				// Expected Adv/Scanning time for this phi
	double chargeAdvExp = 0;		// Expected charge consumed by advertiser
	double chargeScanExp = 0;			// Expected charge consumed by scanner
	double currentT = 0;			// Current time
	uint8_t channel = 0;			// Current Channel the scanner listenes on

	double TaIdeal = 0;				// Ideal time an advertising event will be sent (without advDelay)
	double TaReal = 0;				// Mean time an advertising event is sent in reality (with advDelay = 5ms in average)

	double dEarly = 0;				// dEarly determines the time an advertising event is received successfully before the beginning of a scan event.
	double dLate = 0;				// dLate determines the time an advertising event is not received before the end of a scan event

	uint32_t k = 0;					// Scan event counter
	uint32_t kMin = 0;				// The minimun scan event considered for an advertising event
	uint32_t kMax = 0;				// The maximum scan event considered for an advertising event


	uint32_t nScanEventsBeforeAdvertising = 0;		//Number of scan events that have taken place before the advertiser started advertising (events in time phi)
	double scanEnergyBeforeAdvertsing = 0;			//The energy spent by the scanner before the advertiser started advertising (energywithin time phi)
	double scanTimeOnEdge = 0;						//Scan time of a fraction of scan events that happen within phi but do not end within phi
	double scanEnergyOnEdge = 0;					//Energy for scan events that happenw ithin phi but do not end whithin phi
	uint32_t nFullScanEvents = 0;
	double timeLeft = 0;
	double pk;										//probability a scan event k recieves an advertising event


	//idle times and charges
	double t39idle = ble_e_model_c_getDurationEventSamePayload(1,0,3,0,BLE_E_MOD_CE_ADV_IND_PKG_LEN,3,-1);
	double q39idle = ble_e_model_c_getChargeEventSamePayload(1,0,3,0,BLE_E_MOD_CE_ADV_IND_PKG_LEN,3,-1);

	//charges for last, successful event
	double q37 = ble_e_model_c_getChargeEventSamePayload(1,0,1,BLE_E_MOD_CE_CON_REQ_LEN,BLE_E_MOD_CE_ADV_IND_PKG_LEN,3,-1);
	double q38 = ble_e_model_c_getChargeEventSamePayload(1,0,2,BLE_E_MOD_CE_CON_REQ_LEN,BLE_E_MOD_CE_ADV_IND_PKG_LEN,3,-1);
	double q39 = ble_e_model_c_getChargeEventSamePayload(1,0,3,BLE_E_MOD_CE_CON_REQ_LEN,BLE_E_MOD_CE_ADV_IND_PKG_LEN,3,-1);



	while((1 - pCummMiss < epsilonHit)&&(tExp < maxTime)){		//wait until either the maximum time is exceeded or the cummulative miss probability get sufficiently small

		//compute the thimes the advertising events begin
		TaIdeal = phi + ((double) n)*Ta;
		TaReal = TaIdeal + ((double) n)*rhoMax/2.0;

		//determine boarders kMin and kMax for the scan events k that need to be considered for a given n
		kMin = floor(TaIdeal/Ts);
		kMax = floor((TaIdeal + ((double)n)*rhoMax)/Ts);

		pHit = 0;

		//compute time/energy spent whithin phi

		//full scan events
		nScanEventsBeforeAdvertising = floor(phi/Ts);
		scanEnergyBeforeAdvertsing = ((double) nScanEventsBeforeAdvertising) * ble_e_model_sc_getChargeScanEvent(ds,SC_EVENT_TYPE_NO_RECEPTION,SC_SCAN_TYPE_PERIODIC,BLE_E_MOD_CE_ADV_IND_PKG_LEN,0,0,0.0);

		//partial scan events. Perhaps not the most exact solution as some pre/postprocessing might be swallowed, but error is low.
		scanTimeOnEdge = phi - ((double) nScanEventsBeforeAdvertising) * Ts;
		if(scanTimeOnEdge > ds){
			scanEnergyOnEdge = 0;
			scanEnergyBeforeAdvertsing += ble_e_model_sc_getChargeScanEvent(ds,SC_EVENT_TYPE_NO_RECEPTION,SC_SCAN_TYPE_PERIODIC,0,0,0,0);
		}else{
			scanEnergyOnEdge = ble_e_model_sc_getChargeScanEvent(ds,SC_EVENT_TYPE_ABORTED,SC_SCAN_TYPE_PERIODIC,0,0,0,scanTimeOnEdge);
			scanEnergyBeforeAdvertsing += ble_e_model_sc_getChargeScanEvent(ds,SC_EVENT_TYPE_ABORTED,SC_SCAN_TYPE_PERIODIC,0,0,0,ds - scanTimeOnEdge);
		}

		//consider all possible scan events that might have success
		for(k = kMin; k <= kMax; k++){

			//determine current channel
			channel = 37 + k%3;


			//compute dEarly and dLate for the current channel
			switch(channel){
			case 37:
				dEarly = 0;
				dLate = 8e-6*BLE_E_MOD_CE_ADV_IND_PKG_LEN;
				break;
			case 38:
				dEarly = 8e-6*BLE_E_MOD_CE_ADV_IND_PKG_LEN + 150e-6;
				dLate = 2.0*8e-6*BLE_E_MOD_CE_ADV_IND_PKG_LEN + 150e-6;
				break;
			case 39:
				dEarly = 2.0*(8e-6*BLE_E_MOD_CE_ADV_IND_PKG_LEN + 150e-6);
				dLate = 3.0*8e-6*BLE_E_MOD_CE_ADV_IND_PKG_LEN + 2.0*150e-6;
				break;
			default:
#if BLE_MODEL_PLATTFORM == BLE_MODEL_PLATTFORM_POSIX
				printf("invalid channel: %d",channel);
				exit(1);
#else
				return result;
#endif
			}

			//probability that the current scan event k receives an advertising event
			pk = _ble_model_discovery_getApproxProbab(TaReal,n,sqrt(((double) n)/12.0)*rhoMax, k * Ts + ds - dLate,TaIdeal,rhoMax) - _ble_model_discovery_getApproxProbab(TaReal,n,sqrt(((double) n)/12.0)*rhoMax,k*Ts - dEarly,TaIdeal,rhoMax);

			//hit-probability of the advertising event n
			pHit = pHit + pk;


			//compute expected discovery latency that slightly differs on the channel
			switch(channel){
				case 37:
					currentT = (n* (Ta + rhoMax/2.0) + 8e-6*BLE_E_MOD_CE_ADV_IND_PKG_LEN);
					break;
				case 38:
					currentT = (n* (Ta + rhoMax/2.0) + 2.0* 8e-6*BLE_E_MOD_CE_ADV_IND_PKG_LEN + 150e-6);
					break;
				case 39:
					currentT = (n* (Ta + rhoMax/2.0) + 3.0* 8e-6*BLE_E_MOD_CE_ADV_IND_PKG_LEN + 2.0*150e-6);
					break;

				default:
				break;

			}

			//expected spent for discovery so far
			tExp = tExp + pCummMiss *  pk * currentT;


			//expected charge spent by advertiser

			//consider all idle events
			if(n >= 1){
				chargeAdvExp = chargeAdvExp + pCummMiss * pk * (n - 1)* q39idle;
				chargeAdvExp = chargeAdvExp + pCummMiss * pk * (n - 1)* (Ta - t39idle) * BLE_E_MOD_G_ISL;
			}

			//consider last, successful event
			switch(channel){
			case 37:
				chargeAdvExp = chargeAdvExp + pCummMiss * pk*q37;
				break;
			case 38:
				chargeAdvExp = chargeAdvExp + pCummMiss * pk*q38;
				break;
			case 39:
				chargeAdvExp = chargeAdvExp + pCummMiss * pk*q39;
				break;
			default:
				break;
			}

			//expected charge spent by scanner
			nFullScanEvents = floor((currentT + phi)/Ts);
			timeLeft = (phi + currentT) - nFullScanEvents * Ts;
			chargeScanExp = chargeScanExp + pCummMiss * pk * ((double) nFullScanEvents) * ble_e_model_sc_getChargeScanEvent(ds,SC_EVENT_TYPE_NO_RECEPTION,SC_SCAN_TYPE_PERIODIC,0,0,0,0);
			if(timeLeft > ds){
				chargeScanExp += pCummMiss * pk * ble_e_model_sc_getChargeScanEvent(ds,SC_EVENT_TYPE_NO_RECEPTION,SC_SCAN_TYPE_PERIODIC,0,0,0,0);
			}else{
				chargeScanExp += ble_e_model_sc_getChargeScanEvent(ds,SC_EVENT_TYPE_ABORTED,SC_SCAN_TYPE_PERIODIC,0,0,0,timeLeft);
			}
		}


		//adjust the cummulative miss proboability
		pCummMiss = pCummMiss * (1 - pHit);


		//stop algorithm for times beeing to high
		if(tExp > maxTime){
			tExp = maxTime;
			break;
		}

		//next advertising event
		n = n + 1;

	}

	if(tExp > maxTime){
		tExp = maxTime;
	}

	result.discoveryLatency = tExp;
	result.chargeAdv = chargeAdvExp;
	result.chargeScan = chargeScanExp - scanEnergyBeforeAdvertsing;
	return result;
}

discoveryModelResult_t ble_model_discovery_getResult(uint32_t nPoints,double epsilonHit, double Ta, double Ts, double ds,double rhoMax, double maxTime){
	double delta = (3.0*Ts) / ((double) nPoints);
	double phi = 0;
	discoveryModelResult_t resultSingle;
	discoveryModelResult_t resultJoined;
	resultJoined.chargeAdv = 0;
	resultJoined.chargeScan = 0;
	resultJoined.discoveryLatency = 0;

	uint32_t cnt;
	for(cnt = 0; cnt < nPoints; cnt++){
		#if BLE_MODEL_PLATTFORM == BLE_MODEL_PLATTFORM_POSIX
			DEBUG("cnt= %d/%d\n",cnt,nPoints);
		#endif
		resultSingle = _ble_model_discovery_getResultOnePhi(epsilonHit,Ta,Ts,ds,phi,rhoMax, maxTime);
		resultJoined.discoveryLatency += resultSingle.discoveryLatency;

		resultJoined.chargeAdv += resultSingle.chargeAdv;
		resultJoined.chargeScan += resultSingle.chargeScan;
		phi = phi + delta;
	}

	resultJoined.discoveryLatency /= nPoints;
	resultJoined.chargeAdv /= nPoints;
	resultJoined.chargeScan /= nPoints;
	return resultJoined;
}

