/**
 * \file libBPMcInterface.h
 * \brief Interface between BLE Serial Line Interface (SLI) and Power Managment Strategy (PMS)
 * (c) Oct 2013, Philipp Kindt <kindt@rcs.ei.tum.de>
 * The libBPMInterface is an interface between SLI and PMS.
 * The SLI will call power-management functions of the libBPMcInterface which will pass these functions on to the PMS. In doing so, the SLI does not need to now which PMS is
 * active and does not need to know any of its implementation details. The PMS allso calls functions of the interface to communicate with the SLI.
 *
 * Therefore, both the SLI and the PMS register a couple of callback functions at a libBPMcInterface instance. The libBPMcInterface provides an API to execute these callback functions.
 *
 * Callbacks can be registered at the interface by using the libBPMcInterface_set*() - functions. These callbacks can ba called then via the libBPMcInterface_call*()-functions.
 * All callbacks are illustrated in the figure below.
 * \image html Interface.png "Callbacks supported and handled by the libBPMcInterface"
 *
 * All callbacks are optional. This means: if they are not providedby the SLI or the PMS, respecitively, a corresponding call at the libBPMcInterface
 * will no effect but will cause no error.
 *
 * Before the libBPMcInterface can be used, \ref libBPMcInterface_init() has to be called.
 *
 *
 *
 * \par How use
 *
 * There are three software-modules involved
 *
 * 1) The main program (or any application module)
 *
 *
 * - It will initialize a BLE power management strategy and the libBPMcInterface (alternatively, these steps can also be done in the SLI)
 * - It will initialize and startup the BLE Serial Line Interface (SLI)
 * - It will make the power management strategy known to the BLE SLI via the libBPMcInterface (alternatively, this step can also be done in the SLI)
 *
 * 2) the BLE power management strategy (PMS)
 * - Is initialized by the application or any external programm
 * - It will provide an initialized bleBPMcInterface-structure as soon as it is initialized
 * - It will provide a subset of the following callbacks
 *  - addCallBackIn/Out()        : Is called by the BLE serial line interface whenever new data is to be sent or received (= added to in/out buffer)
 *  - readCallBackIn/Out()       : Is called by the BLE serial line interface whenever new data is read from the in/out buffer
 *  - startInfoCallBack()        : Is called by the BLE serial line interface before addCallBackIn()/addCallBackOut() is called the first time.
 * 	                                It provides initial information like the start interval at the beginning
 *  - setBufferPtrCallBack       : Is called by the BLE serial line interface once before addCallBackIn()/addCallBackOut() is called the first time.
 *  - setTrafficBoundsCallBack() : Called by the BLE SLI or the application once before addCallBackIn()/addCallBackOut() to inform the PMS about the minimum/maximum possible traffic the application might generate
 *  						        It contains pointers to the send and receive-buffer
 *  - Paremeter Interface        : A set of callbacks that implement the parameter interface to allow external applications to adjust the parameters of this interface.
 *  						       Basically, this is a string-based interface. There is a "set parameter" callback to write parameter values, a "get parameter" to read parameter values
 *
 *  						       and a "get parameter list" that exposes a list of paramters the PMS supplies in machine-readable form. In addition, there is a "get ident" callback where the PMS proivdes the caller an unique identification string.
 *  						       Read more about the param interface below.
 *  - Activation callback        : Callback function acting as a switch to switch the PMS on or off
 *  - Destruction callback       : Callback to destroy the PMS. When called, the PMS should deallocate all the memory it has allocated.
 *  - Set role Callback	         : Callback function called by the SLI to set the role of the PMS (master or slave).
 *  	                           In master role, a call to the libBPMcSetConIntCallBack() will update the connection interval directly ASAP.
 *  	                           In slave role, a call to the libBPMcSetConIntCallBack() will, depending on the implementation of the SLIs on both nodes, either suggest the master's PMS to update the connection interval or have no effect, respectively.
 * -  connection updated CB      : Called by the BLE serial line interface (SLI) whenever a connection parameter update actually took place.
 *
 * The PMS calls the libBPMcSetConIntCallBack()-funcion that is provided by the serial line interface whenever it attemts to update the connection interval
 *
 * The parameter interface works as follows:
 *
 * Parameters can be exchanged between SLI or application and PMS  without the SLI/application having to know
 * anything of the PMS that implements libHDRParamInterface. The exchange format is as follows.
 *
 * - Each string used for param exchange can have max. 50 characters
 * - Each param is a key-value-pair. The ident of a param gives it a name and is an arbitrary string.
 * - The value of the param is a string, containing the value of the param.
 *  -For decimal and floating point values, the format is the string representation of the number (for example, valid value would be: "10", "-1", "0.34", "-0.24")
 *  -For string values, any string can be transported
 * - getParam will fill a string buffer that has to have at least 50 bytes with the current value
 * - setParam will set the param
 * - getParamList will return a list of all params available for that class having the
 *   format specified. It will allocate the buffer needed for that, and the application has to deAlloc the buffer by calling free()
 *  - param List format (EBNF): Ascii file that is fomatted like that:
 *    paramList = {paramLine}
 *    paramLine = ident "|" decident "|" limitMin "|" limitMax "\n"
 *    ident		: string identifiing the param, must be compatible with the first param of getParam();
 *    decident	: decident. The first character identifies the value type of the param. Possible values:<br>
 *    						"f": float<br>
 *    						"d": decimal<br>
 *    						"s": string<br>
 *    			  in case of floats, "f" may be followed by the number of decimals. Example: "f11" => 11 decimals precision
 *	limitMin	 :floating point number specifyng the minimal value of the param. ignored for strings
 *	limitMax	 :floating point number specifying the maximal value of the param. For strings, if 0, the string can have abitrary lenght. Other wise, the max lenght will be set to this value
 *
 *
 * 3) Serial line interface (SLI)
 *    The BLE serial line interface calls the callback-functions described above when appropriate.
 *    In addition, as soon as the libBPMcInterface is registered, it provides the libBPMcSetConIntCallBack()-function.
 *    It is called by the BLE power management module if the connection interval is to be updated
 *
 *
 *
 * Don't even think of calling functions of the PMS or SLI directly without using the libBPMcInterface. Eat some bricks instead!
 *
 *
 * Initialization order:
 * The SLI shall call the PMS initialization callbacks (if defined) in the following order:
 * ( 1) Initialization function of PMS)
 *   2) setRoleCB();
 *   3) setBufferPointerCB();
 *   4) setLatencyConstraintsCB();
 *   5) setTrafficBoundsCB();
 *   6) setStartInfoCB();
 *   7) activationCB()
 *
 *   8) Other callbacks, no order specified
 */

