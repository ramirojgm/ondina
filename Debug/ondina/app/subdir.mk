################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ondina/app/odnapplication.c \
../ondina/app/odncache.c \
../ondina/app/odncontext.c \
../ondina/app/odncontroller.c \
../ondina/app/odnmodel.c \
../ondina/app/odnparser.c \
../ondina/app/odnresult.c \
../ondina/app/odnview.c 

OBJS += \
./ondina/app/odnapplication.o \
./ondina/app/odncache.o \
./ondina/app/odncontext.o \
./ondina/app/odncontroller.o \
./ondina/app/odnmodel.o \
./ondina/app/odnparser.o \
./ondina/app/odnresult.o \
./ondina/app/odnview.o 

C_DEPS += \
./ondina/app/odnapplication.d \
./ondina/app/odncache.d \
./ondina/app/odncontext.d \
./ondina/app/odncontroller.d \
./ondina/app/odnmodel.d \
./ondina/app/odnparser.d \
./ondina/app/odnresult.d \
./ondina/app/odnview.d 


# Each subdirectory must supply rules for building sources it contributes
ondina/app/%.o: ../ondina/app/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I"/home/ramiro/git/ondina/ondina" -I"/home/ramiro/git/ondina/application" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


