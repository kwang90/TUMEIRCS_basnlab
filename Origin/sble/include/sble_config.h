/**
 * \file sble_config.h
 * \brief Configuration file for SLBE. Will include a platform-dependant configuration file.
 *
 *
 *  \date 06.01.2013
 *  \author: Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#ifndef SBLE_CONFIG_H_
#define SBLE_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "sble_platform_config.h"

#define SBLE_PARAMETER_MAX_SEQUENCES 5

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX

#include "ports/POSIX/sble_config_POSIX.h"

#elif SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#include "ports/FreeRTOS/sble_config_FreeRTOS.h"

#endif


#ifdef __cplusplus
}
#endif

#endif /* SBLE_CONFIG_H_ */
