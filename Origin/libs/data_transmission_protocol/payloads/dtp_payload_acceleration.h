/*
 * dtp_payoad_acceleration.h
 * Simple payload profile for accelerometer data
 *
 *  Created on: 12.03.2013
 *      Author: kindt
 */

#ifndef DTP_PAYOAD_ACCELERATION_H_
#define DTP_PAYOAD_ACCELERATION_H_
#include "dtp_payload_type_ids.h"

typedef struct _dtp_payload_acceleration{
	int32_t aX;
	int32_t aY;
	int32_t aZ;
} dtp_payload_acceleration;

#endif /* DTP_PAYOAD_ACCELERATION_H_ */
