/**
 * \file sble_bgapi_call.h
 * \brief Interface for blocking - and nonblocking calls to BGAPI BLE stack - used for multi threading.
 *
 * -Nonblocking Call: An api command is sent and the calling function returns before the response has arrived.\n
 *  WARNING: no BGAPI-Call must be sent until the previous command has been acknowledged by its response.\n
 * -Blocking Call: A BGAPI-Call is sent and the function returns to the callee after the response has been received.\n
 *
 * Plase note a response is not the same as an event. For example: a ble_cmd_attributes_read is sent by the application. The BLE stack
 * ackowledges this by a ble_rsp_attributes_read. When the data to be read arrives, a ble_evt_attclient_attribute_value-event occurs.
 *
 *	Example: How to make a blocking call:\n
 *	sble_call_bl(ble_cmd_attclient_find_information_idx,con,1,0xffff);\n
 *
 * Parameters and conventions:\n
 *  The parameters given to sble_call_bl() and sble_call_nb() are the same as for the BGAPI-Functions, except that the first param is
 *  the ble_msg_idx of the corresponding call.
 *  Example:\n
 *  BGAPI : ble_cmd_attclient_find_information(con,1,0xffff);\n
 *  SBLE call: 	sble_call_bl(ble_cmd_attclient_find_information_idx,con,1,0xffff);\n
 *
 *
 *  \date 04.09.2013
 *  \author Mathias Gopp
 *
 */



#ifndef SBLE_BGAPI_CALL_MULTI_THREADING_H_
#define SBLE_BGAPI_CALL_MULTI_THREADING_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "sble_scheduler.h"
#include "sble_io.h"
#include "sble_state.h"
#include "sble_debug.h"
#include <stdarg.h>
#include "../bglib/cmd_def.h"


/**
 * Perform a blocking call to an BGAPI function with multi threading.
 *
 *  Parameters and conventions:\n
 *  The parameters given to and sble_call_nb() are the same as for the BGAPI-Functions, except that the first param is
 *  the ble_msg_idx of the corresponding call.\n
 *  Example:\n
 *  BGAPI : ble_cmd_attclient_find_information(,con,1,0xffff);\n
 *  SBLE call: 	sble_call_bl(ble_cmd_attclient_find_information_idx,con,1,0xffff);\n
 *
 */
#define sble_call_bl_ext_thread(...) sble_bgapi_call_internal_bl_init_ext_thread(__VA_ARGS__);ble_send_message(__VA_ARGS__);sble_bgapi_call_internal_bl_delay_ext_thread();


/**
 * Internal function used by BLE to prepare a blocking call to BGAPI\n
 * Never use manually. Used in combination with multi threading.
 */
void sble_bgapi_call_internal_bl_init_ext_thread(uint8_t message,...);

/**
 * Internal function used by BLE to wait for the response after a blocking call.\n
 * Never use. Used in combination with multi threading.
 */
sble_bool sble_bgapi_call_internal_bl_delay_ext_thread();


#ifdef __cplusplus
}
#endif

#endif /* SBLE_BGAPI_CALL_MULTI_THREADING_H_ */
