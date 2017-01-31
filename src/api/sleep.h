#ifndef __SLEEP_H__
#define __SLEEP_H__

#include <stdint.h>

typedef enum { EM0=0,EM1,EM2,EM3,EM4,EM_MAX } e_emode;
extern uint8_t sleep_block_counter[EM_MAX];

/* Energy Modes - Notes

    EM0 (run) - 211uA/MHz
        H/L clocks, all peripherals

    EM1 (sleep) - 63uA/MHz
        No CPU, H/L Clocks, all peripherals

    EM2 (deep sleep) - .95uA (with RTC) 
        No CPU, L Clock only
        Peripherals: LCD, RTC, LETIMER, PCNT, LEUART,
                     I2C, LESENSE, OPAMP, USB, WDOG, ACM

    EM3 (stop) - .65uA
        No CPU, No clock
        Peripherals: ACMP, asynchronous external interrupt, PCNT, and I2C 

    EM4 (shutdown) - 20nA
*/


/* Block the microcontroller from sleeping below a certain mode

 This will block sleep() from entering an energy mode below the one given.
 -- To be called by peripheral HAL's --

 After the peripheral is finished with the operation, it should call unblock with the same state

 Inputs: e_emode
    Minimum sleep mode
 Globals:  sleep_block_counter

*/
void blockSleepMode(e_emode minimumMode);

/* Unblock the microcontroller from sleeping below a certain mode

 This will unblock sleep() from entering an energy mode below the one given.
 -- To be called by peripheral HAL's --

 This should be called after all transactions on a peripheral are done.

 Inputs: e_emode
    Minimum sleep mode
 Globals:  sleep_block_counter
*/
void unblockSleepMode(e_emode minimumMode);

#endif