#ifndef LIBBPMCINTERFACE_H_
#define LIBBPMCINTERFACE_H_
#include <inttypes.h>
#include "libBPMcTraffic.h"
#include "libBPMcBuffer.h"
#include "libBPMcIdleLineDetection.h"
struct _libBPMcInterface;		//fwd declaration

//Callbacks PMS => SLI

///Callback called by the Power Management Strategy and implemented by the Serial Line Interface to update the connection interval
typedef uint8_t (*libBPMcSetConIntCallBack)(struct _libBPMcInterface* interface, uint32_t tcNew, double nSeqNew);


//Callbacks SLI => PMS

///Callback called by Serial Line Interface to tell the Power Management Strategy the inital connection parameters
typedef void (*libBPMcSetStartInfoCallBack)(struct _libBPMcInterface* interface, uint32_t tcIinital, double nSeqInitial);

///Callback called by the SLI to set the latency constraints of the BLE connection the PMS should keep, i.e. minimum an d maximum connection interval
typedef void (*libBPMcSetLatencyConstraintsCallBack)(struct _libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax);

///Callback called by the SLI to set the minimum and maximum traffic that could occur
typedef void (*libBPMcSetTrafficBoundsCallBack)(struct _libBPMcInterface* interface, double rateMin, double rateMax);

///Callback called by the SLI to set the role of the PMS (master or slave). In slave role, a call to this will suggest the remote (at the master) PMS to update the connection interval.
typedef void (*libBPMcSetRoleCallBack)(struct _libBPMcInterface* interface, uint8_t masterOrSlave);

///Callback called by the Serial Line Interface and provided by the Power Managment strategy called when new data is either received or transmitted (=written to the buffer) or read from the buffer
typedef void (*libBPMcAddCallBack)(struct _libBPMcInterface* interface,libBPMcTraffic tr);

