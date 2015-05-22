/*
 * libBPMcEmergencyProcedure.c
 *
 *  Created on: 08.11.2013
 *      Author: kindt
 */

#include "libBPMcEmergencyProcedure.h"
#include "libBPMcParams.h"
#include "libBPMcDebug.h"
#include <math.h>

uint32_t libBPMcEmergencyProcedure_getWorstCaseMinConnectionInterval(libBPMcBuffer* buf, uint32_t tcOld, double nSeq, double maxRate){
	uint32_t newInterval;
		double bytesFree = libBPMcBuffer_getBytesLeft(buf);
		//printf("==[free: %.1f by, maxRate = %f by/s, nSeq = %f]\n",bytesFree,maxRate,nSeq);

		double windowBufferSpace = maxRate * LIBBPMC_PARAM_WORST_CASE_WINDOW - LIBBPMC_PARAM_WORST_CASE_WINDOW * maxRate * (1.0-LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION);

		//						 40   * Nseq *  Ni,m																				*  (1 - nywc)				   -			Tc,old					* Ni,m       									  * Rapp,max 						   +	Nb,f			- Nwnd 						- Nmaxw									/ ( Rapp,max 					  * Ni,m)
		double newIntervalPhy = (40.0 * nSeq * ((double) LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE ) *  (1.0 - LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION) - (double) tcOld * 0.00125 * (double) LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * maxRate + (double) bytesFree - (double) windowBufferSpace - (double) LIBBPMC_PARAMS_IO_R_LENGTH)/(maxRate * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE);


		if(newIntervalPhy <= 0){
			newInterval = 6;
		}else{

			newInterval = floor(newIntervalPhy/0.00125);
		}

		if(newInterval < 6){
				newInterval = 6;
			}
//		printf("-> emergency Interval is %u (bytesFree = %f,  maxRate = %f) \n",newInterval,bytesFree, maxRate);
//		newInterval = 9999;
		return newInterval;
}


