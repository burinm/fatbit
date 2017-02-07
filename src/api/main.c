#include "em_device.h"
#include "em_chip.h"
#include "periph.h"
#include "sleep.h"
#include "main.h"
#include "timers.h"
#include "debug.h"


#include "bsp_trace.h"

int main(void)
{
  /* Chip errata */
  CHIP_Init();


// Note: this also turns on GPIO clock 
BSP_TraceProfilerSetup();

setupSWOForPrint();
PRINTSWO_UINT(87654321);

clock_defaults();

//   LED0_setup();
//   LED1_setup();
//   blockSleepMode(EM1);

    uint16_t ulfrco_ticks =calibrate_ULFRCO_ticks();
    PRINTSWO_UINT(ulfrco_ticks);
    PRINTSWO_UINT(12345678);
    
  /* Infinite loop */
  while (1) {
//    sleep();
  }
}
