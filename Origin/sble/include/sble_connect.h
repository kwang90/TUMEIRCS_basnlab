/**
 * \file sble_connect.h
 * \brief connection handling
 *
 * This file handles connections to external BLE nodes.

 * \section connection modes
 * A device can act in one of two roles:\n
 * <b>Advertiser:</b>\n
 * \arg Sends atvertising packets on channels 37,38 and 39
 * \arg Advertising packets can received by a device that is in scanner mode
 * \arg Advertising packets are sent repeatedly within an advertising interval
 * <b>Scanner:</b>\n
 * \arg A Scanner listens for advertising packets
 * \arg In case of a reception of an adv packet, a "scan request" packet is sent
 * \arg The advertiser sents a scan response packet.
 *
 * (Source and more information: LPRF San Diego, Bluetooth Low Energy Deep Dive, http://e2e.ti.com/support/low_power_rf/m/videos__files/653593/download.aspx)
 *
 * To learn more about the exact timing, consult page 18 of:
 * BLE Stack API reference v1.3, Bluegiga Technologies
 *
 * 	\image html advertiser_scanner.png "avertiser and scanner"
 * 	\image latex advertiser_scanner.png "advertiser and scanner" width=10cm
 *
 *  \date 06.07.2012
 *  \author kindt
 *  \example sble_example_check_for_attribute.c
 *  \example sble_example_client.c
 */

#ifndef SBLE_CONNECT_H_
#define SBLE_CONNECT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "sble_bgapi_call.h"
#include "sble_ll.h"
#include <inttypes.h>
typedef enum {SBLE_ADDRESS_PUBLIC, SBLE_ADDRESS_RANDOM} sble_addr_type;

/**
 * Connect to any BLE-Node in range by starting scanning and connecting to the first scan response that has been received.
 * The function returns, if a connection has been established.
 * (Source for params: BLE Stack API reference v1.3, Bluegiga)
 * \param con_int_min Minimum connection interval (unit: 1.25ms), Range 7.5ms to 4000ms
 * \param con_int_max Maximum connection Interval (unit: 1.25ms), Range 7.5ms to 4000ms
 * \param timeout Supervision timeout (unit: 10ms) - Range 100ms to 32 seconds, must be bigger than connection interval. If no packets are received either by the master or the slave within this time interval, the connection is terminated.
 * \param latency Slave latency - the number of connection intervals the slave need not response. Higher latency will save energy. Range: 0-500
 * \return Connection number.
 */
uint8_t sble_connect_to_any(uint16_t con_int_min, uint16_t con_int_max, uint16_t timeout, uint16_t latency);

/**Make the device connectable by any device. After calling this function, it returns immediately but the BLE-Radio will start
 * advertising. To wait for a connection to be established, use sble_scheduler_wait_for_event() and trigger for SBLE_STATE_CONNECTION_EVENT.\n
 * (Source for params: BLE Stack API reference v1.3, Bluegiga)
 * \param adv_int_min Minimum advertisment interval (unit: 625us)
 * \param adv_int_max Maximum advertisment interval (unit: 625us)
 */
void sble_make_connectable_by_any(uint16_t adv_int_min,uint16_t adv_int_max);

/**Waits until a new connection comes in. Mostly used after sble_make_connectable_by_any():
 * \return connection number
 */


/**
 * Connect to the BLE-Node having a given BLE address.
 * The function returns, if a connection has been established.\n
 * (Source for params: BLE Stack API reference v1.3, Bluegiga)
 * \param addr Address to connect to
 * \param con_int_min Minimum connection interval (unit: 1.25ms), Range 7.5ms to 4000ms
 * \param con_int_max Maximum connection Interval (unit: 1.25ms), Range 7.5ms to 4000ms
 * \param timeout Supervision timeout (unit: 10ms) - Range 100ms to 32 seconds, must be bigger than connection interval. If no packets are received either by the master or the slave within this time interval, the connection is terminated.
 * \param latency Slave latency - the number of connection intervals the slave need not response. Higher latency will save energy. Range: 0-500
 * \return Connection number.
 */uint8_t sble_connect_to(sble_array* addr, sble_addr_type type,uint16_t con_interval_min, uint16_t con_interval_max, uint16_t timeout, uint16_t slave_latency);

 /**
  * Close a connection.
  * \param con Connection number to terminate
  * \return SBLE_TRUE on success, SBLE_FALSE otherwise. Not all errors are deteceted, yet.
  */
 sble_bool sble_disconnect(uint8_t con);

#ifdef __cplusplus
}
#endif

#endif /* SBLE_CONNECT_H_ */
