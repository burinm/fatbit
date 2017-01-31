################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/opt/Gecko_SDK-5.0.0/platform/Device/SiliconLabs/EFM32LG/Source/system_efm32lg.c 

S_UPPER_SRCS += \
/opt/Gecko_SDK-5.0.0/platform/Device/SiliconLabs/EFM32LG/Source/GCC/startup_efm32lg.S 

OBJS += \
./CMSIS/EFM32LG/startup_efm32lg.o \
./CMSIS/EFM32LG/system_efm32lg.o 

C_DEPS += \
./CMSIS/EFM32LG/system_efm32lg.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/EFM32LG/startup_efm32lg.o: /opt/Gecko_SDK-5.0.0/platform/Device/SiliconLabs/EFM32LG/Source/GCC/startup_efm32lg.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Assembler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -c -x assembler-with-cpp -I"/opt/Gecko_SDK-5.0.0//platform/CMSIS/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/bsp" -I"/opt/Gecko_SDK-5.0.0//platform/emlib/inc" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/drivers" -I"/opt/Gecko_SDK-5.0.0//platform/Device/SiliconLabs/EFM32LG/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/EFM32LG_STK3600/config" '-DEFM32LG990F256=1' -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CMSIS/EFM32LG/system_efm32lg.o: /opt/Gecko_SDK-5.0.0/platform/Device/SiliconLabs/EFM32LG/Source/system_efm32lg.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DEFM32LG990F256=1' '-DDEBUG=1' -I"/opt/Gecko_SDK-5.0.0//platform/CMSIS/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/bsp" -I"/opt/Gecko_SDK-5.0.0//platform/emlib/inc" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/drivers" -I"/opt/Gecko_SDK-5.0.0//platform/Device/SiliconLabs/EFM32LG/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/EFM32LG_STK3600/config" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"CMSIS/EFM32LG/system_efm32lg.d" -MT"CMSIS/EFM32LG/system_efm32lg.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


