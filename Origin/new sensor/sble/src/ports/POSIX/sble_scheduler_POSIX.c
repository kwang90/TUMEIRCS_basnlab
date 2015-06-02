/*
 * sble_scheduler.c
 *
 *  Created on: 06.07.2012
 *      Author: kindt
 */

#include "sble_scheduler.h"
#include "sble_state.h"
#include "sble_io.h"
#include "sble_debug.h"
#include "sble_bgapi_call.h"
#if SBLE_PLATTFORM_ARCHITECTURE == SBLE_PLATTFORM_ARCHITECTURE_POSIX
#include <pthread.h>
#include <fcntl.h>

static uint8_t wuDispatcherBeforeSleep = 0;
static uint8_t wuMainBeforeSleep = 0;

void* sble_callback_dispatcher(void* threadarg)
{
	SBLE_DEBUG("Callback dispatcher thread has started.");
	sble_scheduler_wakeup(SBLE_THREAD_MAIN);
	sble_signed_integer result;
	pthread_mutex_lock(&dstate.mutexData);
	while (dstate.filedescriptor >= 0)
	{

		//SBLE_DEBUG("Waiting for API RSP/EVT...");
		if (dstate.flags & SBLE_STATE_TERMINATE)
		{
			pthread_mutex_unlock(&dstate.mutexData);
			//		SBLE_DEBUG("Waking Up main thread - doing wakeup");

			sble_scheduler_wakeup(SBLE_THREAD_MAIN);


			SBLE_DEBUG("Dispatcher thread terminating due to external request.");
			return NULL;
		}
		pthread_mutex_unlock(&dstate.mutexData);
		//SBLE_DEBUG("IO/R");

		result = sble_io_read();
		pthread_mutex_lock(&dstate.mutexData);

		if (dstate.flags & SBLE_STATE_TERMINATE)
		{
			pthread_mutex_unlock(&dstate.mutexData);

			SBLE_DEBUG("Dispatcher thread terminating due to external request.");

			return NULL;
		}
		pthread_mutex_unlock(&dstate.mutexData);

		if (result <= 0)
		{
			SBLE_DEBUG("Read failed - terminating dispatcher thread...");
			sble_io_disconnect();
			sble_scheduler_wakeup(SBLE_THREAD_MAIN);
			sble_scheduler_events_set(SBLE_STATE_TERMINATE);
			SBLE_DEBUG("goodbye!");
			return NULL;
		}
		//		SBLE_DEBUG("CBD: GOT API RSP/EVT after read...");
		//		SBLE_DEBUG_CON("Flags: ");
		//		sble_print_bitfield(dstate.flags,32);

		pthread_mutex_lock(&dstate.mutexData);
		dstate.flags_thread2 |= dstate.flags;

		if (dstate.flags & SBLE_STATE_EVENT)
		{

			if (dstate.flags & dstate.evt_clear_list)
			{

				//SBLE_DEBUG("Autoclear for event flag...");
				//SBLE_DEBUG_CON("Flags: ");
				//sble_print_bitfield(dstate.flags,32);
				//SBLE_DEBUG_CON("EVC: ");
				//sble_print_bitfield(dstate.evt_clear_list,32);

				dstate.flags &= ~(SBLE_STATE_EVENT);
				dstate.flags &= ~(dstate.evt_clear_list);
				//				 SBLE_DEBUG_CON("Flags (after): ");
				//				 sble_print_bitfield(dstate.flags,32);
				//				SBLE_DEBUG("\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!\nDROPPED EVENT\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			}



		}
		if (dstate.flags_thread2 & SBLE_STATE_EVENT){

			if (dstate.flags_thread2 & dstate.evt_clear_list_thread2){
/*
				SBLE_DEBUG("Ext: Autoclear for event flag...");
				SBLE_DEBUG_CON("Ext: Flags: ");
				sble_print_bitfield(dstate.flags_thread2,32);
				SBLE_DEBUG_CON("Ext: EVC: ");
				sble_print_bitfield(dstate.evt_clear_list,32);
*/
				dstate.flags_thread2 &= ~(SBLE_STATE_EVENT);
				dstate.flags_thread2 &= ~(dstate.evt_clear_list_thread2);
			/*
				SBLE_DEBUG_CON("Ext: Flags (after): ");
				 sble_print_bitfield(dstate.flags_thread2,32);
				SBLE_DEBUG("\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!\nExt: DROPPED EVENT\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			*/
			}
		}

		//external thread requests sensing a command
		if((!(dstate.flags & SBLE_STATE_CMD_SENT)) && (dstate.cmdExtRequest == SBLE_EXTCMD_REQUEST))
		{
	//		SBLE_DEBUG("Ext. CMD Request");
			dstate.cmdExtRequest = SBLE_EXTCMD_ACTIVE;
			pthread_mutex_unlock(&dstate.mutexData);
			sble_scheduler_wakeup(SBLE_THREAD_CMD_EXTERNAL);
			pthread_mutex_lock(&dstate.mutexData);

		//external thread waits for response AND response came in
		}else if((dstate.flags & SBLE_STATE_RESPONSE_RECEIVED)&&(dstate.flags_sched_internal & SBLE_FLAG_SCHED_FOR_RESPONSE_ACK)&&(dstate.cmdExtRequest == SBLE_EXTCMD_ACTIVE)){
			pthread_mutex_unlock(&dstate.mutexData);
	//		SBLE_DEBUG("Ext. CMD response");
			sble_scheduler_wakeup(SBLE_THREAD_CMD_EXTERNAL);
			sble_scheduler_wait(SBLE_THREAD_DISPATCHER);
			pthread_mutex_lock(&dstate.mutexData);

		}


		//events and commands for thread 1
		if ((dstate.flags & SBLE_STATE_EVENT)|| ((dstate.flags & SBLE_STATE_RESPONSE_RECEIVED)&& (dstate.flags_sched_internal& SBLE_FLAG_SCHED_FOR_RESPONSE_ACK)&&(dstate.cmdExtRequest == SBLE_EXTCMD_IDLE)))
		{
		//	SBLE_DEBUG("Thread 1 Response or event...");

			//if there is no send request or data are received then wake up main thread
			//else wake up the external cmd thread

			pthread_mutex_unlock(&dstate.mutexData);
		//	SBLE_DEBUG("wake up main thread");
			sble_scheduler_wakeup(SBLE_THREAD_MAIN);
			sble_scheduler_wait(SBLE_THREAD_DISPATCHER);
			pthread_mutex_lock(&dstate.mutexData);


		}
		//events for thread 2
		if (dstate.flags_thread2 & SBLE_STATE_EVENT)
		{
		//	SBLE_DEBUG("Ext: Waiting for EVENT flag to be cleared (thread 2)...");

			//if there is no external cmd  request or data are received then wake up main thread
			//else wake up the sending thread

			pthread_mutex_unlock(&dstate.mutexData);
		//	SBLE_DEBUG("wake up external cmd thread (thread2)");
			sble_scheduler_wakeup(SBLE_THREAD_EVENT_EXTERNAL);
			sble_scheduler_wait(SBLE_THREAD_DISPATCHER);
			pthread_mutex_lock(&dstate.mutexData);

		}

		if (dstate.flags & SBLE_STATE_TERMINATE)
		{
			pthread_mutex_unlock(&dstate.mutexData);
			SBLE_DEBUG("Dispatcher thread shutting down due to external shutdown request");
			return NULL;
		}


		//SBLE_DEBUG("next cycle...");
	}
	return NULL;
}

void sble_scheduler_init() {
	sem_init(&dstate.mutexDispatcher, 0,0);

	sem_init(&dstate.mutexMain, 0,0);

	sem_init(&dstate.mutexCmdExternal, 0,0);

	sem_init(&dstate.mutexEventExternal, 0,0);

	pthread_mutex_init(&dstate.mutexData, NULL);

	pthread_mutex_init(&dstate.mutexMultExtThread,NULL);


	SBLE_DEBUG("Starting callback dispatcher thread...");
	//setup callback dispatcher
	sble_scheduler_dispatcher_start();
	//sble_scheduler_read_event_start();

	SBLE_DEBUG("Initialization done...");
}
void _sble_scheduler_posix_cleanup(){
	SBLE_DEBUG("Cleanup running...");
	pthread_mutex_unlock(&dstate.mutexData);
//	pthread_mutex_unlock(&dstate.mutexMain);
//	pthread_mutex_unlock(&dstate.mutexDispatcher);
//	pthread_mutex_unlock(&dstate.mutexCmdExternal);
//	pthread_mutex_unlock(&dstate.mutexEventExternal);
//	pthread_cond_broadcast(&dstate.cwDispatcher);
//	pthread_cond_broadcast(&dstate.cwCmdExternal);
//	pthread_cond_broadcast(&dstate.cwEventExternal);
//	pthread_cond_broadcast(&dstate.cwMain);

//	pthread_mutex_destroy(&dstate.mutexMain);
//  pthread_mutex_destroy(&dstate.mutexDispatcher);
//	pthread_mutex_destroy(&dstate.mutexCmdExternal);
//	pthread_mutex_destroy(&dstate.mutexEventExternal);
	pthread_mutex_destroy(&dstate.mutexData);
//	pthread_cond_destroy(&dstate.cwDispatcher);
//	pthread_cond_destroy(&dstate.cwCmdExternal);
//	pthread_cond_destroy(&dstate.cwEventExternal);
//	pthread_cond_destroy(&dstate.cwMain);

	SBLE_DEBUG("Cleanup done.");
}
;void sble_scheduler_dispatcher_start() {

	int thread_arg;
	pthread_create(&dstate.sble_thread, NULL, sble_callback_dispatcher,
			&thread_arg);
	//pthread_cleanup_push(_sble_scheduler_posix_cleanup,&dstate.sble_thread);
	sble_scheduler_wait(SBLE_THREAD_MAIN);
	SBLE_DEBUG("DISPATCHER STARTUP SEQUENCE DONE.");
}

void sble_scheduler_wait(sble_thread thread) {
	pthread_mutex_lock(&dstate.mutexData);

	if (dstate.flags & SBLE_STATE_TERMINATE) {
		pthread_mutex_unlock(&dstate.mutexData);
		SBLE_DEBUG("Termination requested...");
		return;
	}
	pthread_mutex_unlock(&dstate.mutexData);

	if (thread == SBLE_THREAD_DISPATCHER)
	{
//		SBLE_DEBUG("Dispatcher Thread going asleep.");
		sem_wait(&dstate.mutexDispatcher);


//		SBLE_DEBUG("DISPATCHER Thread woke up.");
	}

	if (thread == SBLE_THREAD_MAIN)
	{
//		SBLE_DEBUG("Main Thread going asleep.");
		sem_wait(&dstate.mutexMain);

//		SBLE_DEBUG("MAIN Thread woke up.");
	}

	if (thread == SBLE_THREAD_CMD_EXTERNAL)
	{
//		SBLE_DEBUG("External CMD thread going asleep.");
		sem_wait(&dstate.mutexCmdExternal);


//		SBLE_DEBUG("External CMD Thread woke up.");
	}
	if (thread == SBLE_THREAD_EVENT_EXTERNAL)
	{
		//	SBLE_DEBUG("External event thread going asleep.");
		sem_wait(&dstate.mutexEventExternal);

		//		SBLE_DEBUG("External event thread woke up.");
	}

}


void sble_scheduler_unlock_mutex() {
	//SBLE_DEBUG("unlock");
	pthread_mutex_unlock(&dstate.mutexData);
}

void sble_scheduler_lock_mutex() {
	//SBLE_DEBUG("lock");
	pthread_mutex_lock(&dstate.mutexData);
}

void sble_scheduler_wakeup(sble_thread thread) {

	if (thread == SBLE_THREAD_DISPATCHER) {
		//		SBLE_DEBUG("Waking Up dispatcher thread.");

		sem_post(&dstate.mutexDispatcher);
		//	SBLE_DEBUG("Done: Wake up Dispatcher thread");

	}

	if(thread == SBLE_THREAD_MAIN)
	{

		//		SBLE_DEBUG("Waking Up main thread ");

		sem_post(&dstate.mutexMain);
	}

	if(thread == SBLE_THREAD_CMD_EXTERNAL)
	{
		//		SBLE_DEBUG("Waking Up External CMD thread");

		sem_post(&dstate.mutexCmdExternal);
	}

	if(thread == SBLE_THREAD_EVENT_EXTERNAL)
	{
		//		SBLE_DEBUG("Waking Up External EVENT thread");

		sem_post(&dstate.mutexEventExternal);
	}

}

void sble_scheduler_autoclear_prevent(uint32_t events) {
	pthread_mutex_lock(&dstate.mutexData);
	dstate.evt_clear_list &= ~(events);
	pthread_mutex_unlock(&dstate.mutexData);
}

void sble_scheduler_autoclear_do(uint32_t events) {
	pthread_mutex_lock(&dstate.mutexData);
	dstate.evt_clear_list |= (events);
	pthread_mutex_unlock(&dstate.mutexData);

}

void sble_scheduler_events_clear(uint32_t events) {
	pthread_mutex_lock(&dstate.mutexData);
	dstate.flags &= ~(events);
	pthread_mutex_unlock(&dstate.mutexData);

}

void sble_scheduler_events_set(uint32_t events) {
	pthread_mutex_lock(&dstate.mutexData);
	dstate.flags |= (events);
	pthread_mutex_unlock(&dstate.mutexData);

}

void sble_scheduler_wait_for_event(sble_thread thread, uint32_t event_flag_mask) {
	uint8_t first = 1;
	while (1) {
		//SBLE_DEBUG("not got any event yet, going to sleep...");
		pthread_mutex_lock(&dstate.mutexData);
		if(dstate.flags & SBLE_STATE_TERMINATE){
			pthread_mutex_unlock(&dstate.mutexData);
			pthread_mutex_unlock(&dstate.mutexMain);
			pthread_mutex_unlock(&dstate.mutexDispatcher);
			pthread_cond_broadcast(&dstate.mutexMain);

			SBLE_DEBUG("Termination requested...");
			return;
		}
		pthread_mutex_unlock(&dstate.mutexData);
		if(!first){
			sble_scheduler_wait(thread);
		}
		pthread_mutex_lock(&dstate.mutexData);

		if(dstate.flags & SBLE_STATE_TERMINATE){
			pthread_mutex_unlock(&dstate.mutexData);
			pthread_mutex_unlock(&dstate.mutexMain);
			pthread_mutex_unlock(&dstate.mutexDispatcher);
			pthread_cond_broadcast(&dstate.mutexMain);

			SBLE_DEBUG("Termination requested...");
			return;
		}

		//			SBLE_DEBUG("woken up......");

		if (dstate.flags & event_flag_mask) {
			dstate.flags &= ~(event_flag_mask);
			dstate.evt_clear_list |= (event_flag_mask);
			SBLE_DEBUG("An event waited for was received. returning control flow to applicaton");

			//dstate.flags &= ~(SBLE_STATE_EVENT);

			pthread_mutex_unlock(&dstate.mutexData);
			//wakeup other thread (usually CBD thread)
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_MAIN : SBLE_THREAD_DISPATCHER);
			}
			return;

		} else {
			dstate.flags &= ~(SBLE_STATE_EVENT);
			/*

			 SBLE_DEBUG_CON("Event flags: ")
			 sble_print_bitfield(dstate.flags,16);
			 SBLE_DEBUG_CON("waiting for: ")
			 sble_print_bitfield(event_flag_mask,16);
			 */
			//wakeup other thread (usually CBD thread)
			pthread_mutex_unlock(&dstate.mutexData);
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_MAIN : SBLE_THREAD_DISPATCHER);
			}
		}

		first = 0;
	}
}


