/**
 * \file sble_scheduler.h
 * \brief Scheduling for sble. freeRTOS-Port!
 *
 * \section Scheduling in SBLE
 * This files contains everything related to the scheduling. SBLE runs within two threads:
 * \arg The main thread which receives function calls from the programm using SBLE
 * \arg The callback dispatcher thread which is woken up by the operating system every time data is received via the UART.
 *
 * \subsection Asynchronous events
 * The BLE radio can signal two message types:
 * \arg Responses to a command issued by the main thread before
 * \arg Asynchronous events (example: Value received via radio link)
 *
 * Everytime something from the BLE radio is received, the callback dispatcher thread is woken up by the operating system. Depending on the
 * preceeding calls within the main thread, the callback dispatcher perfoms different actions. In any case, a callback corresponding to the event or response that occured
 * is called. Within the callback, the global state variable dstate is modified usually. In some cases, received vallues are pushed onto one of the reception stack.
 *
 * \subsection Function calls
 * Most SBLE-functionallity is handled by sble-function-calls from the application using SBLE. These calls invoke the SBLE-Scheduler with different possibilities
 * <b>Non-Blocking call:</b>\n
 * \arg The main thread sends an api command and returns to caller immediately.
 * \arg The callback dispatcher thread is not invoked
 *
 * <b>Blocking call:</b>\n
 * \arg The main thread sends an api command and goes asleep
 * \arg The dispatcher thread wakes up the main thread if the response for this call has been signaled
 * \arg The dispatcher thread goes asleep
 * \arg the main thread wakes up the dispatcher thread if it has received the response to the command
 * \arg the calling functions returns and the dispatcher thread sleeps until the next data is received from the radio
 *
 * <b>Event waiting functions</b>\n
 * Some functions use void sble_scheduler_wait_for_event().
 * This function waits until at least one event within specified set of events had occured. It works like this:
 * \arg First, a blocking call is issued (see above)
 * \the callback dispatcher thread sleeps until any event occures
 * \the callback dispatcher thread signals the event to the main thread and wakes it up and goes asleep after that
 * \the main thread checks if the event it has ben waiting for occured. If yes, it wakes up the dispatcher thread that will go asleep again to be woken up by the operating system and returns to the caller. If not, it wakes up the dispatcher thread that will wait for the oerating system again, but will set the main-thread asleep again until the next event occurs.
 *
 *
 *  \date 06.07.2012
 *  \author: Philipp Kindt
 */

#ifndef SBLE_SCHEDULER_FREERTOS_H_
#define SBLE_SCHEDULER_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sble_config.h"

#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <inttypes.h>

/**
 * Enum that specifies one of two threads:
 * \arg SBLE_THREAD_MAIN the main thread
 * \arg SBLE_THREAD_DISPATCHER the callback dispatcher thread
 */
typedef enum _sble_thread{SBLE_THREAD_MAIN,SBLE_THREAD_DISPATCHER,SBLE_THREAD_CMD_EXTERNAL, SBLE_THREAD_EVENT_EXTERNAL} sble_thread;

/**Initialize the scheduler and start the dispatcher thread
 *
 */
void sble_scheduler_init();


/**
 * The callback dispatcher thread function. It contains an endless loop and is to be started
 * as its own thread by sble_scheduler_init(). It will call the callbacks and, to a huge extend, handle the sleep-and-wakeup-procedures
 * for the main thread in collaboration with other functions
 * \param threadarg Argument for the thread - unused.
 *
 */
void* sble_callback_dispatcher(void* threadarg);

/**Make a thread sleep.
 * \param thread The thread-identifier qualifying the thread that shall go asleep..
 */
void sble_scheduler_wait(sble_thread thread);

/**Unlock a synchronisation mutex to access global shared data.
 */
void sble_scheduler_unlock_mutex();

/**Lock a synchronisation mutex to access global shared data.
 */
void sble_scheduler_lock_mutex();


/**Wake up a sleeping thread.
 * \param thread The thread-identifier qualifying the calling thread.
 */
void sble_scheduler_wakeup(sble_thread thread);

