#include "main.h"
#include "lesense.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_acmp.h"
#include "em_lesense.h"

#include "caplesense.h"

/* Most of this code lifted from the Cap sense touch demo, modified for
    a single touch event - wake up */

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

uint8_t first_scan=1;

void capSenseChTrigger(void);
static void (*lesenseChCb)(void) = capSenseChTrigger;

#ifdef PULSE_RATE_SENSOR
    void pulseChTrigger(void);
    static void (*lesensePulseCb)(void) = pulseChTrigger;
#endif

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

    //ACMP negSel
    //I think since lesenseACMPModeMuxThres is set, +Sel is set by LESENSE
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
            .startDelay     =          0U
        },

        .perCtrl          =
        { 
            .dacCh0Data     = lesenseACMPThres,      //Compare to x/63 * VDD
            .dacCh0ConvMode = lesenseDACConvModeDisable,
            .dacCh0OutMode  = lesenseDACOutModeDisable,
            .dacCh1Data     = lesenseACMPThres,
            .dacCh1ConvMode = lesenseDACConvModeDisable,
            .dacCh1OutMode  = lesenseDACOutModeDisable,
            .dacPresc       =                        0U,
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
    LESENSE_ScanFreqSet(0U, 64U);
    LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_2);

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
    for (int i=0;i<LSENSE_TOTAL_CHANNELS;i++) {
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

void capSenseChTrigger(void)
{
    led0_toggle();
}

#ifdef PULSE_RATE_SENSOR
    void pulseChTrigger(void) {
        led1_toggle();
    }
#endif

void LESENSE_IRQHandler(void)
{ 
CORE_CriticalDisableIrq();
    uint32_t count;
    uint32_t lsense_ints;

    uint16_t lsense_cap_sens;

    /* LESENSE scan complete interrupt. */
    if (LESENSE_IF_SCANCOMPLETE & LESENSE_IntGetEnabled())
    { 
        LESENSE_IntClear(LESENSE_IF_SCANCOMPLETE);

        if (first_scan) {
            //Calibrate on first scan, Only do this once
            first_scan=0;
            LESENSE_IntDisable(LESENSE_IEN_SCANCOMPLETE);

                //count = LESENSE_ScanResultDataGet();
                lsense_cap_sens = LESENSE_ScanResultDataBufferGet(0);

#ifdef PULSE_RATE_SENSOR
                count = LESENSE_ScanResultDataBufferGet(1);
                //count = LESENSE_ScanResultDataGet();
#endif

            lsense_cap_sens -=  CAPLESENSE_SENSITIVITY_OFFS;

            LESENSE_ChannelThresSet(CAP_ACMP_EXTERNAL_PIN,
                                    LESENSE_ACMP_CAP_VDD_SCALE,
                                    count);

        }
    }

    lsense_ints = LESENSE_IntGetEnabled();
    /* LESENSE channel interrupt. */
    if (CAPLESENSE_CHANNEL_INT & lsense_ints)
    { 
        /* Clear flags. */
        LESENSE_IntClear(CAPLESENSE_CHANNEL_INT);

        /* Call callback function. */
        if (lesenseChCb != 0x00000000)
        { 
            lesenseChCb();
        }
    }
#ifdef PULSE_RATE_SENSOR
    if (PULSE_CHANNEL_INT & lsense_ints)
    { 
        /* Clear flags. */
        LESENSE_IntClear(PULSE_CHANNEL_INT);

        /* Call callback function. */
        if (lesensePulseCb != 0x00000000)
        { 
            lesensePulseCb();
        }
    }
#endif

CORE_CriticalEnableIrq();
}
