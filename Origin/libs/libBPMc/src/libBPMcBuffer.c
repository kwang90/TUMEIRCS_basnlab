/*
 * libBPMcBuffer.c
 *
 *  Created on: 16.10.2013
 *      Author: kindt
 */


#include "libBPMcBuffer.h"
void libBPMcBuffer_init(libBPMcBuffer* buf){
	libBPMcBuffer_initVarLength(buf,LIBBPMC_BUFFER_QUEUE_BUF_LEN);
}
void libBPMcBuffer_initVarLength(libBPMcBuffer* buf, uint32_t size){
//	printf("INIT\n");
	virThread_mutex_init(&(buf->lock),0);
	virThread_sema_init(&(buf->sleep_rd),0);
	virThread_sema_init(&(buf->sleep_wr),0);
	buf->rd_mark = 0;
	buf->wr_mark = 0;
	buf->len = 0;
	buf->size = size;
	buf->dbuf = malloc(size*sizeof(uint8_t) + LIBBPMC_BUFFER_IO_R_LENGHT);
}

void libBPMcBuffer_lockMutex(libBPMcBuffer* buf) {
	virThread_mutex_lock(&(buf->lock));
}

void libBPMcBuffer_unlockMutex(libBPMcBuffer* buf) {
	virThread_mutex_unlock(&buf->lock);
}

void libBPMcBuffer_sleepRd(libBPMcBuffer* buf) {
	virThread_mutex_unlock(&buf->lock);
	virThread_sema_lock(&buf->sleep_rd);
	virThread_mutex_lock(&buf->lock);

}

void libBPMcBuffer_sleepWr(libBPMcBuffer* buf) {
	virThread_mutex_unlock(&buf->lock);
	virThread_sema_lock(&buf->sleep_wr);
	virThread_mutex_lock(&buf->lock);
}


void libBPMcBuffer_wakeupRd(libBPMcBuffer* buf) {
	virThread_sema_unlock(&buf->sleep_rd);
}

void libBPMcBuffer_wakeupWr(libBPMcBuffer* buf) {
	virThread_sema_unlock(&buf->sleep_wr);
}


int32_t libBPMcBuffer_read(libBPMcBuffer* buf, uint8_t* buf_dest, uint32_t len, uint8_t blockingOrNot){

	uint32_t bytes_to_read;
	int32_t tmp;
	libBPMcBuffer_lockMutex(buf);
	// printf("bytes-to-read: orig: %d ",len);
	if(len > buf->size){
		len = buf->size;
	}
	// printf("final: %d \n",len);

	if(blockingOrNot){
		//blocking read

		//if the number of bytes requested is not available, block the calling thread
		while(len > buf->len){
			libBPMcBuffer_sleepRd(buf);
		}

			bytes_to_read = len;
	}else{
		//non-blocking read

		//block the calling thread until at least one byte can be read
		while(buf->len == 0){
			libBPMcBuffer_sleepRd(buf);
		}

		if(len < buf->len){
			bytes_to_read = len;
		}else{
			bytes_to_read = buf->len;
		}
	}
//	printf("before rd: tmp < 0 - tmp = %d, rd-mark = %d, wr_mark = %d, buf->len = %d, len = %d\n",tmp,rd_mark,wr_mark,buf->len,len);


	//some pointer magic.
	if(buf->rd_mark + bytes_to_read > buf->size - 1){
		//wrap around
		tmp = buf->size - buf->rd_mark;
		if(tmp < 0){
			printf("error: tmp < 0 - tmp = %d, rd-mark = %d, wr_mark = %d, buf->len = %d, len = %d\n",tmp,buf->rd_mark,buf->wr_mark,buf->len,len);
			exit(1);
			tmp =0;
		}
//		printf("rd: %d - %d / wrap / %d - %d\n", rd_mark, rd_mark + tmp, 0, bytes_to_read - tmp - 2);
		memcpy(buf_dest,buf->dbuf + buf->rd_mark,tmp);
		if(bytes_to_read > tmp){
			//printf("bytes_to_read: %d - tmp: %d - thsi->len: %d, len: %d\n", bytes_to_read, tmp,buf->len,len);
			memcpy(buf_dest + tmp, buf->dbuf,bytes_to_read - tmp);
		}
/*
		printf("rd: whole buf: \n");
		write(1,buf_dest,len);
		printf("\n");
*/
	 }else{
		//no wrap
		memcpy(buf_dest,buf->dbuf + buf->rd_mark,len);
	}
//	printf("!!Bytes To Read (fin) = %d, buf->len=%d\n",bytes_to_read,buf->len);
	if(bytes_to_read <= buf->len){
		buf->len -= bytes_to_read;
	}else{
		buf->len = 0;
	}
	buf->rd_mark += bytes_to_read;
	buf->rd_mark %= buf->size;
//	printf("--New: RD-Mark: %d, buf->len: %d\n",rd_mark,buf->len);

	//wake up the write thread as data might have become available in meantime. (if it is not asleep, this has no effect)
	if(buf->len > 0){
		libBPMcBuffer_unlockMutex(buf);
		libBPMcBuffer_wakeupWr(buf);
	}else{
		libBPMcBuffer_unlockMutex(buf);

	}


	return bytes_to_read;

}

