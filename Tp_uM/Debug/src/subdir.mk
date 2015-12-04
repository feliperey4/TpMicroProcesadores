################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/miadc.c \
../src/mistring.c \
../src/miuart.c 

OBJS += \
./src/main.o \
./src/miadc.o \
./src/mistring.o \
./src/miuart.o 

C_DEPS += \
./src/main.d \
./src/miadc.d \
./src/mistring.d \
./src/miuart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D__CODE_RED -D__LPC43XX__ -D__REDLIB__ -D__USE_LPCOPEN -DCORE_M4 -DDEBUG -I"C:\CIAA\WorkspaceProgMicros\Tp_uM\inc" -I"C:\CIAA\WorkspaceProgMicros\lpcopen_lpc4337\inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


