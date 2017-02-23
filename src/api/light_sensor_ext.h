#ifndef __LIGHT_SENSOR_EXT___H
#define __LIGHT_SENSOR_EXT___H

#include <stdint.h>

#define LIGHT_SENSOR_THRESH_LOW     0x000f
#define LIGHT_SENSOR_THRESH_HIGH    0x0800 
#define LOW_BYTE16(x)  (x & 0xff)
#define HIGH_BYTE16(x)  ((x >> 8) & 0xff)


#define LIGHT_SENSOR_POWER_PORT     gpioPortD
#define LIGHT_SENSOR_POWER_PORT_NUM 0

#define LIGHT_SENSOR_INT_PORT     gpioPortD
#define LIGHT_SENSOR_INT_PORT_NUM 1

/* Turn on external Light Sensor power rail
 *
 */
void light_sensor_power_on();

/* Program external Light Sensor
 *  This initializersd I2C lines, programs thresholds
 *
 *  Note - This must be called with interrupts already turned off
 */
void light_sensor_program();

/* Power doeen external Light Sensor
 *  This turns off GPIO interrupts for external interrupt
 *  Light sensor power rail turned off
 *
 *  Note - This must be called with interrupts already turned off
 */
void light_sensor_power_off();

/* Reads GPIO port attached to External Light Sensor interrupt line
 *
 * Return: 0 inactive, >0 active
 */
uint8_t light_sensor_is_active();



#endif
