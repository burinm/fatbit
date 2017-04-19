#include "main.h"
#include "letimer.h"
#include "sleep.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_letimer.h"
#include "em_core.h"
#include "em_acmp.h"

#if defined USING_DMA_FOR_LIGHT || defined USING_DMA_FOR_LEUART
    #include "dma.h"
    #include "em_dma.h"
#endif

#ifdef USING_DMA_FOR_LIGHT
    #include "adc.h"
#endif

#ifdef USING_DMA_FOR_LEUART
    #include <string.h>
#endif

#include "periph.h"

#ifdef INTERNAL_LIGHT_SENSOR
    #include "acmp.h"
#endif

//Maybee combine functionality?
#include "main.h"
#include "leuart.h"
#include "../../atmel/src/s_message.h"
#include "circbuf_tiny.h"
#include "em_leuart.h"
#include <stdlib.h> //free

#include "debug.h"

uint16_t le_send_message_ticks;
uint16_t le_regular_on_ticks;
static uint8_t le_is_message_send_interrupt=0;

#ifdef THREE_PART_TIMER
    static uint8_t letimer_frame=0;
#endif


void LETIMER0_calc_le_ticks(e_emode e, double le_period_seconds, double le_on_seconds,
                            uint16_t *le_comp0, uint16_t *le_comp1) {
    // Ticks calculations

    // LXFO Timings
    uint16_t le_lfxo_ticks_second = LETIMER_LFXO_TICK_S / (LE_DIVIDER ? LE_DIVIDER:1); //Divider on/off

    uint16_t le_comp0_em2 = le_period_seconds * le_lfxo_ticks_second; 
    uint16_t le_comp1_em2 = le_comp0_em2 - (le_on_seconds * le_lfxo_ticks_second); 

    // ULFRCO setup, oscillator ticks are milliseconds
    uint16_t le_ulfrco_ticks_second = ulfrco_ticks;
    uint16_t le_comp0_em3 = le_period_seconds * le_ulfrco_ticks_second;
    uint16_t le_comp1_em3 = le_comp0_em3 - (le_on_seconds * le_ulfrco_ticks_second);

    PRINTSWO_UINT(77777);
    PRINTSWO_UINT(le_comp0_em3);
    PRINTSWO_UINT(le_comp1_em3);
    PRINTSWO_UINT(77777);

    //End ticks calc

    // LETIMER COMP0 will be used for period, COMP1 for duration
    if (e < EM3) {
        *le_comp0 = le_comp0_em2;
        *le_comp1 = le_comp1_em2;
    } else {
        *le_comp0 = le_comp0_em3;
        *le_comp1 = le_comp1_em3;
    }
}

void LETIMER0_setup(e_emode e, uint16_t le_comp0, uint16_t le_comp1) {

    // LETIMER jams up after use in ULFRCO and one-shot mode, hard reset
    LETIMER_Reset(LETIMER0);

    blockSleepMode(e);

    if (e < EM3) {
        CMU_OscillatorEnable(cmuOsc_LFXO,true,false);
        CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    } else {
        CMU_OscillatorEnable(cmuOsc_ULFRCO,true,false);
        CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
    }

    CMU_ClockEnable(cmuClock_HFLE, true);
    CMU_ClockEnable(cmuClock_LETIMER0, true);

    LETIMER_CompareSet(LETIMER0, 0, le_comp0);
    LETIMER_CompareSet(LETIMER0, 1, le_comp1);

    if (e < EM3) {
        if (LE_DIVIDER == 2) { // Scaling needed for periods 2 seconds or greater
            CMU->LFAPRESC0 |=
          ((_CMU_LFAPRESC0_LETIMER0_DIV2 << _CMU_LFAPRESC0_LETIMER0_SHIFT) & _CMU_LFAPRESC0_MASK);
        }
        if (LE_DIVIDER == 4) { // Scaling needed for periods 4 seconds or greater
            CMU->LFAPRESC0 |=
          ((_CMU_LFAPRESC0_LETIMER0_DIV4 << _CMU_LFAPRESC0_LETIMER0_SHIFT) & _CMU_LFAPRESC0_MASK);
        }
    }

    // Defaults modified from Silicon AN0026SW examples
    const LETIMER_Init_TypeDef letimerInit =
    {
        .enable         = false,                // Do not start counting when init completed
        .debugRun       = false,                // Do not stop counter during debug halt
        .comp0Top       = true,                 // Load COMP0 into CNT on underflow
        .bufTop         = false,                // Do not load COMP1 into COMP0 when REP0 reaches 0
        .out0Pol        = 0,                    // Idle value 0 for output 0
        .out1Pol        = 0,                    // Idle value 0 for output 1
        .ufoa0          = letimerUFOANone,      // No action on underflow on output 0
        .ufoa1          = letimerUFOANone,      // No action on underflow on output 1
        .repMode        = letimerRepeatFree     // Count until stopped by SW
    };

    // Initialize LETIMER0
    LETIMER_Init(LETIMER0, &letimerInit);

#ifdef THREE_PART_TIMER
    /* Accelerometer will run in 3 frames (periods) in a row
        1) Power on, program
        2,3) Detect motion
        3) Power off
    */ 
    letimer_frame=0;
#endif

    // Setup LETIMER interrupts
    CORE_CriticalDisableIrq();
    LETIMER0->IFC   = LETIMER_IFC_COMP0;
    LETIMER0->IFC   = LETIMER_IFC_COMP1;
    LETIMER0->IEN   |= LETIMER_IEN_COMP0;
    LETIMER0->IEN   |= LETIMER_IEN_COMP1;
    NVIC_EnableIRQ(LETIMER0_IRQn);
    CORE_CriticalEnableIrq();

    LETIMER_Enable(LETIMER0, true);
}

