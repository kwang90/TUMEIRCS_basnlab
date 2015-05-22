/**
 * @file printBackTrace.cpp
 *
 * @brief Print a backtrace (list of active functions)
 *
 * Prints a backtrace of the current situation to stdout. Make sure that the linker-flag
 * -rdynamic is set, otherwise yo will just get raw addresses instead of symbols.
 * This code is taken from the Linux-kernel manpages @ http://www.kernel.org/doc/man-pages/
 * and has been slightly modified by Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 * Original copyright notice:
 * Copyrights: These man pages come under various copyrights.
 * All pages are freely distributable when the nroff source is included.
 *
 */
#include "sble_debug.h"
#include "sble_config.h"

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//--------------------[BEGIN Code from Linux Kernel]---------------------------------------------

void print_backtrace(){
    int j, nptrs;
#define SIZE 100
    void *buffer[100];
    char **strings;

   nptrs = backtrace(buffer, SIZE);
   fprintf(stderr,"\n--------------------------[BACKTRACE]-------------------------------\n");
    fprintf(stderr, "backtrace() returned %d addresses\n", nptrs);

   /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */

   strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

   for (j = 0; j < nptrs; j++)
        fprintf(stderr,"%s\n", strings[j]);

   free(strings);
   fprintf(stderr,"-------------------------[BACKTRACE END]-----------------------------\n");

};
//--------------------[END Code from Linux Kernel]---------------------------------------------
#endif
