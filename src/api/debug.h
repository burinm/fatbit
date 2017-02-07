#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "em_device.h"
#include <stdio.h>


#define PRINTF_BUF_SIZE 20
extern char PRINTF_BUF[PRINTF_BUF_SIZE];

void setupSWOForPrint(void);
void printswo_uint(unsigned int);


#ifdef DEBUG_ON
    void printswo_uint(unsigned int n) {\
    uint8_t i=0;\
        sprintf(PRINTF_BUF,"%u\n",n);\
        while(PRINTF_BUF[i]) {\
            ITM_SendChar(PRINTF_BUF[i]);\
            i++;\
        }\
    }
    #define PRINTSWO_UINT(n) printswo_uint(n) 
#else
    #define PRINTSWO_UINT(n)
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
