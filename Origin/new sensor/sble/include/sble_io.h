/**
 * \file sble_io.h
 * \brief Input/Output backend
 *
 * This file does the I/O to the BLE112 or BLED112-Device. The header-file is platform-independant, whereas the
 * corresponding .c-files is highly platform-dependant. It must be reimplemented for any platform.
 * Naming Convention for implementations of this header: sble_io_[platform].c in the ports subdirctory.
 *
 * \author Philipp Kindt
 * \date 06.2012
 */

#include <inttypes.h>
#ifndef SBLE_IO_H_
#define SBLE_IO_H_
#include "sble_config.h"
#ifdef __cplusplus
extern "C" {
#endif



typedef void (*sble_wakeup_function_init)(uint8_t wakeup);

typedef struct _sble_io_init_state{
	sble_wakeup_function_init wufinit;
}sble_init_state;

/**
 *Establihes a connection to bluetooth low energy module
 * via a (pseudo-)serial device node such as /dev/ttyACM0 on posix
 * \param param A parameter. Its interpretation is plattform-dependant. On posix: path to the device file connecting to the radio, such as /dev/ttyACM0
 */
 void sble_io_init(char* param);

/**
 * Disconnects from bluetooth low energy module
 *
 */


 /** Returns SBLE_TRUE if the IO connection to BLE112 exists
  * \param none
  * \return SLBE true if connected to BLE112, SBLE_FALSE, otherwise
  */
 sble_bool sble_io_is_connected();

 void sble_io_disconnect();

/**
 *Send BGAPI-message to bluetooth low energy module via BGAPI
 * \param len1 length of first part of message (header)
 * \param dbuf1 data for first part of the message (header)
 * \param len2 length of second part of the message (payload)
 * \param dbuf2 data of the second part of the message (payload)
 **/
void sble_io_out(uint8_t len1, uint8_t* dbuf1, uint16_t len2, uint8_t* dbuf2);
/**
 * Reads a requested amount of bytes from the BLE's filedescriptor. Blocks until the amount of bytes
 * has ben revceived.
 *  \param buf Pointer to a buffer the data is written to. Must be at least len bytes of allocated memory.
 *  \param len Number of bytes to read.
 */
sble_signed_integer _sble_io_read_raw(uint8_t* buf, uint32_t len);


/**
 *Read out BGAPI-message. This function must be polled continuously.
 *It invokes the callbacks for BGAPI
 */
sble_signed_integer sble_io_read();


/**
 *reset BLE112/BLED112 device.
 * \param device Parameter qualifiing the device. Its interpretation is plattform-dependant. On posix: path to the device file connecting to the radio, such as /dev/ttyACM0
 */
void sble_io_reset(char* device);

#ifdef __cplusplus
}
#endif

#endif /*SBLE_I_H_ */
