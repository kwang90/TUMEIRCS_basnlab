/**
 * \file sble_state.h
 * \brief The state of SBLE
 *
 * The state of BLE consists of a global variable of type sble_driver_state, called dstate.
 * It contains properties that are global to all connections, and at its cons[]-array, properties that
 * are individual to each acitve connection.
 * This File contains the plattform-independant parts of the state. Further code is added by the port's state headers.
 * 
 *
 * \date 06.07.2012
 * \author kindt
 */

#ifndef SBLE_STATE_H_
#define SBLE_STATE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "sble_config.h"
#include "sble_ll.h"
#include "sble_types.h"

/**
 * Type for the connection paremter update callback. I is called whenever
 * a connection is updatesd
 *
 */
typedef void (*sble_connection_update_callback_t)(uint8_t con, uint32_t tcNew, uint32_t latencyNew, uint32_t timeoutNew);


typedef enum _sble_ext_cmd_state_t{SBLE_EXTCMD_IDLE, SBLE_EXTCMD_REQUEST, SBLE_EXTCMD_ACTIVE} sble_ext_cmd_state_t;

//###########################FLAGS for global state: _sble_driver_state -> flags ###########################

///a response to a command has been answered successfully
#define SBLE_STATE_CMD_SUCCESS 1

///the system is about to terminate/is currently terminating
#define SBLE_STATE_TERMINATE 2

///a command has been sent
#define SBLE_STATE_CMD_SENT 4


///the response of a command has been recieved
#define SBLE_STATE_RESPONSE_RECEIVED 8

///an arbitrary event has occured
#define SBLE_STATE_EVENT 16

///a scan response received
#define SBLE_STATE_SCAN_RESPONSE_EVENT 32

///connection has been established
#define SBLE_STATE_CONNECTION_EVENT 64


///a scan for attributes has revealed an attribute
#define SBLE_STATE_ATTRIBUTE_INFORMATION_FOUND_EVENT 128

///ATT procedure has been completed. This might happen after for example attribute information lists are transferred
#define SBLE_STATE_ATTRIBUTE_PROCEDURE_COMPLETED_EVENT 256


///an attribute value has been received
#define SBLE_STATE_ATTRIBUTE_VALUE_EVENT 512

///a GATT value has been read by request from the local GATT DB
#define SBLE_STATE_GATT_VALUE_RESPONSE 1024

///a GATT value has been received from remote node via ATT
#define SBLE_STATE_GATT_VALUE_EVENT 2048

///connection lost
#define SBLE_STATE_DISCONNECT_EVENT 4096

///indication_confirm event


///IO port status wakeup occurres when an external pin (I.E. wakeup-pin) is riesed
#define SBLE_STATE_IO_PORT_STATUS 8192

#define SBLE_STATE_INDICATION_CONFIRM_EVENT 16384

///The default event clear list. Clears everything but SBLE_STATE_CMD_SUCCESS | SBLE_STATE_TERMINATE|SBLE_STATE_CMD_SENT|SBLE_STATE_RESPONSE_RECEIVED|SBLE_STATE_EVENT autmatically
#define SBLE_EVENT_CLEAR_LIST_DEFAULT (~(0) & ~(SBLE_STATE_CMD_SUCCESS | SBLE_STATE_TERMINATE|SBLE_STATE_CMD_SENT|SBLE_STATE_RESPONSE_RECEIVED|SBLE_STATE_EVENT))



//###########################[ per-connection flags, cannot be waited for (for sble_state.secondary_flags)]###########################




///Connection status event occured
#define SBLE_STATE_SECONDARY_CONNECTION_UPDATE_OCCURED 1

//###########################[ FLAGS scheduler-internal flags for dstate.flags_sched_internal - cannot be set or cleared by hand]###########################

///Internal flag for the scheduler that a command response has been acknowledged
#define SBLE_FLAG_SCHED_FOR_RESPONSE_ACK 1

//###########################[ FLAGS for setting the transmission mode ]#################################################

#define SBLE_ATTCLIENT_SENDING_MODE_CONFIRMED 0
#define SBLE_ATTCLIENT_SENDING_MODE_UNCONFIRMED 1

//###########################[ End of flags section ]##########################################################




/**
 * A handler function for events. Currently unused.
 */
typedef void (*sble_event_handler)();


/**
 * \brief The sble_state struct is a per-conection-state and constitutes each entry in the cons[]-array.
 */
typedef struct _sble_state{

	///The connection number. One unique number is assigned for each active connection
	uint8_t con;

	///The remote's BLE address
	uint8_t addr[6];

	///The address-type. Use either gap_address_type_random or ap_address_type_public.
	uint8_t addr_type;

	///Linked list for reception queue
	sble_ll* ll_rcvqueue;

	///Linked list for attribute lists (after a call to sble_attclient_getlist(), this list contains all attributes supported by the remote's GATT server)
	sble_ll* ll_attributes;

	///pointer for exchanging general params between event and application
	void* param;

	///secondary flags = flags that the scheduler cannot wait for...
	uint8_t secondary_flags;

} sble_state;

/**
 * Initialitzes the global state. Creates all the linked-lists used.
 */
void sble_state_init();

/**
 * Finalizes the global state. Destroys all the linked-lists used by sble_state abd sble_driver_state.
 */
void sble_state_finalize();


/**
 * Register a connection update callback function. This callback will be called each time a connection paramter update
 * takes place.
 * @param cb Address of callback function
 */
void sble_state_register_connection_update_callback(sble_connection_update_callback_t cb);


//Plattform-dependant includes
#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
#include "ports/POSIX/sble_state_POSIX.h"

#elif SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#include "ports/FreeRTOS/sble_state_FreeRTOS.h"
#else
#error "wrong/unsupported plattform."
#endif
#ifdef __cplusplus
}
#endif

#endif /* SBLE_STATE_H_ */