uint32_t libBPMcBuffer_getBytesLeft_nts(libBPMcBuffer* buf) {
	return (buf->size - buf->len);
}

uint32_t libBPMcBuffer_getBytesLeft(libBPMcBuffer* buf) {
	uint32_t bytesLeft;
	libBPMcBuffer_lockMutex(buf);
	bytesLeft = libBPMcBuffer_getBytesLeft_nts(buf);
	libBPMcBuffer_unlockMutex(buf);
	return bytesLeft;
}

uint32_t libBPMcBuffer_getBytesUsed_nts(libBPMcBuffer* buf) {
	return (buf->len);
}

uint32_t libBPMcBuffer_getBytesUsed(libBPMcBuffer* buf) {
	uint32_t bytesLeft;
	libBPMcBuffer_lockMutex(buf);
	bytesLeft = libBPMcBuffer_getBytesUsed_nts(buf);
	libBPMcBuffer_unlockMutex(buf);
	return bytesLeft;
}



uint32_t libBPMcBuffer_getSize_nts(libBPMcBuffer* buf) {
	return buf->size;
}

uint32_t libBPMcBuffer_getSize(libBPMcBuffer* buf) {
	uint32_t rv;
	libBPMcBuffer_lockMutex(buf);
	rv = libBPMcBuffer_getSize_nts(buf);
	libBPMcBuffer_unlockMutex(buf);
	return rv;
}

int32_t libBPMcBuffer_write(libBPMcBuffer* buf, uint8_t* buf_src, uint32_t len, uint8_t blockingOrNot)
{
	if(blockingOrNot)
	{
		return libBPMcBuffer_write_blocking(buf,buf_src,len);
	}
	else
	{
		return libBPMcBuffer_write_nonblocking(buf,buf_src,len);
	}
}

int32_t libBPMcBuffer_write_blocking(libBPMcBuffer* buf, uint8_t* buf_src, uint32_t len){
	uint32_t bytes_written = 0;

	do{
		bytes_written += libBPMcBuffer_write_nonblocking(buf, buf_src + bytes_written,len - bytes_written);

		//if not all bytes could be written, block the calling thread until more space is available (read() will wake us up again...)
		if(bytes_written < len){
			//2do: removeme
			printf("Buffer is full.\n");
		//	exit(1);

			libBPMcBuffer_lockMutex(buf);
			libBPMcBuffer_sleepWr(buf);
			libBPMcBuffer_unlockMutex(buf);
		}

	//not all bytes written after having been woken up? => try to write more...
	}while(bytes_written < len);

	//buf->len += bytes_written; //@gopp
	//printf("Bytes written: %u",buf->len);

	return bytes_written;
}

int32_t libBPMcBuffer_write_nonblocking(libBPMcBuffer* buf, uint8_t* buf_src, uint32_t len){

	uint32_t bytes_to_write;
	uint32_t tmp;
	libBPMcBuffer_lockMutex(buf);

	if(len > libBPMcBuffer_getBytesLeft_nts(buf))
	{

		//2do: removeme
		printf("BULLSHIT\n");
		exit(1);

		bytes_to_write = libBPMcBuffer_getBytesLeft_nts(buf);
	}
	else
	{
		bytes_to_write = len;
	}
	//printf("bytes to write: orig: %d - actual: %d\n",len,bytes_to_write);
	if(buf->wr_mark + bytes_to_write > buf->size - 1)
	{
		//wrap around

		//number of bytes to write this time
		tmp = buf->size - buf->wr_mark;
	//	printf("wr: %d - %d / wrap / %d - %d\n", buf->wr_mark, buf->wr_mark + tmp, 0, bytes_to_write - tmp - 2);
		memcpy(buf->dbuf + buf->wr_mark,buf_src,tmp);
	//	printf("left: %d\n",(int32_t) bytes_to_write - (int32_t) tmp);
		memcpy(buf->dbuf,buf_src + tmp, bytes_to_write - tmp);


	}
	else
	{
		//no wrap
		memcpy(buf->dbuf + buf->wr_mark,buf_src,bytes_to_write);
	}

	buf->len += bytes_to_write;
	buf->wr_mark += bytes_to_write;
	buf->wr_mark %= buf->size;
//	printf("WR-Mark: %d\n",wr_mark);
	libBPMcBuffer_unlockMutex(buf);
	if(len > 0)
	{
		libBPMcBuffer_wakeupRd(buf);
	}
	return bytes_to_write;

}

