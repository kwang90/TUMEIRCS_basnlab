/**
 * \file sble_state.h
 * \brief The state of SBLE
 *
 * The state of BLE consists of a global variable of type sble_driver_state, called dstate.
 * It contains properties that are global to all connections, and at its cons[]-array, properties that
 * are individual to each acitve connection.
 * This File is highly plattform dependant and has to be adapted if SBLE is to be ported to a new plattform.
 *
 * \date 06.07.2012
 * \author kindt
 */

#ifndef SBLE_STATE_FREERTOS_H_
#define SBLE_STATE_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sble_config.h"



#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#include "sble_ll.h"
#include "sble_types.h"
#include <FreeRTOS.h>
#include <inttypes.h>
#include "semphr.h"
#include "task.h"

/**
 * Type for _ handle for disconnection callback function
 */
typedef void (*sble_callback_disconnect_t)(uint8_t con, uint32_t reason);

/**
 * Type for hardware interaction callback for waking BLE112 up if in deep sleep mode and for reseting the BLE device
 * @param action 0 => no wakeup / no reset, 1 => wakeup/ reset
 */
typedef void (*sble_hw_callback_t)(uint8_t action);

/**
 * Type for the connection paremter update callback. I is called whenever
 * a connection is updatesd
 *
 */
typedef void (*sble_connection_update_callback_t)(uint8_t con, uint32_t tcNew, uint32_t latencyNew, uint32_t timeoutNew);


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

	///variable for a send request
	uint8_t cmdExtRequest;

	///placeholder for an address that is filled by some command responses/events
	uint8_t addr[6];

	///placeholder for an address type is set by some command responses/events
	uint8_t addr_type;

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

	///binary semaphore used to put the main-thread asleep
	 xSemaphoreHandle sleepMain;

	///binary semaphore used to put the callback-dispatcher-thread asleep
	 xSemaphoreHandle sleepDispatcher;

	///binary semaphore used to put the external commad-thread asleep
	 xSemaphoreHandle sleepCmdThread;

	///binary semaphore used to put the external event theread asleep
	 xSemaphoreHandle sleepEventThread;

	///counting semaphore used to put the sync-thread asleep
	 xSemaphoreHandle sleepSync;

	 ///data protection mutex
	 xSemaphoreHandle mutexData;

	 ///Multiple command prevention mutex
	  xSemaphoreHandle mutexCmd;

	///Mutex for sleep & wakeup for commands
	  xSemaphoreHandle sleepIOP;

      ///Semaphore to be locked by multiple external threads to prevent race conditions
	  xSemaphoreHandle mutexMultExtThread;



	 //a thread handle for the callback dispatchr therd
	 xTaskHandle sble_thread;

	 ///a thread-handle for the main thread
	xTaskHandle main_thread;


	///function handle for disconnect handler
	sble_callback_disconnect_t callback_disconnect;

	///wakeup callback function.
	sble_hw_callback_t wuf;

	///reset callback function.
	sble_hw_callback_t reset;

	///set the transmitting of the BLE to confirmed or unconfirmed mode
	uint8_t sble_sending_mode;

	///connection update callback function
	sble_connection_update_callback_t connection_update_callback;


}sble_driver_state;


extern sble_driver_state dstate;

#endif

#ifdef __cplusplus
}
#endif

#endif /* SBLE_STATE_FREERTOS_H_ */
