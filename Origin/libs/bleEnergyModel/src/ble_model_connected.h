/**
 * \file ble_model_connected.h
 * \brief BLE Energy model for the connected mode
 *
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


#ifndef BLE_MODEL_CONNECTED_H_
#define BLE_MODEL_CONNECTED_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <inttypes.h>

///maximum number of communication sequences possible (for pre-mallocking to avoid the use of malloc)
#define BLE_E_MODE_INT_MAXSEQUENCES 15

/** Returns the charge of all constant parts of the model.
 *  These are: head, preprocessing, transient state, postprocessing, tail
 * 	@return charge of all constant parts [C]
 */
double ble_e_model_c_getChargeConstantParts();

/** Returns the duration of all constant parts of the model.
 *  These are: head, preprocessing, transient state, postprocessing, tail
 *  @return duration of all constant parts [t]
 */
double ble_e_model_c_getDurationConstantParts();

/** Returns the charge of the communication sequence phases. Each sequence may have an unique number of bytes sent and received.
 * These are: Communication preamble, Window-Widening (slave), Rx, Rx2Tx,Tx,Tx2Rx
 * \param masterOrSlave: 1=>master, 0=>Slave. For the slave, Rx and Tx are swapped (Master: first Tx, then Rx; Slave: first Rx, then Tx) and there is window-widening and a longer dPreRx for the first sequence in an event.
 * \param Tc Connection interval
 * \param nSeq Number of sequences (pairs of packets per connection event)
 * \param nRx[] Number of bytes received. Each array element contains the number of bytes received per sequence (pair of packet). Must include all protocol overheads.
 * \param nTx[] Number of bytes sent. Each array element contains the number of bytes sent per sequence (pair of packet). Must include all protocol overheads.If nTx is < 0 for a certain pair of packets, no transmission attempt is made in for this sequence number (i.e. the device only receives in this sequence).
 * \param txPower Tx-Power setting of the device
 * \param constantWindowWidening If < 0, the amount of window-widening is computed using the connection interval as specified by the BLE specification. If > 0, a constant amount of window-widening which has the value of constantWindowWidening seconds is assumed insted.
 * @return charge consumed by the sequences [C]
 */
double ble_e_model_c_getChargeSequences(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx[], int8_t nTx[],uint8_t txPower, double constantWindowWidening);

/** Returns the charge of the communication sequence phases. Each must have the same number of bytes sent or received (to overcome this limitation, plase use ble_e_model_c_getChargeSequences() )
 * These sequences are: Communication preamble, Window-Widening (slave), Rx, Rx2Tx,Tx,Tx2Rx
 * \param masterOrSlave: 1=>master, 0=>Slave. For the slave, Rx and Tx are swapped (Master: first Tx, then Rx; Slave: first Rx, then Tx) and there is window-widening and a longer dPreRx for the first sequence in an event.
 * \param Tc Connection interval
 * \param nSeq Number of sequences (pairs of packets per connection event)
 * \param nRx[] Number of bytes received. Each array element contains the number of bytes received per sequence (pair of packet). Must include all protocol overheads.
 * \param nTx[] Number of bytes sent. Each array element contains the number of bytes sent per sequence (pair of packet). Must include all protocol overheads. If nTx is < 0 for a certain pair of packets, no transmission attempt is made in for this sequence number (i.e. the device only receives in this sequence).
 * \param txPower Tx-Power setting of the device
 * \param constantWindowWidening If < 0, the amount of window-widening is computed using the connection interval as specified by the BLE specification. If > 0, a constant amount of window-widening which has the value of constantWindowWidening seconds is assumed insted.
 * @return Charge consumed by the sequences [C]
 */
double ble_e_model_c_getChargeSequencesSamePayload(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx, int8_t nTx,uint8_t txPower, double constantWindowWidening);

/** Returns the duration of the communication sequence phases. Each sequence may have an unique number of bytes sent and received.
 * Sequences accounted for are: Communication preamble, Window-Widening (slave), Rx, Rx2Tx,Tx,Tx2Rx
 * \param masterOrSlave: 1=>master, 0=>Slave. For the slave, Rx and Tx are swapped (Master: first Tx, then Rx; Slave: first Rx, then Tx) and there is window-widening and a longer dPreRx for the first sequence in an event.
 * \param Tc Connection interval
 * \param nSeq Number of sequences (pairs of packets per connection event)
 * \param nRx[] Number of bytes received. Each array element contains the number of bytes received per sequence (pair of packet). Must include all protocol overheads.
 * \param nTx[] Number of bytes sent. Each array element contains the number of bytes sent per sequence (pair of packet). Must include all protocol overheads.If nTx is < 0 for a certain pair of packets, no transmission attempt is made in for this sequence number (i.e. the device only receives in this sequence).
 * \param txPower Tx-Power setting of the device
 * \param constantWindowWidening If < 0, the amount of window-widening is computed using the connection interval as specified by the BLE specification. If > 0, a constant amount of window-widening which has the value of constantWindowWidening seconds is assumed insted.
 * @return Time taken by the communcaiton sequences [s]
 */
