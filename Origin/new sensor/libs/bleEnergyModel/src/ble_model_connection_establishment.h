/**
 * \file ble_model_connection_establishment.h
 * \brief Energy model for BLE connection request procedures and for connection update procedures
 *
 * This file implements a model for connection parameter update procedures and for connection request procedures
 *
 * 2.10.2013, Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 * (c) 2013, Philipp Kindt
 * (c) 2013, Lehrstuhl für Realzeit-Computersysteme (RCS), Techinsche Universität München (TUM)
 *
 *	This file is part of bleemod.
 *
 *   bleemod is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   bleemod is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with bleemod.  If not, see <http://www.gnu.org/licenses/>

 */

#ifndef BLE_MODEL_CONNECTION_ESTABLISHENT_H_
#define BLE_MODEL_CONNECTION_ESTABLISHENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "ble_model_params_connection_establishment.h"
#include "ble_model_scanning.h"

/**
 * \brief type of the connection update event used for scheduling the update
 */
typedef enum _ble_e_model_ce_updateEventType {BLE_E_MODEL_CE_EVENT_TYPE_FULL, BLE_E_MODEL_CE_EVENT_TYPE_EMBEDDED, BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY} ble_e_model_ce_updateEventType;

/**
 * \brief Computes the charge consumed by establishing a connection or updating the connection parameters of an existing connection.
 *
 * This function computes the charge consumed by a connection request procedure or connection parameter update procedure.
 * A connection request procedure occures, after a scanner has received an ADV_IND advertising packet from an advertiser. After that,
 * the scanner might respond with an CON_REQ packet, which is the first part of the connection establishment procedure.
 * The packet lengths of packets involved in these procedures are taken from the values in \ref ble_model_params_connection_establishment.h
 *
 * The process of advertising and scanning until there is the first reception is not accounted for in this function. This process
 * is referred to as neighbor discovery and needs to be modeled separately.
 *
 * @param establishmentOrUpdate 1 => connection establishment procedure, 0 => connection parameter update
 * @param scanType Determines the connection comes about by continuous or periodic scanning. Only relevant for connection requests for initiators.
 * @param masterOrSlave 1 => master, 0 => slave
 * @param TcOld	Connection interval [s] before the parameter update. For connection establishment procdures, this value is ignored.
 * @param TcNew	The future connection interval [s] after the connection request or establishment procedure
 * @param eventType The type of update event. This might have the following values:<br>
 * - BLE_E_MODEL_CE_EVENT_TYPE_FULL: A full connection event (consisting of a pair of packets (update-req, empty response) is accounted for the update cost.
 *  This is normally never the case, since it only replaces another event
 * - BLE_E_MODEL_CE_EVENT_TYPE_EMBEDDED:
 *   Within a normal connection event, the update packet and its corresponding empty response packet is added as another pair of packets to an empty event.
 *   Might be the case for multiple pairs of packets
 * - BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY: This is always the case for confirmed mode with one pair of packets per event. In this case, the update packet is
 * sent instead of an empty packet to the slave. The overhead for the vent is a 22byte instead of a 10 byte packet, only.
 *  1 => The connection update is piggy-back'ed on a normal connection event that transports payload. This means that the effective cost is lowered, as most parts of the event take place anyway. 0=> A whole, dedicated connection event is counted as part of the update cost
 * This only applies for connection update procedures - not for connection establishment.
 * Example: payload is sent between master and slave in one pair of packets. The update takes place within this event as another pair of packets => the update vent cost is only one pair of packets without sleep/preprocessing/postprocessing, etc.
 *
 * @return Charge consumed by the parameter update
 */
double ble_e_model_ce_getChargeForConnectionProcedure(uint8_t establishmentOrUpdate, ble_e_model_sc_scan_type scanType, uint8_t masterOrSlave, double TcOld, double TcNew, ble_e_model_ce_updateEventType eventType);

#ifdef __cplusplus
}
#endif
#endif /* BLE_MODEL_CONNECTION_ESTABLISHENT_H_ */
