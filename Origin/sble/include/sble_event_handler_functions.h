/**
 * \file sble_event_handler_functions.h
 * \brief functions internally used by SBLE & BGAPI as handler functions for some events.
 *
 * Never use theese functions. They are used by the callbacks in bglib/commands.c
 *  \date 11.07.2012
 *  \author Philipp Kindt
 */

#ifndef SBLE_EVENT_HANDLER_FUNCTIONS_H_
#define SBLE_EVENT_HANDLER_FUNCTIONS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "../bglib/cmd_def.h"

/**
 * This function is called autmatically if a connection has been established. It will set up the correpsonding queues and more.
* Never use directly.
 */
void sble_evth_connection_established(const struct ble_msg_connection_status_evt_t *msg);


/**
 * This function is called autmatically if a connection has been terminateed. It free the correpsonding queues and more.
 * Never use directly.
 */
void sble_evth_disconnected(const struct ble_msg_connection_disconnected_evt_t *msg);
#ifdef __cplusplus
}
#endif

#endif /* SBLE_EVENT_HANDLER_FUNCTIONS_H_ */
