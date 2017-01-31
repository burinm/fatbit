#include "em_device.h"
#include "em_chip.h"
#include "periph.h"

#include "bsp_trace.h"

int main(void)
{
  /* Chip errata */
  CHIP_Init();

BSP_TraceProfilerSetup();

    CMU_setup();
    LED0_setup();
    LETIMER0_setup();

    GPIO_PinOutSet(gpioPortE, 2);

  /* Infinite loop */
  while (1) {
    sleep();
  }
}
