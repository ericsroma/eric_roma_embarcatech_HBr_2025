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

void play_buffer_on_two_buzzers(uint pin1, uint pin2, uint16_t *buffer, size_t n_samples, uint32_t sample_rate)
{
    uint32_t clock = clock_get_hz(clk_sys);
    uint32_t top = clock / sample_rate - 1;
    float gain = 1.7f; 

    for (size_t i = 0; i < n_samples; ++i) {
        uint32_t amplified = (uint32_t)(buffer[i] * gain);
        if (amplified > 4095) amplified = 4095;
        uint32_t level = (amplified * top) / 4095;
        pwm_set_gpio_level(pin1, level);
        pwm_set_gpio_level(pin2, level);
        sleep_us(1000000 / sample_rate);
    }
    pwm_set_gpio_level(pin1, 0);
    pwm_set_gpio_level(pin2, 0);
}