void sble_scheduler_wait_for_event_ext_thread(sble_thread thread, uint32_t event_flag_mask)
{
	SBLE_DEBUG("POSIX wait for event");
	uint8_t first = 1;
	while (1) {
	//	SBLE_DEBUG("not got any event yet, going to sleep...");
		pthread_mutex_lock(&dstate.mutexData);
		if(dstate.flags_thread2 & SBLE_STATE_TERMINATE){
			pthread_mutex_unlock(&dstate.mutexData);
			pthread_mutex_unlock(&dstate.mutexCmdExternal);
			pthread_mutex_unlock(&dstate.mutexEventExternal);
			pthread_mutex_unlock(&dstate.mutexDispatcher);
			pthread_cond_broadcast(&dstate.mutexEventExternal);

			SBLE_DEBUG("Termination requested...");
			return;
		}
		pthread_mutex_unlock(&dstate.mutexData);
		if(!first){
			sble_scheduler_wait(thread);
		}
		pthread_mutex_lock(&dstate.mutexData);

		if(dstate.flags_thread2 & SBLE_STATE_TERMINATE){
			pthread_mutex_unlock(&dstate.mutexData);

			SBLE_DEBUG("Termination requested...");
			return;
		}

		//			SBLE_DEBUG("woken up......");

		if (dstate.flags_thread2 & event_flag_mask) {
			dstate.flags_thread2 &= ~(event_flag_mask);
			dstate.evt_clear_list_thread2 |= (event_flag_mask) | (SBLE_STATE_EVENT);
			SBLE_DEBUG("An event waited for was received. returning control flow to application");

			dstate.flags_thread2 &= ~(SBLE_STATE_EVENT);			//required in oppsite to first thread variant as bgapi_call for thread 2 does not reset the event flag, but thread 1 does

			pthread_mutex_unlock(&dstate.mutexData);
			//wakeup other thread (usually CBD thread)
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_EVENT_EXTERNAL: SBLE_THREAD_DISPATCHER);
			}

			return;

		} else {
			dstate.flags_thread2 &= ~(SBLE_STATE_EVENT);
			/*

			 SBLE_DEBUG_CON("Event flags: ")
			 sble_print_bitfield(dstate.flags,16);
			 SBLE_DEBUG_CON("waiting for: ")
			 sble_print_bitfield(event_flag_mask,16);
			 */
			//wakeup other thread (usually CBD thread)
			pthread_mutex_unlock(&dstate.mutexData);
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_EVENT_EXTERNAL : SBLE_THREAD_DISPATCHER);
			}
		}

		first = 0;
	}
}
void sble_scheduler_wait_for_event_no_reset_ext_thread(sble_thread thread, uint32_t event_flag_mask)
{
	SBLE_DEBUG("POSIX wait for event");
	uint8_t first = 1;
	while (1) {
	//	SBLE_DEBUG("not got any event yet, going to sleep...");
		pthread_mutex_lock(&dstate.mutexData);
		if(dstate.flags_thread2 & SBLE_STATE_TERMINATE){
			pthread_mutex_unlock(&dstate.mutexData);
			pthread_mutex_unlock(&dstate.mutexCmdExternal);
			pthread_mutex_unlock(&dstate.mutexEventExternal);
			pthread_mutex_unlock(&dstate.mutexDispatcher);
			pthread_cond_broadcast(&dstate.mutexEventExternal);

			SBLE_DEBUG("Termination requested...");
			return;
		}
		pthread_mutex_unlock(&dstate.mutexData);
		if(!first){
			sble_scheduler_wait(thread);
		}
		pthread_mutex_lock(&dstate.mutexData);

		if(dstate.flags_thread2 & SBLE_STATE_TERMINATE){
			pthread_mutex_unlock(&dstate.mutexData);

			SBLE_DEBUG("Termination requested...");
			return;
		}

		//			SBLE_DEBUG("woken up......");

		if (dstate.flags_thread2 & event_flag_mask) {
			dstate.flags_thread2 &= ~(event_flag_mask);
			//dstate.evt_clear_list_thread2 |= (event_flag_mask) | (SBLE_STATE_EVENT);
			SBLE_DEBUG("An event waited for was received. returning control flow to application");

			dstate.flags_thread2 &= ~(SBLE_STATE_EVENT);			//required in oppsite to first thread variant as bgapi_call for thread 2 does not reset the event flag, but thread 1 does

			pthread_mutex_unlock(&dstate.mutexData);
			//wakeup other thread (usually CBD thread)
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_EVENT_EXTERNAL: SBLE_THREAD_DISPATCHER);
			}

			return;

		} else {
			dstate.flags_thread2 &= ~(SBLE_STATE_EVENT);
			/*

			 SBLE_DEBUG_CON("Event flags: ")
			 sble_print_bitfield(dstate.flags,16);
			 SBLE_DEBUG_CON("waiting for: ")
			 sble_print_bitfield(event_flag_mask,16);
			 */
			//wakeup other thread (usually CBD thread)
			pthread_mutex_unlock(&dstate.mutexData);
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_EVENT_EXTERNAL : SBLE_THREAD_DISPATCHER);
			}
		}

		first = 0;
	}
}

