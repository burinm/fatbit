#include "letimer.h"
#include "sleep.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_letimer.h"
#include "em_core.h"
#include "em_acmp.h"
#include "main.h"
#include "periph.h"
#include "acmp.h"

#include "debug.h"

void LETIMER0_setup(e_emode e) {

    // Ticks calculations
    double le_period_seconds = LE_PERIOD_SECONDS;
    double le_on_seconds = LE_ON_SECONDS;

    // LXFO Timings
    uint16_t le_lfxo_ticks_second = LETIMER_LFXO_TICK_S / (LE_DIVIDER ? LE_DIVIDER:1); //Divider on/off

    uint16_t le_comp0_em2 = le_period_seconds * le_lfxo_ticks_second; 
    uint16_t le_comp1_em2 = le_comp0_em2 - (le_on_seconds * le_lfxo_ticks_second); 

    // ULFRCO setup, oscillator ticks are milliseconds
    uint16_t le_ulfrco_ticks_second = ulfrco_ticks;
    uint16_t le_comp0_em3 = le_period_seconds * le_ulfrco_ticks_second;
    uint16_t le_comp1_em3 = le_comp0_em3 - (le_on_seconds * le_ulfrco_ticks_second);

    PRINTSWO_UINT(77777);
    PRINTSWO_UINT(le_comp0_em3);
    PRINTSWO_UINT(le_comp1_em3);
    PRINTSWO_UINT(77777);

    //End ticks calc

    // LETIMER jams up after use in ULFRCO and one-shot mode, hard reset
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
        LETIMER_CompareSet(LETIMER0, 0, le_comp0_em2);
        LETIMER_CompareSet(LETIMER0, 1, le_comp1_em2);
        if (LE_DIVIDER == 2) { // Scaling needed for periods 2 seconds or greater
            CMU->LFAPRESC0 |=
          ((_CMU_LFAPRESC0_LETIMER0_DIV2 << _CMU_LFAPRESC0_LETIMER0_SHIFT) & _CMU_LFAPRESC0_MASK);
        }
        if (LE_DIVIDER == 4) { // Scaling needed for periods 4 seconds or greater
            CMU->LFAPRESC0 |=
          ((_CMU_LFAPRESC0_LETIMER0_DIV4 << _CMU_LFAPRESC0_LETIMER0_SHIFT) & _CMU_LFAPRESC0_MASK);
        }
    } else {
        LETIMER_CompareSet(LETIMER0, 0, le_comp0_em3);
        LETIMER_CompareSet(LETIMER0, 1, le_comp1_em3);
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

        /* Temperature section */

        blockSleepMode(EM1);
        ADC0->CMD = ADC_CMD_SINGLESTART;

        /* Light indicator section */
        CMU_ClockEnable(cmuClock_GPIO, true);
        GPIO_PinOutSet(LES_LIGHT_EXCITE_PORT, LES_LIGHT_EXCITE_PORT_NUM);
        if (is_led0_on()) {
           ACMP_fire_up(VDD_LIGHTNESS);
        } else {
            ACMP_fire_up(VDD_DARKNESS);
        }
        while ((ACMP0->STATUS & ACMP_STATUS_ACMPACT) == 0);
    }

    if (intFlags & LETIMER_IFS_COMP1) {
        if (is_led0_on()) {
            if (ACMP0->STATUS & ACMP_STATUS_ACMPOUT) {
                led0_off();
            }
        } else {
            if ((ACMP0->STATUS & ACMP_STATUS_ACMPOUT) == 0) {
                led0_on();
            }
        }
        ACMP_Disable(ACMP0);
        GPIO_PinOutClear(LES_LIGHT_EXCITE_PORT, LES_LIGHT_EXCITE_PORT_NUM);
        CMU_ClockEnable(cmuClock_GPIO, false);
    }

CORE_CriticalEnableIrq();
}
