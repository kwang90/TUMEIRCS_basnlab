/**
 * \file ble_model_scanning.h
 * \brief Energy model for BLE scan events
 *
 * 2.10.2013, Philipp Kindt <kindt@rcs.ei.tum.de>
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

#ifndef BLE_MODEL_SCANNING_H_
#define BLE_MODEL_SCANNING_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "ble_model_params_scanning.h"

#include "inttypes.h"

///The type of scan event
typedef enum _ble_e_model_sc_event_type_t{
	SC_EVENT_TYPE_NO_RECEPTION,				///< Scan event that does not receive anything
	SC_EVENT_TYPE_PASSIVE_SCANNING,			///< Scan event for passive scanning. No matter if advertising packets are received or not, this type is valid for passive scanning. Only exception: A CON_REQ packet is sent
	SC_EVENT_TYPE_ACTIVE_SCANNING,			///< Scan event for active scanning without connecting. An advertising packet is received within this event, otherwise, it would be idle scanning and \ref SC_EVENT_TYPE_NO_RECEPTION must be used.
	SC_EVENT_TYPE_CON_REQ,					///< Connection request packet is sent within that scan event. Takes into account whole scan event.
	SC_EVENT_TYPE_CON_REQ_OFFSET,			///< Connection request packet is sent within that scan event. This type only takes into account the energy spent for the additional energy compared to an idle scan event without reception which is aborted after the reception of an ADV_IND packet. This energy constists of the CON_REQ packet and drxtx, only! It is used for compability with the discovery energy model: To geht the energy for discovery + connection, add the discovery energy and a packet of this type!
	SC_EVENT_TYPE_ABORTED			///< Synonym for SC_EVENT_TYPE_NO_RECEPTION, but ends after receptionAfterTime of \ref ble_e_model_sc_getChargeScanEvent(), leading to a shorter effective scan duration. It should be used for 'aborted' passive scanning event aborted after a certain amount of time due to a successful event. \ref SC_EVENT_TYPE_CON_REQ_OFFSET + SC_EVENT_TYPE_ABORTED should model the whole event.
} ble_e_model_sc_event_type_t;


///Determines weather we use continuous or periodic scanning
typedef enum _ble_e_model_sc_scan_type_t{
	SC_SCAN_TYPE_PERIODIC,					///< Scan window < scan interval => periodic scanning
	SC_SCAN_TYPE_CONTINUOUS					///< Scan window = scan interval => continuous scanning
} ble_e_model_sc_scan_type;


/**
 * \brief Calculates the charge consumed by a scan event.
 * This function calculates the charge consumed by a scan event. Different event types are supported.
 * Further, it is distinguished between continous scanning and periodic scaning. For periodic scanning,
 * each event begins with preprocessing and ends with postprocessing.
 * For continuous scanning, no beginning and end can be distinguished. Therefore, the event duration is determined by definition: The event beginns when a scanner
 * begins scanning on a certain channel and ends with the end of the channel-changing to the next channel. Therefore, each scan event contains on channel-changing phase.
 *
 * @param scanWindow			Scan window [s]. for SC_EVENT_TYPE_CON_REQ_OFFSET, this parameter is discarded
 * @param eventType					Type of the scan event that occurs.
 * @param scanType				Determines weather periodic scanning (scan window < scan interval) or continous scanning (scan window = scan interval) takes place.
 * @param nBytesAdvInd			Bytes of the ADV_IND packet received. This value is currently ignored, it may be set to any value. It is reserved for future use.
 * @param nBytesTx				Number of bytes sent in a scan request or connection request packet by the master . Only used for \ref SC_EVENT_TYPE_ACTIVE_SCANNING, \ref SC_EVENT_TYPE_CON_REQ and \ref SC_EVENT_TYPE_CON_REQ_OFFSET
 * @param nBytesRx				Number of bytes received in a scan response. Only used for \ref SC_EVENT_TYPE_ACTIVE_SCANNING .
 * @param receptionAfterTime	Number of seconds beginning from the scan event after which an advertising packet has been received comletely. As this value is unknown most times, the beginning of the reception can be inserted
 * @return						Charge consumed by the scan event [As]
 */
double ble_e_model_sc_getChargeScanEvent(double scanWindow, ble_e_model_sc_event_type_t eventType, ble_e_model_sc_scan_type scanType, uint8_t nBytesAdvInd, uint8_t nBytesTx, uint8_t nBytesRx, double receptionAfterTime);
#ifdef __cplusplus
}
#endif
#endif /* BLE_MODEL_SCANNING_H_ */
