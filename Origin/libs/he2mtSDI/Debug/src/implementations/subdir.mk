################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/implementations/he2mtSDIRest.cpp 

OBJS += \
./src/implementations/he2mtSDIRest.o 

CPP_DEPS += \
./src/implementations/he2mtSDIRest.d 


# Each subdirectory must supply rules for building sources it contributes
src/implementations/%.o: ../src/implementations/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


