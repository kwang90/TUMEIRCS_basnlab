################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rcs_dsp_iir.c \
../rcs_dsp_iir_pipeline.c \
../rcs_dsp_ll.c 

OBJS += \
./rcs_dsp_iir.o \
./rcs_dsp_iir_pipeline.o \
./rcs_dsp_ll.o 

C_DEPS += \
./rcs_dsp_iir.d \
./rcs_dsp_iir_pipeline.d \
./rcs_dsp_ll.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


