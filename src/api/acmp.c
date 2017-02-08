#include "acmp.h"
#include "periph.h"
#include "em_acmp.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"

void ACMP_fire_up(uint8_t vdd) {
    CMU_ClockEnable(cmuClock_ACMP0, true);

    const ACMP_Init_TypeDef ACMP_init = {
        .fullBias               = false,                /* fullBias */
        .halfBias               = true,                 /* halfBias */
        .biasProg               = 0x0,                  /* biasProg */
        .interruptOnFallingEdge = true,                 /* No interrupt on falling edge. */
        .interruptOnRisingEdge  = false,                /* No interrupt on rising edge. */
        .warmTime               = acmpWarmTime512,      /* 512 cycle warmup to be safe */
        .hysteresisLevel        = acmpHysteresisLevel7, /* Not sure this matters for our app */
        .inactiveValue          = false,                /* Disabled emitting inactive value during warmup. */
        .lowPowerReferenceEnabled = true,               /* Saves 2-3 uA!! low power reference */
        .vddLevel               = vdd,                  /* VDD level */
        .enable                 = false                 /* Do not enable after init. */
    };
    ACMP_Init(ACMP0, &ACMP_init);

    // Select Negative Input scaled VDD, positive channel6 = LES_LIGHT_SENSE
    ACMP_ChannelSet(ACMP0, acmpChannelVDD, acmpChannel6);
    ACMP_Enable(ACMP0);

#if 0
CORE_CriticalDisableIrq();
    ACMP0->IFC   = ACMP_IFC_EDGE;
    ACMP0->IFC   = ACMP_IFC_WARMUP;
    ACMP0->IEN   |= ACMP_IEN_EDGE;
    ACMP0->IEN   |= ACMP_IEN_WARMUP;
    NVIC_EnableIRQ(ACMP0_IRQn);
CORE_CriticalEnableIrq();
#endif

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
