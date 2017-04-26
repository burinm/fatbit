#include "peripheral.h"
#include "gpio.h"
#include "dualtimer.h"

static volatile uint8_t Timer_loop_count = 0;

void configure_gpio(void) {
    struct gpio_config config_gpio;
    gpio_get_config_defaults(&config_gpio);
    config_gpio.direction  = GPIO_PIN_DIR_OUTPUT;
    config_gpio.input_pull = GPIO_PIN_PULL_NONE;

    //On Board LED
    gpio_pin_set_config(LED_GPIO, &config_gpio);
    gpio_pinmux_cofiguration(LED_GPIO,LED_PINMUX);

    //Haptic Feedback Buzzer
    gpio_pin_set_config(BUZZER_GPIO, &config_gpio);
    gpio_pinmux_cofiguration(BUZZER_GPIO,BUZZER_GPIO_MUX);
}

void hw_timer_start_ms(uint32_t delay) {
    if (delay <= 0) {
        delay = 1;
    }

    dualtimer_set_counter(DUALTIMER_TIMER1,DUALTIMER_SET_CURRUNT_REG,26000*delay);
    dualtimer_enable(DUALTIMER_TIMER1);
}

void  hw_timer_stop_ms(void) {
    dualtimer_disable(DUALTIMER_TIMER1);
}

void timer_callback_handler(void) {
    /* Stop timer */
    hw_timer_stop_ms();

    if (Timer_loop_count == 1) {
        GPIO_OFF(BUZZER_GPIO);
        LED_Off(LED_GPIO);
        return;
    }

    switch(Timer_loop_count % 2) { //Even or Odd
        case 0:
            GPIO_OFF(BUZZER_GPIO);
            LED_Off(LED_GPIO);
            /* Start timer */
            hw_timer_start_ms(100);
            break;
        case 1:
            GPIO_ON(BUZZER_GPIO);
            LED_On(LED_GPIO);
            /* Start timer */
            hw_timer_start_ms(200);
            break;
    };

    if (Timer_loop_count) {
        Timer_loop_count--;
    }
}

void buzz_start(uint8_t times) {
cpu_irq_enter_critical();
    //Add alerts
    Timer_loop_count += (times * 2) + 1;
cpu_irq_leave_critical();

    hw_timer_start_ms(100);
}
