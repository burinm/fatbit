#ifndef __LETIMER_H__
#define __LETIMER_H__

#include "sleep.h"

//This assumes the C preprocessor can do floating point math
#define LED_OFF_SECONDS (1.25) // Period must be in the range 0 - 2 seconds
#define LED_ON_SECONDS (.002)  //  Off + On period duration must cumulatively be 2 seconds or less


// LED will be turned on every LETIMER_PERIOD_MS for LETIMER_BLINK_MS
#define LETIMER_PERIOD_S (LED_OFF_SECONDS)
#define LETIMER_BLINK_S  (LED_ON_SECONDS)

// LXFO Setup
#define LETIMER_LFXO_TICK_S   32768
#define LETIMER_TOP_EM2 ((LETIMER_PERIOD_S * LETIMER_LFXO_TICK_S) -1)
#define LETIMER_COMP1_EM2 (LETIMER_TOP_EM2 - (LETIMER_BLINK_S * LETIMER_LFXO_TICK_S))

// ULFRCO setup, oscillator ticks are milliseconds
#define LETIMER_TOP_EM3 (LETIMER_PERIOD_S * 1000) 
#define LETIMER_COMP1_EM3 (LETIMER_TOP_EM3 - (LETIMER_BLINK_S * 1000))

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
    Globals: none
    Return: none
*/
void LETIMER0_IRQHandler();

#endif
