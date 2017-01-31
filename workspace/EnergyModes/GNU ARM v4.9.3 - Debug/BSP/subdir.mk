################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/opt/Gecko_SDK-5.0.0/hardware/kit/common/bsp/bsp_trace.c 

OBJS += \
./BSP/bsp_trace.o 

C_DEPS += \
./BSP/bsp_trace.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/bsp_trace.o: /opt/Gecko_SDK-5.0.0/hardware/kit/common/bsp/bsp_trace.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DEFM32LG990F256=1' '-DDEBUG=1' -I"/opt/Gecko_SDK-5.0.0//platform/CMSIS/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/bsp" -I"/opt/Gecko_SDK-5.0.0//platform/emlib/inc" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/drivers" -I"/opt/Gecko_SDK-5.0.0//platform/Device/SiliconLabs/EFM32LG/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/EFM32LG_STK3600/config" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"BSP/bsp_trace.d" -MT"BSP/bsp_trace.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


