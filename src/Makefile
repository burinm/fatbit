
SIMPLICITY_ROOT=/home/burin/SimplicityStudio_v4
#SIMPLICITY_SDK=$(SIMPLICITY_ROOT)/developer/sdks/exx32/v5.0.0.0
SIMPLICITY_SDK=/opt/Gecko_SDK-5.0.0

TOOLPATH=$(SIMPLICITY_ROOT)/developer/toolchains/gnu_arm/4.9_2015q3/bin

CC=$(TOOLPATH)/arm-none-eabi-gcc
OBJCOPY=$(TOOLPATH)/arm-none-eabi-objcopy
SIZE=$(TOOLPATH)/arm-none-eabi-size
GDB=$(TOOLPATH)/arm-none-eabi-gdb

COMMANDER=$(SIMPLICITY_ROOT)/developer/adapter_packs/commander

INCLUDES += -I$(SIMPLICITY_SDK)/hardware/kit/EFM32LG_STK3600/examples/blink \
-I$(SIMPLICITY_SDK)/hardware/kit/EFM32LG_STK3600/config \
-I$(SIMPLICITY_SDK)/platform/CMSIS/Include \
-I$(SIMPLICITY_SDK)/platform/emlib/inc \
-I$(SIMPLICITY_SDK)/hardware/kit/common/bsp \
-I$(SIMPLICITY_SDK)/platform/Device/SiliconLabs/EFM32LG/Include \
-I$(SIMPLICITY_SDK)/hardware/kit/common/drivers \
-I./api

CFLAGS = -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 -O0 -Wall -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections
CFLAGS += '-DDEBUG_EFM=1' '-DEFM32LG990F256=1' '-DDEBUG=1'

SFLAGS = -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -x assembler-with-cpp '-DEFM32LG990F256=1'

LDFLAGS = -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "$(LINKER_FILE)" -Xlinker --gc-sections -Xlinker -Map="STK3600_blink.map" --specs=nano.specs -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group

vpath %.c $(SIMPLICITY_SDK)/platform/emlib/src 
vpath %.c $(SIMPLICITY_SDK)/hardware/kit/common/bsp
vpath %.c $(SIMPLICITY_SDK)/platform/Device/SiliconLabs/EFM32LG/Source 
vpath %.S $(SIMPLICITY_SDK)/platform/Device/SiliconLabs/EFM32LG/Source/GCC
vpath %.c $(SIMPLICITY_SDK)/hardware/kit/common/drivers
vpath %.c ./api

LINKER_FILE=$(SIMPLICITY_SDK)/platform/Device/SiliconLabs/EFM32LG/Source/GCC/efm32lg.ld

SOURCES = bsp_bcc.c bsp_stk_leds.c bsp_stk.c bsp_trace.c
SOURCES += segmentlcd.c em_lcd.c
SOURCES += em_letimer.c em_core.c em_assert.c em_emu.c em_cmu.c em_ebi.c em_gpio.c em_system.c em_usart.c
#SOURCES += em_assert.c  em_ebi.c em_gpio.c em_system.c
SOURCES += em_timer.c
SOURCES += system_efm32lg.c
SOURCES += main.c sleep.c periph.c timers.c debug.c
#SOURCES += retargetio.c
OBJS :=$(SOURCES:%.c=%.o)
OBJS += startup_efm32lg.o

TARGET=out

$(TARGET).hex: $(TARGET).axf
	$(OBJCOPY)  -O ihex $^ $@

$(TARGET).axf: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
	$(SIZE) $@

%.o:%.c
	$(CC) $(INCLUDES) $(CFLAGS) $< -c -o $@

%.o:%.S
	$(CC) $(SFLAGS) $< -c -o $@

clean:
	rm -rf $(OBJS) $(TARGET).axf $(TARGET).hex *.map

debug:
	$(GDB) $(TARGET).axf
