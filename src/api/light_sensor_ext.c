#include "light_sensor_ext.h"
#include "../../../drivers/src/driver/tsl2651.h"
#include "em_gpio.h"
#include "em_cmu.h"

void light_sensor_power_on() {

//Turn power on

//Wait for power up



}

void light_sensor_program() {

//Initialize GPIO PINS/I2C reset
tsl2651_open();

//Power up register on
tsl2651_on(1);

// Interrupt threshold 0x00f -> 0x0800
tsl2651_write_register(TSL2651_ADDR_THRESHLOW_LOWB, 0x0f);
tsl2651_write_register(TSL2651_ADDR_THRESHLOW_HIGHB, 0x00);

tsl2651_write_register(TSL2651_ADDR_THRESHHIGH_LOWB, 0x00);
tsl2651_write_register(TSL2651_ADDR_THRESHHIGH_HIGHB, 0x08);

// Integration timing 101ms, Low gain
tsl2651_write_register(TSL2651_ADDR_TIMING, TSL2651_TIMING_INTEG_101_MS);

// Setup interrupt input on Gecko board
CMU_ClockEnable(cmuClock_GPIO, true);
GPIO_PinModeSet(LIGHT_SENSOR_INT_PORT, LIGHT_SENSOR_INT_PORT_NUM,
    gpioModeInputPull, 1);

// Persistance level 4, level interrupts on
tsl2651_write_register(TSL2651_ADDR_INT, TSL2651_INT_PERSIST_4 | TSL2651_INT_CTRL_LEVEL);


#if 0
uint8_t b;
b=tsl2651_read_register(TSL2651_ADDR_THRESHLOW_LOWB);
b=tsl2651_read_register(TSL2651_ADDR_THRESHLOW_HIGHB);
b=tsl2651_read_register(TSL2651_ADDR_THRESHHIGH_LOWB);
b=tsl2651_read_register(TSL2651_ADDR_THRESHHIGH_HIGHB);
b=tsl2651_read_register(TSL2651_ADDR_TIMING);
b=tsl2651_read_register(TSL2651_ADDR_INT);
#endif

}

uint8_t light_sensor_is_active() {

return ( (GPIO_PortInGet(LIGHT_SENSOR_INT_PORT) & (1 << LIGHT_SENSOR_INT_PORT_NUM)) == 0);

}

void light_sensor_power_off() {

//Disable interrupts

//Disable GPIO PINS - I2C pins, interrupt pin

//Disable power

}

