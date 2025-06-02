#include <stdio.h>
#include "include/buzzer.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "pico/types.h"

void pwm_init_buzzer(uint pin, uint32_t freq_hz) 
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    uint32_t clock = clock_get_hz(clk_sys);
    uint32_t top = clock / freq_hz - 1;
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, top);
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0);
}

void play_buffer_on_buzzer(uint pin, uint16_t *buffer, size_t n_samples, uint32_t sample_rate) 
{
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock = clock_get_hz(clk_sys);
    uint32_t top = clock / sample_rate - 1;

    for (size_t i = 0; i < n_samples; ++i) {
        uint16_t value = buffer[i];
        uint32_t level = (value * top) / 4095;
        pwm_set_gpio_level(pin, level);
        sleep_us(1000000 / sample_rate);
    }
    pwm_set_gpio_level(pin, 0);
}