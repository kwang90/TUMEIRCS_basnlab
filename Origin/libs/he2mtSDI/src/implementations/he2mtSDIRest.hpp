/**
 * \file he2mtSDIRest.hpp
 * \brief Example implementation of the He2MT interface
 *
 * This file provides a sample interface that provides a resful API to the appserver (AS).
 * It acts as a REST client and keeps the specification of Reliatec's appserver.
 *
 *  2014, Philipp Kindt
 */

#ifndef HE2MT_SDI_REST_HPP_
#define H2MT_SDI_REST_HPP_

#include "../he2mtSDI.hpp"
#define QT_NO_KEYWORDS

#include <rest/rest-proxy.h>
class he2mtSDIRest: public he2mtSDI{
private:
		RestProxy* proxy;
		GError* error = NULL;
		uint32_t lastErrNo;
		bool authorized;
		char oAuthToken[257];
		char serverURL[257];
public:


		he2mtSDIRest();
		/**
		 * \brief Initialization function.
		 * This function will be called once before any other function is called.
		 */
		virtual void init();

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
		virtual bool authorize(char* applicationID, char* applicationSecret, char* username, char* password);

		/**
		 * \brief write new sensor data
		 * This function is called whenever sensor data
		 * @param dataSlot	A data slot identifying the type of data that is to be written (e.g., detected activities, raw acceleration, ...)
		 * @param sensorAddress The MAC adress of the sensor as a string.
		 * @param timeStampSensor Time stamp of the first data value created on the sensor
		 * @param nValues The number of values in valueList
		 * @param valueList Pointer to a list of values. The type of the values is determined by the dataSlot. It must be allocated and free'ed by the caller. However, the caller does not need to guaranty the existance of the buffer once the function has compleeted.
		 * @param samplingPeriod: Time [s] inb etween two samples.
		 * @return True, if successful; False, otherwise.
		 */
		virtual bool setSensorData(dataslot_t dataSlot, char* sensorAddress ,char* timeStampSensor, double samplingPeriod, uint32_t nValues, void* valueList);

		/**
		 * \brief disconnect SAS from the AS/RF.
		 * This function is called when the sensor interface wishes to unregister with the AS/RF.
		 * The expected behaviour is that the authorization is invalidated.
		 * @return t
		 */
		virtual bool disconnect();

		/**
		 * \brief Get the number of the last error that hasoccurred
		 * @return Error code of the last error that has occurred.
		 */
		virtual errorcode_t getLastError();

		virtual void* getApplicationErrorInfo();


public:
		/****** [generic parameter interface] *******/
		void getIdent(char* ident);
		bool setParam(const char* param, const char* value);
		bool getParam(const char* param, char* value);
		char* getParamList();
};
#endif
