/**
 * \file sble_attclient.h
 * \brief BLE attribute client (ATT) functions
 *
 * Attritbute protocol (ATT) client functions can be used to read and write attributes on a different node's GATT server. The
 * there must be a BLE-connection established to the foreign host. Each attribute consists of a handle (which is an interger unique to each attribute on the atrribute server,
 * but dependant of the node), a UUID (up to 16 bit value identifying an attribute uniquely, independant of the node) and its value.
 *
 * \image html gatt_and_att.png "GATT and ATT"
 * \image latex gatt_and_att.png "GATT and ATT" width=10cm
 *
 * \example sble_example_check_for_attribute.c
 * \example sble_example_client.c
 * \example sble_example_minimal_client.c
 *
 * \date: 09.07.2012
 * \author Philipp Kindt
 */

#ifndef SBLE_ATTCLIENT_H_
#define SBLE_ATTCLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sble_state.h"
#include "sble_config.h"
#include "sble_types.h"
#include "sble_ll.h"
#include "sble_bgapi_call.h"
#include "sble_bgapi_call_ext_thread.h"
#include <inttypes.h>

/**
 * This FLAG sets if mode with which the Bluetooth device sends data.
 * 0 -> slow sending
 * 1 -> fast sending
 */
//#define SBLE_ATTCLIENT_SENDING_MODE_CONFIRMED 0
//#define SBLE_ATTCLIENT_SENDING_MODE_UNCONFIRMED 1

#define SBLE_ATTCLIENT_SENDING_MODE SBLE_ATTCLIENT_SENDING_MODE_CONFIRMED

/**
 * Retrives a list of attributes. It will be stored in dstate->cons[con].ll_gatt_values.
 * If called more than once,the attribute list will be resetted before each call.
 * \param con connection number
 */
void sble_attclient_getlist(uint32_t con);

/** Check if an attribute is within the attribute list dstate->cons[con].ll_gatt_values
 * The list must have been retrived by sble_attclient_getlist() before.
 * \param con connection number
 */
sble_bool sble_attclient_is_in_list(uint32_t con, const sble_attribute* uuid);

/**Returns a pointer on the entry in a connection's attribute list whose uuid corresponds to the one specified in the uuid-parameter.
 * \param con connection number
 * \param uuid pointer to a sble_attribute-structure. The only field that has to be filled in the the uuid-param is uuid->uuid.
 * \return pointer to entry in the attribute-list dstate->cons[con].ll_gatt_values
 */
sble_attribute* sble_attclient_get_from_list(uint32_t con, const sble_attribute* att);

/**Write an attribute on the remote node specified by con. The attribute is given by the handle on the remote server.
 * \param con connection number
 * \param handle Unique number that qualifies the attribute on the remote node. It differs from node to node!
 * \return SBLE_TRUE on success, SBLE_FALSE otherwise. At the moment, the return value is allways true.
 */
sble_bool sble_attclient_write_by_handle(uint8_t con, uint16_t handle, sble_array* data);
sble_bool sble_attclient_write_by_handle_multithreaded(uint8_t con, uint16_t handle, sble_array* data, uint8_t extThread);

/**Write an attribute on the remote node specified by con. The attribute is given by the attributes' global UUID.
 * \param con connection number
 * \param att sble_attribute that contains the UUID of the attribute. All other fields of att are neglected
 * \return SBLE_TRUE if attribute exists, SBLE_FALSE otherwise. No writing errors are detected currently.
 */
sble_bool sble_attclient_write_by_attribute(uint8_t con, sble_attribute* att, sble_array* data);
sble_bool sble_attclient_write_by_attribute_multithreaded(uint8_t con, sble_attribute* att, sble_array* data,uint8_t extThread);

/**Write an attribute on the remote node specified by con. The attribute is given by the attributes' global UUID.
 * \param con connection number
 * \param uuid UUID of the attribute
 * \return SBLE_TRUE if attribute exists, SBLE_FALSE otherwise. No writing errors are detected currently.
 */
sble_bool sble_attclient_write_by_uuid(uint8_t con, sble_array* uuid, sble_array* data);


/**Read an attribute on the remote node specified by con. The attribute is given by the handle on the remote server.
 * \param con connection number
 * \param handle Unique number that qualifies the attribute on the remote node. It differs from node to node!
 * \return the payload (contains the handle's value)
 */
sble_payload* sble_attclient_read_by_handle(uint8_t con, uint16_t handle);

/**Read an attribute on the remote node specified by con. The attribute is given by the attributes' global UUID.
 * \param con connection number
 * \param uuid UUID of the attribute
 * \return pointer to the payload received. This pointer should be freed when its content is not needed anymore by using sble_payload_free_whole()
 */
sble_payload* sble_attclient_read_by_uuid(uint8_t con, sble_array* uuid);

/**Read an attribute on the remote node specified by con. The attribute is given by the attributes' global UUID.
 * \param con connection number
 * \param att sble_attribute that contains the UUID of the attribute. All other fields of att are neglected
 * \return pointer to the payload received. This pointer should be freed when its content is not needed anymore by using sble_payload_free_whole()
 */
sble_payload* sble_attclient_read_by_attribute(uint8_t con, sble_attribute* att);


/**
 * Waits until any payload is received from the node qualified by con.
 * \param con Connection number
 * \return pointer to the payload received. This pointer should be freed when its content is not needed anymore by using sble_payload_free_whole()
 */
sble_payload* sble_attclient_wait_for_payload(uint8_t con);

/**
 * Sets the mode how data are transmitted over BLE (confirmed / unconfirmed)
 * At the initialization the confirmed mode is used
 * \param sending mode (SBLE_ATTCLIENT_SENDING_MODE_CONFIRMED or SBLE_ATTCLIENT_SENDING_MODE_UNCONFIRMED)
 */
void sble_attclient_set_sending_mode(uint8_t sending_mode);

#ifdef __cplusplus
}
#endif

#endif /* SBLE_ATTCLIENT_H_ */
