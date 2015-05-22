#include "libBPMBuffer.hpp"
#include "libBPM.hpp"
libBPMBuffer::libBPMBuffer(){
//	printf("INIT\n");
#if LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_POSIX
	pthread_mutex_init(&(lock),NULL);
	pthread_cond_init(&(sleep_rd),NULL);
	pthread_cond_init(&(sleep_wr),NULL);

#elif LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
	lock = xSemaphoreCreateMutex();
	vSemaphoreCreateBinary(sleep_rd);
	vSemaphoreCreateBinary(sleep_wr);
	xSemaphoreTake(sleep_rd,0);
	xSemaphoreTake(sleep_wr,0);
#else
	printf("LIBBPM_PLATFORM_ARCHITECTURE macro not set at compile-time!\n");
	exit(1);
#endif

	rd_mark = 0;
	wr_mark = 0;
	len = 0;
}

void libBPMBuffer::lockMutex() const{
#if LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_POSIX
	pthread_mutex_lock(&lock);
#elif LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
	xSemaphoreTake(lock, portMAX_DELAY);
#endif

}

void libBPMBuffer::unlockMutex() const{

	#if LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_POSIX
	pthread_mutex_unlock(&lock);
#elif LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
	xSemaphoreGive(lock);
#endif

}

void libBPMBuffer::sleepRd() const{

#if LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_POSIX
	pthread_cond_wait(&sleep_rd, &lock);
#elif LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
	xSemaphoreGive(lock);
	xSemaphoreTake(sleep_rd, portMAX_DELAY);
	xSemaphoreTake(lock, portMAX_DELAY);
#endif

}

void libBPMBuffer::sleepWr() const{

#if LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_POSIX
		pthread_cond_wait(&sleep_wr, &lock);
#elif LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
	xSemaphoreGive(lock);
	xSemaphoreTake(sleep_wr, portMAX_DELAY);
	xSemaphoreTake(lock, portMAX_DELAY);
#endif

}


void libBPMBuffer::wakeupRd() const{

#if LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_POSIX
	pthread_cond_broadcast(&sleep_rd);
#elif LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
	xSemaphoreGive(sleep_rd);
#endif
}

void libBPMBuffer::wakeupWr() const{

#if LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_POSIX
	pthread_cond_broadcast(&sleep_wr);
#elif LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
	xSemaphoreGive(sleep_wr);
#endif
}


int32_t libBPMBuffer::read(uint8_t* buf_dest, uint32_t len, bool block){

	uint32_t bytes_to_read;
	int32_t tmp;
	lockMutex();
	if(len > QUEUE_BUF_LEN){
		len = QUEUE_BUF_LEN;
	}

	if(block){
		//blocking read

		//if the number of bytes requested is not available, block the calling thread
		while(len > this->len){
			sleepRd();
		}


		bytes_to_read = len;
	}else{
		//non-blocking read

		//block the calling thread until at least one byte can be read
		while(this->len == 0){
			sleepRd();
		}
		if(len < this->len){
			bytes_to_read = len;
		}else{
			bytes_to_read = this->len;
		}
	}		
//	printf("before rd: tmp < 0 - tmp = %d, rd-mark = %d, wr_mark = %d, this->len = %d, len = %d\n",tmp,rd_mark,wr_mark,this->len,len);


	//some pointer magic.
	if(rd_mark + bytes_to_read > QUEUE_BUF_LEN - 1){
		//wrap around
		tmp = QUEUE_BUF_LEN - rd_mark;
		if(tmp < 0){
			printf("error: tmp < 0 - tmp = %d, rd-mark = %d, wr_mark = %d, this->len = %d, len = %d\n",tmp,rd_mark,wr_mark,this->len,len);
			exit(1);
			tmp =0;
		}
//		printf("rd: %d - %d / wrap / %d - %d\n", rd_mark, rd_mark + tmp, 0, bytes_to_read - tmp - 2);
		memcpy(buf_dest,buf + rd_mark,tmp);
		if(bytes_to_read > tmp){
			//printf("bytes_to_read: %d - tmp: %d - thsi->len: %d, len: %d\n", bytes_to_read, tmp,this->len,len);
			memcpy(buf_dest + tmp, buf,bytes_to_read - tmp);
		}
/*
		printf("rd: whole buf: \n");
		write(1,buf,len);
		printf("\n");
*/
	 }else{
		//no wrap
		memcpy(buf_dest,buf + rd_mark,len);
	}			
//	printf("!!Bytes To Read (fin) = %d, this->len=%d\n",bytes_to_read,this->len);
	if(bytes_to_read <= this->len){
		this->len -= bytes_to_read;
	}else{
		this->len = 0;
	}
	rd_mark += bytes_to_read;
	rd_mark %= QUEUE_BUF_LEN;
//	printf("--New: RD-Mark: %d, this->len: %d\n",rd_mark,this->len);

	//wake up the write thread as data might have become available in meantime. (if it is not asleep, this has no effect)
	if(len > 0){
		wakeupWr();
	}


	unlockMutex();
	return bytes_to_read;

}

