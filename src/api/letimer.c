#include "main.h"
#include "letimer.h"
#include "sleep.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_letimer.h"
#include "em_core.h"
#include "em_acmp.h"
#ifdef USING_DMA_FOR_TEMP
    #include "adc.h"
    #include "dma.h"
    #include "em_dma.h"
#endif
#include "periph.h"

#ifdef INTERNAL_LIGHT_SENSOR
    #include "acmp.h"
#else
    #include "light_sensor_ext.h"
#endif

//Maybee combine functionality?
#include "main.h"
#include "leuart.h"
#include "../../atmel/src/s_message.h"
#include "circbuf_tiny.h"
#include "em_leuart.h"
#include <stdlib.h> //free

#include "debug.h"

uint8_t gpio_global_enabled=0;

uint16_t le_send_message_ticks;
uint16_t le_regular_on_ticks;
static uint8_t le_is_message_send_interrupt=0;

#ifndef INTERNAL_LIGHT_SENSOR
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

#ifndef INTERNAL_LIGHT_SENSOR
    //External light sensor will run in 3 frames (periods) in a row
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

#ifndef INTERNAL_LIGHT_SENSOR
        letimer_frame++;
        if (letimer_frame == 1 ) {
            CMU_ClockEnable(cmuClock_GPIO, true); gpio_global_enabled = GPIO_ON;
        }
#endif

        /* Temperature section */

        // DMA setup
#ifdef USING_DMA_FOR_TEMP
        DMA_ActivateBasic(DMA_CHANNEL_FOR_ADC, true, false, &adc_sample_buffer,(void*)&(ADC0->SINGLEDATA), ADC_NUMBER_SAMPLES-1);
#endif
        // ADC on
        blockSleepMode(EM1);
        ADC0->CMD = ADC_CMD_SINGLESTART;

        /* Light indicator section */
#ifdef INTERNAL_LIGHT_SENSOR
        CMU_ClockEnable(cmuClock_GPIO, true); gpio_global_enabled = GPIO_ON;
        GPIO_PinOutSet(LES_LIGHT_EXCITE_PORT, LES_LIGHT_EXCITE_PORT_NUM);
        if (is_led0_on()) {
            ACMP_fire_up(VDD_LIGHTNESS);
        } else {
            ACMP_fire_up(VDD_DARKNESS);
        }
        while ((ACMP0->STATUS & ACMP_STATUS_ACMPACT) == 0);
#else // External Light Sensor

        //There is a weird bug where this doesn't happen until the 3rd LETIMER pop
        // after a cold boot. Somehow 2 timers expire before we enter here...
        switch (letimer_frame) {
            case 1:
                light_sensor_power_on();
                break;
        }
#endif
    }

    // Second part of sequence
    if (intFlags & LETIMER_IFS_COMP1) {
        if (le_is_message_send_interrupt == 0) {


#ifdef INTERNAL_LIGHT_SENSOR
            if (is_led0_on()) {
                if (ACMP0->STATUS & ACMP_STATUS_ACMPOUT) {
                    led0_off();

                    #ifdef SEND_EXTERNAL_NOTIFICATIONS
                        // enqueue led off message
                        m = s_message_new(S_LED_OFF);
                        circbuf_tiny_write(&O_Q, (uint32_t*)m);
                    #endif

                }
            } else {
                if ((ACMP0->STATUS & ACMP_STATUS_ACMPOUT) == 0) {
                    led0_on();

                    #ifdef SEND_EXTERNAL_NOTIFICATIONS
                        // enqueue led on message
                        m = s_message_new(S_LED_ON);
                        circbuf_tiny_write(&O_Q, (uint32_t*)m);
                    #endif
                }
            }
            ACMP_Disable(ACMP0);
            GPIO_PinOutClear(LES_LIGHT_EXCITE_PORT, LES_LIGHT_EXCITE_PORT_NUM);
#else // External Light Sensor

            switch (letimer_frame) {
                case 1:
                    light_sensor_program();
                    break;
                case 3:
                    light_sensor_power_off();
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
                    // Process all pending outgoing message Q
                    while (circbuf_tiny_read(&O_Q,(uint32_t**)&m)) {
                        if (m) {

                            //Since this is a complicated state machine,
                            // keep track of gpio enabled, and temporarily
                            // trun on/off if they are currently off
                            if (gpio_global_enabled == GPIO_OFF) {
                                CMU_ClockEnable(cmuClock_GPIO, true);
                            }
                            LEUART0_enable();
                            leuart0_tx_string(m->message);
                            LEUART0_disable();
                            if (gpio_global_enabled == GPIO_OFF) {
                                CMU_ClockEnable(cmuClock_GPIO, false);
                            }

                            free(m);
                        }
                    }
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

        if (le_is_message_send_interrupt == 0) {
#ifdef INTERNAL_LIGHT_SENSOR
            CMU_ClockEnable(cmuClock_GPIO, false); gpio_global_enabled = GPIO_OFF;
#else // External Light Sensor

            switch (letimer_frame) {
                case 3:
                    CMU_ClockEnable(cmuClock_GPIO, false); gpio_global_enabled = GPIO_OFF;
                    letimer_frame=0;
                break;
            }
#endif
        }

    }

CORE_CriticalEnableIrq();
}
