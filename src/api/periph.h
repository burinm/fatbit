#ifndef __PERIPH_H__
#define __PERIPH_H__

/* Turn on necessary clocks for application
    Currently unused
*/
void CMU_setup();

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


#endif
