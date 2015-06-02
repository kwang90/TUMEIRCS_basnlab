/*
 * \file libBPMTraffic.hpp
 * \brief Structure representing traffic rates (bytes/second)
 *  Created on: 15.10.2013
 *      Author: kindt
 */

#ifndef LIBBPMTRAFFIC_HPP_
#define LIBBPMTRAFFIC_HPP_
#include <time.h>

///Structure representing traffic rates
struct libBPMTraffic{
	uint32_t nBytes;		///< Number of bytes transferred
	struct timespec time;	///< Timestamp
};

#endif /* LIBBPMTRAFFIC_HPP_ */
