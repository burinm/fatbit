/* sleep.c
    burin - Jan 29, 2017
    Extended from class notes ecen5023 - IOT
*/

#include "sleep.h"
#include "em_core.h"
#include "em_emu.h"
#include <stdbool.h>

uint8_t sleep_block_counter[EM_MAX];

void sleep(void) {
    if (sleep_block_counter[EM0] > 0) {
        return;
    } else if (sleep_block_counter[EM1] > 0) {
        EMU_EnterEM1();
    } else if (sleep_block_counter[EM2] > 0) {
        EMU_EnterEM2(true);
    } else if (sleep_block_counter[EM3] > 0) {
        EMU_EnterEM3(true);
    } else{
        EMU_EnterEM3(true);
        //EMU_EnterEM4();
    }
    return;
}

void blockSleepMode(e_emode minimumMode)
{
CORE_CriticalDisableIrq();
    sleep_block_counter[minimumMode]++;
CORE_CriticalEnableIrq();
}

void unblockSleepMode(e_emode minimumMode)
{
CORE_CriticalDisableIrq();
    if(sleep_block_counter[minimumMode] > 0) {
        sleep_block_counter[minimumMode]--;
    }
CORE_CriticalEnableIrq();
}
