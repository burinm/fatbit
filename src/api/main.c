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
#include "leuart.h"

    #include "em_leuart.h"
#include "debug.h"

    #include "../../atmel/src/s_message.h"
    #include <string.h> //memset

//outgoing message queue, circular buffer
circbuf_tiny_t O_Q;

//#include "bsp_trace.h"

// This will be calibrated if CALIBRATE_LE_ULFRCO is true
uint16_t ulfrco_ticks = LETIMER_ULFRCO_TICK_S;

int main(void)
{

  //Setup outgoing message Q
  circbuf_tiny_init(&O_Q); 

  //Clear out sleep_block_counter
  memset(sleep_block_counter,EM0,EM_MAX-1);

  /* Chip errata */
  CHIP_Init();

// Note: this also turns on GPIO clock 
//BSP_Init();
//BSP_TraceProfilerSetup();

SETUPSWOFORPRINT();
PRINTSWO_UINT(87654321);

clock_defaults();

    if (CALIBRATE_LE_ULFRCO) {
        ulfrco_ticks = calibrate_ULFRCO_ticks();
        clock_defaults();
    }

//    PRINTSWO_UINT(ulfrco_ticks);
//    PRINTSWO_UINT(12345678);

    LED1_setup();
    led1_off(); //start off for debugging

    LED0_setup();
    /* Initial state is darkness/LED on
        LED0 is both the state and the indicator for dark mode
    */
    led0_on();


    //Need to adjust power settings now...
    LEUART0_setup();
    leuart0_tx_string("########");
#if 0
    LEUART0_enable();
    LEUART0_disable();
#endif

    // Initial state is darkness/LED on message
    s_message *m = s_message_new(S_LED_ON);
    circbuf_tiny_write(&O_Q, (uint32_t*)m);

#ifdef INTERNAL_LIGHT_SENSOR
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

#else // External Light Sensor
    CMU_ClockEnable(cmuClock_GPIO, true);
#endif

    // Setup for temperature measurement    
    ADC0_Setup();

    // Setup DMA
#ifdef USING_DMA_FOR_TEMP
    DMA_Setup();
#endif

  //This needs to happen last because it is the main driver
  LETIMER0_setup(LOWEST_POWER_MODE);

  /* Infinite loop */
  while (1) {
    sleep();
  }
}
