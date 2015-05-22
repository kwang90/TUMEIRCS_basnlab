#!/bin/bash
if [ -n "$1" ]
then
OFILE=`echo $1 | sed 's/\.c//' `

gcc $1 -DSBLE_PLATTFORM_ARCHITECTURE=SBLE_PLATTFORM_ARCHITECTURE_POSIX -g -I ../include/ -I ../include/ports/POSIX -I ../bglib ../../libs/data_transmission_protocol/libdtp_posix.a ../sble_posix.a -I ../../libs -rdynamic -lpthread -lrt -o $OFILE;
echo "Built example: " $OFILE;
else
echo "Usage: compile.sh [source-file]";
fi
