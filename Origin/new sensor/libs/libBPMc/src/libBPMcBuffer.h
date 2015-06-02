/**
 * \file libBPMcBuffer.h
 * \brief General purpose buffering FIFO that allows blocking read/write
 * libBPMTrafficMeasurement.h
 */
#ifndef LIBBPMCBUFFER_H_
#define LIBBPMCBUFFER_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <string.h>
#include "libBPMcPlatform.h"
#include "../../virThread/virThread.h"
#include "libBPMcParams.h"

//Chose the right includes for POSIX and freeRTOS


#define LIBBPMC_BUFFER_IO_R_LENGHT LIBBPMC_PARAMS_IO_R_LENGTH		///<The maximum number of bytes that can be written or read with one read() or write() - call

#define LIBBPMC_BUFFER_QUEUE_BUF_LEN LIBBPMC_PARAM_DEFAULT_BUFFERLEVEL	///< The maximum number of bytes that can stored within the buffer


typedef struct _libBPMcBuffer{
	virThread_mutex_t lock;
	virThread_sema_t sleep_rd;
	virThread_sema_t sleep_wr;

	uint8_t* dbuf;
	uint32_t size;
    uint32_t rd_mark;					//pointer offset to the position the next read() will start. The address is  *(buf + rd_mark)
	uint32_t wr_mark;					//pointer offset to the postiion the next byte will written to when calling write()
	uint32_t len;						//number of bytes currently stored in buf
} libBPMcBuffer;


void libBPMc_Buffer_lockMutex(libBPMcBuffer* buf);
void libBPMc_Buffer_unlockMutex(libBPMcBuffer* buf);
void libBPMc_Buffer_sleepRd(libBPMcBuffer* buf) ;
void libBPMc_Buffer_wakeupRd(libBPMcBuffer* buf) ;
void libBPMc_Buffer_sleepWr(libBPMcBuffer* buf) ;
void libBPMc_Buffer_wakeupWr(libBPMcBuffer* buf) ;



/**
 * Constructor - starts all necessary threads and semaphores and that will initialize the class
 */
    void libBPMcBuffer_init(libBPMcBuffer* buf);
    void libBPMcBuffer_initVarLength(libBPMcBuffer* buf, uint32_t size);

    /**
     * \brief Read from buffer
     * Reading might be both blocking or non blocking.
     * Blocking policy: The function will only return after the requested number of bytes or \ref LIBBPMC_BUFFER_IO_R_LENGHT bytes have been read, respectively.
     * Non-Blocking policy: The function will only block the calling thread if there is zero data in the buffer.
     * As soon as the buffer is non-empty, the calling thread will continue with any number of bytes > zero read.
     * @param buf_dest	Pre-malloced memory area to write the data into, that provides sufficient space
     * @param len	Number of bytes to read
     * @param block	true => Blocking read policy; false: non-blocking policy
     * @return Number of bytes read. Negative number means error.
     */
 	int32_t libBPMcBuffer_read(libBPMcBuffer* buf, uint8_t* buf_dest, uint32_t len, uint8_t blockingOrNot);

 	/**
 	 * \brief Get number of free bytes in buffer (non-threadsave version)
	 * This function returns the number of bytes the buffer is capable to host.
	 * It does not acquire the data lock semaphore and is therefore non-threadsafe.
 	 * @return Number of free bytes in Buf
 	 */
	uint32_t libBPMcBuffer_getBytesLeft_nts(libBPMcBuffer* buf) ;

 	/**
 	 * \brief Get number of free bytes in buffer (threadsave version)
	 * This function returns the number of bytes the buffer is capable to host.
	 * It acquires the data lock semaphore before reading the number of bytes and frees it after.
	 * Therefore, it is threadsafe.
 	 * @return Number of free bytes in buffer.
 	 */
	uint32_t libBPMcBuffer_getBytesLeft(libBPMcBuffer* buf) ;
	/**
	 * \brief total number of bytes stored (used) in buffer
	 * 	It does not acquire the data lock semaphore and is therefore non-threadsafe.
	 *
	 * @return Number of bytes stored in buffer.
	 */
	uint32_t libBPMcBuffer_getBytesUsed_nts(libBPMcBuffer* buf) ;

	/**
	 * \brief total number of bytes stored (used) in buffer
	 * It acquires the data lock semaphore before reading the number of bytes and frees it after.
	 * Therefore, it is threadsafe.
	 * @return Number of bytes stored in buffer.
	 */
	uint32_t libBPMcBuffer_getBytesUsed(libBPMcBuffer* buf) ;
	/**
	 * \brief Get total number of bytes in buffer (free + used).
	 * 	It does not acquire the data lock semaphore and is therefore non-threadsafe.
	 *
	 * @return Number of bytes in buffer.
	 */
	uint32_t libBPMcBuffer_getSize_nts(libBPMcBuffer* buf) ;

	/**
	 * \brief Get total number of bytes in buffer.
	 * It acquires the data lock semaphore before reading the number of bytes and frees it after.
	 * Therefore, it is threadsafe.
	 * @return Number of bytes in buffer.
	 */
	uint32_t libBPMcBuffer_getSize(libBPMcBuffer* buf) ;

	/**
	 * \brief Write data to buffer.
	 * Both blocking and non-blocking writes are available.
	 * Blocking policy: The calling thread is blocked until the requested number of bytes could be written successfully.
	 * Non-Blocking policy: The calling thread is never blocked
	 * @param buf_src Memory-area containing the data to write to the buf
	 * @param len Number of bytes to write.
	 * @param blocking Determines the blocking policy. true => blocking write, false: non-blocking write
	 * @return Number of bytes written or < 0 in case of an error
	 */
	int32_t libBPMcBuffer_write(libBPMcBuffer* buf, uint8_t* buf_src, uint32_t len, uint8_t blockingOrNot);

	/**
	 * \brief Blocking write data to buffer.
	 * Blocking policy: The calling thread is blocked until the requested number of bytes could be written successfully.
	 * @param buf_src Memory-area containing the data to write to the buf
	 * @param len Number of bytes to write.
	 * @return Number of bytes written or < 0 in case of an error
	 */
	int32_t libBPMcBuffer_write_blocking(libBPMcBuffer* buf, uint8_t* buf_src, uint32_t len);

	/**
	 * \brief Non-Blocking write data to buffer.
	 * Non-Blocking policy: The calling thread is never blocked
	 * @param buf_src Memory-area containing the data to write to the buf
	 * @param len Number of bytes to write.
	 * @return Number of bytes written or < 0 in case of an error
	 */
	int32_t libBPMcBuffer_write_nonblocking(libBPMcBuffer* buf, uint8_t* buf_src, uint32_t len);





#endif /* LIBBPMBUFFER_H_ */
