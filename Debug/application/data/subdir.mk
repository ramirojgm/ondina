################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../application/data/model.c 

OBJS += \
./application/data/model.o 

C_DEPS += \
./application/data/model.d 


# Each subdirectory must supply rules for building sources it contributes
application/data/%.o: ../application/data/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I"/home/ramiro/github/ondina/ondina" -I"/home/ramiro/github/ondina/application" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


