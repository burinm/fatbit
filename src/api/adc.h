#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

#define SUNLIGHT_LOW_THRESH 20 
#define SUNLIGHT_ALERT_TIMES 3

#define ADC_INPUT_PORT      gpioPortD
#define ADC_INPUT_PORT_NUM  2

#define ADC_NUMBER_SAMPLES 100
#define ADC_NUMBER_AVERAGES 10 //Keep under 255, counter is uint8_t 

//Measures 1.94ms, calculated 1.93ms - 100 samples
#define ADC_PRESCALE 18 // (10 - 1)  51851 samples/s  3us aquisition/12 bits, 15 clocks

extern uint16_t adc_sample_buffer[ADC_NUMBER_SAMPLES];

/* Setup ADC0 for temperature measurement aquisition
*/
void ADC0_Setup();

/* ADC0 interrupt handler, used for interrupt driven
 *  temperature sampling
 */
#ifndef USING_DMA_FOR_LIGHT
    void ADC0_IRQHandler();
#endif

/* Tally up average, return temperature
    **Critical, protect **

    Return: average sunlight exposure 

 */
uint8_t sunlight_tally();

/* convertToCelsius
    Silicon Labs (c) 2017 - from class notes ecen5023 - IOT
*/
float convertToCelsius(int32_t adcSample);

#endif
