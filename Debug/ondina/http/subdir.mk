################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ondina/http/httppackage.c \
../ondina/http/httprequest.c \
../ondina/http/httpresponse.c 

OBJS += \
./ondina/http/httppackage.o \
./ondina/http/httprequest.o \
./ondina/http/httpresponse.o 

C_DEPS += \
./ondina/http/httppackage.d \
./ondina/http/httprequest.d \
./ondina/http/httpresponse.d 


# Each subdirectory must supply rules for building sources it contributes
ondina/http/%.o: ../ondina/http/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I"/home/ramiro/git/ondina/ondina" -I"/home/ramiro/git/ondina/application" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


