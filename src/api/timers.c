#include "timers.h"
#include "periph.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "em_letimer.h"
#include "em_core.h"
#include "debug.h"

void wait_m_sec(uint16_t msecs) { //4651 usec max

    //LETIMER0
    CMU_OscillatorEnable(cmuOsc_LFXO,true,false);
    CMU_ClockEnable(cmuClock_HFLE, true);
   // CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_HFCLKLE);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_LETIMER0, true);

    const LETIMER_Init_TypeDef letimerInit =
{   
    .enable         = false,                // Do no start counting when init completed
    .debugRun       = true,                 // Do not stop counter during debug halt
    .comp0Top       = false,                // Do not Load COMP0 into CNT on underflow
    .bufTop         = false,                // Do not load COMP1 into COMP0 when REP0 reaches 0
    .out0Pol        = 0,                    // Idle value 0 for output 0
    .out1Pol        = 0,                    // Idle value 0 for output 1
    .ufoa0          = letimerUFOANone,      // No action on underflow on output 0
    .ufoa1          = letimerUFOANone,      // No action on underflow on output 1
    .repMode        = letimerRepeatOneshot  // One Shot 
};

    LETIMER_Init(LETIMER0, &letimerInit);
    LETIMER0->CNT=(msecs * 33);
    LETIMER_Enable(LETIMER0, true);
    while(LETIMER_CounterGet(LETIMER0));
}

uint16_t calibrate_ULFRCO_ticks() {

    //Build 32 bit timer with TIMER0,1
    TIMER_32_init();

    //LETIMER0
    CMU_OscillatorEnable(cmuOsc_LFXO,true,false);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_HFLE, true);
    CMU_ClockEnable(cmuClock_LETIMER0, true);

    const LETIMER_Init_TypeDef letimerInit =
{   
    .enable         = false,                 // Start counting when init completed
    .debugRun       = true,                // Do not stop counter during debug halt
    .comp0Top       = false,                 // Do not Load COMP0 into CNT on underflow
    .bufTop         = false,                // Do not load COMP1 into COMP0 when REP0 reaches 0
    .out0Pol        = 0,                    // Idle value 0 for output 0
    .out1Pol        = 0,                    // Idle value 0 for output 1
    .ufoa0          = letimerUFOANone,      // No action on underflow on output 0
    .ufoa1          = letimerUFOANone,      // No action on underflow on output 1
    .repMode        = letimerRepeatOneshot  // One Shot 
};

    LETIMER_Init(LETIMER0, &letimerInit);
    LETIMER0->CNT=0x8000;

    TIMER_Enable(TIMER1, true);
    TIMER_Enable(TIMER0, true);
    LETIMER_Enable(LETIMER0, true);
    while(LETIMER_CounterGet(LETIMER0));

    TIMER_Enable(TIMER0, false);
    LETIMER_Enable(LETIMER0, false);
    uint32_t reference_lo = TIMER_CounterGet(TIMER0);
    uint32_t reference_hi = TIMER_CounterGet(TIMER1);
    double reference_time = (65536 * reference_hi) + reference_lo;
    PRINTSWO_UINT(reference_lo);
    PRINTSWO_UINT(reference_hi);
    //PRINTSWO_UINT(reference_time);

    //Now measure ULFRCO
    CMU_OscillatorEnable(cmuOsc_ULFRCO,true,false);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
    LETIMER_Init(LETIMER0, &letimerInit);
    LETIMER0->CNT=1000;
    TIMER0->CNT=0;
    TIMER1->CNT=0;
    TIMER_Enable(TIMER1, true);
    TIMER_Enable(TIMER0, true);
    LETIMER_Enable(LETIMER0, true);
    while(LETIMER_CounterGet(LETIMER0));

    TIMER_Enable(TIMER0, false);
    LETIMER_Enable(LETIMER0, false);
    uint32_t ulfrco_lo = TIMER_CounterGet(TIMER0);
    uint32_t ulfrco_hi = TIMER_CounterGet(TIMER1);
    double ulfrco_time = (65536 * ulfrco_hi) + ulfrco_lo;


    PRINTSWO_UINT(ulfrco_lo);
    PRINTSWO_UINT(ulfrco_hi);
    //PRINTSWO_UINT(ulfrco_time);

    double clock_ratio = reference_time / ulfrco_time;
    clock_ratio *= 1000;
    uint16_t ulfrco_ticks = clock_ratio;

    clock_defaults();
    return ulfrco_ticks;
}

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

TIMER_Init(TIMER1, &timer1Init);
TIMER_Init(TIMER0, &timer0Init);
}

#if 0
// Setup TIMER interrupts
CORE_CriticalDisableIrq();
    TIMER0->IFC   = TIMER_IFC_OF;
    TIMER0->IEN   |= TIMER_IFC_OF;
    NVIC_EnableIRQ(TIMER0_IRQn);

    TIMER1->IFC   = TIMER_IFC_OF;
    TIMER1->IEN   |= TIMER_IFC_OF;
    NVIC_EnableIRQ(TIMER1_IRQn);
CORE_CriticalEnableIrq();



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

#endif
