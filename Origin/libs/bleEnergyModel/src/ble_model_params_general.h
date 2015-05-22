/**
 * \file  ble_model_params_general.h
 * \brief Device-dependant model params for Bluegiga BLE112 devices that are independant from the mode (connected/advertising/scanning/...)
 *
 * This file contains device-dependant values for Bluegiga BLE112-devices that are independant from the mode.
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

#ifndef BLE_MODEL_PARAMS_GENERAL_H_
#define BLE_MODEL_PARAMS_GENERAL_H_

#define BLE_E_MOD_G_SCA			50				///> sleep clock accuracy (ppm)
#define BLE_E_MOD_G_ISL			0.9e-6			///> sleep current [A]



#ifdef __cplusplus
}
#endif



#endif /* BLE_MODEL_PARAMS_CONNECTED_H_ */
