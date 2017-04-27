#include "main.h"
#include "lesense.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_acmp.h"
#include "em_lesense.h"
#include "rtc.h"
#include "em_rtc.h"
#include "periph.h"

#ifdef LCD_MESSAGES
    #include "letimer.h"
    #include "segmentlcd.h"
#endif

#include "caplesense.h"

#ifdef SEND_EXTERNAL_NOTIFICATIONS
    #include "../../atmel/src/s_message.h"
    #include "circbuf_tiny.h"
#endif

/* Most of the initialization code for LESENSE/ACMP lifted from the Cap sense
   touch demo, modified for
    a single touch event, pulse sensor - wake up */

/******************************************************************************
 * License
 * Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

static uint32_t channel_results[LESENSE_TOTAL_CHANNELS] = {0};

static uint8_t pulse_duration_count=0;
static uint8_t pulse_last_time=0;

void LESENSE_First() {
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_HFLE, true);
    CMU_ClockEnable(cmuClock_LESENSE, true);
    CMU_ClockDivSet(cmuClock_LESENSE, cmuClkDiv_1);

    //ACMP0 - external capacitance sensor, and pulse sensor

    //Port C1, input external capacitance sensor
    GPIO_PinModeSet(CAP_ACMP_EXTERNAL_PORT, CAP_ACMP_EXTERNAL_PIN, gpioModeInputPull, 0);

#ifdef PULSE_RATE_SENSOR
    //Port C3, input external pulse sensor
    GPIO_PinModeSet(PULSE_ACMP_EXTERNAL_PORT, PULSE_ACMP_EXTERNAL_PIN, gpioModeInputPull, 0);

#if 0
    #ifdef PULSE_RATE_EXT_POWER
        GPIO_PinModeSet(PULSE_POWER_EXTERNAL_PORT, PULSE_POWER_EXTERNAL_PIN, gpioModePushPullDrive, 1);
        //Start with power on, otherwise glitch in capacitive touch sensor
        PULSE_POWER_ON;
    #endif
#endif

#endif

    //Use ACMP0
    CMU_ClockEnable(cmuClock_ACMP0, true);

    //Disable ACMP0 out to a pin
    ACMP_GPIOSetup(ACMP0, 0, false, false);

    /* ACMP configuration for external capacitance sensor. */
    static const ACMP_Init_TypeDef initACMP =
    {
        .fullBias                 = false,
        .halfBias                 = false,
        .biasProg                 = 0x7,
        .warmTime                 = acmpWarmTime512,
        .hysteresisLevel          = acmpHysteresisLevel7,
        .lowPowerReferenceEnabled = false,
        .vddLevel                 = 0x3D,
        .enable                   = false
    };

    ACMP_Init(ACMP0, &initACMP);

    //ACMP negSel, since lesenseACMPModeMuxThres is set, posSel is set by LESENSE
    ACMP_ChannelSet(ACMP0, acmpChannelVDD, acmpChannel1);

    //Actual LESENSE set
    static const LESENSE_Init_TypeDef  initLESENSE =
    { 
        .coreCtrl         =
        { 
            .scanStart    = lesenseScanStartPeriodic,
            .prsSel       = lesensePRSCh0,
            .scanConfSel  = lesenseScanConfDirMap,
            .invACMP0     = false,
            .invACMP1     = false,
            .dualSample   = false,
            .storeScanRes = true,
            .bufOverWr    = true,
            .bufTrigLevel = lesenseBufTrigHalf,
            .wakeupOnDMA  = lesenseDMAWakeUpDisable,
            .biasMode     = lesenseBiasModeDutyCycle,
            .debugRun     = false
        },

        .timeCtrl         =
        { 
            .startDelay   = 0U
        },

        .perCtrl          =
        { 
            .dacCh0Data     = lesenseACMPThres,      //Compare to x/63 * VDD
            .dacCh0ConvMode = lesenseDACConvModeDisable,
            .dacCh0OutMode  = lesenseDACOutModeDisable,
            .dacCh1Data     = lesenseACMPThres,
            .dacCh1ConvMode = lesenseDACConvModeDisable,
            .dacCh1OutMode  = lesenseDACOutModeDisable,
            .dacPresc       = 0U,
            .dacRef         = lesenseDACRefBandGap,
            .acmp0Mode      = lesenseACMPModeMuxThres, //Using ACMP0
            .acmp1Mode      = lesenseACMPModeDisable,
            .warmupMode     = lesenseWarmupModeNormal
        },

        .decCtrl          =
        { 
            .decInput  = lesenseDecInputSensorSt,
            .chkState  = false,
            .intMap    = true,
            .hystPRS0  = false,
            .hystPRS1  = false,
            .hystPRS2  = false,
            .hystIRQ   = false,
            .prsCount  = true,
            .prsChSel0 = lesensePRSCh0,
            .prsChSel1 = lesensePRSCh1,
            .prsChSel2 = lesensePRSCh2,
            .prsChSel3 = lesensePRSCh3
        }
    };

    LESENSE_Init(&initLESENSE, true);
}
    

