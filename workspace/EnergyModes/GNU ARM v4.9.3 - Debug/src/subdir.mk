################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/burin/Dropbox/ecen5023/src/api/letimer.c \
/home/burin/Dropbox/ecen5023/src/api/main.c \
/home/burin/Dropbox/ecen5023/src/api/periph.c \
/home/burin/Dropbox/ecen5023/src/api/sleep.c 

OBJS += \
./src/letimer.o \
./src/main.o \
./src/periph.o \
./src/sleep.o 

C_DEPS += \
./src/letimer.d \
./src/main.d \
./src/periph.d \
./src/sleep.d 


# Each subdirectory must supply rules for building sources it contributes
src/letimer.o: /home/burin/Dropbox/ecen5023/src/api/letimer.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DEFM32LG990F256=1' '-DDEBUG=1' -I"/opt/Gecko_SDK-5.0.0//platform/CMSIS/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/bsp" -I"/opt/Gecko_SDK-5.0.0//platform/emlib/inc" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/drivers" -I"/opt/Gecko_SDK-5.0.0//platform/Device/SiliconLabs/EFM32LG/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/EFM32LG_STK3600/config" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"src/letimer.d" -MT"src/letimer.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/main.o: /home/burin/Dropbox/ecen5023/src/api/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DEFM32LG990F256=1' '-DDEBUG=1' -I"/opt/Gecko_SDK-5.0.0//platform/CMSIS/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/bsp" -I"/opt/Gecko_SDK-5.0.0//platform/emlib/inc" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/drivers" -I"/opt/Gecko_SDK-5.0.0//platform/Device/SiliconLabs/EFM32LG/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/EFM32LG_STK3600/config" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"src/main.d" -MT"src/main.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/periph.o: /home/burin/Dropbox/ecen5023/src/api/periph.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DEFM32LG990F256=1' '-DDEBUG=1' -I"/opt/Gecko_SDK-5.0.0//platform/CMSIS/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/bsp" -I"/opt/Gecko_SDK-5.0.0//platform/emlib/inc" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/drivers" -I"/opt/Gecko_SDK-5.0.0//platform/Device/SiliconLabs/EFM32LG/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/EFM32LG_STK3600/config" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"src/periph.d" -MT"src/periph.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/sleep.o: /home/burin/Dropbox/ecen5023/src/api/sleep.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DEFM32LG990F256=1' '-DDEBUG=1' -I"/opt/Gecko_SDK-5.0.0//platform/CMSIS/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/bsp" -I"/opt/Gecko_SDK-5.0.0//platform/emlib/inc" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/common/drivers" -I"/opt/Gecko_SDK-5.0.0//platform/Device/SiliconLabs/EFM32LG/Include" -I"/opt/Gecko_SDK-5.0.0//hardware/kit/EFM32LG_STK3600/config" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"src/sleep.d" -MT"src/sleep.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


