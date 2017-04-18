#include "adc.h"
#include "periph.h"
#include "sleep.h"
#include "em_cmu.h"
#include "em_adc.h"
#include "em_core.h"
#include "em_gpio.h"
#include "letimer.h"

//Maybee combine functionality?
#include "main.h"
#include "leuart.h"
#include "../../atmel/src/s_message.h"
#include "circbuf_tiny.h"

    #include "debug.h"

uint16_t adc_sample_count=0;
uint16_t adc_sample_buffer[ADC_NUMBER_SAMPLES] = {0};

void ADC0_Setup() {

    CMU_ClockEnable(cmuClock_ADC0, true);

    uint8_t adc_timebase = ADC_TimebaseCalc(0); //0 causes auto clock speed fetch
    const ADC_Init_TypeDef ADC_init = {
        .ovsRateSel = adcOvsRateSel2,             /* 2x oversampling (if enabled). */
        //.warmUpMode = adcWarmupNormal,            /* ADC shutdown after each conversion. */
        .warmUpMode = adcWarmupKeepADCWarm,  
        .timebase = adc_timebase,                 /* Timebase calulated from clock. */
        //.prescale =  _ADC_CTRL_PRESC_DEFAULT,     /* Use HW default value. */
        /** Clock division factor N, ADC clock =  HFPERCLK / (N + 1). */
        .prescale =  ADC_PRESCALE,
        .tailgate = false                         /* Do not use tailgate. */
    };
        
    ADC_Init(ADC0, &ADC_init);

     const ADC_InitSingle_TypeDef ADC_singleinit = {
        .prsSel =  adcPRSSELCh0,              /* PRS ch0 (if enabled). */
        .acqTime = adcAcqTime1,              /* Don't know what this does, needs to be 1. */
        .reference = adcRefVDD,              /* VDD internal reference. */
        .resolution = adcRes12Bit,            /* 12 bit resolution. */
        .input = adcSingleInputCh2,            /* External Channel 2 selected. */
        .diff = false,                        /* Single ended input. */
        .prsEnable = false,                   /* PRS disabled. */
        .leftAdjust = false,                  /* Right adjust. */
        .rep = true                           /* Repeat after one scan sequence. */
    }; 

    ADC_InitSingle(ADC0, &ADC_singleinit);
    
#ifndef USING_DMA_FOR_LIGHT
    CORE_CriticalDisableIrq();
        ADC0->IFC = ADC_IFC_SINGLE;
        ADC0->IEN |= ADC_IEN_SINGLE;
        NVIC_EnableIRQ(ADC0_IRQn);
    CORE_CriticalEnableIrq();
#endif

}

#ifndef USING_DMA_FOR_LIGHT
    void ADC0_IRQHandler() {
    int intFlags;
    uint8_t tempC;

    CORE_CriticalDisableIrq();
        intFlags = ADC_IntGet(ADC0);
        ADC_IntClear(ADC0, ADC_IFC_SINGLE);

       if (intFlags & ADC_IF_SINGLE) {
            adc_sample_buffer[adc_sample_count] = ADC0->SINGLEDATA;
            adc_sample_count++;
            if (adc_sample_count == ADC_NUMBER_SAMPLES) {
                adc_sample_count = 0;

                //ADC off
                ADC0->CMD = ADC_CMD_SINGLESTOP;
                unblockSleepMode(EM1);
                GPIO_PinOutClear(LES_LIGHT_EXCITE_PORT, LES_LIGHT_EXCITE_PORT_NUM);

                tempC = sunlight_tally();

                #ifdef SEND_EXTERNAL_NOTIFICATIONS
                    //enqueue temperature message
                    s_message *m = s_message_new(S_TEMP);
                    s_message_set_value(m,tempC);
                    circbuf_tiny_write(&O_Q, (uint32_t*)m);
                #endif
            }
        }

    CORE_CriticalEnableIrq();
    }
#endif

// Critical section - LED0 state
uint8_t sunlight_tally() {
uint32_t average=0;

    for (int i=0; i<ADC_NUMBER_SAMPLES; i++) {
        average+=adc_sample_buffer[i];
        if (i % 10 == 0) { PRINTSWO_UINT( adc_sample_buffer[i] ); }
    }

//PRINTSWO_UINT(average);

    average = average / ADC_NUMBER_SAMPLES;
//PRINTSWO_UINT(average);


return average;
}    
