/**
 * \file sble_debug_posix.h
 * \brief Debugging tools for SBLE - FreeRTOS-Port
 *
 * Debugging tools to write on stdout.
 *
 *  4.11.2012
 *  Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#ifndef SBLE_DEBUG_FREERTOS_H_
#define SBLE_DEBUG_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sble_config.h"

#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <FreeRTOS.h>

#define SBLE_DEBUG_MESSAGES_FREERTOS 0	 //enable debugging output for the FreeRTOS port. This is a risky thing as debug-messages within the BLE scheduler might cause scheduling problens and the system might hang.

#define SBLE_DEBUG_RTOS_KERNEL 1


#if SBLE_DEBUG_MESSAGES_FREERTOS==1
/**Enable RTOS kernel debugging if defined
 *
 *
 */

/**Print a debug message to STDOUT.
 * File, Line and Function are printed in addition to the debug message itself.
 * Syntax: The same as for printf(). It will terminate the message with a newline automatically.
 */
#define SBLE_DEBUG(...) printf("[DEBUG: %s,l. %d, %s @ %s] ",__FILE__, __LINE__, __FUNCTION__,pcTaskGetTaskName(NULL)); printf(__VA_ARGS__); printf("\n");
//#define SBLE_DEBUG(...) printf(__VA_ARGS__); printf("\n");
//#define SBLE_DEBUG(...) printf("[DEBUG: %s,l. %d, %s] ",__FILE__, __LINE__, __FUNCTION__); printf(__VA_ARGS__); printf("\n");

//tfp_printf("[%s, l.%d @ %s,] ",__FUNCTION__,__LINE__,pcTaskGetTaskName(NULL));
/**Print a debug message to STDOUT.
 * File, Line and Function are not printed in addition to the debug message itself.
 * Syntax: The same as for printf(). It won't terminate the message with a newline automatically.
 */
#define	 SBLE_DEBUG_CON(...) printf(__VA_ARGS__);

/**Print a rrror message to STDOUT and terminate the program.
 * File, Line and Function are printed in addition to the error message itself.
 * Syntax: The same as for printf(). It will terminate the message with a newline automatically.
 */
#define SBLE_ERROR(...) tfp_printf("[ERROR: %s,l. %d, %s @ %s] ",__FILE__, __LINE__,__FUNCTION__,pcTaskGetTaskName(NULL)); printf(__VA_ARGS__); printf("\n"); vTaskEndScheduler(); while(1){;};

/**Print a rrror message to STDOUT, but do not terminate the program.
 * File, Line and Function are printed in addition to the error message itself.
 * Syntax: The same as for printf(). It will terminate the message with a newline automatically.
 */
#define SBLE_ERROR_CONTINUABLE(...) tfp_printf("[ERROR_CONTINUABLE: %s,l. %d] ",__FILE__, __LINE__); printf(__VA_ARGS__); printf("\n");

#else
#define SBLE_DEBUG(...)
#define SBLE_DEBUG_CON(...)
#define SBLE_ERROR(...)
#define SBLE_ERROR_CONTINUABLE(...)
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* SBLE_DEBUG_FREERTOS_H_ */
