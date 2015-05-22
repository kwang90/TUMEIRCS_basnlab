/*
 * dtp_cfg.h - Configuration of the data transmission protocol
 *
 *  Created on: 12.03.2013
 *     Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#ifndef DTP_CFG_H_
#define DTP_CFG_H_

#include <inttypes.h>

//type definitions
typedef uint8_t dtp_cfg_id_type;				//Data type for type IDs
typedef uint8_t dtp_cfg_size_type;				//Data type for size storages

#define DTP_CFG_REALLOC_PRESENT 0				//if 1, realloc() is used, otherwise, malloc() and free()
#define DTP_CFG_STATIC_ALLOC_MAXCHUNCS 10		//The maximum number of chuncs in one frame. Keep as low as possible to get a small memory footprint


//Set the default platform if nothing is set by external define flags. Possible values:
//DTP_CFG_PLATTFORM_POSIX				=> Posix
//DTP_CFG_PLATTFORM_STM32F4_FREERTOS	=> RCS FreeRTOS-based sensor node plattform
#ifndef DTP_CFG_PLATTFORM
#define DTP_CFG_PLATTFORM DTP_CFG_PLATTFORM_POSIX
#endif
#endif /* DTP_CFG_H_ */