/**Wait for one event out of a list of given events to occur.
 * Until this occured, set the thread of the calling function asleep.\n
 * Procedure:\n
 * \arg The function calling this function must first remove all events of flag_mask from the event autoclear list in the global state (dstate.evt_clear_list) by using sble_scheduler_autoclear_prevent
 * \arg Usually, the calling function issues a BGAPI-command
 * \arg After that, sble_scheduler_wakeup() is called. It waits until one of the events specified occures
 * \arg sble_scheduler_wakeup() restores those events to the event autoclear list.
 * \param thread The thread identifier of the calling thread
 * \param event_flag_mask A bitfiled specifying the events to wait for. This can be any combination of the SBLE_STATE_*-macros defined in sble_state.h
 *
 */
void sble_scheduler_wait_for_event(sble_thread thread, uint32_t event_flag_mask);
void sble_scheduler_wait_for_event_ext_thread(sble_thread thread, uint32_t event_flag_mask);
void sble_scheduler_wait_for_event_no_reset_ext_thread(sble_thread thread, uint32_t event_flag_mask);

/**Wait for one event out of a list of given events to occur.
 * Until this occured, set the thread of the calling function asleep.\n
 * Same as sble_scheduler_wait_for_event(), but does not restore the event autoclear list automatically and does not
 * remove the corresponding event-flag automatically.
 * Procedure:\n
 * \arg The function calling this function must first remove all events of flag_mask from the event autoclear list in the global state (dstate.evt_clear_list) by using sble_scheduler_autoclear_prevent
 * \arg Usually, the calling function issues a BGAPI-command
 * \arg After that, sble_scheduler_wakeup() is called. It waits until one of the events specified occures
 * \param thread The thread identifier of the calling thread
 * \param event_flag_mask A bitfiled specifying the events to wait for. This can be any combination of the SBLE_STATE_*-macros defined in sble_state.h
 *
 */

void sble_scheduler_wait_for_event_no_reset(sble_thread thread, uint32_t event_flag_mask);

/**
 * Shuts down the event dispatcher thread.
 * \param thread Even though it is possible to shut down the main thread, too, this param should allways be SBLE_THREAD_DISPATCHER.
 * Usually called by sble_shutdown().
 */
void sble_scheduler_dispatcher_shutdown(sble_thread thread);

/**
 * Start the dispatcher thead. Usually called by sble_scheduler_init() and, thus, sble_init();
 *
 */
void sble_scheduler_dispatcher_start();

/**
 *
 * Removes the events specified by a given bitfield from the event autoclear list. All events in this list will be acknowledged autmatically, so the dispatcher
 * thread continues automatically if one of these events occur. If an event occurs that is not on this list, the dispatcher thread will sleep until woken up
 * by another thread.
 * \param events Bitfield specifying the events to remove from the aec list. It can  conatin any combination of the SBLE_STATE_*-macros defined in sble_state.h
 */
void sble_scheduler_autoclear_prevent(uint32_t events);

/**
 *
 * Add the events specified by a given bitfield to the event autoclear list. All events in this list will be acknowledged autmatically, so the dispatcher
 * thread continues automatically if one of these events occur. If an event occurs that is not on this list, the dispatcher thread will sleep until woken up
 * by another thread.
 *
 * \param events Bitfield specifying the events to remove from the aec list. It can  conatin any combination of the SBLE_STATE_*-macros defined in sble_state.h
 *
 */
void sble_scheduler_autoclear_do(uint32_t events);

/**
 *
 * After an event has occured, for most events, a flag is set in the global state's flags (dstate.flags) that indicates the event has occured.
 * This function clears the events that might have occured that are specified by a bitmask
 * \param events bitmask specifiing the events to be cleared
 *
 */
void sble_scheduler_events_clear(uint32_t events);

/**
 *
 * After an event has occured, for most events, a flag is set in the global state's flags (dstate.flags) that indicates the event has occured.
 * This function makrks some events specified by a bitmask as "occured"
 * \param events bitmask specifiing the events to be set in the global state's (dstate.flags)
 *
 */
void sble_scheduler_events_set(uint32_t events);


void sble_scheduler_start_extThreadAccess();
void sble_scheduler_finish_extThreadAccess();

void sble_schedulerFRT_wait_for_hw_wakeup();
void sble_schedulerFRT_trigger_hw_wakeup();

#endif

#ifdef __cplusplus
}
#endif

#endif /* SBLE_SCHEDULER_FREERTOS_H_ */
