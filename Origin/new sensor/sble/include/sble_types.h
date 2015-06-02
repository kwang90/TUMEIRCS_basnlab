/**
 * \file sble_types.h
 *	\brief Data Types master include
 *
 *	This file includes other header files of SBLE that contain many data-types.
 *
 *  \date 09.07.2012
 *  \author: Philipp Kindt
 */

#ifndef SBLE_TYPES_H_
#define SBLE_TYPES_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>

#include "sble_payload.h"
#include "sble_attribute.h"
#include "sble_array.h"
#include "sble_ll.h"
#include "sble_config.h"

typedef void (*sble_reconnect_callback)();
#ifdef __cplusplus
}
#endif

#endif /* SBLE_TYPES_H_ */
