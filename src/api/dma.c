#include "dma.h"
#include "em_cmu.h"
#include "em_core.h"
#include "dmactrl.h"
#include "adc.h"
#include "sleep.h"

//Maybee combine functionality?
#include "main.h"
#include "leuart.h"
#include "../../atmel/src/s_message.h"
#include "circbuf_tiny.h"

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
uint8_t tempC;

     //ADC off
    ADC0->CMD = ADC_CMD_SINGLESTOP;
    unblockSleepMode(EM1);

    tempC = temperature_tally();
    CORE_CriticalDisableIrq();
        //enqueue temperature message, we are already in critial section
        s_message *m = s_message_new(S_TEMP);
        s_message_set_value(m,tempC);
        circbuf_tiny_write(&O_Q, (uint32_t*)m);
    CORE_CriticalEnableIrq();
}
