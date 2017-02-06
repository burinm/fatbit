#include "em_device.h"
#include "em_chip.h"
    #include "em_cmu.h" //move to CPU_INIT
#include "periph.h"
#include "sleep.h"
#include "main.h"
#include "timers.h"


#include "bsp_trace.h"

int main(void)
{
  /* Chip errata */
  CHIP_Init();

// SegmentLCD_Init(false);
// SegmentLCD_AllOn();
//SegmentLCD_Write("Hello");

// Note: this also turns on GPIO clock 
BSP_TraceProfilerSetup();

    // This should be the default setup, but being pedantic
//    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
 //   CMU_HFRCOBandSet(_CMU_HFRCOCTRL_BAND_14MHZ);

/*
    // Disable other oscillators
    CMU_OscillatorEnable(cmuOsc_HFXO, false, false);
    CMU_OscillatorEnable(cmuOsc_LFXO, false, false);
    CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);
    CMU_OscillatorEnable(cmuOsc_ULFRCO, false, false);
    CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, false);
*/

    // Enable HFRCO, should already be on
  //  CMU_OscillatorEnable(cmuOsc_HFRCO,true,false);

    LED0_setup();
    LED1_setup();
    blockSleepMode(EM1);
    TIMER_32_init();
    //CMU_setup();
    //LETIMER0_setup(LOWEST_POWER_MODE);

  /* Infinite loop */
  while (1) {
//    sleep();
  }
}
