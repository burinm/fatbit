#ifndef __ACCEL_EXT_H__
#define __ACCEL_EXT_H__

#include <stdint.h>

#define ACCEL_SENSOR_POWER_PORT     gpioPortD
#define ACCEL_SENSOR_POWER_PORT_NUM 0

#define ACCEL_SENSOR_INT_PORT     gpioPortD
#define ACCEL_SENSOR_INT_PORT_NUM 1

//Threshold .063g/bit * 16 = 1.008g
//#define ACCEL_SENSOR_G_THRESH       16
#define ACCEL_SENSOR_G_THRESH      17 

//With ODR=6.25, Normal mode step = 20ms, 20ms * 10 = 200ms
// Note, changing to low power mode, changes this...
//#define ACCEL_SENSOR_DEBOUNCE_CNT  5 
#define ACCEL_SENSOR_DEBOUNCE_CNT 3 

// Single Tap threshold
#define SINGTAP_SENSOR_G_THRESH_Z  10 
//With ODR=6.25, Normal mode, LPF off 5* 10 = 50ms
#define SINGTAP_TIME_LIMIT  15 
#define SINGTAP_LATENCY  3 
//Must be shorter than SINGTAP_TIME_LIMIT
#define DOUBLETAP_WINDOW 40 

/* Turn on external accelerometer power rail
 *
 */
void accel_sensor_power_on();

/* Program external accelerometer 
 *  This initializersd I2C lines, programs thresholds
 *
 *  Note - This must be called with interrupts already turned off
 */
void accel_sensor_program();

/* Power down external accelerometer
 *  This turns off GPIO interrupts for external interrupt
 *  accelerometer sensor power rail turned off
 *
 *  Note - This must be called with interrupts already turned off
 */
void accel_sensor_power_off();

/* Reads GPIO port attached to External accelerometer interrupt line
 *
 * Return: 0 inactive, >0 active
 */
uint8_t accel_sensor_is_active();


#endif
