#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <stdint.h>

/* Wait for x milliseconds using LXFO LETIMER timer */
//4651 usec max
void wait_m_sec(uint16_t usecs);


/* Return calibrated ticks for ULRCO based on LXFO */
uint16_t calibrate_ULFRCO_ticks();

/* Setup TIMER0 and TIMER1 in a 32bit combined timer */
void TIMER_32_init();


#endif