uint32_t libBPMBuffer::getBytesLeft_nts() const{
	return QUEUE_BUF_LEN - len;
}

uint32_t libBPMBuffer::getBytesLeft() const{
	lockMutex();
	return getBytesLeft_nts();
	unlockMutex();
}

uint32_t libBPMBuffer::getLen_nts() const{
	uint32_t rv;
	lockMutex();
	rv = len;
	unlockMutex();
	return rv;
}

uint32_t libBPMBuffer::getLen() const{
	lockMutex();
	return getLen_nts();
	unlockMutex();

}

int32_t libBPMBuffer::write(uint8_t* buf_src, uint32_t len, bool blocking){
	if(blocking){
		return write_blocking(buf_src,len);
	}else{
		return write_nonblocking(buf_src,len);
	}
}

int32_t libBPMBuffer::write_blocking(uint8_t* buf_src, uint32_t len){
	uint32_t bytes_written = 0;
	do{
		bytes_written += libBPMBuffer::write_nonblocking(buf_src + bytes_written,len - bytes_written);

		//if not all bytes could be written, block the calling thread until more space is available (read() will wake us up again...)
		if(bytes_written < len){
			lockMutex();
			sleepWr();
			unlockMutex();
		}

	//not all bytes written after having been woken up? => try to write more...
	}while(bytes_written < len);
}

int32_t libBPMBuffer::write_nonblocking(uint8_t* buf_src, uint32_t len){

	uint32_t bytes_to_write;
	uint32_t tmp;
	lockMutex();
	if(len > getBytesLeft_nts()){
		bytes_to_write = getBytesLeft_nts();
	}else{
		bytes_to_write = len;
	}
//	printf("bytes to write: %d\n",bytes_to_write);
	if(wr_mark + bytes_to_write > QUEUE_BUF_LEN - 1){
		//wrap around
		
		//number of bytes to write this time
		tmp = QUEUE_BUF_LEN - wr_mark;
//		printf("wr: %d - %d / wrap / %d - %d\n", wr_mark, wr_mark + tmp, 0, bytes_to_write - tmp - 2);
		memcpy(buf + wr_mark,buf_src,tmp);
//		printf("left: %d\n",(int32_t) bytes_to_write - (int32_t) tmp);
		memcpy(buf,buf_src + tmp, bytes_to_write - tmp);
/*
		printf("wr: whole buf: \n");
		write(1,buf,len + bytes_to_write);
		printf("\n");
*/
	}else{
//		printf("\n");
		//no wrap
		memcpy(buf + wr_mark,buf_src,bytes_to_write);
	}			
	this->len += bytes_to_write; 
	wr_mark += bytes_to_write;
	wr_mark %= QUEUE_BUF_LEN;
//	printf("WR-Mark: %d\n",wr_mark);
	unlockMutex();
	if(len > 0){
		wakeupRd();
	}
	return bytes_to_write;

}

