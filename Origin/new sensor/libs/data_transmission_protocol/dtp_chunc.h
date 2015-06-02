/*
 * dtp_chunc.h - Defines one signle chunc in a DTP-frame.
 *
 *  Created on: 12.03.2013
 *      Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#ifndef DTP_CHUNC_C_
#define DTP_CHUNC_C_
#include "dtp_cfg.h"
typedef struct __attribute__ ((packed)) _dtp_chunc{
		dtp_cfg_id_type type_id;						//a unique ident for this data type. See payloads/dtp_payload_type_ids.h
		dtp_cfg_size_type size;							//the size of payload data of this chunc
		uint8_t* data;									//pointer to the payload data of this chunc
} dtp_chunc;


#endif /* DTP_CHUNC_C_ */
