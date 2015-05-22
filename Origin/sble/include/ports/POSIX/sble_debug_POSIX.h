/**
 * \file sble_debug_posix.h
 * \brief Debugging tools for SBLE - POSIX-Port
 *
 * Debugging tools to write on stdout.
 *
 *  4.11.2012
 *  Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#ifndef SBLE_DEBUG_POSIX_H_
#define SBLE_DEBUG_POSIX_H_
#include "sble_config.h"

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
#include <stdio.h>
//0 -> no debug messages | 1 -> debug messages
#define SBLE_DEBUG_MESSAGES_POSIX 0

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>

#if SBLE_DEBUG_MESSAGES_POSIX == 1
/**Print a debug message to STDOUT.
 * File, Line and Function are printed in addition to the debug message itself.
 * Syntax: The same as for printf(). It will terminate the message with a newline automatically.
 */
#define SBLE_DEBUG(...) fprintf(stderr,"[DEBUG: %s,l. %d, %s] ",__FILE__, __LINE__, __FUNCTION__); fprintf(stderr,__VA_ARGS__); fprintf(stderr,"\n");
//#define SBLE_DEBUG(...)
/**Print a debug message to STDOUT.
 * File, Line and Function are not printed in addition to the debug message itself.
 * Syntax: The same as for printf(). It won't terminate the message with a newline automatically.
 */
#define	 SBLE_DEBUG_CON(...) fprintf(stderr,__VA_ARGS__);
//#define	 SBLE_DEBUG_CON(...)
#else
#define SBLE_DEBUG(...)
#define	SBLE_DEBUG_CON(...)
#endif
/**Print an error message to STDOUT and terminate the program.
 * File, Line and Function are printed in addition to the error message itself.
 * Syntax: The same as for printf(). It will terminate the message with a newline automatically.
 */
#define SBLE_ERROR(...) fprintf(stderr,"[ERROR: %s,l. %d, %s] ",__FILE__, __LINE__,__FUNCTION__); printf(__VA_ARGS__); printf("\n"); exit(1);

/**Print a error message to STDOUT, but do not terminate the program.
 * File, Line and Function are printed in addition to the error message itself.
 * Syntax: The same as for printf(). It will terminate the message with a newline automatically.
 */
#define SBLE_ERROR_CONTINUABLE(...) fprintf(stderr,"[ERROR_CONTINUABLE: %s,l. %d] ",__FILE__, __LINE__); printf(__VA_ARGS__); printf("\n");

#endif
#endif /* SBLE_DEBUG_POSIX_H_ */