///Callback called by the SLI to tell the PMS the addresses of the in- and out-buffer
typedef void (*libBPMcSetBufferPtrCallBack)(struct _libBPMcInterface* interface,libBPMcBuffer* bufIn, libBPMcBuffer* bufOut);

///Callback called by the SLI or any application to set a parameter of the PMS
typedef uint8_t (*libBPMcGetParamCallBack)(struct _libBPMcInterface* interface,const char* ident, char* value);

///Callback called by the SLI or any application to read a parameter of the PMS
typedef uint8_t (*libBPMcSetParamCallBack)(struct _libBPMcInterface* interface,const char* ident, const char* value);

///Callback called by the SLI or any application to read the machine-readable list of parameters the power management provides
typedef char* (*libBPMcGetParamListCallBack)(struct _libBPMcInterface* interface);

///Callback called by the SLI or any application to read the name of the power managment strategy
typedef void (*libBPMcGetIdentCallBack)(struct _libBPMcInterface* interface, char* ident);

///Callback called by the SLI or any application to activate the PMS
typedef void (*libBPMcActivationCallBack)(struct _libBPMcInterface* interface, uint8_t activated);

///Callback called by the SLI to destroy the PMS (destructor)
typedef void (*libBPMcDestroyCallBack)(struct _libBPMcInterface* interface);

///Callback called by the SLI whenever a connection update took place
///at the point in time the update really took place (after the 6 Connection intervals)
typedef void (*libBPMcConnectionUpdateCallBack)(struct _libBPMcInterface* interface, uint32_t tcNew, uint32_t latencyNew, uint32_t timeOutNew);

typedef struct _libBPMcInterface{
	libBPMcSetConIntCallBack setConIntCallBack;							///<[PMS=>SLI] Set connection interval
	libBPMcActivationCallBack activationCallBack;						///<[SLI/APP=>PMS] Activate the PMS. When activated, the PMS calls the setConIntCallback() - callback function, otherwise not. It should be disactivated by default
	libBPMcDestroyCallBack destroyCallBack;								///<[SLI/APP=>PMS] Destroy the power management strategy
	libBPMcSetStartInfoCallBack setStartInfoCallBack;					///<[SLI=>PMS] Set the initial parameters of the BLE connection, i.e. initial connection interval
	libBPMcSetLatencyConstraintsCallBack setLatencyConstraintsCallBack;	///<[APP=>PMS] Set the latency constraints of the BLE connection the PMS should keep, i.e. minimum and maximum connection interval
	libBPMcSetTrafficBoundsCallBack setTrafficBoundsCallBack;			///<[APP/SLI=>PMS] Set the minimum and maximum possible traffic of the application
	libBPMcSetRoleCallBack setRoleCallBack;								///<[SLI=>PMS] Set the role of the PMS (master or slave). In master role, a call to the \ref setConIntCallBack - callback will update the connection interval. In slave role, a call to this will suggest the remote (at the master) PMS to update the connection interval.
	libBPMcSetBufferPtrCallBack setBufferPtrCallBack;					///<[SLI=>PMS] Set pointers to the in- and out buffer. The PMS can analyze their content to do determine power management decisions
	libBPMcAddCallBack addCallBackIn;									///<[SLI=>PMS] New traffic has been received via BLE to be handed over to the application
	libBPMcAddCallBack addCallBackOut;									///<[SLI=>PMS] New traffic has arrived by the application and is to be sent by the PMS
	libBPMcAddCallBack readCallBackIn;									///<[SLI=>PMS] Data has been read from the in buffer by the application
	libBPMcAddCallBack readCallBackOut;									///<[SLI=>PMS] Data has been removed from the out buffer and sent via BLE
	libBPMcGetParamCallBack getParamCallBack;							///<[SLI/APP=>PMS] Set a param value provided by the PMS
	libBPMcSetParamCallBack setParamCallBack;							///<[SLI/APP=>PMS] Read a param value provided by the PMS
	libBPMcGetParamListCallBack getParamListCallBack;					///<[SLI/APP=>PMS] Read out the list of parameters provided by the PMS
	libBPMcGetIdentCallBack getIdentCallBack;							///<[SLI/APP=>PMS] Return the name of the PMS strategy as a string
	libBPMcConnectionUpdateCallBack connectionUpdateCallBack;			///<[SLI=>PMS] Called whenever a connection update took place (after 6 old connection intervals)
	uint8_t active;														///< flag: 1=> PMS active and doing power management; 0=> PMS disactivated
	void* internalData;	///< Pointer to store any additional internal information of the PMS
	libBPMcIdleLineInternals idleLine;
	uint32_t tcMax;
	uint32_t tcMin;
	uint32_t tcCurrent;
	uint32_t tcScheduled;

	double maxRate;
	libBPMcBuffer* bufOut;
	double nSeqScheduled;
	double nSeqCurrent;
} libBPMcInterface;

