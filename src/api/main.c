#include "em_device.h"
#include "em_chip.h"
#include "periph.h"
#include "sleep.h"
#include "main.h"
#include "timers.h"
#include "letimer.h"
#include "debug.h"

    #include "em_acmp.h"
    #include "em_cmu.h"
    extern acmp0_warm = 0;

#include "bsp_trace.h"

int main(void)
{
  /* Chip errata */
  CHIP_Init();


// Note: this also turns on GPIO clock 
BSP_TraceProfilerSetup();

setupSWOForPrint();
PRINTSWO_UINT(87654321);

clock_defaults();

   LED0_setup();
   LED1_setup();
//   blockSleepMode(EM1);

    uint16_t ulfrco_ticks = calibrate_ULFRCO_ticks();
    PRINTSWO_UINT(ulfrco_ticks);
    PRINTSWO_UINT(12345678);


    //CMU_ClockSelectSet(cmuClock_ACMP0, cmuSelect_HFRCO);
    #define VDD_DARKNESS 2 
    CMU_ClockEnable(cmuClock_ACMP0, true);
    CMU_ClockEnable(cmuClock_GPIO, true);

    const ACMP_Init_TypeDef ACMP_init = {
        .fullBias               = true,                 /* fullBias */
        .halfBias               = false,                /* halfBias */
        .biasProg               = 0x7,                  /* biasProg */
        .interruptOnFallingEdge = true,                 /* No interrupt on falling edge. */
        .interruptOnRisingEdge  = false,                /* No interrupt on rising edge. */
        .warmTime               = acmpWarmTime512,      /* 512 cycle warmup to be safe */
        .hysteresisLevel        = acmpHysteresisLevel0,
        .inactiveValue          = false,                /* Disabled emitting inactive value during warmup. */
        .lowPowerReferenceEnabled = false,              /* low power reference */
        .vddLevel               = VDD_DARKNESS,               /* VDD level */ 
        .enable                 = false                  /* Enable after init. */ 
    };
    ACMP_Init(ACMP0, &ACMP_init);


    // Select Negative Input scaled VDD, positive channel6 = LES_LIGHT_SENSE
    ACMP_ChannelSet(ACMP0, acmpChannelVDD, acmpChannel6); 

    //LES_LIGHT_SENSE
    GPIO_PinModeSet(gpioPortC, 6, gpioModeDisabled, 0); //Pull up down??

    //Turn on LES_LIGHT_EXCITE
    GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
    GPIO_PinOutSet(gpioPortD, 6);
    wait_m_sec(4);

CORE_CriticalDisableIrq();
    ACMP0->IFC   = ACMP_IFC_EDGE;
    ACMP0->IFC   = ACMP_IFC_WARMUP;
    ACMP0->IEN   |= ACMP_IEN_EDGE;
    ACMP0->IEN   |= ACMP_IEN_WARMUP;
    NVIC_EnableIRQ(ACMP0_IRQn);
CORE_CriticalEnableIrq();

    ACMP_Enable(ACMP0);

#if 0
    while(1) {
        if (ACMP0->STATUS & ACMP_STATUS_ACMPOUT) {
            led0_on();
        } else {
            led0_off();
        }
    }
#endif
        

LETIMER0_setup(EM3);
    
  /* Infinite loop */
  while (1) {
//    sleep();
  }
}

void ACMP0_IRQHandler() {
int intFlags;

CORE_CriticalDisableIrq();
    intFlags = ACMP_IntGet(ACMP0);
    ACMP_IntClear(ACMP0, ACMP_IFC_EDGE);
    ACMP_IntClear(ACMP0, ACMP_IFC_WARMUP);

    if (intFlags & ACMP_IF_EDGE) {
        if (acmp0_warm) {
        led1_toggle();
        }
    }
    if (intFlags & ACMP_IF_WARMUP) {
        acmp0_warm=1;
        led1_on();
    }

CORE_CriticalEnableIrq();
}
