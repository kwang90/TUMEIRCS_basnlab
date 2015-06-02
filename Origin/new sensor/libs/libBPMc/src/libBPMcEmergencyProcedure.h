/**
 * \file libBPMcEmergencyProcedure.h
 * \brief Functions helping the BLE SLI to determine data for initiating the emergency procedure
 *
 *  Created on: 08.11.2013
 * \author Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#ifndef LIBBPMCEMERGENCYPROCEDURE_H_
#define LIBBPMCEMERGENCYPROCEDURE_H_
#include "libBPMcBuffer.h"


/**
 * \brief Calculates the worst case maximal connection interval that still guarantees that the buffer does not overflow if the maximum traffic rate occurs
 * @param buf Pointer to the output buffer
 * @param tc Current connection interval in multiples of 1.25 ms
 * @param nSeq Number of packet pairs per connection interval
 * @param nIntervalsLeft Number of connection intervals left until an update can take place
 * @param maxRate The Maximum traffic rate the application might produce
 * @return Maximum connection interval in multiples of 1.25 ms
 */
uint32_t libBPMcEmergencyProcedure_getWorstCaseMinConnectionInterval(libBPMcBuffer* buf, uint32_t tcOld, double nSeq, double maxRate);

#endif /* LIBBPMCEMERGENCYPROCEDURE_H_ */
