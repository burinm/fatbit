#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

#define ADC_TIMEBASE 13000 //1us at 13MHz

/* Setup ADC0 for temperature measurement aquisition
*/
void ADC0_Setup();

/* ADC0 interrupt handler, used for interrupt driven
 *  temperature sampling
 */
void ADC0_IRQHandler();

/* Tally up average */
void temperature_tally();

/* convertToCelsius
    Silicon Labs (c) 2017 - from class notes ecen5023 - IOT
*/
float convertToCelsius(int32_t adcSample);

#endif
