#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>

extern uint16_t pulse_measure;

//These two must be changed together
#define RTC_PRESCALE            cmuClkDiv_2
#define RTC_TICKS_PER_SECOND    16384

#define SLOWEST_HEARTBEAT      24576 // RTC_TICKS_PER_SECOND / (40bpm / 60s/m) BPS
#define FASTEST_HEARTBEAT      4468 // RTC_TICKS_PER_SECOND / (220bpm / 60s/m) BPS

void RTC_Setup();
void RTC_IRQHandler(void);



#endif
