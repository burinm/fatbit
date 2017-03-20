#ifndef __LEUART_H__
#define __LEUART_H__

#include <stdint.h>

#define LEUART_9600_BAUD_DIV    616
#define LEUART_SEND_STRING_MAX  255

/* Corresponds to
        TX  PD4 /  external pin 12
        RX  PD5 /  external pin 14 
*/
#define LEUART_TX_PORT      gpioPortD
#define LEUART_TX_PORT_NUM  4
#define LEUART_RX_PORT      gpioPortD
#define LEUART_RX_PORT_NUM  5

void LEUART0_setup();
void LEUART0_enable();
void LEUART0_disable();
void LEUART0_tx_on_only();

//void leuart0_txbyte(uint8_t);
#define leuart0_txbyte(x) LEUART_Tx(LEUART0,x)
void leuart0_tx_string(char*);

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
