#include "rtc.h"
#include "em_rtc.h"
#include "em_cmu.h"

uint16_t pulse_measure=0;

void RTC_Setup() {
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_RTC, true);
    CMU_ClockDivSet(cmuClock_RTC, RTC_PRESCALE);

    static const RTC_Init_TypeDef initRTC = 
    {
        .enable = false,
        .debugRun = false,
        .comp0Top = false
    };

    RTC_Init(&initRTC);
CORE_CriticalDisableIrq();
    RTC_IntClear(RTC_IFC_OF);
    RTC_IntEnable(RTC_IEN_OF);
CORE_CriticalEnableIrq();
}

void RTC_IRQHandler(void) {
uint32_t flags;
CORE_CriticalDisableIrq();
    flags = RTC_IntGet();

    if (flags & RTC_IFS_OF) {
        //Timer overflowed. Currently set at 2 seconds.
        // This would be a pulse 30 BPM - so stop timer
        pulse_measure=0;
        RTC_Enable(false);
    }
  
CORE_CriticalEnableIrq();
}


