#ifndef __ACMP_H__
#define __ACMP_H__

#include <stdint.h>

#define VDD_DARKNESS 4 
#define VDD_LIGHTNESS 31

/* Get ACMP0 ready for use, connects to photodiode, but does not turn on exciter
    Inputs: vdd
      voltage for comparator = 3.3 * vdd / 63 

    Globals: none
    Return: none
*/
void ACMP_fire_up(uint8_t); 

#endif
