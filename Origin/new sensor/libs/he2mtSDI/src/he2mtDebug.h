/**
 * \file he2mtDebug.h - Simple debugging tool that defines a printf-analogue macro HE2MT-DEBUG
 *
 *  Created on: 29.04.2014, Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 */

#ifndef HE2MTDEBUG_H_
#define HE2MTDEBUG_H_
#include <stdio.h>
#define HE2MT_DEBUG(...) printf("[%s,l. %d, %s] ",__FILE__, __LINE__, __FUNCTION__); printf(__VA_ARGS__); printf("\n");

#endif /* HE2MTDEBUG_H_ */
