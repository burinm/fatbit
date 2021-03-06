#ifndef __UART_SAMB11_H__
#define __UART_SAMB11_H__

#include "circbuf_tiny.h"

extern struct uart_module my_uart_instance;
extern struct dma_resource uart_dma_resource_tx;
extern struct dma_resource uart_dma_resource_rx;

extern circbuf_tiny_t M_Q;

void uart_setup(void);

#define DMA_RX_COUNT    1

#endif