//Make sure GPIO clock is enabled
void LESENSE_Setup() {

    LESENSE_ScanStop();
    while (LESENSE_STATUS_SCANACTIVE & LESENSE_StatusGet());
    LESENSE_IntClear(LESENSE_IEN_SCANCOMPLETE);
    LESENSE_ResultBufferClear();
    LESENSE_ScanFreqSet(0U, LESENSE_SCAN_PERIOD);
    LESENSE_ClkDivSet(lesenseClkLF, LESENSE_CLK_DIV);

    //Scan pins, configure
    static const LESENSE_ChAll_TypeDef initChsSense = {
        {                                                    
          LESENSE_DISABLED_CH_CONF,        /* Channel 0. */  
          LESENSE_CAPSENSE_CH_CONF_SLEEP,  /* Channel 1.  PortC, 1*/  
          LESENSE_DISABLED_CH_CONF,        /* Channel 2. */  
#ifdef PULSE_RATE_SENSOR
          LESENSE_PULSE_CH_CONF_SLEEP,     /* Channel 3. */  
#else
          LESENSE_DISABLED_CH_CONF,        /* Channel 3. */  
#endif
          LESENSE_DISABLED_CH_CONF,        /* Channel 4. */  
          LESENSE_DISABLED_CH_CONF,        /* Channel 5. */  
          LESENSE_DISABLED_CH_CONF,        /* Channel 6. */  
          LESENSE_DISABLED_CH_CONF,        /* Channel 7. */  
          LESENSE_DISABLED_CH_CONF,        /* Channel 8. ACMP1_CH0 */
          LESENSE_DISABLED_CH_CONF,        /* Channel 9. */  
          LESENSE_DISABLED_CH_CONF,        /* Channel 10. */ 
          LESENSE_DISABLED_CH_CONF,        /* Channel 11. */ 
          LESENSE_DISABLED_CH_CONF,        /* Channel 12. */ 
          LESENSE_DISABLED_CH_CONF,        /* Channel 13. */ 
          LESENSE_DISABLED_CH_CONF,        /* Channel 14. */ 
          LESENSE_DISABLED_CH_CONF         /* Channel 15. */ 
        }                                                        
    };

    //This should set up channel interrupts
    LESENSE_ChannelAllConfig(&initChsSense);

#if 0
CORE_CriticalDisableIrq();
    LESENSE_IntClear(LESENSE_IEN_SCANCOMPLETE);
    LESENSE_IntEnable(LESENSE_IEN_SCANCOMPLETE);
CORE_CriticalEnableIrq();
#endif

    NVIC_EnableIRQ(LESENSE_IRQn);

#if 0
    for (int i=0;i<LESENSE_TOTAL_CHANNELS;i++) {
    LESENSE->BUF[i].DATA=0xff+i;
    }
    LESENSE_ResultBufferClear();
#endif
    LESENSE_ScanStart();
}

void LESENSE_Calibrate() {
uint16_t capsenseCalibrateVal=0;

#if 1
    while (!(LESENSE->STATUS & LESENSE_STATUS_BUFHALFFULL)); 

    capsenseCalibrateVal = LESENSE_ScanResultDataBufferGet(0) -
                                      CAPLESENSE_SENSITIVITY_OFFS;

    LESENSE_ChannelThresSet(CAP_ACMP_EXTERNAL_PIN,
                            LESENSE_ACMP_CAP_VDD_SCALE,
                            capsenseCalibrateVal);

    //Dummy read to keep buffers aligned
    (void)LESENSE_ScanResultDataBufferGet(1);
#endif
}

