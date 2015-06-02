/**
 * \file sble_config_FreeRTOS.h
 * \brief Plattform-dependant configuration for the FreeRTOS port.
 *
 *  \date 04.11.2012
 *  \author: Philipp Kindt
 */



#ifndef SBLE_CONFIG_FREERTOS_H_
#define SBLE_CONFIG_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sble_config.h"

#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS

#include <inttypes.h>

//typedef unsigned long uint32_t;
//typedef long int32_t;

//Definitions for data-types

///A general-purpose boolean value. Plattform-dependant.
typedef uint32_t sble_bool;

///A unsigned integer. Its size if platform-dependant.
typedef uint32_t sble_unsigned_integer;

///A signed integer. Its size if platform-dependant.
typedef int32_t sble_signed_integer;


///Values for sble_bool
#define SBLE_TRUE 1
#define SBLE_FALSE 0

#ifndef NULL
	#define NULL ((void*) 0)
#endif


///Stack size for callback dispatcher task
#define SBLE_CONFIG_FREERTOS_STACK_SIZE_CBD configMINIMAL_STACK_SIZE + 1024

///Stack size for thread synchronization task
#define SBLE_CONFIG_FREERTOS_STACK_SIZE_TS configMINIMAL_STACK_SIZE + 1024

#endif

#ifdef __cplusplus
}
#endif

#endif /* SBLE_CONFIG_FREERTOS_H_ */