void LETIMER0_IRQHandler() {
int intFlags;

CORE_CriticalDisableIrq();

    #ifdef SEND_EXTERNAL_NOTIFICATIONS
        // outgoing queue handle
        s_message *m=NULL;
    #endif

    intFlags = LETIMER_IntGet(LETIMER0);
    LETIMER_IntClear(LETIMER0,LETIMER_IFS_COMP0);
    LETIMER_IntClear(LETIMER0,LETIMER_IFS_COMP1);

    // First part of sequence
    if (intFlags & LETIMER_IFS_COMP0) {

        /* Light indicator section */
#ifdef INTERNAL_LIGHT_SENSOR
        GPIO_PinOutSet(LES_LIGHT_EXCITE_PORT, LES_LIGHT_EXCITE_PORT_NUM);
#endif

#ifdef PULSE_RATE_SENSOR
        /*
            If not using LESENSE, turn on ACMP here
            ACMP_fire_up();
            while ((ACMP0->STATUS & ACMP_STATUS_ACMPACT) == 0);
        */
#endif

#ifdef THREE_PART_TIMER
        letimer_frame++;
        switch (letimer_frame) {
            case 1:
                //Power on peripheral 
                break;
        }
#endif
    }

    // Second part of sequence
    if (intFlags & LETIMER_IFS_COMP1) {
        if (le_is_message_send_interrupt == 0) {

        /* Light indicator section */
#ifdef INTERNAL_LIGHT_SENSOR

        // DMA setup
#ifdef USING_DMA_FOR_LIGHT
        DMA_ActivateBasic(DMA_CHANNEL_FOR_ADC, true, false, &adc_sample_buffer,(void*)&(ADC0->SINGLEDATA), ADC_NUMBER_SAMPLES-1);
#endif
        // ADC on
        blockSleepMode(EM1);
        ADC0->CMD = ADC_CMD_SINGLESTART;

        //Remember to turn off EXCITE Pin in respective handlers
        //GPIO_PinOutClear(LES_LIGHT_EXCITE_PORT, LES_LIGHT_EXCITE_PORT_NUM);
#endif

#ifdef PULSE_RATE_SENSOR
            /*
            if (ACMP0->STATUS & ACMP_STATUS_ACMPOUT) {
                //Do something
            }
            ACMP_Disable(ACMP0);
           */ 
#endif

#ifdef THREE_PART_TIMER
            switch (letimer_frame) {
                case 1:
                    //Program peripheral 
                    break;
                case 3:
                    //Power off peripheral 
                    break;
            }
#endif
        }
            #ifdef SEND_EXTERNAL_NOTIFICATIONS

               // If we are in EM2 or it is the extra timer pop,
               //   which only gets setup in EM3, then
               //   send message. 
               if ((LOWEST_POWER_MODE == EM2) ||
                    (le_is_message_send_interrupt == 1)) {
                    #ifdef USING_DMA_FOR_LEUART
                        //If LEUART DMA channel is free, queue up a message
                        //if ((DMA->CHREQSTATUS & DMA_CHANNEL_FOR_LEUART_WAIT_STATUS ) == 0) {
                        if ((DMA->CHENS & DMA_CHENS_CH1ENS ) == 0) { //TODO: #define for this
                            if (circbuf_tiny_read(&O_Q,(uint32_t**)&m)) {
                                if (m) {
                                    memcpy(&leuart_message_buffer, m->message, SOURCE_MESSAGE_LENGTH);
                                    free(m);

                                    LEUART0_enable();

                                    //Wakeup from EM2 when byte is ready
                                    LEUART0->CTRL |= LEUART_CTRL_TXDMAWU;

                                    //Let DMA transfer bytes to LEUART
                                    DMA_ActivateBasic(DMA_CHANNEL_FOR_LEUART, true, false,
                                    (void*)&(LEUART0->TXDATA),
                                    &leuart_message_buffer,
                                    SOURCE_MESSAGE_LENGTH-1);
                                }
                            } 
                        }
                    #else
                        // Process all pending outgoing message Q
                        while (circbuf_tiny_read(&O_Q,(uint32_t**)&m)) {
                            if (m) {
                                LEUART0_enable();
                                leuart0_tx_string(m->message);
                                LEUART0_disable();
                                free(m);
                            }
                        }
                    #endif
                }

               if (LOWEST_POWER_MODE == EM3 ) {
                   // If we are in EM3, setup another timer pop
                   if (le_is_message_send_interrupt == 0) {
                       LETIMER_CompareSet(LETIMER0, 1, le_send_message_ticks);
                       le_is_message_send_interrupt = 1;
                       blockSleepMode(EM2);
                   } else { // Extra timer pop, restore COMP1 values
                       LETIMER_CompareSet(LETIMER0, 1, le_regular_on_ticks);
                       le_is_message_send_interrupt = 0;
                       unblockSleepMode(EM2);
                   }
               }
            #endif

#ifdef THREE_PART_TIMER
        if (le_is_message_send_interrupt == 0) {

            switch (letimer_frame) {
                case 3:
                    letimer_frame=0;
                break;
            }
        }
#endif

    }

CORE_CriticalEnableIrq();
}