void LESENSE_IRQHandler(void)
{ 
CORE_CriticalDisableIrq();
    uint32_t count;
    uint32_t lsense_ints;
    uint32_t pulse_read_time;

    uint16_t lsense_cap_sens;

    /* LESENSE scan complete interrupt. */
    if (LESENSE_IF_SCANCOMPLETE & LESENSE_IntGetEnabled())
    { 
        LESENSE_IntClear(LESENSE_IF_SCANCOMPLETE);

        for (int i=0; i<LESENSE_TOTAL_CHANNELS;i++) {
            channel_results[i] = LESENSE_ScanResultDataGet();
        }

          if (channel_results[1]) {
                pulse_duration_count++;

                /* Unless the high signal from the pulse sensor is
                    38ms, don't count it as a pulse

                              PULSE_DURATION_COUNT = 5
                              PULSE_SAMPLE_LENGTH  = 127

                    (1 / (32768/2) * 127) = 7.75mS * 5 = 38.76mS
                */             
                #ifdef LCD_MESSAGES
                    SegmentLCD_ARing(pulse_duration_count & 7,1);
                #endif

                if (pulse_duration_count == PULSE_DURATION_COUNT) {
                    pulse_read_time=RTC->CNT;

                    /* Skip measuring if this is the first consecutive beat
                        within 2 second RTC window, in which case the stopped
                        timer will read RTC->CNT=0

                        Also, if the RTC timer has been started, skip if the
                        measurement is out of the range of a human heartbeat.
                        This will filter out some noise from the sensor
                    */
                    if ( pulse_read_time > FASTEST_HEARTBEAT && 
                        pulse_read_time < SLOWEST_HEARTBEAT) {
                        pulse_measure= RTC_TICKS_PER_SECOND * 60 / pulse_read_time;    

                        /* One last check, if the pulse varied by more than
                            PULSE_MAX_VARIANCE, then throw out the result
                        */

                        if ( pulse_measure < (pulse_last_time + PULSE_MAX_VARIANCE) &&
                             pulse_measure > (pulse_last_time - PULSE_MAX_VARIANCE)) {
                            /*
                                For those playing along at home, it takes three
                                successful pulses to get into here
                            */
                            #ifdef LCD_MESSAGES
                                lcd_keep_on=2;
                                    SegmentLCD_Number(pulse_measure);
                                    SegmentLCD_EnergyMode(0,1);
                            #endif

                            pulse_count_buffer[pulse_count_buffer_idx]=pulse_measure;
                            pulse_count_buffer_idx++;
                            if (pulse_count_buffer_idx == PULSE_COUNT_BUFFER_SIZE) {
                                pulse_count_buffer_idx=0;
                            }
                        }
                        pulse_last_time = pulse_measure;

                    }
                    //End of pulse - start timer
                    RTC_Enable(false);
                    RTC_Enable(true);
                }
          } else {
                    #ifdef LCD_MESSAGES
                        for (int j=0;j<8;j++) {
                            SegmentLCD_ARing(j,0);
                        }
                        SegmentLCD_EnergyMode(0,0);
                    #endif
                pulse_duration_count=0;
                LESENSE_IntDisable(LESENSE_IEN_SCANCOMPLETE);
         }

    }

    lsense_ints = LESENSE_IntGetEnabled();
    /* LESENSE channel interrupt. */
    if (CAPLESENSE_CHANNEL_INT & lsense_ints)
    { 
        /* Clear flags. */
        LESENSE_IntClear(CAPLESENSE_CHANNEL_INT);

        //If capacitive pad touched, turn on Heart Rate sensor LEDs
        #ifdef PULSE_RATE_EXT_POWER
            PULSE_POWER_ON;
        #endif

        #ifdef LCD_MESSAGES
                SegmentLCD_Symbol(LCD_SYMBOL_ANT,1);
        #endif
    }

#ifdef PULSE_RATE_SENSOR
    if (PULSE_CHANNEL_INT & lsense_ints)
    { 
        /* Clear flags. */
        LESENSE_IntClear(PULSE_CHANNEL_INT);
        LESENSE_IntEnable(LESENSE_IEN_SCANCOMPLETE);
    }
#endif

CORE_CriticalEnableIrq();
}
