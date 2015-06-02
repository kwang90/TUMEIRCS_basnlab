/*
 * libBPMcDebug.h
 *
 *  Created on: 04.11.2013
 *      Author: kindt
 */

#ifndef LIBBPMCDEBUG_H_
#define LIBBPMCDEBUG_H_

//#define LIBBPMC_DEBUG(...) printf("[%s,l. %d, %s @ %s] ",__FILE__, __LINE__, __FUNCTION__, pcTaskGetTaskName(NULL)); printf(__VA_ARGS__); printf("\n");
#include <stdio.h>

//#define LIBBPMC_DEBUG(...) fprintf(stderr,"[DEBUG: %s,l. %d, %s] ",__FILE__, __LINE__, __FUNCTION__); fprintf(stderr,__VA_ARGS__); fprintf(stderr,"\n");
#define LIBBPMC_DEBUG(...)
#endif /* LIBBPMCDEBUG_H_ */
