// Funções de controle e visualização para o display OLED SSD1306
#include <stdio.h>
#include <string.h> 
#include "include/display.h"
#include "inc/ssd1306.h"
#include "pico/stdlib.h"

/**
 * Inicializa a interface I2C para comunicação com o display OLED.
 * 
 * Configura o barramento I2C1 com a frequência definida em ssd1306_i2c_clock,
 * além de configurar os pinos SDA e SCL como função I2C e ativar pull-up.
 */
void setup_i2c(void)
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

/**
 * Desenha a forma de onda do áudio no display OLED.
 * 
 * @param buffer      Buffer de imagem do display (array de bytes)
 * @param adc_buffer  Buffer de amostras de áudio (valores do ADC)
 * @param n_samples   Número de amostras a serem exibidas
 * 
 * O topo do display mostra o texto "Audio Synth". Abaixo do texto,
 * cada coluna representa uma amostra do áudio, com altura proporcional ao valor.
 * Se houver mais amostras que colunas, faz downsampling.
 */
void desenhar_onda_audio(uint8_t *buffer, uint16_t *adc_buffer, size_t n_samples) 
{
    // Limpa o buffer de exibição
    memset(buffer, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT / 8);

    // Escreve o título no topo do display
    ssd1306_draw_string(buffer, 24, 0, "Audio Synth");

    // Define a área útil para a onda (abaixo do texto)
    int y_offset = 10; // Altura do texto (ajuste conforme fonte)
    int wave_height = DISPLAY_HEIGHT - y_offset;

    // Para cada coluna do display, desenha a onda
    size_t max_cols = (n_samples < DISPLAY_WIDTH) ? n_samples : DISPLAY_WIDTH;
    for (size_t x = 0; x < max_cols; x++) 
    {
        // Downsample se necessário para ajustar ao número de colunas do display
        size_t idx = (n_samples > DISPLAY_WIDTH) ? (x * n_samples / DISPLAY_WIDTH) : x;
        uint16_t value = adc_buffer[idx];
        int col_height = (value * wave_height) / 4095; // Altura proporcional ao valor da amostra

        // Desenha a coluna de baixo para cima, abaixo do texto
        for (int y = DISPLAY_HEIGHT - 1; y >= DISPLAY_HEIGHT - col_height; y--) 
        {
            if (y >= y_offset) // Não sobrescreve o texto
                ssd1306_set_pixel(buffer, x, y, true);
        }
    }

    // Define qual parte do display será usada (tela toda)
    struct render_area frame_area = 
    {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area); // Calcula o tamanho do buffer para renderização
    render_on_display(buffer, &frame_area);           // Atualiza o display OLED com o conteúdo do buffer
}

/**
 * Limpa o display OLED, apagando todo o conteúdo.
 * 
 * @param buffer Buffer de imagem do display (array de bytes)
 * 
 * Preenche o buffer com zeros e atualiza toda a área do display.
 */
void limpar_display(uint8_t *buffer) 
{
    memset(buffer, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT / 8);

    struct render_area frame_area = 
    {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);
    render_on_display(buffer, &frame_area);
}