#!/bin/bash

ATMEL_ASF_SDK=/home/burin/xdk-asf-3.32.0

find . -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile  | grep -v "BLEDEBUG" > cscope.files
find ${ATMEL_ASF_SDK}/common2/components/memory/eeprom/at30tse75x -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/drivers/i2c -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/common/utils/interrupt -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/boards/samb11_xplained_pro -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/drivers/dualtimer -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/drivers/gpio -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/drivers/system -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/drivers/timer -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/drivers/uart -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/utils/cmsis/samb11/source/gcc -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/utils/cmsis/samb11/source -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/utils/stdio -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/utils/syscalls/gcc -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/thirdparty/wireless/ble_smart_sdk -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ATMEL_ASF_SDK}/sam0/drivers/dma/ -iname "*.h" -or -iname "*.c" -or -iname "*.s" -or -name Makfile >> cscope.files

cscope -b
ctags -L ./cscope.files
cscope