void sble_scheduler_wait_for_event_no_reset(sble_thread thread, uint32_t event_flag_mask) {
	uint8_t first = 1;

	while (1) {
		pthread_mutex_lock(&dstate.mutexData);
		if(dstate.flags & SBLE_STATE_TERMINATE){
			pthread_mutex_unlock(&dstate.mutexData);

			SBLE_DEBUG("Termination requested...");
			return;
		}
		pthread_mutex_unlock(&dstate.mutexData);

	//	SBLE_DEBUG("not got any event yet, going to sleep...");
		if(!first){
			sble_scheduler_wait(thread);

		}
		pthread_mutex_lock(&dstate.mutexData);
		if(dstate.flags & SBLE_STATE_TERMINATE){
			pthread_mutex_unlock(&dstate.mutexData);
			SBLE_DEBUG("Termination requested...");
			return;
		}


		if (dstate.flags & event_flag_mask) {
			//	dstate.flags &= ~(event_flag_mask);
			//	dstate.evt_clear_list |= (event_flag_mask);
			SBLE_DEBUG("An event waited for was received. returning control flow to application");
			//wakeup other thread (usually CBD thread)
			pthread_mutex_unlock(&dstate.mutexData);
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_MAIN : SBLE_THREAD_DISPATCHER);
			}

			return;

		} else {
			dstate.flags &= ~(SBLE_STATE_EVENT);
			/*

			 SBLE_DEBUG_CON("Event flags: ")
			 sble_print_bitfield(dstate.flags,16);
			 SBLE_DEBUG_CON("waiting for: ")
			 sble_print_bitfield(event_flag_mask,16);
			 */
			//wakeup other thread (usually CBD thread)
			pthread_mutex_unlock(&dstate.mutexData);
			if(!first)
			{
				sble_scheduler_wakeup((thread == SBLE_THREAD_DISPATCHER) ? SBLE_THREAD_MAIN : SBLE_THREAD_DISPATCHER);
			}
		}

		first = 0;
	}
}

