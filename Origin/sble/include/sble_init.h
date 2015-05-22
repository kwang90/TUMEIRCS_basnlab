/**
 * \file sble_init.h
 * \brief Initialization and shutdown of SBLE.
 *
 *  \date 16.07.2012
 *  \author: kindt
 */

#ifndef SBLE_INIT_H_
#define SBLE_INIT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "sble_init.h"
#include "sble_config.h"
#include "sble_scheduler.h"
#include "sble_debug.h"
#include "../bglib/cmd_def.h"
#include "sble_bgapi_call.h"
#include <inttypes.h>

/**
 * Initialize SBLE\n
 * Starts the callback dispatcher thread and initializes all data structures. Tries to connect to the BLE112 device.
 */
void sble_init(uint8_t* param);


/**
 * Shut down SBLE\n
 * Stops the callback dispatcher thread and disconnects the I/O to the BLE112 device.
 */
void sble_shutdown();

#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS

#include "ports/FreeRTOS/sble_init_FreeRTOS.h"

#endif
#ifdef __cplusplus
}
#endif

#endif /* SBLE_INIT_H_ */
