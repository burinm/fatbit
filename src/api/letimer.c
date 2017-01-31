/***************************************************************************//**
 * Modified by  - burin 1/29/2017
 *
 * Parts taken from the ANN0026SW demo code
 * em_letimer.h
 * Low Energy Timer (LETIMER) peripheral API
 * version 5.0.0
 *******************************************************************************
 * section License
 * Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include "letimer.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_letimer.h"
#include "em_core.h"

#include "sleep.h"

#define LETIMER_TICK_MS   33UL

#define LETIMER_PERIOD_MS 1750UL
#define LETIMER_BLINK_MS  30UL
#define LETIMER_TOP (LETIMER_PERIOD_MS * LETIMER_TICK_MS)
#define LETIMER_COMP1 (LETIMER_TOP - (LETIMER_BLINK_MS * LETIMER_TICK_MS))

void LETIMER0_setup() {

blockSleepMode(EM2);

CMU_OscillatorEnable(cmuOsc_LFXO,true,false);
    /* Enable necessary clocks */
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    //CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_HFCLKLE);
    CMU_ClockEnable(cmuClock_HFLE, true);
    CMU_ClockEnable(cmuClock_LETIMER0, true);
//    CMU_ClockDivSet(cmuClock_LETIMER0,cmuClkDiv_2); 

    // Set initial compare values for COMP0, COMP1
    LETIMER_CompareSet(LETIMER0, 0, LETIMER_TOP);
    LETIMER_CompareSet(LETIMER0, 1, LETIMER_COMP1);


    /* Set configurations for LETIMER 0 */
    const LETIMER_Init_TypeDef letimerInit =
    {
        .enable         = true,                 // Start counting when init completed
        .debugRun       = false,                // Do not stop counter during debug halt
        .comp0Top       = true,                // Load COMP0 into CNT on underflow
        .bufTop         = false,                // Do not load COMP1 into COMP0 when REP0 reaches 0
        .out0Pol        = 0,                    // Idle value 0 for output 0
        .out1Pol        = 0,                    // Idle value 0 for output 1
        .ufoa0          = letimerUFOANone,      // No action on underflow on output 0
        .ufoa1          = letimerUFOANone,      // No action on underflow on output 1
        .repMode        = letimerRepeatFree     // Count until stopped by SW
    };

    /* Initialize LETIMER */
    LETIMER_Init(LETIMER0, &letimerInit);

CORE_CriticalDisableIrq();
    // Clear all interrupts for LETIMER0
    LETIMER0->IFC   = LETIMER_IFC_COMP0;
    LETIMER0->IFC   = LETIMER_IFC_COMP1;
    //Set COMP0 interrupt
    LETIMER0->IEN   |= LETIMER_IEN_COMP0;
    LETIMER0->IEN   |= LETIMER_IEN_COMP1;
    NVIC_EnableIRQ(LETIMER0_IRQn);
CORE_CriticalEnableIrq();

//BlockSleep.....


}

void LETIMER0_IRQHandler() {
int intFlags;

CORE_CriticalDisableIrq();
    intFlags = LETIMER_IntGet(LETIMER0);
    LETIMER_IntClear(LETIMER0,LETIMER_IFS_COMP0);
    LETIMER_IntClear(LETIMER0,LETIMER_IFS_COMP1);

    if (intFlags & LETIMER_IFS_COMP0) {
        GPIO_PinOutSet(gpioPortE, 2);
        //GPIO_PinOutToggle(gpioPortE, 2);
    }
    if (intFlags & LETIMER_IFS_COMP1) {
        GPIO_PinOutClear(gpioPortE, 2);
        //GPIO_PinOutToggle(gpioPortE, 2);
    }


CORE_CriticalEnableIrq();
}
