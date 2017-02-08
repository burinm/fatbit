#include "acmp.h"
#include "periph.h"
#include "em_acmp.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"

//extern int acmp0_warm = 0;


void ACMP_fire_up(uint8_t vdd) {
    //CMU_ClockSelectSet(cmuClock_ACMP0, cmuSelect_HFRCO);
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
        .vddLevel               = vdd,                  /* VDD level */
        .enable                 = false                 /* Do not enable after init. */
    };
    ACMP_Init(ACMP0, &ACMP_init);


    // Select Negative Input scaled VDD, positive channel6 = LES_LIGHT_SENSE
    ACMP_ChannelSet(ACMP0, acmpChannelVDD, acmpChannel6);

    //LES_LIGHT_SENSE
    GPIO_PinModeSet(gpioPortC, 6, gpioModeDisabled, 0); //Pull up down??

    //Turn on LES_LIGHT_EXCITE
    GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
    GPIO_PinOutSet(gpioPortD, 6);
    //wait_m_sec(4);

#if 0
CORE_CriticalDisableIrq();
    ACMP0->IFC   = ACMP_IFC_EDGE;
    ACMP0->IFC   = ACMP_IFC_WARMUP;
    ACMP0->IEN   |= ACMP_IEN_EDGE;
    ACMP0->IEN   |= ACMP_IEN_WARMUP;
    NVIC_EnableIRQ(ACMP0_IRQn);
CORE_CriticalEnableIrq();
#endif

    ACMP_Enable(ACMP0);
}

#if 0
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
#endif

