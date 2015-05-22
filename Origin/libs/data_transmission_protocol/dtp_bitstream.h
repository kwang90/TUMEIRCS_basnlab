/*
 * dtp_bitstream.h - Raw data bitstream.
 *
 *  Created on: 12.03.2013
 *      Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#ifndef DTP_BITSTREAM_H_
#define DTP_BITSTREAM_H_

#include "dtp_cfg.h"
#include "dtp_frame.h"
#include "dtp_chunc.h"
#ifdef __cplusplus
#pragma message("C++ detected")
extern "C" {
#endif


/** Generate bitstream from a dtp_frame
 * \param f				Pointer so input bitstream
 * \param bitstream		Pointer to the an existing bitstream. If this pointer is NULL, a new bitstream is malloc'ed.
 * 						Otherwise, the bitstream is tried to be realloc'ed
 * \param size			If not NULL, the size of the generated bitsream is stored in this param
 * \return				Pointer to generated btw. reallocated bitstream
 */
uint8_t* dtp_bitstream_generate(dtp_frame* f, uint8_t *bitstream, dtp_cfg_size_type *size);

/**
 * Print interpreted bitstream to stdout
 * \param bitstream		pointer to a bitstream
 */
void dtp_bitstream_print(uint8_t* bitstream);
#ifdef __cplusplus
}
#endif


#endif /* DTP_BITSTREAM_H_ */
