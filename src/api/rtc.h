#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>

extern uint16_t pulse_measure;

//These two must be changed together
#define RTC_PRESCALE            cmuClkDiv_2
#define RTC_TICKS_PER_SECOND    16384

#define SLOWEST_HEARTBEAT      400 // RTC_TICKS_PER_SECOND / 40 BPM

void RTC_Setup();
void RTC_IRQHandler(void);



#endif
