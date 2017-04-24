#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "periph.h"
#include "sleep.h"
#include "main.h"
#if defined USING_DMA_FOR_LIGHT || defined USING_DMA_FOR_LEUART
    #include "dma.h" 
#endif
#include "timers.h"
#include "letimer.h"
#include "adc.h"
#include "light_sensor_ext.h"
#include "leuart.h"

#ifdef LCD_MESSAGES
    #include "segmentlcd.h"
#endif

    #include "em_leuart.h"
#include "debug.h"

    #include "../../atmel/src/s_message.h"
    #include <string.h> //memset

#ifdef SEND_EXTERNAL_NOTIFICATIONS
    //outgoing message queue, circular buffer
    circbuf_tiny_t O_Q;
#endif

//#include "bsp_trace.h"

// This will be calibrated if CALIBRATE_LE_ULFRCO is true
uint16_t ulfrco_ticks = LETIMER_ULFRCO_TICK_S;

int main(void)
{

#ifdef SEND_EXTERNAL_NOTIFICATIONS
  //Setup outgoing message Q
  circbuf_tiny_init(&O_Q); 
#endif

  //Clear out sleep_block_counter
  memset(sleep_block_counter,EM0,EM_MAX-1);

  /* Chip errata */
  CHIP_Init();

// Note: this also turns on GPIO clock 
//BSP_Init();
//BSP_TraceProfilerSetup();

SETUPSWOFORPRINT();
PRINTSWO_UINT(87654321);

CMU_ClockEnable(cmuClock_GPIO, true);

#ifdef ACCELEROMETER_SENSOR
    //Power up I2C device here so it will be ready after
    // clock calibration routine
    accel_sensor_power_on();
#endif


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
    led0_off();

#ifdef LCD_MESSAGES
    SegmentLCD_Init(false);
    SegmentLCD_Write("FAT BIT"); 
    lcd_keep_on=3;
    lcd_motion_keep_on=3;
    //SegmentLCD_Disable();
#endif

#ifdef SEND_EXTERNAL_NOTIFICATIONS
    //Need to adjust power settings now...
    LEUART0_setup();
    leuart0_tx_string("Hello World.");
    LEUART0_disable();
#endif


#ifdef SEND_EXTERNAL_NOTIFICATIONS
    // Initial state is darkness/LED on message
    s_message *m = s_message_new(S_LED_ON);
    circbuf_tiny_write(&O_Q, (uint32_t*)m);

    #if 0
    for (int test=0;test<10;test++) {
        m = s_message_new(S_LED_ON);
        circbuf_tiny_write(&O_Q, (uint32_t*)m);
        m = s_message_new(S_LED_OFF);
        circbuf_tiny_write(&O_Q, (uint32_t*)m);
    }
    #endif
#endif


#ifdef INTERNAL_LIGHT_SENSOR
    //setup all GPIO before entering sleep
    //LES_LIGHT_SENSE
    GPIO_PinModeSet(LES_LIGHT_SENSE_PORT, LES_LIGHT_SENSE_PORT_NUM,
                        gpioModeDisabled, 0);
    
    //Setup LES_LIGHT_EXCITE 
    GPIO_PinModeSet(LES_LIGHT_EXCITE_PORT, LES_LIGHT_EXCITE_PORT_NUM,
                        gpioModePushPull, 0);

      //ADC input channel 2
    GPIO_PinModeSet(ADC_INPUT_PORT, ADC_INPUT_PORT_NUM,
                        gpioModeInput, 0);

    // Use for sampling light sensor 
    ADC0_Setup();
#endif


#if defined USING_DMA_FOR_LIGHT || defined USING_DMA_FOR_LEUART
     DMA_Setup();
#endif

    // Setup DMA
#ifdef USING_DMA_FOR_LIGHT
    DMA_Setup_ADC();
#endif

#ifdef USING_DMA_FOR_LEUART
    DMA_Setup_LEUART();
#endif

#ifdef ACCELEROMETER_SENSOR
    accel_sensor_program();
#endif

#ifdef CAPSENSE_ENABLE
    LESENSE_First();
    LESENSE_Setup();
    LESENSE_Calibrate();
#endif

  //This needs to happen last because it is the main driver
  uint16_t le_comp0;

  // Calculate ticks needed to wait to send messages (LFXO warmup)
  LETIMER0_calc_le_ticks(EM3,
                LE_PERIOD_SECONDS, LE_WAIT_TO_SEND_MESSAGE, &le_comp0, &le_send_message_ticks);

  LETIMER0_calc_le_ticks(LOWEST_POWER_MODE,
                LE_PERIOD_SECONDS, LE_ON_SECONDS, &le_comp0, &le_regular_on_ticks);
  LETIMER0_setup(LOWEST_POWER_MODE, le_comp0, le_regular_on_ticks);

  RTC_Setup();
  //blockSleepMode(EM2);

  /* Infinite loop */
  while (1) {
    sleep();
  }
}
