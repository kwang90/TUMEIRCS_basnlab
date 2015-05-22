/*
 * dtp_payoad_rssi.h
 * Simple payload profile for BLE Received Signal Strength Indicator (RSSI)
 *
 *  Created on: 12.03.2013
 *      Author: kindt
 */

#ifndef DTP_PAYOAD_RSSI_H_
#define DTP_PAYOAD_RSSI_H_
#include "dtp_payload_type_ids.h"

typedef struct _dtp_payload_rssi{
	int8_t rssi;
} dtp_payload_rssi;

#endif /* DTP_PAYOAD_RSSI_H_ */
