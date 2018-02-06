################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../application/controller/clientcontroller.c \
../application/controller/defaultcontroller.c 

OBJS += \
./application/controller/clientcontroller.o \
./application/controller/defaultcontroller.o 

C_DEPS += \
./application/controller/clientcontroller.d \
./application/controller/defaultcontroller.d 


# Each subdirectory must supply rules for building sources it contributes
application/controller/%.o: ../application/controller/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I"/home/ramiro/github/ondina/ondina" -I"/home/ramiro/github/ondina/application" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


