Accsensor Sever
Server for RCS Activity sensors

(C) Lehrstuhl für Realzeit-Computersysteme (RCS)
 Universität München
 Arcisstr. 21
 80333 München

2013, Philipp Kindt / RCS <kindt@rcs.ei.tum.de>

************* Description *************
This programm can
- Receive activity data from the RCS activity sensors, attached to to a the PC by a Bluetooth Low Energy or USB Link
- Simulate activities manually
- Send the detected activity to any XML-RPC server that supports the Med-CPS interface (developed at RCS and DFKI)


Prerequisites
************* Description *************
To run the programm, you will need:
- a Linux PC. This software has been tested with Linux Mint 15 (Olivia) on a Lenovo X201 laptop 
- a Bluegiga BLED112 USB dongle for receiving wireless sensor data
- The QT Library, version 4
For Compiling, you need:
- GCC 4.7 or newer
- QMake


************ Compiling ****************
To compile the programm, run make. If you have added/removed/renamed a file, run qmake before

************ Known Problems ***********
None


