/**
 * \file ble_model_discovery.h
 * \brief Energy model for device discovery in BLE
 *
 * Call \ref ble_model_discovery_getResult() to get an estimate for the device-discovery latency and the corresponsing energies
 * spent by the master and  the slave. This function requires high amounts of computational power. Set the parameters, especially nPoints, maxTime and epsilonHit
 * carefully. You probably don't want to use this function for online power management as it is computationally expensive.
 *
 *  Okt 2013, Philipp Kindt <kindt@rcs.ei.tum.de>
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

#ifndef BLE_MODEL_DISCOVERY_H_
#define BLE_MODEL_DISCOVERY_H_
#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MODEL_DISCOVERY_FLOATING_EPSILON 1e-9


/**
 * Result of the model for device discovery.
 */
typedef struct _discoveryModelResult_t{
		double discoveryLatency;			///< The discovery latency [s]
		double chargeAdv;					///< The charge consumed by the advertiser for device discovery [As]
		double chargeScan;					///< The charge consumed by the scanner for device discovery [As]
} discoveryModelResult_t;


/**
 * \file Gaussian cumulative distribution function (CDF)
 * @param x
 * @return
 */
double _ble_model_discovery_gausscdf(double x);

/**
 * \brief Calculates cumulative density function (CDF) of a (mu, sigma)-normal distribution by transforming the Gaussian CDF
 * For the math behind this transformation, see http://en.wikipedia.org/wiki/Normal_distribution
 * @param x Value to evaluate
 * @param mu Mean value of the distribution
 * @param sigma Standard deviation of the distribution
 * @return Value of the CDF evaluated at x
 */
double _ble_model_discovery_normcdf(double x, double mu, double sigma);

/**
 * \brief Computes the approximate probability of an advertising event having started before time t
 * @param mu Mean value of the starting time of the advertising event (=TaReal)
 * @param n Number of the advertising event. 0 => The event right after phi. 1=> Event after one advertising interval, 2=> Event after two advertising intervals
 * @param sigma	Standard deviation of the starting time of the advertising event
 * @param t	Time to evaluate the CDF
 * @param TaIdeal Ideal point in time the advertising event  starts(when no random advertising delay would exist)
 * @param rhoMax Maximum random advDelay
 * @return Approximate probabilty that the advertising event starts before t
 */
double _ble_model_discovery_getApproxProbab(double mu, uint32_t n, double sigma, double t, double TaIdeal, double rhoMax);

/**
 * \brief Returns the model results (discovery-latency and discover-energy both for advertiser and scanner) for a given value offset phi
 * @param epsilonHit The hit probability of all advertising events examined. As soon as the algorithm has reached this probability, it will consider the results as stable and stop.
 * 		  The closer this value becomes to one, the more precise the results become, but the algorithm takes longer as more advertising events are examined.
 * 		  Example: The first advertising event hits with probability 0.5, the second with 0.25 and the third with 0.15. The hit probability would be 0.5+0.25+0.15 = 0.9
 * 		  If epsilonHit is 0.89, the algorithm would end after three advertising events
 * @param Ta Advertising interval [s]
 * @param Ts Scan interval [s]
 * @param ds Scan window [s]
 * @param phi Offset of the first scan event (n=0) from the beginning of the scanning process
 * @param rhoMax Maximum advertising delay [s]. Should be 10 ms according to the BLE specification
 * @param maxTime The maxmimum discovery latency possible. After that, the algorithm stops due to performance reasons
 * @return Discovery latency and the discovery energy spent by the advertiser and the scanner
 */
discoveryModelResult_t _ble_model_discovery_getResultOnePhi(double epsilonHit, double Ta, double Ts, double ds, double phi,double rhoMax, double maxTime);

/**
 * \brief Returns the model results (discovery-latency and discover-energy both for advertiser and scanner) for varying advertising offsets phi.
 * nPoints different values of phi are examined.
 * @param nPoints Number of advertising events phi to be examined. The higher, the better the accuracy becomes but the longer the computation takes.
 * @param epsilonHit The hit probability of all advertising events examined for a particular phi. As soon as the algorithm has reached this probability, it will consider the results as stable and stop.
 * 		  The closer this value becomes to one, the more precise the results become, but the algorithm takes longer as more advertising events are examined.
 * 		  Example: The first advertising event hits with probability 0.5, the second with 0.25 and the third with 0.15. The hit probability would be 0.5+0.25+0.15 = 0.9
 * 		  If epsilonHit is 0.89, the algorithm would end after three advertising events
 * @param Ta Advertising interval [s]
 * @param Ts Scan interval [s]
 * @param ds Scan window [s]
 * @param rhoMax Maximum advertising delay [s]. Should be 10 ms according to the BLE specification
 * @param maxTime The maxmimum discovery latency possible. After that, the algorithm stops due to performance reasons
 * @return Discovery latency and the discovery energy spent by the advertiser and the scanner
 */
discoveryModelResult_t ble_model_discovery_getResult(uint32_t nPoints,double epsilonHit, double Ta, double Ts, double ds,double rhoMax, double maxTime);



#ifdef __cplusplus
}
#endif

#endif /* BLE_MODEL_DISCOVERY_H_ */
