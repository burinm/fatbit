#include "em_device.h"
#include "em_chip.h"
#include "periph.h"
#include "sleep.h"
#include "main.h"

//#include "bsp_trace.h"


int main(void)
{
  /* Chip errata */
  CHIP_Init();

// Note: this also turns on GPIO clock 
//BSP_TraceProfilerSetup();

    CMU_setup();
    LED0_setup();
    LETIMER0_setup(LOWEST_POWER_MODE);

  /* Infinite loop */
  while (1) {
    sleep();
  }
}
