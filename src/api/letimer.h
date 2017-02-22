#ifndef __LETIMER_H__
#define __LETIMER_H__

#include "sleep.h"

extern uint16_t ulfrco_ticks;
extern uint8_t letimer_frame;

#define LETIMER_ULFRCO_TICK_S  1000 
#define LETIMER_LFXO_TICK_S   32768

//#define LE_DIVIDER 2        //Needed for periods 2 seconds and over
#define LE_DIVIDER 4        //Needed for periods 4 seconds and over
#define LE_PERIOD_SECONDS (4.25)
#define LE_ON_SECONDS (.004)

/* Setup LETIMER0 depending on energy mode. Timer
    will interrupt on COMP1, COMP2

    Inputs: e_emode
        Speicify lowest energy mode timer will use

    Globals: none
    Return: none

*/
void LETIMER0_setup(e_emode);

/* Interrupt routine for LETIMER, flashes LED
    Inputs: none
    Globals: ulfrco_ticks, used to set LETIMER with ULFRCO
    Return: none
*/
void LETIMER0_IRQHandler();

#endif
