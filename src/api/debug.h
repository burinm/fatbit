#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "main.h"
#include "em_device.h"
#include <stdio.h>

// Define DEBUG_ON to use, currently in main.h


#define PRINTF_BUF_SIZE 20
extern char PRINTF_BUF[PRINTF_BUF_SIZE];

/* Need to call to turn on SWO printf-like functionality
     Inputs: none, Globals: none, Return: none
*/
void setupSWOForPrint(void);

/* Print an unsigned interger to SWO console 
     Inputs: none

     Globals: PRINTF_BUF[PRINTF_BUF_SIZE]
        Used with sprintf

     Return: none
*/
void printswo_uint(unsigned int);


#ifdef DEBUG_ON
    #define PRINTSWO_UINT(n) printswo_uint(n) 
    #define SETUPSWOFORPRINT() setupSWOForPrint() 
#else
    #define PRINTSWO_UINT(n)
    #define SETUPSWOFORPRINT()
#endif

#if 0
//Broken...
void printswo_float(float f) {
uint8_t i=0;
    sprintf(PRINTF_BUF,"%f\n",f);
    while(PRINTF_BUF[i]) {
        ITM_SendChar(PRINTF_BUF[i]);
        i++;
    }
}
#endif



#endif
