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

void capSenseScanComplete(void);
void capSenseChTrigger(void);

static void (*lesenseScanCb)(void) = capSenseScanComplete;
static void (*lesenseChCb)(void) = capSenseChTrigger;

void LESENSE_First() {
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_HFLE, true);
    CMU_ClockEnable(cmuClock_LESENSE, true);
    CMU_ClockDivSet(cmuClock_LESENSE, cmuClkDiv_1);

    //Port C1, input
    GPIO_PinModeSet(CAP_ACMP_EXTERNAL_PORT, CAP_ACMP_EXTERNAL_PIN, gpioModeInputPull, 0);

    //Use ACMP0 for external custom touch
    CMU_ClockEnable(cmuClock_ACMP0, true);

    //Disable ACMP out to a pin
    ACMP_GPIOSetup(ACMP0, 0, false, false);

    /* ACMP capsense configuration constant table. */
    //static const ACMP_CapsenseInit_TypeDef initACMP =
    static const ACMP_Init_TypeDef initACMP =
    {
        .fullBias                 = false,
        .halfBias                 = false,
        .biasProg                 =                  0x7,
        .warmTime                 = acmpWarmTime512,
        .hysteresisLevel          = acmpHysteresisLevel7,
        //.resistor                 = acmpResistor0,
        .lowPowerReferenceEnabled = false,
        .vddLevel                 =                 0x3D,
        .enable                   = false
    };

    //ACMP_CapsenseInit(ACMP1, &initACMP);
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
            .storeScanRes = false,
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
    LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_1);

    //Scan pins, configure
    static const LESENSE_ChAll_TypeDef initChsSense = {
        {                                                    
          LESENSE_DISABLED_CH_CONF,        /* Channel 0. */  
          LESENSE_CAPSENSE_CH_CONF_SLEEP,  /* Channel 1.  PortC, 1*/  
          LESENSE_DISABLED_CH_CONF,        /* Channel 2. */  
          LESENSE_DISABLED_CH_CONF,        /* Channel 3. */  
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
    LESENSE_ScanStart();
}

void LESENSE_Calibrate() {
uint16_t capsenseCalibrateVal=0;

    while (!(LESENSE->STATUS & LESENSE_STATUS_BUFHALFFULL)); 

    capsenseCalibrateVal = LESENSE_ScanResultDataBufferGet(LESENSE_CHANNEL_INPUT);
    //capsenseCalibrateVal = LESENSE_ScanResultDataBufferGet(LESENSE_CHANNEL_INPUT) -
    //                                  CAPLESENSE_SENSITIVITY_OFFS;

#if 1
    LESENSE_ChannelThresSet(CAP_ACMP_EXTERNAL_PIN,
                            LESENSE_ACMP_VDD_SCALE,
                            capsenseCalibrateVal);
#endif
}

void capSenseScanComplete(void) {
    //led0_toggle();
};

void capSenseChTrigger(void)
{
    led1_toggle();
}

void LESENSE_IRQHandler(void)
{ 
CORE_CriticalDisableIrq();
    uint32_t count;

    /* LESENSE scan complete interrupt. */
    if (LESENSE_IF_SCANCOMPLETE & LESENSE_IntGetEnabled())
    { 
        LESENSE_IntClear(LESENSE_IF_SCANCOMPLETE);


        /* Read out value from LESENSE buffer */
        count = LESENSE_ScanResultDataGet();

        /* Call callback function. */
        if (lesenseScanCb != 0x00000000)
        { 
            lesenseScanCb();
        }
    }

    /* LESENSE channel interrupt. */
    if (CAPLESENSE_CHANNEL_INT & LESENSE_IntGetEnabled())
    { 
        /* Clear flags. */
        LESENSE_IntClear(CAPLESENSE_CHANNEL_INT);

        /* Call callback function. */
        if (lesenseChCb != 0x00000000)
        { 
            lesenseChCb();
        }
    }
CORE_CriticalEnableIrq();
}
