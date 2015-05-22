all: posix stm32f4_freeRTOS
#	echo "Please type either 'make posix' or 'make stm32f4_freeRTOS" to choose a plattform architecture 
	
posix:
	make -f Makefile.posix

stm32f4_freeRTOS:
	make -f Makefile.stm32f4_freeRTOS

clean:
	-rm *.o
	-rm *.d
	-rm src/*.o
	-rm src/*.d
	-rm src/ports/POSIX/*.o
	-rm src/ports/FreeRTOS/*.o
	-rm src/ports/POSIX/*.d
	-rm src/ports/FreeRTOS/*.d
	-rm bglib/*.o
	-rm bglib/*.d

realclean: clean
	-rm sble_*.a
