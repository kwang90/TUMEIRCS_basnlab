/**
 * \file  ble_model_params_connected.h
 * \brief Device-dependant model params for Bluegiga BLE112 devices in connected mode
 *
 * This file contains device-dependant values for the connected mode for Bluegiga BLE112-devices.
 * The values have been optimized for the slave role, but should however match also master modes
 *
 * Change the values for different devices. For the model, please refer to the publication
 * "A precise energy model for the Bluetooth Low Energy Protocol" by Philipp Kindt
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

#ifndef BLE_MODEL_PARAMS_CONNECTED_H_
#define BLE_MODEL_PARAMS_CONNECTED_H_

#define BLE_E_MOD_C_DHEAD		0.578e-3			///< Duaration of head phase [s]
#define BLE_E_MOD_C_DPRE		0.305e-3			///< Duaration of prprocessing phase [s]
#define BLE_E_MOD_C_DCPRE		0.073e-3			///< Duaration of communication preamble phase [s]
#define BLE_E_MOD_C_DPRERX		0.123e-3			///< Duration of the prerx phase [s] for the master and for theslave except first rx-phase of slave within an event. The first duration of a slave is longer, see \ref BLE_E_MOD_C_DPRERX_SL1 . The prerx phase is the phase where the receiver is switched on, but no bits are transmitted. Therefore, the rx-phase is  by dprerx longer than 8 mikrosecods * bytes received
#define BLE_E_MOD_C_DPRERX_SL1	0.388e-3			///< Duration of the first prerx phase [s] of a slave. It is longer than different prerx phases and not related to window-widening. see \ref BLE_E_MOD_C_DPRERX_SL
#define BLE_E_MOD_C_DRXTX		0.08e-3				///< Duration of the Rx2Tx-phase [s]
#define BLE_E_MOD_C_DPRETX		0.053e-3			///< Duration of the pretx phase [s] (tx-phase is longer than 8 mikroseconds * bytes sent as the radio has to prepare)
#define BLE_E_MOD_C_DTXRX		0.057e-3			///< Duration of the Rx2Rx-phase [s]
#define BLE_E_MOD_C_DTRA		0.066e-3			///< Duration of the transient phase [s]
#define BLE_E_MOD_C_DPOST		0.860e-3			///< Duration of the posrprocessing phase [s]
#define BLE_E_MOD_C_DTAIL		0.08e-3				///< Duration of the tail phase [s]
#define BLE_E_MOD_C_IHEAD		5.924e-3			///< Current magnitude of the head phase [A]
#define BLE_E_MOD_C_IPRE		7.691e-3			///< Current magnitude of the preprocessing phase [A]
#define BLE_E_MOD_C_ICPRE		12.238e-3			///< Current magnitude of the communication preamble phase [A]
#define BLE_E_MOD_C_IRX			26.505e-3			///< Current magnitude of the reception phase [A]
#define BLE_E_MOD_C_IRXTX		14.128e-3			///< Current magnitude of the Rx2Tx phase [A]
#define BLE_E_MOD_C_ITX			36.445e-3			///< Current magnitude of the Tx phase [A]
#define BLE_E_MOD_C_ITXRX		15.125e-3			///< Current magnitude of the Tx2Rx phase [A]
#define BLE_E_MOD_C_ITRA		11.636e-3			///< Current magnitude of the transient phase [A]
#define BLE_E_MOD_C_IPOST		7.980e-3			///< Current magnitude of the postprocessing phase [A]
#define BLE_E_MOD_C_ITAIL		4.129e-3			///< Current magnitude of the tail phase [A]
#define BLE_E_MOD_C_QTO			-1.2e-6				///< Communication sequence correction offset [As]












#ifdef __cplusplus
}
#endif


#endif /* BLE_MODEL_PARAMS_CONNECTED_H_ */
