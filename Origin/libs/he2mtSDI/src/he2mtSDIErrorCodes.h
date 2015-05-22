/** \file he2mtSDIErrorCodes.h
 * \brief error Code defintions for HE2mT SDI
 *
 * This file specifies the error codes for the HE2mT SDI.
 * They can be polled by calling \ref he2mtSDI::getLastError(), which returns the error code of the pervious operation.
 *
 * 2014, Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#ifndef HE2MTSDIERRORCODES_H_
#define HE2MTSDIERRORCODES_H_

/// No error occurred
#define HE2MT_SDI_ERROR_SUCCESS 0

/// An unknown error occured
#define HE2MT_SDI_ERROR_UNKNOWN 1

/// The caller could not authenticate to the software module
#define HE2MT_SDI_ERROR_AUTH_FAILED 2

//// Communication failed: The software module providing the interface could not be reached properly.
#define HE2MT_SDI_ERROR_COMMUNICATION_FAILURE 3



#endif /* HE2MTSDIERRORCODES_H_ */
