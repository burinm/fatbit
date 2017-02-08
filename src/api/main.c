#include "em_device.h"
#include "em_chip.h"
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
BSP_TraceProfilerSetup();

setupSWOForPrint();
PRINTSWO_UINT(87654321);

clock_defaults();

//   blockSleepMode(EM1);

    uint16_t ulfrco_ticks = calibrate_ULFRCO_ticks();
    PRINTSWO_UINT(ulfrco_ticks);
    PRINTSWO_UINT(12345678);


   LED0_setup();
   LED1_setup();
   //LED1 is both the state and the indicator for dark mode
   led1_on();


   LETIMER0_setup(LOWEST_POWER_MODE);
    
  /* Infinite loop */
  while (1) {
    sleep();
  }
}
