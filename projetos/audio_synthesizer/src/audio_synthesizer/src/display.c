#include <stdio.h>
#include <string.h> 
#include "include/display.h"
#include "inc/ssd1306.h"
#include "pico/stdlib.h"

void setup_i2c(void)
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void desenhar_onda_audio(uint8_t *buffer, uint16_t *adc_buffer, size_t n_samples) 
{
    // Limpa o buffer de exibição
    memset(buffer, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT / 8);

    // Escreve o título no topo
    ssd1306_draw_string(buffer, 24, 0, "Audio Synth");

    // Define a área útil para a onda (abaixo do texto)
    int y_offset = 10; // Altura do texto (ajuste conforme fonte)
    int wave_height = DISPLAY_HEIGHT - y_offset;

    // Para cada coluna do display, desenha a onda
    size_t max_cols = (n_samples < DISPLAY_WIDTH) ? n_samples : DISPLAY_WIDTH;
    for (size_t x = 0; x < max_cols; x++) {
        // Downsample se necessário
        size_t idx = (n_samples > DISPLAY_WIDTH) ? (x * n_samples / DISPLAY_WIDTH) : x;
        uint16_t value = adc_buffer[idx];
        int col_height = (value * wave_height) / 4095; // Altura proporcional

        // Desenha a coluna de baixo para cima, abaixo do texto
        for (int y = DISPLAY_HEIGHT - 1; y >= DISPLAY_HEIGHT - col_height; y--) {
            if (y >= y_offset) // Não sobrescreve o texto
                ssd1306_set_pixel(buffer, x, y, true);
        }
    }

    // Define qual parte do display será usada (tela toda)
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);
    render_on_display(buffer, &frame_area);
}

void limpar_display(uint8_t *buffer) 
{
    memset(buffer, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT / 8);

    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);
    render_on_display(buffer, &frame_area);
}