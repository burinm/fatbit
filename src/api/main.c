#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "periph.h"
#include "sleep.h"
#include "main.h"
#include "timers.h"
#include "letimer.h"
#include "debug.h"

#include "bsp_trace.h"

//Set this to tell the main program what the lowest power mode is
#define LOWEST_POWER_MODE   EM3


int main(void)
{
  /* Chip errata */
  CHIP_Init();


// Note: this also turns on GPIO clock 
//BSP_TraceProfilerSetup();

//setupSWOForPrint();
PRINTSWO_UINT(87654321);

clock_defaults();

//   blockSleepMode(EM1);

    uint16_t ulfrco_ticks = calibrate_ULFRCO_ticks();
    clock_defaults();

    PRINTSWO_UINT(ulfrco_ticks);
    PRINTSWO_UINT(12345678);


   //LED0_setup();
   LED1_setup();
   //LED1 is both the state and the indicator for dark mode
   led1_on();


   LETIMER0_setup(LOWEST_POWER_MODE);

    //setup all GPIO before entering sleep
    CMU_ClockEnable(cmuClock_GPIO, true);
    //LES_LIGHT_SENSE
    GPIO_PinModeSet(gpioPortC, 6, gpioModeDisabled, 0); //Pull up down??
    
    //Turn on LES_LIGHT_EXCITE 
    GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
    GPIO_PinOutSet(gpioPortD, 6);

    //GPIOs will keep thier state, so we only need to toggle clock
    CMU_ClockEnable(cmuClock_GPIO, false);

    
  /* Infinite loop */
  while (1) {
    sleep();
  }
}
