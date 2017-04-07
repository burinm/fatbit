#include "light_sensor_ext.h"
#include "../../../drivers/src/driver/tsl2651.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_core.h"
#include "periph.h"

#include "main.h"
#include "leuart.h"
#include "../../atmel/src/s_message.h"
#include "circbuf_tiny.h"

void light_sensor_power_on() {

//Turn power on
GPIO_PinModeSet(LIGHT_SENSOR_POWER_PORT, LIGHT_SENSOR_POWER_PORT_NUM,
    gpioModePushPullDrive, 1);
GPIO_DriveModeSet(LIGHT_SENSOR_POWER_PORT,gpioDriveModeLow);

//Wait for power up
// LETIMER takes care of this
}

//Must be called when interrupts are disabled
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

// Persistance level 4, level interrupts on
tsl2651_write_register(TSL2651_ADDR_INT, TSL2651_INT_PERSIST_4 | TSL2651_INT_CTRL_LEVEL);
tsl2651_int_clear();

// Setup interrupt input on Gecko board
GPIO_PinModeSet(LIGHT_SENSOR_INT_PORT, LIGHT_SENSOR_INT_PORT_NUM,
    gpioModeInputPull, 1);

//Falling edge
GPIO_ExtIntConfig(LIGHT_SENSOR_INT_PORT, LIGHT_SENSOR_INT_PORT_NUM, LIGHT_SENSOR_INT_PORT_NUM,
    false, true, true);

// GPIO interrupts on
NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

uint8_t light_sensor_is_active() {
return ( (GPIO_PortInGet(LIGHT_SENSOR_INT_PORT) & (1 << LIGHT_SENSOR_INT_PORT_NUM)) == 0);
}

//Must be called when interrupts are disabled
void light_sensor_power_off() {

//Disable interrupts
NVIC_DisableIRQ(GPIO_ODD_IRQn);

//Disable GPIO PINS - I2C pins, interrupt pin
tsl2651_on(0); //Soft power off first, before we yank the cord
tsl2651_close();

//Disable external interrupt line
GPIO_PinModeSet(LIGHT_SENSOR_INT_PORT, LIGHT_SENSOR_INT_PORT_NUM,
    gpioModeDisabled, 0);

//Disable power
GPIO_PinOutClear(LIGHT_SENSOR_POWER_PORT, LIGHT_SENSOR_POWER_PORT_NUM);

}

void GPIO_ODD_IRQHandler() {
int intFlags;
uint16_t light=0;

CORE_CriticalDisableIrq();
    intFlags = GPIO_IntGet();
    GPIO_IntClear(1<<LIGHT_SENSOR_INT_PORT_NUM);

    if ( intFlags & (1<<LIGHT_SENSOR_INT_PORT_NUM) ) {
        // These registers must be read low byte first. According to the data
        //  sheet this causes the upper 8 bits to be strobed into a shadow
        //  register allowing them to be read
        light=tsl2651_read_register(TSL2651_ADDR_DATA0_LOWB);
        light+=tsl2651_read_register(TSL2651_ADDR_DATA0_HIGHB) <<8;

        if ( is_led0_on() ) { //Dark state
            if (light >= LIGHT_SENSOR_THRESH_HIGH) {
                led0_off();
                #ifdef SEND_EXTERNAL_NOTIFICATIONS
                    // enqueue led off message
                    s_message *m = s_message_new(S_LED_OFF);
                    circbuf_tiny_write(&O_Q, (uint32_t*)m);
                #endif
            }
        } else { //Light state 
            if (light <= LIGHT_SENSOR_THRESH_LOW) {
                led0_on();
                #ifdef SEND_EXTERNAL_NOTIFICATIONS
                    // enqueue led on message
                    s_message *m = s_message_new(S_LED_ON);
                    circbuf_tiny_write(&O_Q, (uint32_t*)m);
                #endif
            }
        }

        tsl2651_int_clear();
    }
CORE_CriticalEnableIrq();
}