/**
 * \brief Initializes the PMS
 * @param interface
 */
void libBPMcInterface_init(libBPMcInterface* interface);

/**
 * \brief Sets the activation callback function.
 * This function is called by the SLI and activates or disactivates the Power Managment Strategy.
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 */
void libBPMcInterface_setActivationCallBack(libBPMcInterface* interface, libBPMcActivationCallBack cb);

/** \brief Sets the destructor callback function.
 * The destructor callback is provided by the PMS to destroy its allocated data when it is to be destroyed. Similar to c++ destructor function
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 */
void libBPMcInterface_setDestroyCallBack(libBPMcInterface* interface, libBPMcDestroyCallBack cb);

/**
 * \brief Sets the "set connection interval" - callback function.
 * The "set connection interval" callback function which is called by the PMS to update the connection interval
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 *  */
void libBPMcInterface_setSetConIntCallBack(libBPMcInterface* interface, libBPMcSetConIntCallBack cb);

/**
 * \brief Sets the "incoming traffic" callback function.
 * The "incoming traffic" callback is called by the SLI whenever traffic is received via the wireless interface to be passed on to the application
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 *
 */
void libBPMcInterface_setAddCallBackIn(libBPMcInterface* interface, libBPMcAddCallBack cb);

/**
 * \brief Sets the "outgoing traffic" callback function.
 * The "outgoing traffic" callback is called whenever the application writes data to the sendout-buffer (the buffer to be sent via BLE)
 *
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 */
void libBPMcInterface_setAddCallBackOut(libBPMcInterface* interface, libBPMcAddCallBack cb);

/**
 * \brief Sets the "data read from IN-Buffer" callback function.
 * This callback is called whenever the application reads from the reception buffer.
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 *
 */
void libBPMcInterface_setReadCallBackIn(libBPMcInterface* interface, libBPMcAddCallBack cb);

/**
 * \brief Sets the "data read from the OUT-Buffer" callback function.
 * This callback is called whenever data is read from the outgoing buffer to be sent via BLE.
 *
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 */
void libBPMcInterface_setReadCallBackOut(libBPMcInterface* interface, libBPMcAddCallBack cb);

/**
 * \brief sets the "set initial parameters"-callback.
 * The "set initial parameters"-callback is called by the SLI to tell the PMS the initial connection parameters before the PMS has been initialized (i.e. the start connection interval)
 * The setSetStartInfoCallBack() is also called the first traffic has been reported in slave mode. In this case, a call to this callback means the remote PMS at the master's side has updated the connection interval and the new values are as follows...
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 */
void libBPMcInterface_setSetStartInfoCallBack(libBPMcInterface* interface, libBPMcSetStartInfoCallBack cb);

/**
 * \brief Set the callback function to set application's constraints on latency in terms of minimum- and maximum connection interval
 * The set latency callback function is called by the BLE SLI after initialization and before the first call to the AddCallBack*()-functios.
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 */
void libBPMcInterface_setSetLatencyConstraintsCallBack(libBPMcInterface* interface, libBPMcSetLatencyConstraintsCallBack cb);


/**
 * \brief Set the "set traffic bounds" callback
 * With this callback, the application or the SLI informs the PMS about the minimum and maximum possbile traffic
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback fuction
 */
void libBPMcInterface_setSetTrafficBoundsCallBack(libBPMcInterface* interface, libBPMcSetTrafficBoundsCallBack cb);

/**
 * \brief Sets the callback function to tell the PMS its role (master or slave).
 * The set role callback function is called after initialization and before the first call to the AddCallBack*()-functions.
   In master role, a call to the libBPMcSetConIntCallBack() will update the connection interval directly ASAP.
   In slave role, a call to the libBPMcSetConIntCallBack() will, depending on the implementation of the SLIs on both nodes, either suggest the master's PMS to update the connection interval or have no effect, respectively.
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 */
void libBPMcInterface_setSetRoleCallBack(libBPMcInterface* interface, libBPMcSetRoleCallBack cb);

