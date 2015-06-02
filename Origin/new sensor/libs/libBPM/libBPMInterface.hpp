/**
 * \file libBPMInterface.hpp
 * \brief Interface class for BLE Power Management Strategies
 * This Interface needs to be implemented by any BLE Power Management strategy in order to update the connection interval.
 * The BLE Serial Line Deameon will call the \ref add() - function whenerver there is data to be transmitted. The power management interface thereby
 * can analyze this data and update the BLE params by calling a callback function. The clallback function is provided by the BLE line interface which
 * calls \ref setCallBack() before its first call to \ref add().
 *
 * You probably might not want to implement this interface directly (even though its safe to do so).
 * \ref libBMPInterfaceTemplate will provide some common infrastructure for power management strategies and will make your life easier.
 *
 *
 * (c) Oct. 2013, Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 */
#ifndef LIBBPMINTERFACE_H_
#define LIBBPMINTERFACE_H_

#include <inttypes.h>
#include <time.h>
#include "libBPMTraffic.hpp"



class libBPMInterface{
	public:

	///Data type for callback functions that update the connection intervla
	typedef void (*libBPMsetConIntCallBack)(uint32_t TcNew);

	/**
	 * The BLE serial line interface (or similar software) will call this function before \ref add() the first time. It
	 * will disclose its callback function for connection parameter updates.
	 *
	 * @param setConnectionIntervalCB	Callback function for connection parameter updates
	 * @param TcInitial					The initial connection interval the BLE device uses
	 */
	virtual void setCallBack(libBPMsetConIntCallBack* setConnectionIntervalCB, uint32_t TcInitial) = 0;

	/**
	 * This function is called whenever there is a data transmission.
	 * @param sendOrRead	True, if the transmission is sent from the node we're running on to the remove node; False, if vice-versa.
	 * @param traffic		Contains the number of bytes transmitted and the time interval between the previous and the current transmission
	 */
	virtual void add(bool sendOrRead, libBPMTraffic traffic) = 0;

};


#endif /* LIBBPMINTERFACE_H_ */
