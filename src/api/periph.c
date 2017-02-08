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
    GPIO_PinModeSet(gpioPortE, 2, gpioModePushPull, 0);
}

inline void led0_on() {
    GPIO_PinOutSet(gpioPortE, 2);
}

inline void led0_off() {
    GPIO_PinOutClear(gpioPortE, 2);
}

inline void led0_toggle() {
    GPIO_PinOutToggle(gpioPortE, 2);
}

void LED1_setup() {
    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 0);
}

inline void led1_on() {
    GPIO_PinOutSet(gpioPortE, 3);
}

inline void led1_off() {
    GPIO_PinOutClear(gpioPortE, 3);
}

inline void led1_toggle() {
    GPIO_PinOutToggle(gpioPortE, 3);
}
