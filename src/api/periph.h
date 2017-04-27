#ifndef __PERIPH_H__
#define __PERIPH_H__

#include <stdint.h>

#define LED0_PORT       gpioPortE
#define LED0_PORT_NUM   2
#define LED1_PORT       gpioPortE
#define LED1_PORT_NUM   3

#define LES_LIGHT_SENSE_PORT        gpioPortC
#define LES_LIGHT_SENSE_PORT_NUM    6

#define LES_LIGHT_EXCITE_PORT       gpioPortD
#define LES_LIGHT_EXCITE_PORT_NUM   6

#define PULSE_POWER_EXTERNAL_PORT       gpioPortC
//#define PULSE_POWER_EXTERNAL_PIN        6 //This pin doesn't work?
#define PULSE_POWER_EXTERNAL_PIN          0

#define PULSE_POWER_ON   GPIO_PinOutSet(PULSE_POWER_EXTERNAL_PORT, PULSE_POWER_EXTERNAL_PIN)
#define PULSE_POWER_OFF  GPIO_PinOutClear(PULSE_POWER_EXTERNAL_PORT, PULSE_POWER_EXTERNAL_PIN)

/* Clocks */

/* Set clocks to boot up configuration
    Inputs: none, Globals: none, Return: none
*/
void clock_defaults();

/* Turn on necessary clocks for application
    Currently unused
*/
void CMU_setup();

/* LED0 */

/* Set LED0 to push/pull configuration
    Inputs: none, Globals: none, Return: none
*/
void LED0_setup();

/* Is LED0 on 
    Inputs: none, Globals: none, Return:  >0 if on
*/
uint8_t is_led0_on(); 

/* Turn LED0 on 
    Inputs: none, Globals: none, Return: none
*/
void led0_on();

/* Turn LED0 off 
    Inputs: none, Globals: none, Return: none
*/
void led0_off();

/* Toggle LED0
    Inputs: none, Globals: none, Return: none
*/
void led0_toggle();


/* LED1 */

/* Set LED1 to push/pull configuration 
    Inputs: none, Globals: none, Return: none
*/
void LED1_setup();

/* Is LED1 on 
    Inputs: none, Globals: none, Return:  >0 if on
*/
uint8_t is_led1_on(); 

/* Turn LED1 on 
    Inputs: none, Globals: none, Return: none
*/
void led1_on();

/* Turn LED1 off 
    Inputs: none, Globals: none, Return: none
*/
void led1_off();

/* Toggle LED1
    Inputs: none, Globals: none, Return: none
*/
void led1_toggle();


#endif
