#ifndef __LETIMER_H__
#define __LETIMER_H__

#include "sleep.h"

extern uint16_t ulfrco_ticks;
extern uint16_t le_send_message_ticks;
extern uint16_t le_regular_on_ticks;

extern uint16_t motion_ticks;
#define MOTION_TICKS_TOP    10

#ifdef LCD_MESSAGES
    extern uint8_t lcd_keep_on;
    extern uint8_t lcd_motion_keep_on;
#endif

#define LETIMER_ULFRCO_TICK_S  1000 
#define LETIMER_LFXO_TICK_S   32768

#define LE_DIVIDER 2        //Needed for periods 2 seconds and over
//#define LE_DIVIDER 4        //Needed for periods 4 seconds and over
#define LE_PERIOD_SECONDS (3.75)
#define LE_ON_SECONDS (.004)

#define LE_WAIT_TO_SEND_MESSAGE (1.3)

/* Setup LETIMER0 depending on energy mode. Timer
    will interrupt on COMP1, COMP2

    Inputs: e_emode, le_comp0, ,le_comp1
        Speicify lowest energy mode timer will use
        Ticks calculated from LETIMER0_calc_le_ticks

    Globals: LE_DIVIDER
    Return: none

*/
void LETIMER0_setup(e_emode, uint16_t le_comp0, uint16_t le_comp1);


/* Calculate the number of ticks needed in different energy modes
    for the LETIMER

    Inputs: le_period_seconds - total period COMP0, le_on_seconds - second int COMP1
        Specify period and second timer pop

    Globals: LE_DIVIDER
    Return: le_comp0, le_comp1
        Ticks for LETIMER_CompareSet
*/
void LETIMER0_calc_le_ticks(e_emode, double le_period_seconds, double le_on_seconds,
                            uint16_t *le_comp0, uint16_t *le_comp1);

/* Interrupt routine for LETIMER, flashes LED
    Inputs: none
    Globals: ulfrco_ticks, used to set LETIMER with ULFRCO
    Return: none
*/
void LETIMER0_IRQHandler();

#endif
