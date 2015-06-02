/*
 * libBPMcThrougputCalc.h
 *
 *  Created on: 01.04.2014
 *      Author: kindt
 */

#ifndef LIBBPMCRATECALC_H_
#define LIBBPMCRATECALC_H_

#include <inttypes.h>
uint32_t libBPMcRateCalc_rate2TcAvg(double rate, double nSeq);
uint32_t libBPMcRateCalc_rate2TcWC(double rate, double nSeq);
double libBPMcRateCalc_Tc2RateAvg(uint32_t Tc, double nSeq);
double libBPMcRateCalc_Tc2RateWC(uint32_t Tc, double nSeq);

#endif /* LIBBPMCRATECALC_H_ */
