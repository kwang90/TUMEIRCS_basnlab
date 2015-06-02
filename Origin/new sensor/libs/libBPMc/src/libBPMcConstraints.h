/**
 * \file libBPMcConstraints.h
 *  Compute maxium connection interval and required buffer size given a latency constraint and a max. Throughput rate.
 *
 *  \date 10.12.2013
 *  \author: Philipp Kindt <kindt@rcs.ei.tum.>
 */

#ifndef LIBBPMCCONSTRAINTS_H_
#define LIBBPMCCONSTRAINTS_H_
#include <math.h>
#include <inttypes.h>

uint32_t libBPMcConstraints_getMaxConnectionInterval(double nSeq, uint32_t nBytesControl, uint32_t nBytesMaxWrite, double rAppMax, double tLatencyMax, uint32_t qualityOfServiceTcReduction);
uint32_t libBPMcConstraints_getBufferSize(double nSeq, uint32_t nBytesControl, uint32_t nBytesMaxWrite, double rAppMax, double tLatencyMax, uint32_t qualityOfServiceTcReduction);
#endif /* LIBBPMCCONSTRAINTS_H_ */
