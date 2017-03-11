#ifndef __UART_SAMB11_H__
#define __UART_SAMB11_H__

extern struct dma_resource uart_dma_resource_tx;
extern struct dma_resource uart_dma_resource_rx;

void uart_setup(void);

#endif
