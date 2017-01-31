#include "em_device.h"
#include "em_chip.h"
#include "periph.h"
#include "sleep.h"

#include "bsp_trace.h"

int main(void)
{
  /* Chip errata */
  CHIP_Init();

BSP_TraceProfilerSetup();

    CMU_setup();
    LED0_setup();
    LETIMER0_setup(EM3);

  /* Infinite loop */
  while (1) {
    sleep();
  }
}
