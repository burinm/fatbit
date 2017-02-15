#ifndef __DMA_H__
#define __DMA_H__

#include "em_dma.h"

#define DMA_CHANNEL_FOR_ADC 0

extern DMA_CB_TypeDef ADC_cb;


void DMA_Setup();
void ADCdmaTransferDone(unsigned int channel, bool primary, void *user);


#endif
