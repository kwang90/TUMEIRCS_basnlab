/**
 * \file sble_scheduler.h
 * \brief Scheduling for sble. This header file is plattform-dependant. See the ports implementation!
 *
 *
 *  \date 06.07.2012
 *  \author: Philipp Kindt
 */

#ifndef SBLE_SCHEDULER_H_
#define SBLE_SCHEDULER_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "sble_config.h"

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX

#include "ports/POSIX/sble_scheduler_POSIX.h"

#elif SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#include "ports/FreeRTOS/sble_scheduler_FreeRTOS.h"

#endif
#ifdef __cplusplus
}
#endif

#endif /* SBLE_SCHEDULER_H_ */
