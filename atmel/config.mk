#
# Copyright (c) 2011 Atmel Corporation. All rights reserved.
# Modified by burin 3/2017
#
# \Thermometer Demo
#
# \page License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# 4. This software may only be redistributed and used in connection with an
#    Atmel microcontroller product.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# \asf_license_stop
#

# Path to top level ASF directory relative to this project directory.
#PRJ_PATH = ../../../../../../..
#PRJ_PATH = ../../xdk-asf-3.32.0
#This is relative to ecen5023 in my Dropbox, not from the symbolic link
ASF_DIR = xdk-asf-3.32.0
PRJ_PATH = ../../..


# Target CPU architecture: cortex-m3, cortex-m4
ARCH = cortex-m0

# Target part: none, sam3n4 or sam4l4aa
PART = samb11g18a

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_NAME = thermometer_flash
TARGET_FLASH = $(TARGET_NAME).elf
TARGET_SRAM = thermometer_sram.elf

# List of C source files.
CSRCS = \
$(ASF_DIR)/common2/components/memory/eeprom/at30tse75x/at30tse75x.c \
$(ASF_DIR)/sam0/drivers/i2c/i2c_master.c \
$(ASF_DIR)/sam0/drivers/i2c/i2c_master_interrupt.c \
$(ASF_DIR)/common/utils/interrupt/interrupt_sam_nvic.c \
$(ASF_DIR)/sam0/boards/samb11_xplained_pro/board_init.c \
$(ASF_DIR)/sam0/drivers/dualtimer/dualtimer.c \
$(ASF_DIR)/sam0/drivers/gpio/gpio.c \
$(ASF_DIR)/sam0/drivers/system/system_sam_b.c \
$(ASF_DIR)/sam0/drivers/timer/timer.c \
$(ASF_DIR)/sam0/drivers/uart/uart.c \
$(ASF_DIR)/sam0/utils/cmsis/samb11/source/gcc/startup_samb11.c \
$(ASF_DIR)/sam0/utils/cmsis/samb11/source/system_samb11.c \
$(ASF_DIR)/sam0/utils/stdio/read.c \
$(ASF_DIR)/sam0/utils/stdio/write.c \
$(ASF_DIR)/sam0/utils/syscalls/gcc/syscalls.c \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/ble_services/ble_mgr/ble_manager.c \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/services/dualtimer/timer_hw.c \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/services/gpio/button.c \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/services/gpio/led.c \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/services/uart/console_serial.c \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/src/event_handler.c \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/src/platform_drv.c \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/src/port_from_sdk.c \
$(ASF_DIR)/sam0/drivers/dma/dma_sam_b.c \
ecen5023/atmel/src/startup_template_app.c \
ecen5023/atmel/src/uart_samb11.c \
ecen5023/atmel/src/s_queue.c \
ecen5023/atmel/src/s_message.c \
ecen5023/atmel/src/circbuf_tiny.c

# List of assembler source files.
ASSRCS = 

# List of include paths.
INC_PATH = \
ecen5023/atmel/src \
ecen5023/atmel/inc \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/ble_services/ble_mgr \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/inc \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/services/dualtimer \
$(ASF_DIR)/sam0/drivers/dualtimer \
$(ASF_DIR)/sam0/utils \
$(ASF_DIR)/sam0/utils/header_files \
$(ASF_DIR)/sam0/utils/preprocessor \
$(ASF_DIR)/thirdparty/CMSIS/Include \
$(ASF_DIR)/thirdparty/CMSIS/Lib/GCC \
$(ASF_DIR)/common/utils \
$(ASF_DIR)/sam0/utils/cmsis/samb11/include \
$(ASF_DIR)/sam0/utils/cmsis/samb11/source \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/services/gpio \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/services/uart \
$(ASF_DIR)/sam0/utils/stdio/stdio_serial \
$(ASF_DIR)/common/services/serial \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/utils \
$(ASF_DIR)/sam0/boards/samb11_xplained_pro \
$(ASF_DIR)/sam0/boards \
$(ASF_DIR)/common/boards \
$(ASF_DIR)/common2/components/memory/eeprom/at30tse75x \
$(ASF_DIR)/sam0/drivers/system \
$(ASF_DIR)/sam0/drivers/timer \
$(ASF_DIR)/sam0/drivers/gpio \
$(ASF_DIR)/sam0/drivers/i2c \
$(ASF_DIR)/sam0/drivers/dma \
$(ASF_DIR)/sam0/drivers/uart

INC_PATH += \
$(ASF_DIR)/common2/components/memory/eeprom/at30tse75x/module_config \
$(ASF_DIR)/sam0/boards/samb11_xplained_pro/board_config \
$(ASF_DIR)/common/components/memory/eeprom/at30tse75x \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/services/dualtimer/module_config \
$(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/services/uart/module_config \

# Additional search paths for libraries.
LIB_PATH =  \
       $(ASF_DIR)/thirdparty/CMSIS/Lib/GCC                           \
       $(ASF_DIR)/thirdparty/wireless/ble_smart_sdk/lib/cm0/gcc     

# List of libraries to use during linking.
LIBS =  \
       blusdk                                            
#       arm_cortexM0l_math                                 \

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = $(ASF_DIR)/sam0/utils/linker_scripts/samb11/gcc/samb11g18a_sram.ld
LINKER_SCRIPT_SRAM  = $(ASF_DIR)/sam0/utils/linker_scripts/samb11/gcc/samb11g18a_sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = 
DEBUG_SCRIPT_SRAM  = $(ASF_DIR)/sam0/boards/samb11_xplained_pro/debug_scripts/gcc/samb11_xplained_pro_sram.gdb

# Project type parameter: all, sram or flash
PROJECT_TYPE        = flash

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -O1

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS = 

# Extra flags to use when compiling.
CFLAGS = 

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
#   To add a definition use the format "-D name[=definition]".
#   To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
#   BOARD      Target board in use, see boards/board.h for a list.
#   EXT_BOARD  Optional extension board in use, see boards/board.h for a list.
CPPFLAGS = \
-D DEBUG \
-D OTAU_FEATURE=false \
-D BLE_DEVICE_ROLE=BLE_ROLE_ALL \
-D BOARD=SAMB11_XPLAINED_PRO \
-D CHIPVERSION_B0 \
-D OTAU_FEATURE=false \
-D I2C_MASTER_CALLBACK_MODE=true \
-D ARM_MATH_CM0PLUS=true \
-D __SAMB11G18A__

# Extra flags to use when linking
# For: uses 2-byte wchar_t yet the output is to use 4-byte wchar_t; use of wchar_t values across objects may fail
LDFLAGS = \
-Wl,--no-wchar-size-warning

# Pre- and post-build commands
PREBUILD_CMD =
POSTBUILD_CMD =
