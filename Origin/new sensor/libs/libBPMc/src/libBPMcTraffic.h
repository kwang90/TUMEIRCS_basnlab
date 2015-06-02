/**
 * \file libBPMcTraffic.h
 * \brief Structure representing traffic rates (bytes/second)
 *  Created on: 15.10.2013
 *      Author: kindt
 */

#ifndef LIBBPMCTRAFFIC_H_
#define LIBBPMCTRAFFIC_H_
#include <time.h>
#include <inttypes.h>

///Structure representing traffic rates
typedef struct _libBPMcTraffic{
	uint32_t nBytes;		///< Number of bytes transferred
	struct timespec time;	///< duration since the last duration or timestamp when the transmission occured (demends on the context used)
} libBPMcTraffic;

/**
 * \brief Compute number of seconds that have been passed between two points in time.
 * Assumption: t2 > t1 (otherwise, the returned difference will be negative)
 * @param t1 Point in Time 1
 * @param t2 Point in Tim2 2
 * @return Time between t2 and t1 [s]
 */
double libBPMc_Traffic_getTimeDifference(const struct timespec t1, const struct timespec t2);

#endif /* LIBBPMTCRAFFIC_H_ */
