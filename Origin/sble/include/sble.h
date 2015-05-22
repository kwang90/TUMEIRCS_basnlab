/**
 * \file sble.h
 * \brief Master include for all sble-includes
 *
 *  \date 09.07.2012
 *  \author: Philipp Kindt
 * \mainpage SBLE - Serialized, easy-to-use Bluetooth Low Energy (BLE)
 *
 *
 * (c) 2012 Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 * \section sec_intro Introduction
 *	Bluetooth low energy is a lowlevel, packet-based radio procol. Events come in a very asynchronous way.
 *  This toolbox synchronizes the communication between BLE112 / BLED112 modules and your application and makes bluetooth low energy easy to use.
 *
 * \section sec_porting Porting SBLE
 * SBLE is plattform-independant. Currently, there is a port for POSIX which assumes a BLE112/BLED112-Module is attached as a virtual terminal device (/dev/ttyACM*).
 * Another port is for FreeRTOS, which assumes to run on an embedded system which implements a read/write-function to a file-descriptor number
 *
 * \section Multi-Threading
 * In some applications, it is necessary that multiple threads access SBLE.
 * SBLE supports multiple threads as follows:
 *
 * 1) Commands can be issued by an arbitrary amount of threads savely.
 * The first thread should use the functions defined in \ref sble_bgapi_call.c, all other ones
 * the functions defined in \ref sble_bgapi_call_ext_thread.h
 *
 * 2) Waiting for events
 * Two threads can wait for events simultaneously, while an arbitrary amount of threads can issue commands at the same time.
 * The first thread should use the "normal" functions, the secodn one the ones ending with *extThread (e.g., \ref void sble_scheduler_wait_for_event_extThread()
 * More than one external thread (> 2 threads in whole) must not wait for events concurrently. However, there is a mechanism for synchronization:
 * use \ref sble_scheduler_start_extThreadAccess() before a thread executes a commands and waits for a corresponding event
 * and \ref sble_scheduler_finish_extThreadAccess() after that. This ensures that only one external thread is waiting for an event simultaneously.
 *
 * Please note that this is no real limitation to any application, as one external thread can wait for multiple events simultaneously.
 *
 * \section sec_ref Further References

 * \arg<b>Introduction to Blutooth Low Energy standard:</b> http://e2e.ti.com/support/low_power_rf/m/videos__files/653593/download.aspx
 * \arg<b>Institute for Realtime-Computer-Systems (RCS), TUM:</b> http://rcs.ei.tum.de
 * \arg<b>Bluegiga:<b> http://www.bluegiga.com</b>
 * \arg<b>BLE112/BLED112/DKBLE112:<b> http://www.bluegiga.com/bluetooth-low-energy
 */

#ifndef SBLE_H_
#define SBLE_H_

#include "sble_bgapi_call.h"
#include "sble_bgapi_call_ext_thread.h"
#include "sble_io.h"
#include "sble_scheduler.h"
#include "sble_connect.h"
#include "sble_types.h"
#include "sble_debug.h"
#include "sble_attclient.h"
#include "sble_event_handler_functions.h"
#include "sble_type_conversion.h"
#include "sble_gatt.h"
#include "sble_init.h"
#include "sble_serial.h"

#endif /* SBLE_H_ */
