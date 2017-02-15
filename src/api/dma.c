#include "dma.h"
#include "em_cmu.h"
#include "em_core.h"
#include "dmactrl.h"
#include "adc.h"

DMA_CB_TypeDef ADC_cb;


void DMA_Setup() {

//Callback setup
ADC_cb.cbFunc = ADCdmaTransferDone;
ADC_cb.userPtr = NULL;
ADC_cb.primary = true;


CMU_ClockEnable(cmuClock_DMA, true);


DMA_Init_TypeDef DMA_init = {
    .hprot = 0,
    .controlBlock = dmaControlBlock
};

DMA_Init(&DMA_init);

DMA_CfgChannel_TypeDef DMA_cfg_channel = {
    .highPri = false,
    .enableInt = true,
    .select = DMAREQ_ADC0_SINGLE,
    .cb=&ADC_cb
};

DMA_CfgChannel(DMA_CHANNEL_FOR_ADC, &DMA_cfg_channel);


DMA_CfgDescr_TypeDef DMA_cfg_desc = {
    .dstInc = dmaDataInc2,
    .srcInc = dmaDataIncNone,
    .size = dmaDataSize2,
    .arbRate = dmaArbitrate1,
    .hprot = 0
};

DMA_CfgDescr(DMA_CHANNEL_FOR_ADC, true, &DMA_cfg_desc);

CORE_CriticalDisableIrq();
    DMA->IFC = DMA_IFC_CH0DONE;
    DMA->IEN |= DMA_IEN_CH0DONE;
    NVIC_EnableIRQ(DMA_IRQn);
CORE_CriticalEnableIrq();


}

// DMA IRQ routine is set by SDK, callback
void ADCdmaTransferDone(unsigned int channel, bool primary, void *user) {
    temperature_tally();
}
