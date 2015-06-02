/**
 * \file sble_attribute.h
 *
 * \date 12.07.2012
 * \author: Philipp Kindt
 *
 * \brief Data structure representing an ATT/GATT attribute.
 */

#ifndef SBLE_ATTRIBUTE_H_
#define SBLE_ATTRIBUTE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "sble_array.h"
#include "sble_config.h"
#include <inttypes.h>

/**
 * \struct _sble_attribute
 * \brief Represents a GATT/ATT attribute.
 */
typedef struct _sble_attribute{

	///Unique UUID of the attribute determining also its type. The UUID is the same for all nodes (including nodes by different vendors) offering the same attribute.
	sble_array* uuid;

	///Handle on the GATT server. This handle might differ on different nodes offering the same attribute.
	uint8_t handle;
} sble_attribute;


/**
 * Creates a new sble_attribute node. Give a pointer to an pointer to an sble_attribute. The sble_attribute will be created and the pointer is made
 * pointing to it.
 * \param att Pointer to an unitialized pointer on an sble_attribute
 * \param uuid_lenght Number of Bytes of the uuid of the attribute
 */
void sble_attribute_malloc_whole(sble_attribute** att, sble_unsigned_integer uuid_length);


/**
 * Frees the memory for an sble_attribute-structure and the uuid-data assigned to it.
 * \param att Pointer to an (initialized) pointer to a sble_attribute structure.
 */
void sble_attribute_free_whole(sble_attribute** att);



/** Allocates data for the attribute's uuid
 * \param att pointer to an (allocated!) sble_attribute_structure. Its data-pointer will be assigned to newly allocated space.
 */
void sble_attribute_malloc_data(sble_attribute* att, sble_unsigned_integer uuid_length);


/**Frees the uuid-data of an attribute
 * \param att pointer to an attribute
 */
void sble_attribute_free_data(sble_attribute* att);

/**
 * Compares two attributes. Returns true, if the attribute's UUIDs are equal.
 * \param a: pointer to an sble_attribute whose uuid-field is set
 * \param b: pointer to an sble_attribute whose uuid-field is set
 */
sble_bool sble_attribute_uuid_comparator(void* a, void*b);

#ifdef __cplusplus
}
#endif

#endif /* SBLE_ATTRIBUTE_H_ */
