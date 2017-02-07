#include "debug.h"
#include "em_device.h"
#include <stdint.h>
#include <stdio.h>

#define PRINTF_BUF_SIZE 20
char PRINTF_BUF[PRINTF_BUF_SIZE] = {0};

void printswo_uint(unsigned int n) {
uint8_t i=0;
    sprintf(PRINTF_BUF,"%u\n",n);
    while(PRINTF_BUF[i]) {
        ITM_SendChar(PRINTF_BUF[i]);
        i++;
    }
}

void printswo_float(float f) {
uint8_t i=0;
    sprintf(PRINTF_BUF,"%f\n",f);
    while(PRINTF_BUF[i]) {
        ITM_SendChar(PRINTF_BUF[i]);
        i++;
    }
}

void setupSWOForPrint(void)
/* Copied from instructional notes in Silicon Labs, commander application
    installed with Simplicity Studio 4
*/
{ 
  /* Enable GPIO clock. */
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;
  
  /* Enable Serial wire output pin */
  GPIO->ROUTE |= GPIO_ROUTE_SWOPEN;

#if defined(_EFM32_GIANT_FAMILY) || defined(_EFM32_LEOPARD_FAMILY) || defined(_EFM32_WONDER_FAMILY)
  /* Set location 0 */
  GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC0;
  
  /* Enable output on pin - GPIO Port F, Pin 2 */
  GPIO->P[5].MODEL &= ~(_GPIO_P_MODEL_MODE2_MASK);
  GPIO->P[5].MODEL |= GPIO_P_MODEL_MODE2_PUSHPULL;
#else
  /* Set location 1 */
  GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) |GPIO_ROUTE_SWLOCATION_LOC1;
  /* Enable output on pin */
  GPIO->P[2].MODEH &= ~(_GPIO_P_MODEH_MODE15_MASK);
  GPIO->P[2].MODEH |= GPIO_P_MODEH_MODE15_PUSHPULL;
#endif
  
  /* Enable debug clock AUXHFRCO */
  CMU->OSCENCMD = CMU_OSCENCMD_AUXHFRCOEN;
  
  /* Wait until clock is ready */
  while (!(CMU->STATUS & CMU_STATUS_AUXHFRCORDY));
  
  /* Enable trace in core debug */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  ITM->LAR  = 0xC5ACCE55;
  ITM->TER  = 0x0;
  ITM->TCR  = 0x0;
  TPI->SPPR = 2;
  TPI->ACPR = 0xf;
  ITM->TPR  = 0x0;
  DWT->CTRL = 0x400003FE;
  ITM->TCR  = 0x0001000D;
  TPI->FFCR = 0x00000100;
  ITM->TER  = 0x1;
}

/*
 Need to implement the  two Retarget IO functions with the read/write functions we want to use for printf. 
int RETARGET_WriteChar(char c){
  return ITM_SendChar (c);
}

int RETARGET_ReadChar(void){
  return 0;
}
*/
