/*
 * dtp_frame.h - Toplevel representation of a single DTP transmission.
 *
 *  March 2013, Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 *  A DTP frame consists of multiple chuncs. Each chuncs contains a payload.
 *
 * +====================+
 * +     dtp_frame      +
 * +====================+
 * +	chunc 1			+
 * +	 payload 1		+
 * +--------------------+
 * +    chunc 2         +
 * +     payload 2      +
 * +====================+
 *
 * A dtp-bitstream can be generated out of a dtp-frame. This bitstream can be transmitted over any channel and then
 * reassembled to a dtp-frame. From this dtp-frame, the single chuncs and hence, the payloads can be retrived easily.
 *
 *
 *
 * Example:
 * #include <data_transmission_protocol/dtp.h>
 * #include <data_transmission_protocol/payloads/dtp_payload_acceleration.h>
 * #include <data_transmission_protocol/payloads/dtp_payload_rssi.h>
 *
 * void main_receiver(int fd){			//programm receiving a dtp-bitstream from a filedescriptor fd
 *
 *	dtp_payload_acceleration *acc;
 *	dtp_frame* frame;
 *
 *
 *		while ((frame = dtp_frame_generate_from_filedescriptor(fd)) != NULL){
 *			if(dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_ACCELERATION) != NULL){
 *				acc = ((dtp_payload_acceleration*) dtp_frame_get_chunc_by_id(frame,DTP_PAYLOAD_TYPE_ID_ACCELERATION)->data);
 *				printf("acc->aX: %d\n",acc->aX);
 *			}else{
 *				acc = NULL;
 *			}
 *		}
 * }
 *
 *
 * void main_sender(fd){			//programm generating a dtp_bitstream and writes it to filedescriptor fd
 *		dtp_payload_acceleration acc;
 *		dtp_payload_rssi rssi;
 *
 *		dtp_chunc c_accel;
 *		dtp_chunc c_rssi;
 *		dtp_frame f;
 *		dtp_frame_init(&f);						//create a frame

 *		//build the chuncs
 *		c_accel.size = sizeof(acc);								//set size
 *		c_accel.data = &acc;									//set payload
 *		c_accel.type_id = DTP_PAYLOAD_TYPE_ID_ACCELERATION;		//set type id
 *		c_rssi.size = sizeof(rssi);
 *		c_rssi.data = &rssi;
 *		c_rssi.type_id = DTP_PAYLOAD_TYPE_ID_RSSI;
 *
 *		uint8_t* bitstream = NULL;
 *		dtp_cfg_size_type bs_size;
 *
 *		//add chuncs to bitstream
 *
 *		dtp_frame_add_chunc(&f,&c_accel);
 *		dtp_frame_add_chunc(&f,&c_rssi);
 *			while(1){
 *				acc.aX = 3.32323;
 *				acc.Ay = 1.00000;		//any acceleration value...
 *				bitstream = dtp_bitstream_generate(&f,bitstream,&bs_size);
 *				write(fd,bitstream,bs_size);
 *			}
 *}
 *
 */

#ifndef DTP_FRAME_H_
#define DTP_FRAME_H_
#include "dtp_cfg.h"
#include "dtp_chunc.h"
#include <inttypes.h>


/* Toplevel representation of a DTP frame */
typedef struct __attribute__ ((packed)) _dtp_frame{
	dtp_cfg_size_type nChuncs;				//Number of chuncs in this frame
	dtp_cfg_size_type size;					//Size of all chuncs (including the type-IDs and size-fields) in this frame

	dtp_chunc** chuncs;						//pointer to an array of chuncs
} dtp_frame;

#ifdef __cplusplus
#pragma message("C++ detected")
extern "C" {
#endif


/**
 * Initialize a dtp-frame. Call before using a dtp-frame!
 * \param frame pointer to an uniitialized dtp_frame
 */
void dtp_frame_init(dtp_frame* frame);

/** Retrives a chunc from a frame by its type ID. Only works if there is not more than one chunc with the same ID
 *  In a frame.
 *  \param frame		Pointer to an initialized dtp_frame to retrive the chunc from
 *  \param type_id		The type ID of the chunc to retrive
 *  \return 			Pointer to the chunc found, if type_id is contained in the frame; NULL, otherwise.
 */
dtp_chunc* dtp_frame_get_chunc_by_id(dtp_frame* frame, dtp_cfg_id_type type_id);

/**
 * Add a dtp_chunc to a dtp_frame.
 *
 *  \param frame		Pointer to an initialized dtp_frame to instert the chunc into
 *	\param chucn		Pointer to a chunc to insert
 */
void dtp_frame_add_chunc(dtp_frame* frame, dtp_chunc *chunc);

/**
 *  Dealloc a dtp_frame.
 * \param frame					Pointer to an initialized dtp_frame to destroy
 * \param destroy_chuncs		If greater than zero, all dtp_chuncs contained in the frame are free'ed, too
 * \param destroy_chunc_data	If greater than zero, all payloads within all dtp_chuncs in the frame are free'ed, too
 */
void dtp_frame_destroy(dtp_frame* frame, uint8_t destroy_chuncs, uint8_t destroy_chunc_data);

/**
 * Print dtp_frame to stdout
 * \param frame					Pointer to dtp_frame to print
 */
void dtp_frame_print(dtp_frame* frame);

/** Get the size of a dtp-frame
 * \param frame					Pointer to an initialized dtp_frame the size is to be retrived
 * \param wholeframe			If zero, the size of all chuncs (including the chunc id and the chunc's data pointer and their payloads) is returned, but withouth the size of the frame itself.
 *								If greater than zero, the size of the frame header (number of chuncs + pointer to chunc array) is added to this.
 *
 *	\return 					Size of the frame, depending on wholeframe
*/
dtp_cfg_size_type dtp_frame_get_size(dtp_frame* frame, uint8_t wholeFrame);

/**
 * Generate a dtp_frame from a bitstream.
 * \param bitstream				Bitstream containing (only) a dtp-frame
 * \return 						Pointer to newly genrated frame
 */
dtp_frame* dtp_frame_generate(uint8_t *bitstream);

#if DTP_CFG_PLATTFORM== DTP_CFG_PLATTFORM_POSIX
/**
 * Generates a dtp_frame from a bitstream coming from a raw file descriptor. Only available on POSIX-compatible systems.
 * \param fd					posix-filedescriptor to read from
 * \return 						Pointer to newly genrated frame
 */
dtp_frame* dtp_frame_generate_from_filedescriptor(int fd);
#endif

#ifdef __cplusplus
}
#endif
#endif /* DTP_FRAME_H_ */
