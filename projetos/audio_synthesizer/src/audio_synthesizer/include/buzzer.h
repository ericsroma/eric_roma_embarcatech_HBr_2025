#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <stddef.h>
#include "pico/types.h"

void pwm_init_buzzer(uint pin, uint32_t freq_hz);
void play_buffer_on_buzzer(uint pin, uint16_t *buffer, size_t n_samples, uint32_t sample_rate);

#endif