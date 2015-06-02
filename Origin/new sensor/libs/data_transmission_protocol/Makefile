all: posix stm32f4_freertos
#	echo "Please type either 'make posix' or 'make stm32f4_freeRTOS" to choose a plattform architecture 
	
posix:
	make -f Makefile.posix

stm32f4_freertos:
	make -f Makefile.stm32f4_freertos

clean:
	-rm *.o
	-rm *.d

realclean: clean
	-rm libdtp_*.a
