/**
 * \file sble_init_FreeRTOS.h
 * \brief Initialization and shutdown of SBLE - FreeRTOS-port dependant part.
 *
 *  \date 16.07.2012
 *  \author: kindt
 */

#ifndef SBLE_INIT_FREERTOS_H_
#define SBLE_INIT_FREERTOS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "sble_init.h"
#include <inttypes.h>

#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS

/**
 * Initialize SBLE with wake up function callback and reset callback\n
 * Starts the callback dispatcher thread and initializes all data structures. Tries to connect to the BLE112 device.
 */
void sble_init_hw(uint8_t* param,sble_hw_callback_t wuf, sble_hw_callback_t reset);

#endif

#ifdef __cplusplus
}
#endif

#endif /* SBLE_INIT_FREERTOS_H_ */
