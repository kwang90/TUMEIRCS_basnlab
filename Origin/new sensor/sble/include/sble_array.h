/**
 * \file sble_array.h
 *
 *
 *	\brief Datatype representing an array.
 *
 *	An array consists of a pointer to allocated data and its size in bytes.
 *
 *  Created on: 09.07.2012
 *  Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 * \example sble_example_array.c
 *
 */

#ifndef SBLE_ARRAY_H_
#define SBLE_ARRAY_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "sble_config.h"

/**
 * \struct _sble_array
 * \brief represents an array.
 * Represents an array containing a pointer to allocated data and its size in bytes.
 *
 */
typedef struct _sble_array{

	///Pointer to allocated data; "Payload" of the array.
	uint8_t* data;

	///The lengt of the data array in bytes.
	sble_unsigned_integer len;
} sble_array;

/**
 * \brief Allocate memory for the data of an array.
 *
 *  This function allocates memory for an sble_array's data structures, but not for the sble_array-structure itself.
 *  @param array pointer to an sble_array structure that must allready be allocated
 *  @param data_len number of payload bytes in sble_array
 *  @return none
 */
void sble_array_malloc_data(sble_array* array, sble_unsigned_integer data_len);


/**
 * \Brief Resize array to new lenght data_len
 *
 * This function resizes the array. If it is enlarged, the old data will be kept. New memory space might be uninitialized,
 * depending on the targets system's reallc_() - implementation (on POSIX, it will be).
 * @param array pointer to sble_array to be resized
 * @param data_len new lenght of sble_array in bytes
 */
void sble_array_resize(sble_array* array, sble_unsigned_integer data_len);

/**
 * \brief Free the space allocated to an sble_array's payload.
 *  @param array pointer to an sble_array structure. The sble_array for this pointer must be allocated before calling this function.
 *  @return none
 */
void sble_array_free_data(sble_array* array);



/**
* \brief Free the space allocated to an sble_array's payload and the sble_structure itself.
*
*  @param array pointer to a pointer to an sble_array structure. The sble_array this pointer points to does not net to be allocated before.
* 	The *array-pointer will point to the newly allocated sble_array-structure after calling the function.
*  @param data_len number of payload bytes in sble_array.
*  @return none
*/
void sble_array_malloc_whole(sble_array** array, sble_unsigned_integer data_len);

/**
 * \brief Free the space allocated to an sble_array's payload and the corresponding sble_array-data-structure.
 *  @param array pointer to a pointer to an sble_array structure. The sble_array for the *array - pointer must be allocated before calling this function.
 *  @return none
 */
void sble_array_free_whole(sble_array** array);

/**
 * Comparator function for sble_arrays. Returns true, if arrays *a and *b are equal.
 * @params a,b: sble_arrays to compare
 * @return SBLE_TRUE, if both arrays have equal payload. SBLE_FALSE, if not.
 */
sble_bool sble_array_comparator(void* a, void*b);

#ifdef __cplusplus
}
#endif

#endif /* SBLE_ARRAY_H_ */
