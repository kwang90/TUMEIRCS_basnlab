/**
 * \file sble_type_conversion.h
 * \brief Utilities to convert one state to another.
 *
 * This function can convert hex strings to sble_arrays.
 *
 *  \date 12.07.2012
 *  \author: Philipp Kindt
 */

#ifndef SBLE_TYPE_CONVERSION_H_
#define SBLE_TYPE_CONVERSION_H_
#include "sble_types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**convert a hex string into a sble_array.
 * \param str_hex An hex string such as "ABcdeFG1234";
 * \return The sble_array that is returned is newly created and should be free'ed after use by sble_array_free_whole().
 *
 */
sble_array* sble_type_conversion_hexstring_to_binary(char* str_hex) ;

/**helper function used by ble_type_conversion_hexstring_to_binary(). Returns the numeric value of a asci char
 * (for example. 'c' sird zu
 */
uint8_t sble_type_conversion_char_2_numeric(char ch);


/** Creates and allocates a new string containing an a data buffer as hex in zero-terminated string-representation.
 *  \param data Data to be converted to string
 *  \param len Lenght of data buffer in bytes
 *  \retun newly allocated string. free() after use.
 */

char* sble_type_conversion_buf_to_hexstring(const uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif




#endif /* SBLE_TYPE_CONVERSION_H_ */

