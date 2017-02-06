#ifndef __PERIPH_H__
#define __PERIPH_H__

/* Turn on necessary clocks for application
    Currently unused
*/
void CMU_setup();

/* LED0 */

/* Set LED0 to push/pull configuration

    Inputs: none
    Globals: none
    Return: none
*/
void LED0_setup();


/* Turn LED0 on 

    Inputs: none
    Globals: none
    Return: none
*/
void led0_on();

/* Turn LED0 off 

    Inputs: none
    Globals: none
    Return: none
*/
void led0_off();

/* Toggle LED0

    Inputs: none
    Globals: none
    Return: none
*/
void led0_toggle();

/* LED1 */

/* Turn LED0 on 

    Inputs: none
    Globals: none
    Return: none
*/
void LED1_setup();

/* Set LED1 to push/pull configuration

    Inputs: none
    Globals: none
    Return: none
*/
void led1_on();

/* Turn LED1 off 

    Inputs: none
    Globals: none
    Return: none
*/
void led1_off();

/* Toggle LED1

    Inputs: none
    Globals: none
    Return: none
*/
void led1_toggle();


#endif
