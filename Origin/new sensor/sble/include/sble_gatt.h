/**
 * \file sble_gatt.h
 * \brief Functions to access the local attribute (GATT) server
 *
 * The local attribute server stores the values for attributes that can be read and/or written by external nodes using the ATT-Protocol.
 *
 * \image html gatt_and_att.png "GATT and ATT"
 * \image latex gatt_and_att.png "GATT and ATT" width=10cm
 *
 * \date 12.07.2012
 * \author Philipp Kindt
 *
 * \example sble_example_gattserver.c
 * \example sble_example_minimal_gattserver.c
 *
 */



#ifndef SBLE_GATT_H_
#define SBLE_GATT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "sble_types.h"
#include "sble_scheduler.h"
#include "sble_bgapi_call.h"

/**
 * Write attribute by its local handle. The handle is unique for one device/firmware, but not among different devices having different firmwares
 * \param handle Number identifying the attribute on the device
 * \param data The data the attribute's value will be set to.
 */
void sble_gatt_write_by_handle(uint16_t handle,sble_array* data);


/**
 * Wait until a connected device modifies any value on the local GATT server. Any data received is pushed onto the gatt's receive queue (dstate.ll_gatt_values). A pointer to
 * the element added most recently in this queue is returned and the list-entry is removed.
 * Make sure to free the sble_payload structure afterwards using sble_payload_free_whole() .
 *
 * \return Pointer to the payload received.
 */
sble_payload* sble_gatt_receive();
/**
 * Wait until a connected device modifies any value on the local GATT server - version for long values. Any data received is pushed onto the gatt's receive queue (dstate.ll_gatt_values). A pointer to
 * the element added most recently in this queue is returned and the list-entry is removed.
 * Make sure to free the sble_payload structure afterwards using sble_payload_free_whole() !
 * This function will only return if an attribute having the attribute handle atthandle and data_lenght number of bytes lenght has been received.
 * If another attribute is received meanwhile, NULL will be returned. If the attribute with handle atthandle has ben received, but the number of bytes do not match data_lenght, the
 * function will block forever.
 * \param atthandle handle of the attribute to be recieved
 * \param  data_lenght number of bytes the attribute is expected to have
 * \return Pointer to the payload received or NULL, if another handle has been received but not the one specified.
 */
sble_payload* sble_gatt_receive_long(sble_unsigned_integer atthandle, sble_unsigned_integer data_length);

/**
 * Wait until a connected device modifies any value on the local GATT server.
 * Any data received is pushed onto the gatt's receive queue (dstate.ll_gatt_values). A pointer to
 * the element added most recently in this queue is returned and the list-entry is removed.
 * Make sure to free the sble_payload structure afterwards using sble_payload_free_whole() !
 * This function can handle long values automatically. However, it returns when the next attribute
 * is beginning to be received, and not with the end of the current attribute.
 * \return Pointer to the payload received or NULL in case of failure.
 */

sble_payload* sble_gatt_receive_until_next();


/**
 * Read the value of an attribute qualified by its handle.\n
 * This value is not pushed to the GATT receive queue (dstate.pl_gatt_value) as
 * it receives fully deterministically only due to a corresponding request. It is copied to dstate.pl_gatt_value in struct _sble_driver_state(). Do
 * never free the value returned as the pointer points to dstate.pl_gat_value. It will automatically freed when the next value is requested, so do not double-free.
 * If you need more than one of theese values at the same time, make sure to copy the old data somewhere else.
 * \param handle Number identifying the attribute on the device
 * \return Pointer to the payload containing to the attribute's value.\n
 */
sble_payload* sble_gatt_read_by_handle(uint16_t handle);

/**
 * Read out the UUID of an attribute qualified by its handle\n
 * This value is not pushed to the GATT receive queue (dstate.pl_gatt_value) as
 * it receives fully deterministically only due to a corresponding request. It is copied to dstate.pl_gatt_value in struct _sble_driver_state(). Do
 * never free the value returned as the pointer points to dstate.pl_gat_value. It will automatically freed when the next value is requested, so do not double-free.
 * If you need more than one of theese values at the same time, make sure to copy the old data somewhere else.
 * \param handle Number identifying the attribute on the device
 * \return Pointer to the payload containing to the attribute's value.\n
 * \param handle Number identifying the attribute on the device
 * \return UUID of the attribute
 */
sble_array* sble_gatt_get_type(uint16_t handle);

#ifdef __cplusplus
}
#endif

#endif /* SBLE_GATT_H_ */
