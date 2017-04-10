#ifndef __DMA_H__
#define __DMA_H__

#include "em_dma.h"
#include "s_message.h"

#define DMA_CHANNEL_FOR_ADC 0
#define DMA_CHANNEL_FOR_LEUART 1
//#define DMA_CHANNEL_FOR_LEUART_WAIT_STATUS DMA_CHREQSTATUS_CH1REQSTATUS
//#define DMA_CHANNEL_FOR_LEUART_WAIT_STATUS DMA_CHREQSTATUS_CH1REQSTATUS

extern DMA_CB_TypeDef ADC_cb;
extern DMA_CB_TypeDef LEUART_cb;
extern uint8_t leuart_message_buffer[SOURCE_MESSAGE_LENGTH];



void DMA_Setup();
void DMA_Setup_ADC();
void DMA_Setup_LEUART();
void ADCdmaTransferDone(unsigned int channel, bool primary, void *user);
void LEUARTdmaTransferDone(unsigned int channel, bool primary, void *user);


#endif
