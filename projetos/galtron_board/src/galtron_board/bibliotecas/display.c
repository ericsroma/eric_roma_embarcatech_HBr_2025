#include "headers/display.h"
#include <stdio.h>
#include <string.h>
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

// Definição dos pinos I2C
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Desenha no display OLED o tabuleiro de pinos do Galtron board
void tabuleiro(struct render_area *frame_area)
{
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length); // Limpa o buffer do display

    // Parâmetros do tabuleiro
    const int niveis = 6;     // 6 níveis de pinos
    const int inicio_y = 12;   // Mais acima para liberar espaço abaixo
    const int espaco_y = 6;   // Menor para compactar verticalmente
    const int espaco_x = 10;  // Espaço horizontal entre pinos

    // Desenha os pinos em padrão triangular
    for (int linha = 0; linha <= niveis; linha++) 
    {
        int num_pinos = linha + 1;
        int offset_x = (128 - (num_pinos - 1) * espaco_x) / 2; // Centraliza os pinos
        int y = inicio_y + linha * espaco_y;

        for (int p = 0; p < num_pinos; p++) 
        {
            int x = offset_x + p * espaco_x;
            ssd1306_set_pixel(ssd, x, y, true); // Desenha um pino
        }
    }

    // Renderiza no display
    render_on_display(ssd, frame_area);
}

// Função para configurar o I2C
void setup_i2c()
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000); // Inicializa o I2C com a frequência especificada
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura o pino SDA
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura o pino SCL
    gpio_pull_up(I2C_SDA); // Ativa o pull-up no pino SDA
    gpio_pull_up(I2C_SCL); // Ativa o pull-up no pino SCL
}