/**
 * \brief Sets the "set buffer pointers" callback function.
 * The "set buffer pointers" callback function is called by the SLI to provide pointers to the sendout- and incoming buffer to the PMS. The PMS
 * can analyze these buffers for doing power management
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 *
 */
void libBPMcInterface_setSetBufferPtrCallBack(libBPMcInterface* interface, libBPMcSetBufferPtrCallBack cb);

/**
 * \brief Sets the "connection update" callback function.
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to callback function
 */
void libBPMcInterface_setConnectionUpdateCallBack(libBPMcInterface* interface, libBPMcConnectionUpdateCallBack cb);

/**
 * \brief Execute the "call destructor" callback function.
 * The "call desctructor" callback function is called by the SLI when the PMS class is to be detroyed. The PMS should deallocate all memory
 * it has allocated in its function
 *
 * @param interface Pointer to libBPMcInterface instance.
 */
void libBPMcInterface_callDestroyCallBack(libBPMcInterface* interface);

/**
 * \brief Executes the "set connection interval" callback function.
 * The "set connection interval callback function" is called by the PMS to update the connection interval as a consequence of its power management
 * decision. Usually, the SLI provides this function that implements the connection parameter update
 * @param interface Pointer to PMS interface instance
 * @param tcNew New connection interval to be set (as a multiple of 0.6250 ms)
 * @param nSeqNew Number of pairs of packets exchanged per connection interval
 * @return 1=> connection parameter update has been performed, 0=>connection parameter update has not been performed.
 */
uint8_t libBPMcInterface_callSetConIntCallBack(libBPMcInterface* interface, uint32_t tcNew, double nSeqNew);

/**
 * \brief Execute the "incoming traffic" callback function.
 * The SLI calls the "incoming traffic" callback function whenever data is received via the wireless interface
 * @param interface Pointer to PMS interface instance
 * @param traffic Number of bytes received along with a timestamp (time when the data has been received)
 */
void libBPMcInterface_callAddCallBackIn(libBPMcInterface* interface, libBPMcTraffic traffic);

/**
 * \brief Execute the "outgoing traffic" callback function.
 * The SLI calls the "outgoing traffic" callback function whenever data is written to the sendout buffer (and hence sent via the wireless interfaec)
 * @param interface Pointer to PMS interface instance
 * @param traffic Number of bytes to be sent along with a timestamp.
 */
void libBPMcInterface_callAddCallBackOut(libBPMcInterface* interface, libBPMcTraffic traffic);


/**
 * \brief Execute the "data read from input-buffer" callback function.
 * This callback is executed by the SLI whenever the application reads from the input buffer (data previously received via BLE and buffered)
 * @param interface Pointer to PMS interface instance
 * @param traffic Number of bytes read along with a timestamp (time when the data has been read)
 */
void libBPMcInterface_callReadCallBackIn(libBPMcInterface* interface, libBPMcTraffic traffic);

/**
 * \brief Execute the "red from output buffer" callback function.
 * This callback is called by the SLI whenver data is read from the output buffer (and sent via BLE)
 * @param interface Pointer to PMS interface instance
 * @param traffic Number of bytes to be read along with a timestamp when the read request was performed (~ time of sending).
 */
void libBPMcInterface_callReadCallBackOut(libBPMcInterface* interface, libBPMcTraffic traffic);

/**
 * \brief Execute the "set initial parameters" callback function.
 * The "set initial parameters" callback function is called by the SLI before the first traffic is reported to the PMS. It informs the PMS
 * about the initial connection interval
 * The setSetStartInfoCallBack() is also called the first traffic has been reported in slave mode. In this case, a call to this callback means the remote PMS at the master's side has updated the connection interval and the new values are as follows...
 * @param interface Pointer to PMS interface instance
 * @param tcInitial Initial connection interval (In terms of a multiple of 0.6250 ms)
 * @param nSeqInitial The initial number of packet pairs per connection event. Most strategies assume this not to change over the whole time
 */
void libBPMcInterface_callSetStartInfoCallBack(libBPMcInterface* interface, uint32_t tcInitial, double nSeqInitial);

