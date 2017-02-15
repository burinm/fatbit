#include "dma.h"
#include "em_cmu.h"
#include "em_core.h"
#include "adc.h"

DMA_CB_TypeDef ADC_cb;


void DMA_Setup() {

ADC_cb.cbFunc = ADCdmaTransferDone;
ADC_cb.userPtr = NULL;
ADC_cb.primary = true;


CMU_ClockEnable(cmuClock_DMA, true);

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
