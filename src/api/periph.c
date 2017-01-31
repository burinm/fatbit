#include "periph.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_core.h"

void CMU_setup() {

}

void LED0_setup() {
    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(gpioPortE, 2, gpioModePushPull, 0);
}
