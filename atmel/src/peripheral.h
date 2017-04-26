#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

#include "samb11_xplained_pro.h"
#include <stdint.h>

#define LED_GPIO        LED0_GPIO
#define LED_PINMUX      MUX_LP_GPIO_22_GPIO
#define BUZZER_GPIO     EXT3_PIN_5
#define BUZZER_GPIO_MUX MUX_LP_GPIO_20_GPIO

#define LED_Off(led_gpio)     gpio_pin_set_output_level(led_gpio,true)
#define LED_On(led_gpio)      gpio_pin_set_output_level(led_gpio,false)
#define GPIO_ON(x)      gpio_pin_set_output_level(x,true)
#define GPIO_OFF(x)     gpio_pin_set_output_level(x,false)

void configure_gpio(void);
void hw_timer_start_ms(uint32_t delay);
void hw_timer_stop_ms(void);
void timer_callback_handler(void);

void buzz_start(uint8_t times);


#endif
