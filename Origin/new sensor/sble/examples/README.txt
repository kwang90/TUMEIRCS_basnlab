Howto compile and run the examples?
1) Compile sble and bglib to an archive (sble.a)
  1.1) cwd to sble
  1.2) Type 'make realclean && make posix' (or 'make realclean && make stm32f4_freeRTOS' for compuling the lib for the stm32f4 - mpu)
	=> an archive sble_posix.a (or sble_stm32f4_freeRTOS.a) should be created.
2) pwd to sble/examples
3) type ./compile.sh [filename]			, where filename is the c-file of the example. The examples are built for the linux/posix architecture with BLED112 attached to an USB-port. Run as root!

example:
./compile.sh sble_example_client.c
Some examples assume a certain firmware on the BLE112-Device. Please consult the example's source code!
 