void sble_scheduler_dispatcher_shutdown(sble_thread thread) {
	SBLE_DEBUG("Shutting down dispatcher thread.");
	//pthread_mutex_lock(&dstate.mutexData);
	//pthread_mutex_unlock(&dstate.mutexData);
	dstate.flags |= SBLE_STATE_TERMINATE;
	sble_call_nb(ble_cmd_system_hello_idx);

	//	pthread_mutex_unlock(&dstate.mutexDispatcher);
	//	pthread_mutex_unlock(&dstate.mutexMain);

	//	pthread_cond_broadcast(&dstate.cwMain);
	//	pthread_cond_broadcast(&dstate.cwDispatcher);

	//	pthread_cancel(dstate.sble_thread);
	SBLE_DEBUG("Dispatcher thread should terminate soon.");
	pthread_join(dstate.sble_thread,NULL);
	SBLE_DEBUG("Dispatcher thread has terminated.");


	SBLE_DEBUG("SBLE scheduler has been shut down successfully.");
}

//DEPRECATED
/*
void sble_scheduler_read_event_start()
{
	int thread_arg;
	printf("starting read_event_loop...\n");
	pthread_create(&dstate.tRd, NULL, sble_read_event_loop, &thread_arg);
	//sble_scheduler_wait(SBLE_THREAD_MAIN);
}

void sble_read_event_loop()
{
	printf("read event loop started...\n");;
	int32_t bytes_read = 0;

	dstate.rdBuf_wrPos = 0;
	dstate.rdBuf_rdPos = 0;
	dstate.rdBuf_nBytes = 0;
	uint8_t rdBufTmp[1];
	while(bytes_read >= 0){
		//	printf("raw: mutex locked...\n");
		//	printf("raw: received %d bytes bytes in Queue is %d.\n",bytes_read,dstate.rdBuf_nBytes);
			bytes_read = read(dstate.filedescriptor, rdBufTmp, 1);
			pthread_mutex_lock(&dstate.mutexRD);
 *(dstate.rdBuf + dstate.rdBuf_wrPos) = rdBufTmp[0];
			dstate.rdBuf_nBytes += bytes_read;
			dstate.rdBuf_wrPos += bytes_read;
			dstate.rdBuf_wrPos = dstate.rdBuf_wrPos%SBLE_READ_BUFFER_QUEUE_LENGHT;
			if((dstate.rdBuf_nBytes > SBLE_READ_BUFFER_QUEUE_LENGHT)||(bytes_read <= 0)){
				SBLE_ERROR("Could not read. BufSize = %u, bytes_read = %u. Buffer full or I/O error?",dstate.rdBuf_nBytes, bytes_read);

			}
	//		printf("raw: cond BC...\n");
			//pthread_mutex_unlock(&dstate.mutexRDb);
			pthread_cond_broadcast(&dstate.cwRD);
			pthread_mutex_unlock(&dstate.mutexRD);
		//	usleep(100000);
		//	printf("raw: wait for RDb...\n	");

		//	pthread_cond_wait(&dstate.cwRDb, &dstate.mutexRDb);



	}

}

int32_t sble_scheduler_virtio_rd(uint8_t* buf, uint32_t len){
//		printf("virtRD: reading %d bytes... \n",len);
		if(len == 0){
			return 0;
		}
		pthread_mutex_lock(&dstate.mutexRD);
		while(len > dstate.rdBuf_nBytes){
//		printf("virtRD: waiting for I/O\n",len);
//			pthread_cond_wait(&dstate.cwRD, &dstate.mutexRD);
			pthread_cond_wait(&dstate.cwRD,&dstate.mutexRD);
//			printf("I/O received\n",len);
			if(len <= dstate.rdBuf_nBytes){
				break;
			}
	//		pthread_cond_broadcast(&dstate.mutexRDb);

		}
//		printf("virtRD: I/O received - done\n",len);


		memcpy(buf,(dstate.rdBuf + dstate.rdBuf_rdPos),len);
		dstate.rdBuf_nBytes -= len;
		dstate.rdBuf_rdPos += len;
		dstate.rdBuf_rdPos = dstate.rdBuf_rdPos%SBLE_READ_BUFFER_QUEUE_LENGHT;
//		printf("virtRD: unlocking mutex... \n",len);
		pthread_mutex_unlock(&dstate.mutexRD);

//		printf("rd int done -red %u bytes. Bytes in Queue are: %u\n",len,dstate.rdBuf_nBytes);

		return len;
}

 */


void sble_scheduler_start_extThreadAccess(){
	pthread_mutex_lock(&dstate.mutexMultExtThread);
}

void sble_scheduler_finish_extThreadAccess(){
	pthread_mutex_unlock(&dstate.mutexMultExtThread);
}
#endif
