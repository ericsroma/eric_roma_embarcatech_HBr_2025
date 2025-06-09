// Funções de controle e reprodução de áudio nos buzzers via PWM
#include <stdio.h>
#include "include/buzzer.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "pico/types.h"

/**
 * Inicializa o PWM para um buzzer em um pino específico.
 * 
 * @param pin      Pino GPIO conectado ao buzzer
 * @param freq_hz  Frequência do PWM em Hz (deve ser igual à taxa de amostragem do áudio)
 * 
 * Configura o pino para função PWM, define a frequência desejada e inicializa o PWM desligado.
 */
void pwm_init_buzzer(uint pin, uint32_t freq_hz) 
{
    gpio_set_function(pin, GPIO_FUNC_PWM);                  // Configura o pino como saída PWM
    uint slice_num = pwm_gpio_to_slice_num(pin);            // Obtém o slice PWM correspondente ao pino

    uint32_t clock = clock_get_hz(clk_sys);                 // Obtém a frequência do clock do sistema
    uint32_t top = clock / freq_hz - 1;                     // Calcula o valor de "top (wrap)" para a frequência desejada
    pwm_config config = pwm_get_default_config();            // Obtém a configuração padrão do PWM
    pwm_config_set_wrap(&config, top);                      // Define o valor de "top" (resolução do PWM)
    pwm_init(slice_num, &config, true);                     // Inicializa o PWM com a configuração definida
    pwm_set_gpio_level(pin, 0);                             // Garante que o PWM inicia desligado
}

/**
 * Reproduz um buffer de áudio simultaneamente em dois buzzers usando PWM.
 * 
 * @param pin1        Pino do primeiro buzzer
 * @param pin2        Pino do segundo buzzer
 * @param buffer      Buffer de amostras de áudio (valores do ADC)
 * @param n_samples   Número de amostras a serem reproduzidas
 * @param sample_rate Taxa de amostragem do áudio (Hz)
 * 
 * Cada amostra do buffer é amplificada, limitada ao valor máximo do ADC,
 * convertida para duty cycle PWM e enviada para ambos os buzzers.
 */
void play_buffer_on_two_buzzers(uint pin1, uint pin2, uint16_t *buffer, size_t n_samples, uint32_t sample_rate)
{
    uint32_t clock = clock_get_hz(clk_sys);                 // Obtém a frequência do clock do sistema
    uint32_t top = clock / sample_rate - 1;                 // Calcula o valor de "top" para a frequência de reprodução
    float gain = 1.7f;                                      // Fator de amplificação do áudio

    for (size_t i = 0; i < n_samples; ++i) 
    {
        uint32_t amplified = (uint32_t)(buffer[i] * gain);  // Aplica ganho à amostra
        if (amplified > 4095) amplified = 4095;             // Limita ao valor máximo do ADC (12 bits)
        uint32_t level = (amplified * top) / 4095;          // Converte para duty cycle proporcional ao valor da amostra
        pwm_set_gpio_level(pin1, level);                    // Define o nível PWM do primeiro buzzer
        pwm_set_gpio_level(pin2, level);                    // Define o nível PWM do segundo buzzer
        sleep_us(1000000 / sample_rate);                    // Aguarda o tempo correspondente à taxa de amostragem
    }
    pwm_set_gpio_level(pin1, 0);                            // Desliga o PWM do primeiro buzzer ao final
    pwm_set_gpio_level(pin2, 0);                            // Desliga o PWM do segundo buzzer ao final
}