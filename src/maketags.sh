#!/bin/bash

SIMPLICITY_SDK=/opt/Gecko_SDK-5.0.0

find ./api -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile > cscope.files
find ${SIMPLICITY_SDK}/platform/CMSIS/Include -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/platform/emlib/inc -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/platform/emlib/src -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/hardware/kit/common/bsp -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/hardware/kit/common/bsp -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/hardware/kit/common/drivers -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/platform/Device/SiliconLabs/EFM32LG/Source -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/platform/Device/SiliconLabs/EFM32LG/Source/GCC -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/hardware/kit/EFM32ZG_STK3200/config -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files

find ${SIMPLICITY_SDK}/platform/Device/SiliconLabs/EFM32LG/Include -iname "efm32lg_*.h" -or -iname "efm32lg_*.c" -or -iname "efm32lg_*.s" -or -name Makfile >> cscope.files
echo ${SIMPLICITY_SDK}/platform/Device/SiliconLabs/EFM32LG/Include/efm32lg990f256.h >> cscope.files
echo ${SIMPLICITY_SDK}/platform/Device/SiliconLabs/EFM32LG/Include/em_device.h >> cscope.files
echo ${SIMPLICITY_SDK}/platform/Device/SiliconLabs/EFM32LG/Include/system_efm32lg.h >> cscope.files
cscope -b
ctags -L ./cscope.files
cscope
