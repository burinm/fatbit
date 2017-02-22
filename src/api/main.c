#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "periph.h"
#include "sleep.h"
#include "main.h"
#ifdef USING_DMA_FOR_TEMP
    #include "dma.h" 
#endif
#include "timers.h"
#include "letimer.h"
#include "adc.h"
#include "light_sensor_ext.h"
#include "debug.h"

    //debug
    #include "em_gpio.h"

//#include "bsp_trace.h"

// This will be calibrated if CALIBRATE_LE_ULFRCO is true
uint16_t ulfrco_ticks = LETIMER_ULFRCO_TICK_S;

int main(void)
{
  /* Chip errata */
  CHIP_Init();


// Note: this also turns on GPIO clock 
//BSP_Init();
//BSP_TraceProfilerSetup();

SETUPSWOFORPRINT();
PRINTSWO_UINT(87654321);

clock_defaults();

#if 0
    if (CALIBRATE_LE_ULFRCO) {
        ulfrco_ticks = calibrate_ULFRCO_ticks();
        clock_defaults();
    }

//    PRINTSWO_UINT(ulfrco_ticks);
//    PRINTSWO_UINT(12345678);

   LED1_setup();
   led1_off(); //start off for debugging

   LED0_setup();
   //LED0 is both the state and the indicator for dark mode
   led0_on();

   LETIMER0_setup(LOWEST_POWER_MODE);

    //setup all GPIO before entering sleep
    CMU_ClockEnable(cmuClock_GPIO, true);
    //LES_LIGHT_SENSE
    GPIO_PinModeSet(LES_LIGHT_SENSE_PORT, LES_LIGHT_SENSE_PORT_NUM,
                        gpioModeDisabled, 0);
    
    //Setup LES_LIGHT_EXCITE 
    GPIO_PinModeSet(LES_LIGHT_EXCITE_PORT, LES_LIGHT_EXCITE_PORT_NUM,
                        gpioModePushPull, 0);

    //GPIOs will keep thier state, so we only need to toggle clock
    CMU_ClockEnable(cmuClock_GPIO, false);

    // Setup for temperature measurement    
    ADC0_Setup();

    // Setup DMA
#ifdef USING_DMA_FOR_TEMP
    DMA_Setup();
#endif
#endif

    light_sensor_power_on();
    light_sensor_program();

    //tsl2651_open();

    //tsl2651_on(1);
    //tsl2651_read_register(0);
     LED0_setup();
  /* Infinite loop */
  while (1) {
    if (GPIO_PortInGet(LIGHT_SENSOR_INT_PORT) & (1 << LIGHT_SENSOR_INT_PORT_NUM))
    {
         led0_on();
    } else {
        tsl2651_int_clear();
         led0_off();
    }
        
    //sleep();
  }
}
