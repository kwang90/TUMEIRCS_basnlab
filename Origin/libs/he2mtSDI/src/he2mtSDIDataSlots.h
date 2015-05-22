/**
 * \file he2mtSDIDataSlots.h
 * \brief Data slot definitions for HE2mT SDI
 *
 * 2014, Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 * This file contains all data slot defintions for the SDI.
 * \section Data Slots Overview
 * Data slots are numbers that define a data channel. Each data channel can be written by calling he2mtSDI::setSensorData().
 * The slot number does not only define which sensor data is written (acceleration, activity, ECG, ...), but also how the data pointer
 * on values is to be interpreted. For example, for an activity, the valueList-parameter is simply a pointer to an uint32_t, whereas for raw acceleration,
 * the pointer points to a field of floats containing the acceleration samples.
 * The interpretation is described in this document, too.
 *
 */

#ifndef HE2MTSDIDATASLOTS_H_
#define HE2MTSDIDATASLOTS_H_

/**
 * \brief Data slot for detected activity by activity sensors.
 * Format contains::
 * - nValues is always one (single value call)
 * - values: pointer to a 32-bit unsigned integer (uint32_t) having the value of the detected activity.
 *    This can be a value according to one of the HE2MT_SDI_DATA_SLOT_ACTIVITY_VALUE_* macros (e.g., \ref HE2MT_SDI_DATA_SLOT_ACTIVITY_VALUE_RESTING)
 *
 */
#define HE2MT_SDI_DATA_SLOT_ACTIVITY 1


///value: Unknown activity
#define HE2MT_SDI_DATA_SLOT_ACTIVITY_VALUE_UNKNOWN 3

///value: Person is resting
#define HE2MT_SDI_DATA_SLOT_ACTIVITY_VALUE_RESTING 1

///value: Person is walking
#define HE2MT_SDI_DATA_SLOT_ACTIVITY_VALUE_WALKING 2

///value: Person is running
#define HE2MT_SDI_DATA_SLOT_ACTIVITY_VALUE_RUNNING 3

/**
 * \brief Data slot for raw acceleration data
 * The raw acceleration slot delivers the three-axis acceleration values of a person wearing the sensor.
 * Each acceleration is a triple of three floats.
 * -nValues: The nValues-parameter of \ref he2mtSDI::setSensorData() contains the number of triples.
 * -values: a pointer to a field of float containing the tripples. Memory layout: {accX[0],accY[0],accZ[0],accX[1],accY[1],accZ[1]}
 */
#define HE2MT_SDI_DATA_SLOT_RAWACC 2

/**
 * \brief Data slot for the RSSI service
 * The RSSI service delivers the received signal strenght in dBM of the sensor's wireless signal for each packet.
 * Each packet delivers a signed 8-bit integer containing the received signal strength in dBm.
 * - nValues: number of packets RSSI - values are delivered for
 * - values: pointer to signed 16-bit integers containing the RSSI values
 *
 */
#define HE2MT_SDI_DATA_SLOT_RSSI 3



#endif /* HE2MTSDIDATASLOTS_H_ */