double ble_e_model_c_getDurationSequences(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx[], int8_t nTx[],uint8_t txPower, double constantWindowWidening);

/** Returns the time spent by the communication sequence phases. Each must have the same number of bytes sent or received (to overcome this limitation, plase use ble_e_model_c_getDurationSequences() )
 * These sequences are: Communication preamble, Window-Widening (slave), Rx, Rx2Tx,Tx,Tx2Rx
 * \param masterOrSlave: 1=>master, 0=>Slave. For the slave, Rx and Tx are swapped (Master: first Tx, then Rx; Slave: first Rx, then Tx) and there is window-widening and a longer dPreRx for the first sequence in an event.
 * \param Tc Connection interval
 * \param nSeq Number of sequences (pairs of packets per connection event)
 * \param nRx[] Number of bytes received. Each array element contains the number of bytes received per sequence (pair of packet). Must include all protocol overheads.
 * \param nTx[] Number of bytes sent. Each array element contains the number of bytes sent per sequence (pair of packet). Must include all protocol overheads.If nTx is < 0 for a certain pair of packets, no transmission attempt is made in for this sequence number (i.e. the device only receives in this sequence).
 * \param txPower Tx-Power setting of the device
 * \param constantWindowWidening If < 0, the amount of window-widening is computed using the connection interval as specified by the BLE specification. If > 0, a constant amount of window-widening which has the value of constantWindowWidening seconds is assumed insted.
 * @return Time spent within the communication sequences [s]
 */
double ble_e_model_c_getDurationSequencesSamePayload(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx, int8_t nTx,uint8_t txPower, double constantWindowWidening);

/** Returns the charge consume dby a BLE connection event. Each sequence may have an unique number of bytes sent and received.
 * The sleep duration in the connection interval is not accounted for. See ble_e_model_c_getChargeConnectionInterval() for the charge consumed per interval.
 * \param masterOrSlave: 1=>master, 0=>Slave. For the slave, Rx and Tx are swapped (Master: first Tx, then Rx; Slave: first Rx, then Tx) and there is window-widening and a longer dPreRx for the first sequence in an event.
 * \param Tc Connection interval
 * \param nSeq Number of sequences (pairs of packets per connection event)
 * \param nRx[] Number of bytes received. Each array element contains the number of bytes received per sequence (pair of packet). Must include all protocol overheads.
 * \param nTx[] Number of bytes sent. Each array element contains the number of bytes sent per sequence (pair of packet). Must include all protocol overheads.If nTx is < 0 for a certain pair of packets, no transmission attempt is made in for this sequence number (i.e. the device only receives in this sequence).
 * \param txPower Tx-Power setting of the device
 * \param constantWindowWidening If < 0, the amount of window-widening is computed using the connection interval as specified by the BLE specification. If > 0, a constant amount of window-widening which has the value of constantWindowWidening seconds is assumed insted.
 * @return Charge consumed by the connection event [C]
 */
double ble_e_model_c_getChargeEvent(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx[], int8_t nTx[],uint8_t txPower, double constantWindowWidening);

/** Returns the duration of a BLE connection event. Each sequence may have an unique number of bytes sent and received.
 *  The sleep duration in the connection interval is not accounted for.
 * \param masterOrSlave: 1=>master, 0=>Slave. For the slave, Rx and Tx are swapped (Master: first Tx, then Rx; Slave: first Rx, then Tx) and there is window-widening and a longer dPreRx for the first sequence in an event.
 * \param Tc Connection interval
 * \param nSeq Number of sequences (pairs of packets per connection event)
 * \param nRx[] Number of bytes received. Each array element contains the number of bytes received per sequence (pair of packet). Must include all protocol overheads.
 * \param nTx[] Number of bytes sent. Each array element contains the number of bytes sent per sequence (pair of packet). Must include all protocol overheads.If nTx is < 0 for a certain pair of packets, no transmission attempt is made in for this sequence number (i.e. the device only receives in this sequence).
 * \param txPower Tx-Power setting of the device
 * @return Duration of the connection event [s]
 * */
double ble_e_model_c_getDurationEvent(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx[], int8_t nTx[],uint8_t txPower, double constantWindowWidening);

/** Returns the charge consumed by a BLE connection event. Each sequence must have the same number of bytes sent and received.
 * The sleep duration in the connection interval is not accounted for. See ble_e_model_c_getChargeConnectionIntervalSamePayload() for the charge consumed per interval.
 * \param masterOrSlave: 1=>master, 0=>Slave. For the slave, Rx and Tx are swapped (Master: first Tx, then Rx; Slave: first Rx, then Tx) and there is window-widening and a longer dPreRx for the first sequence in an event.
 * \param Tc Connection interval
 * \param nSeq Number of sequences (pairs of packets per connection event)
 * \param nRx[] Number of bytes received. Each array element contains the number of bytes received per sequence (pair of packet). Must include all protocol overheads.
 * \param nTx[] Number of bytes sent. Each array element contains the number of bytes sent per sequence (pair of packet). Must include all protocol overheads.If nTx is < 0 for a certain pair of packets, no transmission attempt is made in for this sequence number (i.e. the device only receives in this sequence).
 * \param txPower Tx-Power setting of the device
 * \param constantWindowWidening If < 0, the amount of window-widening is computed using the connection interval as specified by the BLE specification. If > 0, a constant amount of window-widening which has the value of constantWindowWidening seconds is assumed insted.
 * @return Charge consumed by the connection event [C]
 */
