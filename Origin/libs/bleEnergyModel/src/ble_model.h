/**
 * \file ble_model.h - Main include file for Eneergy Model for the Bluetooth Low Energy Protocol
 *
 *
 * \brief Master-include for the BLE Power model
 * \date: 1.10.2013
 * \author Philipp Heinrich Kindt
 *
 * \mainpage BLEeMod: A precise energy model for the Bluetooth Low Energy Protocol
 *
 * \section sec_credits Credits:
 * Power Model: 2013, Philipp Kindt <kindt@rcs.ei.tum.de>
 * 				2013, Daniel Yunge <yunge@rcs.ei.tum.de>
 * 				2013, Robert Diemer <diemer@rcs.ei.tum.de>
 *				2013, Samarjit Chakraborty <samarjit@tum.de>
 *
 * This Software:
 * 				2013, Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 *
 *
 *
 *
 * \section sec_intro Introduction
 * This library contains an easy-to-use and precise energy model for the BLE Protocol.
 * The Data Included is valid for BLE112-devices. The data can be changed for other devices. How to do this: Please
 * refer to the publication "A precise Energy Model for the Bluetooth Low Energy Protocol".
 *
 * All energies are presented in the units of electric charge [As]. By multiplying these values with the supply voltage of the BLE module,
 * Energy in [J] is obtained.
 *
 * \section sec_compile Compiling
 * The library compiles on Linux, libmath from the GNU C library  (or any other compatible c-library providing math.h) must be linked ("-lm" - flag).
 * To compile the examples, go to the examples directory and type "make".
 *
 * \section sec_porting Porting
 * This programm compiles under linux/posix only, if the \ref BLE_MODEL_PLATTFORM macro is set to \ref BLE_MODEL_PLATTFORM_POSIX.
 * In this case won't compile on embedded platforms, but can easily ported:
 * When there is an error, it calls printf() exit(). This is the only platform-dependand call. set\ref BLE_MODEL_PLATTFORM to \ref BLE_MODEL_PLATTFORM_OTHER,
 * and it will compile on any plattform that comes with a c-compiler! Disadvantage: No error messages are issued.
 * .
 * The software does not need malloc().
 *
 *
 * \section sec_license License
 *
 * (c) 2013, Philipp Kindt
 * (c) 2013, Lehrstuhl für Realzeit-Computersysteme (RCS), Techinsche Universität München (TUM)
 *
 *	This file is part of bleemod.
 *
 *  bleemod is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  bleemod is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with bleemod.  If not, see <http://www.gnu.org/licenses/>
 *
 * The function _ble_model_discovery_gausscdf(double x) in ble_model_discovery.c
 * is taken from http://www.johndcook.com/cpp_phi.html
 * It is released in public domain according to the website mentioned above:
 * "This code is in the public domain. Do whatever you want with it, no strings attached."
 *
 * \example examples.c
 */

#ifndef BLE_MODEL_H_
#define BLE_MODEL_H_

#define BLE_MODEL_PLATTFORM_POSIX 1			///< possible value for the \ref BLE_MODEL_PLATTFORM : Posix/Linux operating systems. Only advantage when enabling this: The program will abort with exit() in case of an error
#define BLE_MODEL_PLATTFORM_OTHER 2			///< possible value for the \ref BLE_MODEL_PLATTFORM : any other plattform

///can be set to BLE_MODEL_PLATTFORM_POSIX if it is going to be compiled for Linux/Posix. Otherwise, change to any different value to compile without posix libraries. Only disadvantage: Programm does not abort with exit() in case of error messages.
#define BLE_MODEL_PLATTFORM BLE_MODEL_PLATTFORM_POSIX

#include "ble_model_connected.h"				///< model for the connected mode
#include "ble_model_scanning.h"					///< model for scan events
#include "ble_model_connection_establishment.h"	///< model for connection establishment procedures and update procedures
#include "ble_model_discovery.h"				///< model for device discovery




#endif /* BLE_MODEL_H_ */
