/**
 * \file sble_serial.h
 * \brief sending big buffers with the serial functions
 *
 *  \date 02.05.2013
 *  \author: Gopp Mathias <gopp@rcs.ei.tum.de>
 */

#ifndef SBLE_SERIAL_H_
#define SBLE_SERIAL_H_

#include "sble_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * depending on the platform, there are one or two bytes reserved for sending\n
 * data buffer. If it is 1 then you can send up to 5099Bytes.\n
 * If it is 2 you can send up to 1310700Bytes
 */
#define SBLE_PACKAGE_BYTES 2

/**
 * defines the number of bytes which is in one packet (20 is the maximum)
 */
#define PACKET_SIZE 20

/**
 * Defines if the sble_serial_delay is used(1) or not(0).
 * This is needed for the fast transmitting mode in combination with the wake up pin.
 * Otherwise it is not guaranteed that the Bluetooth device changes to the sleep mode.
 */
#define SBLE_SERIAL_DELAY_STATUS 0


#include "sble_attclient.h"
#include "sble_debug.h"
#include "sble_scheduler.h"
#include "sble_bgapi_call_ext_thread.h"

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
#include <pthread.h>
#include <unistd.h>
#endif



typedef struct _serialParameters
				{
					uint8_t mode;
					uint16_t conInterval;
					uint8_t actualSpeed;
					uint8_t sleepRounds;
					int thread_arg;
				} serialP;
				
extern serialP serialParameters;

typedef uint8_t* _serialBufAdr;
extern _serialBufAdr serialBufAdr;


/**
 * initializes the serial connection
 * There are three differetn modes:
 * 0) full-speed mode: connection runs with maximum speed
 * 1) dual-speed mode: changes the speed of the connection between max and min speed
 * 2) multi-speed mode: changes the speed of the connection in a few steps
 * @param connection mode
 */
int8_t sble_serial_init(uint8_t serialConnectionMode);

/**
 * sends a buffer of specific size\n
 * buffersize depends on which system it is compiled
 * @param buffersize
 * @param pointer to the data buffer
 * @param sble_attribute
 * @return returns 1 if buffer is sent and -1 if there is an error
 */
int8_t sble_serial_send(uint32_t bufsize, uint8_t* databuf, sble_attribute* attribute, uint8_t multithreading);

//same as sble_serial_send only without any header --> cannot recombine data with serial receive
int8_t sble_serial_send_pseudo(uint32_t bufsize, uint8_t* databuf, sble_attribute* attribute, uint8_t multithreading);

/**
 * \brief Receives data via the BLE serial line
 * Short form for  \ref sble_serial_receive_autoReconnect(buf,0,NULL);
 * @param buf buffer where received data is stored
 * @return number of bytes received
 */
uint32_t sble_serial_receive(uint8_t* buf);

uint32_t sble_serial_receive_pseudo(uint8_t* buf);
uint32_t sble_serial_receive_pseudo_autoReconnect(uint8_t* buf, uint8_t reconnect, sble_reconnect_callback c);

/**
 * \brief like \ref sble_serial_receive, but with the possibility to reconnect automatically if
 * the connection gets lost.
 * @param buf buffer where received data is stored
 * @param reconnect 0=> 1=> reconnect
 * @param cb Callack function to call for reconnecting. It should handle the reconnection procedure
 * @return number of bytes received
 */
uint32_t sble_serial_receive_autoReconnect(uint8_t* buf,uint8_t reconnect, sble_reconnect_callback cb);


/**
 * This function calculates the connection interval for the speed for this amount of data based on actual bytesPerSecond
 * @param bytesPerSecond
 * @param actual buffersize
 * @param update period
 * @return connection interval
 */
uint32_t sble_serial_calc_speed_adjustment(double bytesPerSecond, uint32_t buffersize, double period);

/**
 * With this function all speeds which are between 6 and 3200 can be set.
 * It can also be chosen if the command is sent blocking or non blocking.
 * @param minimum connection interval
 * @param maximum connection interval
 * @param blocking or non blocking
 */
uint8_t sble_serial_change_speed_specific(uint16_t conIntervalMin, uint16_t conIntervalMax, uint8_t blocking);

/**
 * starts advertising without the possibility for a connection
 * @param minimum advertising interval
 * @param maximum advertising interval
 */
void sble_serial_advertising(uint16_t adv_int_min,uint16_t adv_int_max);

/**
 * starts scanning active or passive which means with or without reply to a received advertising packet
 * @param scan interval
 * @param scan window
 * @param active or passiv (0,1)
 */
void sble_serial_scanning(uint16_t scan_interval,uint16_t scan_window,uint8_t active);

void sble_serial_setBufAdr(uint8_t* bufAdr);

/**
 * returns a pointer to received data
 * @return pointer to received data
 */
uint8_t* sble_serial_getBufAdr();

#ifdef __cplusplus
}
#endif

#endif /* SBLE_SERIAL_H_ */
