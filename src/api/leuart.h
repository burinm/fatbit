#ifndef __LEUART_H__
#define __LEUART_H__

#include <stdint.h>

#define LEUART_9600_BAUD_DIV    616

void LEUART0_setup();

void leuart0_txbyte(uint8_t);
uint8_t leuart0_getbyte();

void leuart0_setup_for_start(uint8_t key);
void leuart0_wait_for_start();

/* Interrupt routine for LETIMER0,
    Inputs: none
    Globals: none
    Return: none
*/
void LEUART0_IRQHandler();


#endif
