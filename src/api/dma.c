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
#include <stdlib.h> //free

#ifdef USING_DMA_FOR_LEUART
    #include <string.h>
#endif


#if defined USING_DMA_FOR_TEMP || defined USING_DMA_FOR_LEUART
void DMA_Setup() {

    CMU_ClockEnable(cmuClock_DMA, true);

    DMA_Init_TypeDef DMA_init = {
        .hprot = 0,
        .controlBlock = dmaControlBlock
    };

    DMA_Init(&DMA_init);
}
#endif

#ifdef USING_DMA_FOR_TEMP

    DMA_CB_TypeDef ADC_cb;

    void DMA_Setup_ADC() {

    //Callback setup
    ADC_cb.cbFunc = ADCdmaTransferDone;
    ADC_cb.userPtr = NULL;
    ADC_cb.primary = true;

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

        CORE_CriticalDisableIrq();
             //ADC off
            ADC0->CMD = ADC_CMD_SINGLESTOP;
            unblockSleepMode(EM1);

            tempC = temperature_tally();

    #ifdef SEND_EXTERNAL_NOTIFICATIONS
            //enqueue temperature message, we are already in critial section
            s_message *m = s_message_new(S_TEMP);
            s_message_set_value(m,tempC);
            circbuf_tiny_write(&O_Q, (uint32_t*)m);
    #endif
        CORE_CriticalEnableIrq();
    }
#endif

#ifdef USING_DMA_FOR_LEUART

    DMA_CB_TypeDef LEUART_cb;
    uint8_t leuart_message_buffer[SOURCE_MESSAGE_LENGTH];


    void DMA_Setup_LEUART() {

    //Callback setup
    LEUART_cb.cbFunc = LEUARTdmaTransferDone;
    LEUART_cb.userPtr = NULL;
    LEUART_cb.primary = true;

    DMA_CfgChannel_TypeDef DMA_cfg_channel = {
        .highPri = false,
        .enableInt = true,
        .select = DMAREQ_LEUART0_TXBL,
        .cb=&LEUART_cb
    };

    DMA_CfgChannel(DMA_CHANNEL_FOR_LEUART, &DMA_cfg_channel);


    DMA_CfgDescr_TypeDef DMA_cfg_desc = {
        .dstInc = dmaDataIncNone,
        .srcInc = dmaDataInc1,
        .size = dmaDataSize1,
        .arbRate = dmaArbitrate1,
        .hprot = 0
    };

    DMA_CfgDescr(DMA_CHANNEL_FOR_LEUART, true, &DMA_cfg_desc);

    CORE_CriticalDisableIrq();
        DMA->IFC = DMA_IFC_CH1DONE;
        DMA->IEN |= DMA_IEN_CH1DONE;
        NVIC_EnableIRQ(DMA_IRQn);
    CORE_CriticalEnableIrq();


    }

    // DMA IRQ routine is set by SDK, callback
    void LEUARTdmaTransferDone(unsigned int channel, bool primary, void *user) {
        s_message *m=NULL;

        LEUART0->CTRL &= ~LEUART_CTRL_TXDMAWU;
        LEUART0_disable();
        CORE_CriticalDisableIrq();
        // Check message Q for another message. Setup new transfer if not empty
        if (circbuf_tiny_read(&O_Q,(uint32_t**)&m)) {
            if (m) {
                memcpy(&leuart_message_buffer, m->message, SOURCE_MESSAGE_LENGTH);
                free(m);

                LEUART0_enable();
                LEUART0->CTRL |= LEUART_CTRL_TXDMAWU;

                DMA_ActivateBasic(DMA_CHANNEL_FOR_LEUART, true, false,
                                 (void*)&(LEUART0->TXDATA),
                                 &leuart_message_buffer,
                                 SOURCE_MESSAGE_LENGTH-1);
            }
        }

        CORE_CriticalEnableIrq();
    }
#endif
