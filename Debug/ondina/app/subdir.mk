################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ondina/app/odnapplication.c \
../ondina/app/odncache.c 

OBJS += \
./ondina/app/odnapplication.o \
./ondina/app/odncache.o 

C_DEPS += \
./ondina/app/odnapplication.d \
./ondina/app/odncache.d 


# Each subdirectory must supply rules for building sources it contributes
ondina/app/%.o: ../ondina/app/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


