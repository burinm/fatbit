#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <stdint.h>

/* Return calibrated ticks for ULRCO based on LXFO */
uint16_t calibrate_ULFRCO_ticks();

/* Setup TIMER0 and TIMER1 in a 32bit combined timer */
void TIMER_32_init();


#endif
