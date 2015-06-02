/**
 * \file sble_payload.h
 * \brief Data structure representing payload.
 *
 *  Data structure containing a data buffer, an attribute handle and a connection number representing payload transmitted via the BLE radio.
 *
 *  \date 06.07.2012
 *  \author Philipp Kindt
 */

#ifndef SBLE_PAYLOAD_H_
#define SBLE_PAYLOAD_H_
#ifdef __cplusplus
extern "C" {
#endif

//fwd declaration
struct _sble_ll;

#include <inttypes.h>
#include "sble_array.h"

/**Get data buffer from payload. Not Typesafe!
 * \param pl pointer on a sble_payload structure
 * \return pointer to sble_array that contains the payload's data
 */
#define sble_payload_get_data(ppl) return(ppl->data)

/**
 * \brief Structure representing payload that is transmitted via ATT protocol.
 */
typedef struct _sble_payload{

	///The attribute handle corresponding with the payload that is to be / has been transmitted.
	uint16_t atthandle;

	///The actual transmission data.
	sble_array* data;

	///Connection number. Each connected device will get its own number
	uint8_t connection;
} sble_payload;



/**
 * Create a new sble_payload structure.
 * \param pl: Pointer to a pointer to a sble_payload_structure. *pl should be unitialized. The alloc'ed memory will be assigned
 * to pl.
 * \param data_lenght Number of payload bytes - i.e. the number of bytes for the value to be transmitted.
 */
void sble_payload_malloc_whole(sble_payload** pl, sble_unsigned_integer data_length);

/**
 * Free a sble_payload structure. Frees both the data and the sble_payload-structure itself
 * \param pl: Pointer to a pointer to a sble_payload_structure. *pl must point to sble_payload-structure createdby sble_payload_malloc_whole(). *pl will be set to NULL.
 */
void sble_payload_free_whole(sble_payload** pl);

/* Create space for the data for an existing sble_payload object.
 * \param pl Pointer to a sble_payload structure.
 * \param data_length Number of bytes of the data array representing the payload's value.
 */
void sble_payload_malloc_data(sble_payload* pl, sble_unsigned_integer data_length);

/*Frees the data pointer of an sble_payload strucutre, but not the sble_payload-structure itself.
 * \param pl Pointer to a sble_payload structure.
 */
void sble_payload_free_data(sble_payload* pl);

/* Comparator function for sble_payload that compares by handle
 * \param a: pointer to sble_payload to compare
 * \param b: pointer to sble_payload to compare
 * \return SBLE_TRUE, if a->atthandle = b->atthandle; SBLE_False, otherwise.
 */
sble_bool sble_payload_comparator_by_handle(sble_payload *a, sble_payload* b);


/**print hex dump of linked list data of node-type sble_payload
* \param ll linked list containing nodes of type sble_payload
* \return none
*/
void sble_payload_print_ll(struct _sble_ll* ll);

#ifdef __cplusplus
}
#endif

#endif /* SBLE_PAYLOAD_H_ */