/**
 * \brief Calls the callback to set application's constraints on latency in terms of minimum- and maximum connection interval
 * This function is called by the BLE SLI after initialization and before the first call to the AddCallBack*()-functions.
 * @param interface Pointer to PMS interface instance
 * @param tcMin Minimum connection interval as a multiple of 0.625 ms
 * @param tcMax Maximum connection interval as a multiple of 0.625 ms
 */
void libBPMcInterface_callSetLatencyConstraintsCallBack(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax);

/**
* \brief Set the "set traffic bounds" callback
 * With this callback, the application or the SLI informs the PMS about the minimum and maximum possible traffic
 *
 * @param interface Pointer to PMS interface instance
 * @param rateMin Minimum possible traffic rate
 * @param rateMax Maximum possible traffic rate
 */
void libBPMcInterface_setTrafficBoundsCallBack(libBPMcInterface* interface, double rateMin, double rateMax);


/**
 * \brief Calls the callback function to tell the PMS its role (master or slave).
 * The set role callback function is called after initialization and before the first call to the AddCallBack*()-functions.
   In master role, a call to the libBPMcSetConIntCallBack() will update the connection interval directly ASAP.
   In slave role, a call to the libBPMcSetConIntCallBack() will, depending on the implementation of the SLIs on both nodes, either suggest the master's PMS to update the connection interval or have no effect, respectively.
 * @param interface Pointer to PMS interface instance
 * @param masterOrSlave 1 => Master role, 0=>Slave Role
 */
void libBPMcInterface_callSetRoleCallBack(libBPMcInterface* interface, uint8_t masterOrSlave);


/**
 * \brief Execute the "set buffer pointer" callback function.
 * The "set buffer pointer" callback function is called by the SLI to provide pointers to the sending- and incoming buffer to the
 * PMS. It is called before the first traffic is reported to the PMS. The PMS can analyze the buffers to do make power management decisions.
  * @param interface Pointer to PMS interface instance
 * @param bufIn Pointer to reception buffer	(data to
 * @param bufOut Pointer to sendout buffer (data to be sent via the wireless interface) and the reception buffer (data received by the wireless interface)
 */
void libBPMcInterface_callSetBufferPtrCallBack(libBPMcInterface* interface, libBPMcBuffer *bufIn, libBPMcBuffer *bufOut);

/**
 * \brief Execute the "connection update occured" callback function.
 * This callback is to be called by the SLI or SBLE/BLE driver whenever a connection parameter update occurs.
 * It is not called when the update is scheduled, but when the new parameters take effect.
 * @param interface ointer to PMS interface instance
 * @param tcNew New connection interval in multiple of 1.25 ms
 * @param latencyNew New slave latency
 * @param timeoutNew New supervision timeout in multiple of 1.25 ms
 */
void libBPMcInterface_callConnectionUpdateCallBack(libBPMcInterface* interface, uint32_t tcNew, uint32_t latencyNew, uint32_t timeoutNew);

/**
 * \brief Activate the idle-line detection.
 * The Idle-Line detection decreases the connection interval to its max once a given amount of microseconds has passed without any write requests, independantly
 * from the PMS.
 * If the \ref LIBBPMC_PARAM_USE_IDLE_LINE_DETECTION in \ref libBPMcParams.c is 1, the idle-line detection is activated by default. If it is 0, it is disavtivated by default.
 * @param interface	The \ref libBPMcInterface to work on
 * @param nMicroseconds Number of microseconds of inactivity after the connection interval shall be decreased
 */
void libBPMcInterface_acitvateIdleLineDetection(libBPMcInterface* interface, uint32_t nMicroseconds);

/**
 * \brief Deisativate the idle-line detection.
 * The Idle-Line detection decreases the connection interval to its max once a given amount of microseconds has passed without any write requests, independantly
 * from the PMS.
 * If the \ref LIBBPMC_PARAM_USE_IDLE_LINE_DETECTION in \ref libBPMcParams.c is 1, the idle-line detection is activated by default. If it is 0, it is disavtivated by default.
 * @param interface	The \ref libBPMcInterface to work on
 */
void libBPMcInterface_disactivateIdleLineDetection(libBPMcInterface* interface);


/******************[ parameter interface functions ]***********************************************************************************/



/**
 * \brief Sets the "get parameter" callback function.
 * The "get parameter" callback function is called by the application or the SLI to read the value of an arbitrary parameter of the PMS.
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to the callback function
 */
