/*
 * dtp_debug.h - Debug messages for the DTP protocol
 *
 *  Created on: 13.03.2013
 *      Author: kindt
 */

#ifndef DTP_DEBUG_H_
#define DTP_DEBUG_H_


#define DTP_DEBUG(...) printf("[%s,l. %d, %s ] ",__FILE__, __LINE__, __FUNCTION__); printf(__VA_ARGS__); printf("\n");
#define DTP_DEBUG(...)
#endif /* DTP_DEBUG_H_ */
