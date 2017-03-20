#ifndef __MAIN_H__
#define __MAIN_H__

// If defined, "printf"-like logs go to the console
//#define DEBUG_ON

#define LOWEST_POWER_MODE           EM2
#define CALIBRATE_LE_ULFRCO         true

// If defined, you DMA to read internal temp, else poll
#define USING_DMA_FOR_TEMP

// If defined, use Gecko internal sensor, else use TSL2651
//#define INTERNAL_LIGHT_SENSOR

// Enable/Disable sending serial messages about status
#define SEND_EXTERNAL_NOTIFICATIONS

/*
  See letimer.h
    To set Samping time period                  LE_PERIOD_SECONDS

  See adc.h
    To set High/Low tempuratures                TEMP_HI/TEMP_LO
    To set Number of ADC samples                ADC_NUMBER_SAMPLES
    To set ADC prescaler, conversions/second    ADC_PRESCALE
*/

#include "circbuf_tiny.h"

#ifdef SEND_EXTERNAL_NOTIFICATIONS
    //outgoing message queue, circular buffer
    extern circbuf_tiny_t O_Q;
#endif

#endif
