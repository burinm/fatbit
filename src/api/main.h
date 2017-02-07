#ifndef __MAIN_H__
#define __MAIN_H__

//Set this to tell the main program what the lowest power mode is
#define LOWEST_POWER_MODE   EM2

//This assumes the C preprocessor can do floating point math
#define LED_OFF_SECONDS (1.75) // Period must be in the range 0 - 2 seconds
#define LED_ON_SECONDS (.030)  //  Off + On period duration must cumulatively be 2 seconds or less

//#define DEBUG_ON

#endif
