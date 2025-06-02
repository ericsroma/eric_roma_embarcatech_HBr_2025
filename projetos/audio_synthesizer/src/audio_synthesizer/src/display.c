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
    memset(buffer, 0, sizeof(buffer)); // Limpa a tela antes de desenhar

    size_t step = (n_samples > DISPLAY_WIDTH) ? (n_samples / DISPLAY_WIDTH) : 1;

    for (size_t x = 0; x < DISPLAY_WIDTH; x++) {
        size_t idx = x * step;
        if (idx >= n_samples) break;

        uint16_t value = adc_buffer[idx];
        int y = DISPLAY_HEIGHT - 1 - (value * (DISPLAY_HEIGHT - 1)) / 4095;
        ssd1306_set_pixel(buffer, x, y, true);
    }

    // Define qual parte do display será usada (tela toda)
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);  // Calcula o tamanho do buffer de vídeo

    // Envia o buffer para o display
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