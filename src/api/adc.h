#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

// Room temperature ranges
#define TEMP_HI 35 
#define TEMP_LO 15 

// Sensistive to touch at this range
//#define TEMP_HI 26 
//#define TEMP_LO 23 

//#define ADC_NUMBER_SAMPLES 750
//#define ADC_NUMBER_SAMPLES 1000
#define ADC_NUMBER_SAMPLES 500

//#define ADC_PRESCALE 52 // (53 - 1)  20000 samples/3us aquisition/12 bits, 13 clocks
//#define ADC_PRESCALE 24 // (25 - 1)  40000 samples/3us aquisition/12 bits, 14 clocks

//This one measures 25ms, should be 27mS... - 1000 samples
//#define ADC_PRESCALE 26 // (27 - 1)  40000 samples/3us aquisition/12 bits, 14 clocks

//Measures 6.75ms, calculated 5.7ms - 500 samples
#define ADC_PRESCALE 9 // (10 - 1)  87500 samples/3us aquisition/12 bits, 16 clocks

extern uint16_t adc_sample_buffer[ADC_NUMBER_SAMPLES];

/* Setup ADC0 for temperature measurement aquisition
*/
void ADC0_Setup();

/* ADC0 interrupt handler, used for interrupt driven
 *  temperature sampling
 */
#ifndef USING_DMA_FOR_TEMP
    void ADC0_IRQHandler();
#endif

/* Tally up average, return tempurature */
uint8_t temperature_tally();

/* convertToCelsius
    Silicon Labs (c) 2017 - from class notes ecen5023 - IOT
*/
float convertToCelsius(int32_t adcSample);

#endif
