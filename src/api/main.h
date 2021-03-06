#ifndef __MAIN_H__
#define __MAIN_H__

// If defined, "printf"-like logs go to the console
//#define DEBUG_ON

#define LOWEST_POWER_MODE           EM2 //Because of LESENSE, EM2 is the lowest
#define CALIBRATE_LE_ULFRCO         true

// If defined, use DMA to read internal temp, else poll
#define USING_DMA_FOR_LIGHT

// If defined, use DMA to send LEUART messages 
#define USING_DMA_FOR_LEUART

// If defined, implement Gecko internal sensor
#define INTERNAL_LIGHT_SENSOR

// If defined, implement I2C accelerometer
#define ACCELEROMETER_SENSOR

// If defined, turn on touch sensor routines
#define CAPSENSE_ENABLE

// If defined, turn on pulse rate sensor routines
#define PULSE_RATE_SENSOR

//If defined, use external transistor to switch IR Leds off/on
    // (requires re-wiring product)
#define PULSE_RATE_EXT_POWER

// Code to split LETIMER period into three divisions
//#define THREE_PART_TIMER

// Enable/Disable sending serial messages about status
#define SEND_EXTERNAL_NOTIFICATIONS

// Enable LCD messages
#define LCD_MESSAGES

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

#define PULSE_COUNT_BUFFER_SIZE 10
extern volatile uint8_t pulse_count_buffer[PULSE_COUNT_BUFFER_SIZE];
extern volatile uint8_t pulse_count_buffer_idx;

#endif
