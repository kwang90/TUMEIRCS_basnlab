/**
 * \file sble_debug.h
 * \brief Debugging tools for SBLE
 *
 * Debugging tools to write on stdout. The implementations of the Debug-Macros is plattform-dependant, see the appropriate port's headers.
 *
 *  \date 06.07.2012
 *  \author Philipp Kindt
 */

#ifndef SBLE_DEBUG_H_
#define SBLE_DEBUG_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "sble_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
#include "ports/POSIX/sble_debug_POSIX.h"

#elif SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS

#include "ports/FreeRTOS/sble_debug_FreeRTOS.h"

#else
/**Print a debug message to STDOUT.
 * File, Line and Function are printed in addition to the debug message itself.
 * Syntax: The same as for printf(). It will terminate the message with a newline automatically.
 */
#define  NUMARGS(...)  (sizeof((int[]){0, ##__VA_ARGS__})/sizeof(int)-1);
void test_delay();
//#define SBLE_DEBUG(...) write(1,"nanananananananananananananananananananananananana\n",51);
//#define SBLE_DEBUG(...) {unsigned int cnt = NUMARGS(__VA_ARGS__); while(cnt > 0){ cnt--; hhd_usb_cdc_write("testtesttesttesttesttesttesttesttest1234567890\n",47);}}

#define SBLE_DEBUG(...)

/**Print a debug message to STDOUT.
 * File, Line and Function are not printed in addition to the debug message itself.
 * Syntax: The same as for printf(). It won't terminate the message with a newline automatically.
 */
#define	 SBLE_DEBUG_CON(...) //tfp_printf(__VA_ARGS__);

/**Print a rrror message to STDOUT and terminate the program.
 * File, Line and Function are printed in addition to the error message itself.
 * Syntax: The same as for printf(). It will terminate the message with a newline automatically.
 */
#define SBLE_ERROR(...)

/**Print a rrror message to STDOUT, but do not terminate the program.
 * File, Line and Function are printed in addition to the error message itself.
 * Syntax: The same as for printf(). It will terminate the message with a newline automatically.
 */
#define SBLE_ERROR_CONTINUABLE(...)

/**Print a debug message to STDOUT.
 * File, Line and Function are printed in addition to the debug message itself.
 * Syntax: The same as for printf(). It will terminate the message with a newline automatically.
 */




#endif

/**
* Print the data of an array as hex-string to stdout
* \param data Pointer to data to print
* \param len Number of bytes to print - max. the lenght of the data-array
*/
void sble_print_hex_array(const uint8_t *data, uint32_t len);

/**
* Print the data of an array as their asci-chars to stdout
* \param data Pointer to data to print
* \param len Number of bytes to print - max. the lenght of the data-array
*/
void sble_print_char_array(const uint8_t *data, uint32_t len);

/**
 *
 * \brief Prints the bit values (0 or 1) of given bitfield
 * \param field The value to print
 * \param lenght the numer of bits to print. Max. sizeof(field)!
 */
void sble_print_bitfield(sble_unsigned_integer field, sble_unsigned_integer length);

/**
* \brief Prints a backtrace of the current call-situation to stdout.
*
*  Make sure that the linker-flag
* -rdynamic is set, otherwise yo will just get raw addresses instead of symbols.
* This code is taken from the Linux-kernel manpages @ http://www.kernel.org/doc/man-pages/
* and has been slightly modified by Philipp Kindt <kindt@rcs.ei.tum.de>
*
* Original copyright notice:
* Copyrights: These man pages come under various copyrights.
* All pages are freely distributable when the nroff source is included.
*/
void print_backtrace();



#ifdef __cplusplus
}
#endif

#endif /* SBLE_DEBUG_H_ */
