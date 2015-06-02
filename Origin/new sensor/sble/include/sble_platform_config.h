/**
 * \file sble_platform_config.h
 * \brief Plattform-dependant configuration
 *
 *  This file must be adjusted (among some others) if SBLE is beeing ported to a different platform. It
 *  contains platform-secific configruation values.
 *  \date 06.07.2012
 *  \author: kindt
 */

#ifndef SBLE_PLATFORM_CONFIG_H_
#define SBLE_PLATFORM_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif

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


///an ident for the posix-plattform
#define SBLE_PLATTFORM_ARCHITECTURE_POSIX 1
#define SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS 2

/**The actual plattform architecture
 * possible values:
 * \arg SBLE_PLATTFORM_ARCHITECTURE_POSIX => Unix/Posix (ISO/IEEE 9945)
 * \arg SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS => FreeRTOS on stm32f4 providing read/write I/O to ble module
 */
#ifndef SBLE_PLATTFORM_ARCHITECTURE
#define SBLE_PLATTFORM_ARCHITECTURE SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#endif

/**
 * The maximum lenght for all reception buffers. Memory will only be allocated if they're really filled that much.
 */
#define SBLE_BUF_MAXLEN 50


///if defined, the code is optimized for speed at the cost of Debuggability.
#define SBLE_FASTMODE 0


///Values for sble_bool
#define SBLE_TRUE 1
#define SBLE_FALSE 0

#ifndef NULL
	#define NULL ((void*) 0)
#endif
#ifdef __cplusplus
}
#endif

#endif /* SBLE_PLATFORM_CONFIG_H_ */
