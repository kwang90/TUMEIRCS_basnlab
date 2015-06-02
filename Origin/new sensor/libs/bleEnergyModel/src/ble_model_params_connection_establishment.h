/**
 * \file ble_model_params_connection_establishment.h
 * \brief Energy model params for connection establishment and connection parameter updates
 *
 * 2.10.2013, Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 * This file contains parmeter values for BLE112-devices for connection request procedures and
 * connection parameter updates.
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLE_MODEL_PARAMS_CONNECTION_ESTABLISHMENT_H_
#define BLE_MODEL_PARAMS_CONNECTION_ESTABLISHMENT_H_


#define BLE_E_MOD_CE_DTWO_CU			0							///< Transmit window offset for connection update procedures


 /// Transmit window offset for connection request procedures. Contains a nonlinear formula depending on the future connection interval TcNew
#define BLE_E_MOD_CE_DTWO_CR(TcNew)		((TcNew > 0.0125)?(TcNew - 0.006454):(0.389*TcNew+0.000484))
#define BLE_E_MOD_CE_DTW(TcNew)			0.003						///< Transmit window. Might depend on the future connection interval TcNew; for BLE112 devices it does not.
#define BLE_E_MOD_CE_DP(TcNew)			BLE_E_MOD_CE_DTW(TcNew)/2.0	///< The average time the transmission begins after the beginning of the transmit window



//Some packet lengths occurring in connection establishment / update sequences.
#define BLE_E_MOD_CE_ADV_IND_PKG_LEN	37							///< Number of bytes sent in an ADV_IND advertising packet by the advertiser
#define BLE_E_MOD_CE_CON_REQ_LEN		44							///< Number of bytes sent in an CONNECT_REQ packet by the initiator (former scanner)
#define BLE_E_MOD_CE_CON_UP_LEN			22							///< Number of bytes sent in an LL_CONNECTION_UPDATE_REQ packet by the master
#define BLE_E_MOD_CE_CON_UP_SLRSP_LEN	10							///< Number of bytes sent by the slave to the master in the event an LL_CONNECTION_UPDATE_REQ packet has been received
#define BLE_E_MOD_CE_CON_UP_TXPOWER		3							///< Tx power level for connection update
#define BLE_E_MOD_CE_EMPTY_PACKET_LEN	10							///< Number of bytes sent within an empty polling packet

#ifdef __cplusplus
}
#endif

#endif /* BLE_MODEL_PARAMS_CONNECTION_ESTABLISHMENT_H_ */
