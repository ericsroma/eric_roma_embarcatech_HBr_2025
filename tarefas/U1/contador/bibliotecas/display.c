#include "headers/display.h"
#include <stdio.h>
#include <string.h>
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

// Definição dos pinos I2C
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Atualiza o display com os valores atuais
void atualizar_display(int contagem, int cliques, struct render_area *frame_area) 
{
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length); // Limpa o buffer do display

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Contagem: %d", contagem);
    ssd1306_draw_string(ssd, 0, 0, buffer); // Exibe a contagem

    snprintf(buffer, sizeof(buffer), "Cliques B: %d", cliques);
    ssd1306_draw_string(ssd, 0, 16, buffer); // Exibe os cliques do botão B

    render_on_display(ssd, frame_area); // Renderiza no display
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