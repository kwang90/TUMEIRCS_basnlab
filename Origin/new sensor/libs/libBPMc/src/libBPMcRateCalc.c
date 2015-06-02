/*
 * libBPMcThrougputCalc.h
 *
 *  Created on: 01.04.2014
 *      Author: kindt
 */



#include "libBPMcParams.h"
#include "libBPMcRateCalc.h"

#include <math.h>

uint32_t libBPMcRateCalc_rate2TcAvg(double rate, double nSeq){
	return floor (((nSeq*20.0/((double) rate))*(1.0 - LIBBPMC_PARAM_AVERAGE_THROUGHPUT_DEGENERATION_RELATIVE)) / 0.00125);

}
uint32_t libBPMcRateCalc_rate2TcWC(double rate, double nSeq){
	return floor (((nSeq*20.0/((double) rate))*(1.0 - LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION)) / 0.00125);

}
double libBPMcRateCalc_Tc2RateAvg(uint32_t Tc, double nSeq){
	return (nSeq*20.0/((double) Tc*0.00125) * (1.0 - LIBBPMC_PARAM_AVERAGE_THROUGHPUT_DEGENERATION_RELATIVE));
}


double libBPMcRateCalc_Tc2RateWC(uint32_t Tc, double nSeq){
	return nSeq*20.0/((double) Tc*0.00125) * (1.0 - LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION);
}

