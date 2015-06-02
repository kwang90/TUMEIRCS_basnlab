/**
 * \file libBPMBuffer.hpp
 * \brief General purpose buffering FIFO that allows blocking read/write
 * libBPMTrafficMeasurement.h
 */
#ifndef LIBBPMBuffer_H_
#define LIBBPMBuffer_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <string.h>
#include "libBPMPlatform.hpp"

//Chose the right includes for POSIX and freeRTOS
#if LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_ARCHITECTURE_POSIX
#include <pthread.h>
#elif LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <FreeRTOS.h>
#include <task.h>
#include <portmacro.h>
#include <semphr.h>
#endif


#define IO_R_LENGHT 20			///<The maximum number of bytes that can be written or read with one read() or write() - call

#define QUEUE_BUF_LEN 1000		///< The maximum number of bytes that can stored within the buffer


class libBPMBuffer{
	private:

	void lockMutex() const;
	void unlockMutex() const;
	void sleepRd() const;
	void wakeupRd() const;
	void sleepWr() const;
	void wakeupWr() const;

#if LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_POSIX
	mutable pthread_mutex_t lock;
	mutable pthread_cond_t sleep_rd;
	mutable pthread_cond_t sleep_wr;
#elif LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
    mutable xSemaphoreHandle lock;
    mutable xSemaphoreHandle sleep_rd;
    mutable xSemaphoreHandle sleep_wr;
#endif


    uint8_t buf[QUEUE_BUF_LEN];			//the storage for the buffer

    uint32_t rd_mark;					//pointer offset to the position the next read() will start. The address is  *(buf + rd_mark)
	uint32_t wr_mark;					//pointer offset to the postiion the next byte will written to when calling write()
	uint32_t len;						//number of bytes currently stored in buf

	public:

/**
 	 * Constructor starts all necessary threads and semaphores and that will initialize the class
 */
    libBPMBuffer();

    /**
     * \brief Read from buffer
     * Reading might be both blocking or non blocking.
     * Blocking policy: The function will only return after the requested number of bytes or \ref IO_R_LENGHT bytes have been read, respectively.
     * Non-Blocking policy: The function will only block the calling thread if there is zero data in the buffer.
     * As soon as the buffer is non-empty, the calling thread will continue with any number of bytes > zero read.
     * @param buf_dest	Pre-malloced memory area to write the data into, that provides sufficient space
     * @param len	Number of bytes to read
     * @param block	true => Blocking read policy; false: non-blocking policy
     * @return Number of bytes read. Negative number means error.
     */
 	int32_t read(uint8_t* buf_dest, uint32_t len, bool block = false);

 	/**
 	 * \brief Get number of free bytes in buffer (non-threadsave version)
	 * This function returns the number of bytes the buffer is capable to host.
	 * It does not acquire the data lock semaphore and is therefore non-threadsafe.
 	 * @return Number of free bytes in Buf
 	 */
	uint32_t getBytesLeft_nts() const;

 	/**
 	 * \brief Get number of free bytes in buffer (threadsave version)
	 * This function returns the number of bytes the buffer is capable to host.
	 * It acquires the data lock semaphore before reading the number of bytes and frees it after.
	 * Therefore, it is threadsafe.
 	 * @return Number of free bytes in buffer.
 	 */
	uint32_t getBytesLeft() const;

	/**
	 * \brief Get number of bytes in buffer.
	 * 	It does not acquire the data lock semaphore and is therefore non-threadsafe.
	 *
	 * @return Number of bytes in buffer.
	 */
	uint32_t getLen_nts() const;

	/**
	 * \brief Get number of bytes in buffer.
	 * It acquires the data lock semaphore before reading the number of bytes and frees it after.
	 * Therefore, it is threadsafe.
	 * @return Number of bytes in buffer.
	 */
	uint32_t getLen() const;

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
	int32_t write(uint8_t* buf_src, uint32_t len, bool blocking = true);

	/**
	 * \brief Blocking write data to buffer.
	 * Blocking policy: The calling thread is blocked until the requested number of bytes could be written successfully.
	 * @param buf_src Memory-area containing the data to write to the buf
	 * @param len Number of bytes to write.
	 * @return Number of bytes written or < 0 in case of an error
	 */
	int32_t write_blocking(uint8_t* buf_src, uint32_t len);

	/**
	 * \brief Non-Blocking write data to buffer.
	 * Non-Blocking policy: The calling thread is never blocked
	 * @param buf_src Memory-area containing the data to write to the buf
	 * @param len Number of bytes to write.
	 * @return Number of bytes written or < 0 in case of an error
	 */
	int32_t write_nonblocking(uint8_t* buf_src, uint32_t len);




};
#endif
