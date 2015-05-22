/**
 * \file sble_config_POSIX.h
 * \brief Plattform-dependant configuration for POSIX port.
 *
 *  \date 04.11.2012
 *  \author: Philipp Kindt
 */

#ifndef SBLE_CONFIG_POSIX_H_
#define SBLE_CONFIG_POSIX_H_

#include "sble_config.h"

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX

#include <inttypes.h>

//typedef unsigned long uint32_t;
//typedef long int32_t;

//Definitions for data-types

///A general-purpose boolean value. Plattform-dependant.
//typedef uint32_t sble_bool;

///A unsigned integer. Its size if platform-dependant.
//typedef uint32_t sble_unsigned_integer;

///A signed integer. Its size if platform-dependant.
//typedef int32_t sble_signed_integer;



#ifndef NULL
	#define NULL ((void*) 0)
#endif


///Values for sble_bool
#define SBLE_TRUE 1
#define SBLE_FALSE 0

//if this has value 1, time-profiling capabilities are added (see sble_state.h and bglib/commands.c)
#define SBLE_CFG_TIME_PROFILING 1



#endif
#endif /* SBLE_CONFIG_POSIX_H_ */
