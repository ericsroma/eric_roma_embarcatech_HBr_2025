#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "include/led.h"
#include "include/button.h"
#include "include/buzzer.h"
#include "include/filters.h"
#include "include/display.h"
#include "inc/ssd1306.h"

#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)
#define BUZZER_PIN 21
#define N 5 

#define SAMPLE_RATE 40000
#define DURATION_S 3
#define BUFFER_SIZE (SAMPLE_RATE * DURATION_S)

uint16_t adc_buffer[BUFFER_SIZE];
uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];

// Define qual parte do display será usada (tela toda)
struct render_area frame_area = {
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1
};

size_t stream_mic_to_buffer(uint32_t sample_rate_hz, uint32_t duration_s) 
{
    size_t total_samples = sample_rate_hz * duration_s;
    if (total_samples > BUFFER_SIZE) total_samples = BUFFER_SIZE;
    uint32_t sample_period_us = 1000000 / sample_rate_hz;

    adc_select_input(MIC_CHANNEL);

    float alpha_lp = 0.1f;
    float alpha_hp = 0.98f;

    uint16_t y_lp = 0;
    uint16_t y_hp = 0, x_ant = 0;
    uint16_t janela[N] = {0}; int pos = 0; uint32_t soma = 0;

    for (size_t i = 0; i < total_samples; ++i) 
    {
        uint16_t x = adc_read();

        y_lp = filtro_passabaixa(x, y_lp, alpha_lp);
        y_hp = filtro_passaalta(x, x_ant, y_hp, alpha_hp);
        x_ant = x;
        uint16_t y_mm = filtro_media_movel(janela, N, &pos, &soma, x);

        adc_buffer[i] = y_mm;

        sleep_us(sample_period_us);
    }
    return total_samples;
}

int main() 
{
    stdio_init_all();
    sleep_ms(2000);

    adc_gpio_init(MIC_PIN);
    adc_init();
    adc_select_input(MIC_CHANNEL);
    pwm_init_buzzer(BUZZER_PIN, SAMPLE_RATE);

    setup_buttons();
    setup_led_rgb();
    set_led_color(0, 0, 0);

    setup_i2c();
    ssd1306_init();

    limpar_display(display_buffer);

    enum {ESPERA_GRAVAR, GRAVANDO, ESPERA_REPRODUZIR, REPRODUZINDO} estado = ESPERA_GRAVAR;
    size_t n = 0;

    while (true) 
    {
        switch (estado) 
        {
            case ESPERA_GRAVAR:
                printf("Aperte o Botão A para gravar áudio...\n");
                if (button_a_pressed) 
                {
                    button_a_pressed = false;
                    set_led_color(1, 0, 0);
                    printf("Gravando áudio do microfone...\n");
                    n = stream_mic_to_buffer(SAMPLE_RATE, DURATION_S);
                    set_led_color(0, 0, 0);

                    printf("Exibindo forma de onda no display OLED...\n");
                    desenhar_onda_audio(display_buffer, adc_buffer, n);

                    estado = ESPERA_REPRODUZIR;
                }
                break;

            case ESPERA_REPRODUZIR:
                printf("Gravação concluída! Aperte o Botão B para reproduzir...\n");
                if (button_b_pressed) 
                {
                    button_b_pressed = false;
                    set_led_color(0, 1, 0);
                    printf("Reproduzindo buffer no buzzer...\n");
                    play_buffer_on_buzzer(BUZZER_PIN, adc_buffer, n, SAMPLE_RATE);
                    set_led_color(0, 0, 0);

                    printf("Reiniciando ciclo...\n\n");
                    estado = ESPERA_GRAVAR;
                }
                break;

            default:
                estado = ESPERA_GRAVAR;
                break;
        }
        tight_loop_contents(); // Mantém o processador ativo para interrupções
    }
}