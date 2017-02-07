#include "timers.h"
#include "periph.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "em_core.h"

void TIMER_32_init() {

CMU_ClockEnable(cmuClock_TIMER0, true);
CMU_ClockEnable(cmuClock_TIMER1, true);
CMU_ClockSelectSet(cmuClock_TIMER0, cmuSelect_HFRCO);
CMU_ClockSelectSet(cmuClock_TIMER1, cmuSelect_HFRCO);

const TIMER_Init_TypeDef timer0Init = {
    .enable = false,                     /* Enable timer when init complete. */
    .debugRun = true,                  /* Do not stop counter during debug halt. */
    .prescale = timerPrescale1,         /* No prescaling. */
    .clkSel = timerClkSelHFPerClk,      /* Select HFPER clock. */
    .fallAction = timerInputActionNone, /* No action on falling input edge. */
    .riseAction = timerInputActionNone, /* No action on rising input edge. */
    .mode = timerModeUp,                /* Up-counting. */ 
    .dmaClrAct = false,                 /* Do not clear DMA requests when DMA channel is active. */
    .quadModeX4 = false,                /* Select X2 quadrature decode mode (if used). */ 
    .oneShot = false,                   /* Disable one shot. */
    .sync = false                       /* Not started/stopped/reloaded by other timers. */    
};

const TIMER_Init_TypeDef timer1Init = {
    .enable = false,                     /* Enable timer when init complete. */
    .debugRun = true,                  /*  Do not stop counter during debug halt. */
    .prescale = timerPrescale1,         /* No prescaling. */
    .clkSel = timerClkSelCascade,           /* Select Timer0. */
    .fallAction = timerInputActionNone, /* No action on falling input edge. */
    .riseAction = timerInputActionNone, /* No action on rising input edge. */
    .mode = timerModeUp,                /* Up-counting. */ 
    .dmaClrAct = false,                 /* Do not clear DMA requests when DMA channel is active. */
    .quadModeX4 = false,                /* Select X2 quadrature decode mode (if used). */ 
    .oneShot = false,                   /* Disable one shot. */
    .sync = true                            /* Started/stopped/reloaded by other timers. */    
};


// Setup TIMER interrupts
CORE_CriticalDisableIrq();
    TIMER0->IFC   = TIMER_IFC_OF;
    TIMER0->IEN   |= TIMER_IFC_OF;
    NVIC_EnableIRQ(TIMER0_IRQn);

    TIMER1->IFC   = TIMER_IFC_OF;
    TIMER1->IEN   |= TIMER_IFC_OF;
    NVIC_EnableIRQ(TIMER1_IRQn);
CORE_CriticalEnableIrq();

TIMER_Init(TIMER1, &timer1Init);
TIMER_Init(TIMER0, &timer0Init);

}

void TIMER0_IRQHandler() {
int intFlags;

CORE_CriticalDisableIrq();
    intFlags = TIMER_IntGet(TIMER0);
    TIMER_IntClear(TIMER0,TIMER_IFC_OF);
    if (intFlags & TIMER_IFC_OF) {
       led0_toggle();
    }

CORE_CriticalEnableIrq();
}

void TIMER1_IRQHandler() {
int intFlags;

CORE_CriticalDisableIrq();
    intFlags = TIMER_IntGet(TIMER1);
    TIMER_IntClear(TIMER1,TIMER_IFC_OF);
    if (intFlags & TIMER_IFC_OF) {
       led1_toggle();
    }

CORE_CriticalEnableIrq();
}
