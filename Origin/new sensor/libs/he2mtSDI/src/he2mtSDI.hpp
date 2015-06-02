/**
 * \file he2mtSDI.hpp
 * \brief He2mT interface class for all sensors to the replication - and/or application server interface
 * \copyright (c) 2014, Philipp Kindt <kindt@rcs.ei.tum.de>
 * \mainpage
 *
 * \section secScope Scope
 *	This interface must be implemented by an application server (AS) or replication framework (RF) to receive sensor data
 *	from a sensor aggregation system (SAS). In addition, it is used to pass on sensor data from the replication framework (RF) to the appserver (AS).
 *	\image html interfaces.png
 *	\image latex interfaces.png width=1.0\LineWidth
 *
 * \section secGeneral General
 *  All functions of this abstract class need to be implemented in order to receive sensor data.
 *  The functions of genericParamInterface need not be implemented. However, genericParamInterface can be used to expose a parameter interface that allows
 *  the sensor aggregation system to modify certain values of the application server. It is for debugging purposes, only. On java, genericParamInterface needs not to be implemented.
 *
 *	The interface is used as follows:
 *	- First, the application using it calls \ref he2mtSDI::init()
 *	- Then, it calls \ref he2mtSDI::authorize(), providing authentication/authorization credentials. If no authentication/authorization is needed, the class implementing the interface can safely ignore all paramters and do nothing.<br>
 *	- Then, data can be transferred via the interface by calling \ref he2mtSDI::setData()
 *  - When everything is done, the application can call \ref he2mtSDI::disconnect() to stop being authenticated.
 *
 * \section secSlots Data Slots
 * The interface defines data slots. Data slots are virtual channels that an application can read from or write to.
 * The interface is unidirectional, meaning a class implementing the interface can only read from a data slot (by implementing \ref setSensorData),
 * whereas the application using it (SAS or RF) can write to this data slot by calling \ref he2mtSDI::setSensorData().
 * read more: \ref he2mtSDIDataSlots.h
 * \section secSample Sample Program
 * The following code would use the interface correctly:
 *
 * #include "implementations/he2mtSDIRest.hpp"
 *<br>
 * int main(){<br>
 *	float values[6] = {1,2,3,4,5,6};<br>
 *	uint32_t activity = HE2MT_SDI_DATA_SLOT_ACTIVITY_VALUE_WALKING;<br>
 *	he2mtSDIRest r;<br>
 *	r.init();<br>
 *	if(r.authorize("id","secret","he2mt_user","he2mt_user")){<br>
 *		r.setSensorData(HE2MT_SDI_DATA_SLOT_ACTIVITY,"0","0",0.01,1,&activity);<br>
 *		r.setSensorData(HE2MT_SDI_DATA_SLOT_RAWACC,"0","0",0.01,2,&values);<br>
 *	}<br>
 *	r.disconnect();<br>
 *}<br>
 *
 *
 */

#ifndef HE2MT_SDI_HPP_
#define HE2MT_SDI_HPP_


#include <inttypes.h>
#include "he2mtSDIDataSlots.h"
#include "he2mtSDIErrorCodes.h"
#include "genericParamInterface.h"

/** \class he2mtSDI
 * \brief Purely virtual (abstract) class that needs to be implemented to get sensor data via the HE2mT SDI
 *  The class "genericParamInterface" is for debugging purposes, only. It is not part of the interface.
 *  The standard C++ installation comes with a dummy implementation of genericParamInterface as the accsensor_server - application
 *  expects it to be there. In Java on a mobile phone, genericParamInterface needs not to be implemented at all.
 */
class he2mtSDI: public genericParamInterface{

public:
	/******************[He2mT sensor data interface]***********************************************************************************/

	/** Data type for the data slot numbers */
	typedef uint32_t dataslot_t;

	/** Data type for the error codes */
	typedef uint32_t errorcode_t;


	/**
	 * \brief Initialization function.
	 * This function will be called once before any other function is called.
	 */
	virtual void init() = 0;

	/**
	 * \brief authorize sensor aggregation system against remote side
	 * This function authorizes the sensor aggregation system against the replication framework and/or appserver.
	 * It is oauth-compatible.
	 * @param applicationID String containing the application ID
	 * @param applicationSecret String containing the application secret
	 * @param username string containing the username
	 * @param password string containing the passwort of the user
	 * @return True, if successful; False, otherwise.
	 */
	virtual bool authorize(char* applicationID, char* applicationSecret, char* username, char* password) = 0;

	/**
	 * \brief write new sensor data
	 * This function is called whenever sensor data
	 * @param dataSlot	A data slot identifying the type of data that is to be written (e.g., detected activities, raw acceleration, ...)
	 * @param sensorAddress The MAC adress of the sensor as a string.
	 * @param timeStampSensor Time stamp of the first data value created on the sensor
	 * @param nValues The number of values in valueList
	 * @param valueList Pointer to a list of values. The type of the values is determined by the dataSlot. It must be allocated and free'ed by the caller. However, the caller does not need to guaranty the existance of the buffer once the function has compleeted.
	 * @param samplingPeriod: Time [s] in between two samples.
	 * @return True, if successful; False, otherwise.
	 */
	virtual bool setSensorData(dataslot_t dataSlot, char* sensorAddress ,char* timeStampSensor, double samplingPeriod, uint32_t nValues, void* valueList) = 0;

	/**
	 * \brief disconnect SAS from the AS/RF.
	 * This function is called when the sensor interface wishes to unregister with the AS/RF.
	 * The expected behaviour is that the authorization is invalidated.
	 * @return t
	 */
	virtual bool disconnect() = 0;

	/**
	 * \brief Get the number of the last error that has occurred. The error numbers are defined by the interface and are similar for all implementations of this interface.
	 * 	 * @return Error code of the last error that has occurred.
	 */
	virtual errorcode_t getLastError() = 0;
	/**
	 * \brief Get implementation-specific information ont the error
	 * @return error information
	 */
	virtual void* getApplicationErrorInfo() = 0;


};

#endif /* HE2MT_SDI_HPP_ */
