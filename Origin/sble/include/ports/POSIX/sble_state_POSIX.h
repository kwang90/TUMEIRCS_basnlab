/**
 * \file sble_state_POSIX.h
 * \brief The state of SBLE - POSIX-version.
 *
 * The state of BLE consists of a global variable of type sble_driver_state, called dstate.
 * It contains properties that are global to all connections, and at its cons[]-array, properties that
 * are individual to each acitve connection.
 * This File is highly plattform dependant and has to be adapted if SBLE is to be ported to a new plattform.
 *
 * \date 04.11.2012
 * \author Philipp Kindt
 */

#ifndef SBLE_STATE_POSIX_H_
#define SBLE_STATE_POSIX_H_

#include "sble_config.h"
#include <inttypes.h>
#include "sble_ll.h"
#include "sble_types.h"

//Plattform-dependant includes
#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_POSIX
#include <pthread.h>
#include <semaphore.h>
#ifdef SBLE_CFG_TIME_PROFILING
#include <time.h>
#endif
/**
* Type for _ handle for disconnection callback function
*/
typedef void (*sble_callback_disconnect_t)(uint8_t, uint32_t);


/**
 * Type for hardware interaction callback for waking BLE112 up if in deep sleep mode and for reseting the BLE device
 * @param action 0 => no wakeup / no reset, 1 => wakeup/ reset
 */
typedef void (*sble_hw_callback_t)(uint8_t action);


/**
 * \brief The global state (dstate-Variable) of SBLE
 */
typedef struct _sble_driver_state{

	/**Active connection array.
	* Use a connection number to access connection specific data via this array.\n
	* example dstate.cons[0].addr
	*/
 	sble_state** cons;

 	///The lenght of the cons-array
 	sble_unsigned_integer cons_length;

 	///Flags. All SBLE_STATE_* - macros defined in this file
 	uint32_t flags;
 	uint32_t flags_thread2;

 	///Flags used by the scheduler internally. Do not touch!
	uint8_t flags_sched_internal;


	///placeholder for an address that is filled by some command responses/events
	uint8_t addr[6];

	///placeholder for an address type is set by some command responses/events
	uint8_t addr_type;

	///state for handling a send request by an external thread
	/// 0 => no command is to be sent by external thread
	/// 1 => external thread wishes to send something but is waiting for permission
	/// 2 => external thread is active and sending / waiting for response
	sble_ext_cmd_state_t cmdExtRequest;

	/**the event autoclear list. Events on this do not have to be acknowledged by the main thread as they are acknowledged automatically.
	* See \see(sble_scheduler.h) how the eal is used.
	*/
	uint32_t evt_clear_list;
	uint32_t evt_clear_list_thread2;

	///The connection the last response that occured was related to. Set by most responses to commands.
	uint8_t current_con;

	/** Bitfield  for active connection. If a connection is established, the bit at the corresponding position is set
	 *  Example: 01000101 => connections 0,2 and 6 are active, the others are not.
	 */
	uint32_t cons_activity_map;

	///reception queue for gatt values. If a remote node writes an attribute it per ATT-protocol, these values will be stored in this linked-list
	sble_ll* ll_gatt_values;

	///Filled by ble_rsp_attributes_read in commands.h. It is used by all calls that readout the local GATT database.
	sble_payload* pl_gatt_value;


	///the filedescriptor for I/O with BLE112/BLED112
	sble_signed_integer filedescriptor;

	///a thread-handle for the dispatcher thread
	pthread_t sble_thread;


	///Mutex for sleep&wakeup of the dispatcher thread
	sem_t mutexDispatcher;


	///Mutex for sleep/wakeupp of the main thread
	sem_t mutexMain;


	///Mutex for data protection
	pthread_mutex_t mutexData;

	///Mutex for sleep & wakeup of external thread when receiving cmd responses
	sem_t mutexCmdExternal;

	///Mutex for sleep & wakeup of external thread when waiting for events
	sem_t mutexEventExternal;


	///Mutex for blocking any main thread that wants to do a BGAPI call if another call is not handled completely.
	///This mutex is locked from the beginning of sending the request until the respone has been received.
	pthread_mutex_t mutexCmd;

	  ///Semaphore to be locked by multiple external threads to prevent race conditions
	pthread_mutex_t mutexMultExtThread;


	///function handle for disconnect handler
	sble_callback_disconnect_t callback_disconnect;

	///wakeup callback function.
	sble_hw_callback_t wuf;

	///reset callback function
	sble_hw_callback_t reset;

	///set the transmitting of the BLE to confirmed or unconfirmed mode
	uint8_t sble_sending_mode;

	///connection update callback function
	sble_connection_update_callback_t connection_update_callback;

	#ifdef SBLE_CFG_TIME_PROFILING
		struct timespec tScan;
		sble_bool tScanSet;
	#endif

}sble_driver_state;


extern sble_driver_state dstate;


/**
 * Initialitzes the global state. Creates all the linked-lists used.
 */
void sble_state_init();

/**
 * Finalizes the global state. Destroys all the linked-lists used by sble_state abd sble_driver_state.
 */
void sble_state_finalize();

#endif
#endif /* SBLE_STATE_POSIX_H_ */
