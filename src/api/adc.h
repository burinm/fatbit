#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

#define TEMP_HI 25 
#define TEMP_LO 23 
#define ADC_TIMEBASE 13000 //1us at 13MHz
#define ADC_NUMBER_SAMPLES 750

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

/* Tally up average */
void temperature_tally();

/* convertToCelsius
    Silicon Labs (c) 2017 - from class notes ecen5023 - IOT
*/
float convertToCelsius(int32_t adcSample);

#endif
