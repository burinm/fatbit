#ifndef __LETIMER_H__
#define __LETIMER_H__

#include "sleep.h"

/* Setup LETIMER0 depending on energy mode. Timer
    will interrupt on COMP1, COMP2

    Inputs: e_emode
        Speicify lowest energy mode timer will use
*/
void LETIMER0_setup(e_emode);

/* Interrupt routine for LETIMER, flashes LED
*/
void LETIMER0_IRQHandler();

#endif
