#include "periph.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_core.h"

void clock_defaults() {
    // This should be the default setup, but being pedantic
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
    CMU_HFRCOBandSet(_CMU_HFRCOCTRL_BAND_14MHZ);
    CMU_OscillatorEnable(cmuOsc_HFRCO,true,false);
    CMU_OscillatorEnable(cmuOsc_AUXHFRCO,true,false);

    // Disable other oscillators
    CMU_OscillatorEnable(cmuOsc_HFXO, false, false);
    CMU_OscillatorEnable(cmuOsc_LFXO, false, false);
    CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);
    CMU_OscillatorEnable(cmuOsc_ULFRCO, false, false);
    //CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, false);
}

void CMU_setup() {
}

void LED0_setup() {
    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(LED0_PORT, LED0_PORT_NUM, gpioModePushPull, 0);
}

inline uint8_t is_led0_on() {
    return GPIO->P[LED0_PORT].DOUT & (1 << LED0_PORT_NUM);
}

inline void led0_on() {
    GPIO_PinOutSet(LED0_PORT, LED0_PORT_NUM);
}

inline void led0_off() {
    GPIO_PinOutClear(LED0_PORT, LED0_PORT_NUM);
}

inline void led0_toggle() {
    GPIO_PinOutToggle(LED0_PORT, LED0_PORT_NUM);
}

void LED1_setup() {
    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(LED1_PORT, LED1_PORT_NUM, gpioModePushPull, 0);
}

inline uint8_t is_led1_on() {
    return GPIO->P[LED1_PORT].DOUT & (1 << LED1_PORT_NUM);
}

inline void led1_on() {
    GPIO_PinOutSet(LED1_PORT, LED1_PORT_NUM);
}

inline void led1_off() {
    GPIO_PinOutClear(LED1_PORT, LED1_PORT_NUM);
}

inline void led1_toggle() {
    GPIO_PinOutToggle(LED1_PORT, LED1_PORT_NUM);
}
