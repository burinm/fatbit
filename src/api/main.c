#include "em_device.h"
#include "em_chip.h"
    #include "em_cmu.h" //move to CPU_INIT
#include "periph.h"
#include "sleep.h"
#include "main.h"
//#include "timers.h"
#include "em_letimer.h"
#include "em_timer.h"
#include "debug.h"


#include "bsp_trace.h"

int main(void)
{
  /* Chip errata */
  CHIP_Init();


// Note: this also turns on GPIO clock 
BSP_TraceProfilerSetup();

setupSWOForPrint();
printswo_uint(87654321);

// This should be the default setup, but being pedantic
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
    CMU_HFRCOBandSet(_CMU_HFRCOCTRL_BAND_14MHZ);
    CMU_OscillatorEnable(cmuOsc_HFRCO,true,false);
    CMU_OscillatorEnable(cmuOsc_AUXHFRCO,true,false);

    // Disable other oscillators
    CMU_OscillatorEnable(cmuOsc_HFXO, false, false);
    CMU_OscillatorEnable(cmuOsc_LFXO, false, false);
    CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);
    CMU_OscillatorEnable(cmuOsc_ULFRCO, false, false);
    //CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, false);

    LED0_setup();
    LED1_setup();
    blockSleepMode(EM1);
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
    uint32_t reference_lo = TIMER_CounterGet(TIMER0);
    uint32_t reference_hi = TIMER_CounterGet(TIMER1);
    printswo_uint(reference_lo);
    printswo_uint(reference_hi);

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
    uint32_t ulfrco_lo = TIMER_CounterGet(TIMER0);
    uint32_t ulfrco_hi = TIMER_CounterGet(TIMER1);


    printswo_uint(ulfrco_lo);
    printswo_uint(ulfrco_hi);
    printswo_uint(12345678);
    
  /* Infinite loop */
  while (1) {
//    sleep();
  }
}