void libBPMcInterface_setGetParamCallBack(libBPMcInterface* interface, libBPMcGetParamCallBack cb);

/**
 * \brief Set the "set parameter" callback function.
 * The "set parameter" callback function is called by the application or the SLI to set the value of an arbitrary parameter of the PMS.
 *
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to the callback function
 */
void libBPMcInterface_setSetParamCallBack(libBPMcInterface* interface, libBPMcSetParamCallBack cb);

/**
 * \brief Set the "get parameter list" callback function.
 * This callback is provided by the PMS to expose the parameters it provides to the rest of the world.
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to the callback function
 */
void libBPMcInterface_setGetParamListCallBack(libBPMcInterface* interface, libBPMcGetParamListCallBack cb);

/**
 * \brief Set the "get PMS ident" callback function.
 * The "get PMS ident" callback function generates a string that contains the name (ident) of the PMS.
 * @param interface Pointer to PMS interface instance
 * @param cb Pointer to the callback function
 */
void libBPMcInterface_setGetIdentCallBack(libBPMcInterface* interface, libBPMcGetIdentCallBack cb);

/**
 * \brief Executes the "get PMS parameter" callback function.
 * This callback is provided by the PMS. It returns the value of a parameter read.
 * @param interface Pointer to PMS interface instance
 * @param ident Name of the parameter to read as a zero-terminated string
 * @param value Pointer to a pre-allocated memory-region of at least 50 bytes. In this string, the PMS stores the value of the parameter requested.
 * The format is as follows:
 *  - For decimal and floating point values, the format is the string representation of the number (for example, valid value would be: "10", "-1", "0.34", "-0.24")
 *  - For string values, any string can be transported
 * @return 1 in case of success, 0 in case of failure
 */
uint8_t libBPMcInterface_callGetParamCallBack(libBPMcInterface* interface, const char* ident, char* value);

/**
 * \brief Executes the "set PMS parameter" callback function.
 * This callback is provided by the PMS. It writes the value of a parameter.
 * @param interface Pointer to PMS interface instance
 * @param ident Name of the parameter to write as a zero-terminated string
 * @param value String that contains the parameter value.
 * The format is as follows:
 *  - For decimal and floating point values, the format is the string representation of the number (for example, valid value would be: "10", "-1", "0.34", "-0.24")
 *  - For string values, any string can be transported
 * @return 1 in case of success, 0 in case of failure
 */
uint8_t libBPMcInterface_callSetParamCallBack(libBPMcInterface* interface, const char* ident, const char* value);

/**
 * \brief Executes the "list PMS parameters" callback function.
 * The List parameter callback function allocates a string, writes all parameters the PMS supports in a machine-readable format
 * and returns a pointer to that zero-terminated string.
 * @param interface Pointer to PMS interface instance
 * @return Pointer to zero-terminated string (better: pointer to first char...) that lists all parameters the PMS provides via the parameter interface. The format of this string is as follows (in EBNF - see wikipedia what EBNF is...):<br>
 * paramList = {paramLine}<br>
 *    paramLine = ident "|" decident "|" limitMin "|" limitMax "\n"<br>
 *    ident		: string identifiing the param, must be compatible with the first param of getParam();<br>
 *    decident	: decident. The first character identifies the value type of the param. Possible values:<br>
 *    						"f": float<br>
 *    						"d": decimal<br>
 *    						"s": string<br>
 *    			  in case of floats, "f" ma be follwoed by the number of decimals. Example: "f11" => 11 decimals precision<br>
 *	limitMin	 :floating point number specifyng the minimal value of the param. ignored for strings<br>
 *	limitMax	 :floating point number specifying the maximal value of the param. For strings, if 0, the string can have abitrary lenght. Other wise, the max lenght will be set to this value<br>
 */
char* libBPMcInterface_callGetParamListCallBack(libBPMcInterface* interface);

/**
 * \brief Executes the "get PMS ident" callback function.
 * The "get PMS ident" callback function writes its identifier (= its name) as a string to a pre-allocated memory area having at least 50 bytes.
 * @param interface Pointer to PMS interface instance
 * @param ident Pre-allocated memory region of at least 50 bytes lenght the ident string will be written into.
 */
void libBPMcInterface_callGetIdentCallBack(libBPMcInterface* interface,char* ident);




#endif /* LIBBPMCINTERFACE_H_ */