double ble_e_model_c_getChargeEventSamePayload(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx, int8_t nTx,uint8_t txPower, double constantWindowWidening);

/** Returns the duration of a BLEconnection event. Each sequence must have the same number of bytes sent and received.
 *  The sleep duration in the connection interval is not accounted for.
 * \param masterOrSlave: 1=>master, 0=>Slave. For the slave, Rx and Tx are swapped (Master: first Tx, then Rx; Slave: first Rx, then Tx) and there is window-widening and a longer dPreRx for the first sequence in an event.
 * \param Tc Connection interval
 * \param nSeq Number of sequences (pairs of packets per connection event)
 * \param nRx[] Number of bytes received. Each array element contains the number of bytes received per sequence (pair of packet). Must include all protocol overheads.
 * \param nTx[] Number of bytes sent. Each array element contains the number of bytes sent per sequence (pair of packet). Must include all protocol overheads. If nTx is < 0 for a certain pair of packets, no transmission attempt is made in for this sequence number (i.e. the device only receives in this sequence).
 * \param txPower Tx-Power setting of the device
 * \param constantWindowWidening If < 0, the amount of window-widening is computed using the connection interval as specified by the BLE specification. If > 0, a constant amount of window-widening which has the value of constantWindowWidening seconds is assumed insted.
 * @return Duration of the connection event [s]
 * */
double ble_e_model_c_getDurationEventSamePayload(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx, int8_t nTx,uint8_t txPower, double constantWindowWidening);

/** Returns the charge consumed within a BLE connection interval. It includes both the connection event and the sleep duration.
 *  Each sequence may have an unique number of bytes sent and received.
 * \param masterOrSlave: 1=>master, 0=>Slave. For the slave, Rx and Tx are swapped (Master: first Tx, then Rx; Slave: first Rx, then Tx) and there is window-widening and a longer dPreRx for the first sequence in an event.
 * \param Tc Connection interval
 * \param nSeq Number of sequences (pairs of packets per connection event)
 * \param nRx[] Number of bytes received. Each array element contains the number of bytes received per sequence (pair of packet). Must include all protocol overheads.
 * \param nTx[] Number of bytes sent. Each array element contains the number of bytes sent per sequence (pair of packet). Must include all protocol overheads.If nTx is < 0 for a certain pair of packets, no transmission attempt is made in for this sequence number (i.e. the device only receives in this sequence).
 * \param txPower Tx-Power setting of the device
 * \param constantWindowWidening If < 0, the amount of window-widening is computed using the connection interval as specified by the BLE specification. If > 0, a constant amount of window-widening which has the value of constantWindowWidening seconds is assumed insted.
 * @return Charge consumed within one connection interval [C]
 */
double ble_e_model_c_getChargeConnectionInterval(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx[], int8_t nTx[],uint8_t txPower, double constantWindowWidening);

/** Returns the charge consumed within a BLE connection interval. It includes both the connection event and the sleep duration.
 *  Each sequence must have the same number of bytes sent and received.
 * \param masterOrSlave: 1=>master, 0=>Slave. For the slave, Rx and Tx are swapped (Master: first Tx, then Rx; Slave: first Rx, then Tx) and there is window-widening and a longer dPreRx for the first sequence in an event.
 * \param Tc Connection interval
 * \param nSeq Number of sequences (pairs of packets per connection event)
 * \param nRx[] Number of bytes received. Each array element contains the number of bytes received per sequence (pair of packet). Must include all protocol overheads.
 * \param nTx[] Number of bytes sent. Each array element contains the number of bytes sent per sequence (pair of packet). Must include all protocol overheads. If nTx is < 0 for a certain pair of packets, no transmission attempt is made in for this sequence number (i.e. the device only receives in this sequence).
 * \param txPower Tx-Power setting of the device
 * \param constantWindowWidening If < 0, the amount of window-widening is computed using the connection interval as specified by the BLE specification. If > 0, a constant amount of window-widening which has the value of constantWindowWidening seconds is assumed insted.
 * @return Charge consumed within one connection interval [C]
 */
double ble_e_model_c_getChargeConnectionIntervalSamePayload(uint8_t masterOrSlave, double Tc, uint8_t nSeq, uint8_t nRx, int8_t nTx,uint8_t txPower, double constantWindowWidening);

#ifdef __cplusplus
}
#endif

#endif /* BLE_MODEL_CONNECTED_H_ */
