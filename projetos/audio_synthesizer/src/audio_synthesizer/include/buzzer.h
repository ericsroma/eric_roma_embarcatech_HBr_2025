#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <stddef.h>
#include "pico/types.h"

/**
 * Inicializa o PWM para um buzzer em um pino específico.
 * 
 * @param pin      Pino GPIO conectado ao buzzer
 * @param freq_hz  Frequência do PWM em Hz (deve ser igual à taxa de amostragem do áudio)
 * 
 * Configura o pino para função PWM, define a frequência desejada e inicializa o PWM desligado.
 */
void pwm_init_buzzer(uint pin, uint32_t freq_hz);

/**
 * Reproduz um buffer de áudio simultaneamente em dois buzzers usando PWM.
 * 
 * @param pin1        Pino do primeiro buzzer
 * @param pin2        Pino do segundo buzzer
 * @param buffer      Buffer de amostras de áudio (valores do ADC)
 * @param n_samples   Número de amostras a serem reproduzidas
 * @param sample_rate Taxa de amostragem do áudio (Hz)
 * 
 * Cada amostra do buffer é convertida para duty cycle PWM e enviada para ambos os buzzers.
 */
void play_buffer_on_two_buzzers(uint pin1, uint pin2, uint16_t *buffer, size_t n_samples, uint32_t sample_rate);

#endif