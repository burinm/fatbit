#ifndef __LIGHT_SENSOR_EXT___H
#define __LIGHT_SENSOR_EXT___H

#include <stdint.h>

#define LIGHT_SENSOR_POWER_PORT     gpioPortD
#define LIGHT_SENSOR_POWER_PORT_NUM 0

#define LIGHT_SENSOR_INT_PORT     gpioPortD
#define LIGHT_SENSOR_INT_PORT_NUM 1

void light_sensor_power_on();
void light_sensor_program();
uint8_t light_sensor_is_active();
void light_sensor_power_off();


#endif
