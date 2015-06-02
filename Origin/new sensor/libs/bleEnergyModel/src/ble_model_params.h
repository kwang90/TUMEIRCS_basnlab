/**
 * \file ble_model_params.h
 * \brief master include for all numerical values.

 * The only purpose of this file is including further headers that provide all numerical data
 * values for the model parameters.
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

#ifndef BLE_MODEL_PARAMS_H_
#define BLE_MODEL_PARAMS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "ble_model_params_general.h"					///< General parameters that do not correspond to a certain mdoe
#include "ble_model_params_connected.h"					///< Parameters for connection and advertising events
#include "ble_model_params_scanning.h"					///< Parameters for scan events
#include "ble_model_params_connection_establishment.h"	///< Parameters for establishing and updating a connection

#ifdef __cplusplus
}
#endif
#endif /* BLE_MODEL_PARAMS_CONNECTED_H_ */
