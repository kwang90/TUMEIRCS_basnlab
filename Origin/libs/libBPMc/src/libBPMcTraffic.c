/*
 * libBPMcTraffic.c
 *
 *  Created on: 24.10.2013
 *      Author: kindt
 */


#include "libBPMcTraffic.h"

double libBPMc_Traffic_getTimeDifference(const struct timespec t1, const struct timespec t2){
	return (double) t2.tv_sec + ((double) t2.tv_nsec / 1000000000l) - (double) t1.tv_sec - ((double) t1.tv_nsec / 1000000000l);

}



