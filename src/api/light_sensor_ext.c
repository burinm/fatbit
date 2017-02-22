#include "light_sensor_ext.h"
#include "../../../drivers/src/driver/tsl2651.h"
#include "em_gpio.h"
#include "em_cmu.h"

        #include "debug.h"

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
tsl2651_write_register(TSL2651_ADDR_THRESHLOW_LOWB,
         LOW_BYTE16(LIGHT_SENSOR_THRESH_LOW));
tsl2651_write_register(TSL2651_ADDR_THRESHLOW_HIGHB,
         HIGH_BYTE16(LIGHT_SENSOR_THRESH_LOW));

tsl2651_write_register(TSL2651_ADDR_THRESHHIGH_LOWB,
         LOW_BYTE16(LIGHT_SENSOR_THRESH_HIGH));
tsl2651_write_register(TSL2651_ADDR_THRESHHIGH_HIGHB,
         HIGH_BYTE16(LIGHT_SENSOR_THRESH_HIGH));

// Integration timing 101ms, Low gain
tsl2651_write_register(TSL2651_ADDR_TIMING, TSL2651_TIMING_INTEG_101_MS);

// Setup interrupt input on Gecko board
CMU_ClockEnable(cmuClock_GPIO, true);
GPIO_PinModeSet(LIGHT_SENSOR_INT_PORT, LIGHT_SENSOR_INT_PORT_NUM,
    gpioModeInputPull, 1);

#if 1
//Falling edge
GPIO_ExtIntConfig(LIGHT_SENSOR_INT_PORT, LIGHT_SENSOR_INT_PORT_NUM, LIGHT_SENSOR_INT_PORT_NUM, false, true, true);

// GPIO interrupts on
CORE_CriticalDisableIrq();
    NVIC_EnableIRQ(GPIO_ODD_IRQn);
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
CORE_CriticalEnableIrq();
#endif

//Debug Led on
LED0_setup();


// Persistance level 4, level interrupts on
tsl2651_write_register(TSL2651_ADDR_INT, TSL2651_INT_PERSIST_4 | TSL2651_INT_CTRL_LEVEL);
tsl2651_int_clear();


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

void GPIO_ODD_IRQHandler() {
int intFlags;
uint16_t light=0;

CORE_CriticalDisableIrq();
    intFlags = GPIO_IntGet();
    GPIO_IntClear(1<<LIGHT_SENSOR_INT_PORT_NUM);

    if ( intFlags & (1<<LIGHT_SENSOR_INT_PORT_NUM) ) {
        tsl2651_int_clear();
        light=tsl2651_read_register(TSL2651_ADDR_DATA0_HIGHB);
        light<<=8;
        light+=tsl2651_read_register(TSL2651_ADDR_DATA0_LOWB);


        if ( is_led0_on() ) { //Dark state
            if (light >= LIGHT_SENSOR_THRESH_HIGH) {
                led0_off();
            }
        } else { //Light state 
            if (light <= LIGHT_SENSOR_THRESH_LOW) {
                led0_on();
            }
        }
    }

CORE_CriticalEnableIrq();
}
