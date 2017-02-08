#include "letimer.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_letimer.h"
#include "em_core.h"
#include "em_acmp.h"
#include "main.h"
#include "periph.h"
#include "acmp.h"

void LETIMER0_setup(e_emode e) {

//LETIMER_Enable(LETIMER0, false);
LETIMER_Reset(LETIMER0);


blockSleepMode(e);


if (e < EM3) {
    CMU_OscillatorEnable(cmuOsc_LFXO,true,false);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
} else {
    CMU_OscillatorEnable(cmuOsc_ULFRCO,true,false);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);

}
    CMU_ClockEnable(cmuClock_HFLE, true);
    CMU_ClockEnable(cmuClock_LETIMER0, true);

// LETIMER COMP0 will be used for period, COMP1 for duration
if (e < EM3) {
    LETIMER_CompareSet(LETIMER0, 0, LETIMER_TOP_EM2);
    LETIMER_CompareSet(LETIMER0, 1, LETIMER_COMP1_EM2);
    // divide by 2 for LFXO timer
    CMU->LFAPRESC0 |= ((_CMU_LFAPRESC0_LETIMER0_DIV2 << _CMU_LFAPRESC0_LETIMER0_SHIFT) & _CMU_LFAPRESC0_MASK);
} else {
    LETIMER_CompareSet(LETIMER0, 0, 2500);
    LETIMER_CompareSet(LETIMER0, 1, 2497);
}


// Defaults modified from Silicon AN0026SW examples
const LETIMER_Init_TypeDef letimerInit =
{
    .enable         = false,                // Do not start counting when init completed
    .debugRun       = false,                // Do not stop counter during debug halt
    .comp0Top       = true,                 // Load COMP0 into CNT on underflow
    .bufTop         = false,                // Do not load COMP1 into COMP0 when REP0 reaches 0
    .out0Pol        = 0,                    // Idle value 0 for output 0
    .out1Pol        = 0,                    // Idle value 0 for output 1
    .ufoa0          = letimerUFOANone,      // No action on underflow on output 0
    .ufoa1          = letimerUFOANone,      // No action on underflow on output 1
    .repMode        = letimerRepeatFree     // Count until stopped by SW
};

// Initialize LETIMER0
LETIMER_Init(LETIMER0, &letimerInit);
LETIMER0->CNT=0;


// Setup LETIMER interrupts
CORE_CriticalDisableIrq();
    LETIMER0->IFC   = LETIMER_IFC_COMP0;
    LETIMER0->IFC   = LETIMER_IFC_COMP1;
    LETIMER0->IEN   |= LETIMER_IEN_COMP0;
    LETIMER0->IEN   |= LETIMER_IEN_COMP1;
    NVIC_EnableIRQ(LETIMER0_IRQn);
CORE_CriticalEnableIrq();

LETIMER_Enable(LETIMER0, true);

}

void LETIMER0_IRQHandler() {
int intFlags;


CORE_CriticalDisableIrq();
    intFlags = LETIMER_IntGet(LETIMER0);
    LETIMER_IntClear(LETIMER0,LETIMER_IFS_COMP0);
    LETIMER_IntClear(LETIMER0,LETIMER_IFS_COMP1);

    if (intFlags & LETIMER_IFS_COMP0) {
        led0_on();    

        if (is_led1_on()) {
           ACMP_fire_up(VDD_LIGHTNESS);
        } else {
            ACMP_fire_up(VDD_DARKNESS);
        }
        while (ACMP0->STATUS & ACMP_STATUS_ACMPACT == 0);
    }


    if (intFlags & LETIMER_IFS_COMP1) {
        if (is_led1_on()) {
            if (ACMP0->STATUS & ACMP_STATUS_ACMPOUT) {
                led1_off();
            }
        } else {
            if ((ACMP0->STATUS & ACMP_STATUS_ACMPOUT) == 0) {
                led1_on();
            }
        }
        ACMP_Disable(ACMP0);
        led0_off();
    }

CORE_CriticalEnableIrq();
}
