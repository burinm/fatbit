#!/bin/bash

SIMPLICITY_SDK=/opt/Gecko_SDK-5.0.0

find ./api -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile > cscope.files
find ${SIMPLICITY_SDK}/platform/CMSIS/Include -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/platform/emlib/inc -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/hardware/kit/common/bsp -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/platform/Device/SiliconLabs/EFM32LG/Include -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/platform/emlib/src -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/hardware/kit/common/bsp -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/platform/Device/SiliconLabs/EFM32LG/Source -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${SIMPLICITY_SDK}/platform/Device/SiliconLabs/EFM32LG/Source/GCC -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
cscope -b
ctags -L ./cscope.files
cscope
