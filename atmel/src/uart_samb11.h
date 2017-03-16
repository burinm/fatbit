#ifndef __UART_SAMB11_H__
#define __UART_SAMB11_H__

extern struct uart_module my_uart_instance;
extern struct dma_resource uart_dma_resource_tx;
extern struct dma_resource uart_dma_resource_rx;

void uart_setup(void);

#define DMA_RX_COUNT    1

#endif
