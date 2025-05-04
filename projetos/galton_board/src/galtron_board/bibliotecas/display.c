#include "headers/display.h"
#include <stdio.h>
#include <string.h>
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "headers/bola.h"

#define I2C_SDA 14
#define I2C_SCL 15
#define LARGURA_DISPLAY 128
#define ALTURA_DISPLAY 64

void setup_i2c(void)
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void tabuleiro(uint8_t *buffer)
{
    const int niveis = 6;
    const int inicio_y = 12;
    const int espaco_y = 6;
    const int espaco_x = 10;

    for (int linha = 0; linha <= niveis; linha++)
    {
        int num_pinos = linha + 1;
        int offset_x = (LARGURA_DISPLAY - (num_pinos - 1) * espaco_x) / 2;
        int y = inicio_y + linha * espaco_y;

        for (int p = 0; p < num_pinos; p++)
        {
            int x = offset_x + p * espaco_x;
            ssd1306_set_pixel(buffer, x, y, true);
        }
    }
}

void desenhar_bola(uint8_t *buffer, Bola *bola)
{
    int x = (int)bola->x;
    int y = (int)bola->y;

    if (x >= 0 && x < LARGURA_DISPLAY && y >= 0 && y < ALTURA_DISPLAY)
    {
        ssd1306_set_pixel(buffer, x, y, true);
    }
}

void desenhar_histograma(uint8_t *buffer, int *histograma, int num_canaletas)
{
    const int base_y = 60;
    const int altura_max = 30;
    const int largura_barra = LARGURA_DISPLAY / num_canaletas;

    int max = 1;
    for (int i = 0; i < num_canaletas; i++)
    {
        if (histograma[i] > max) max = histograma[i];
    }

    for (int i = 0; i < num_canaletas; i++)
    {
        int altura = (histograma[i] * altura_max) / max;
        int x = i * largura_barra;

        for (int y = 0; y < altura; y++)
        {
            for (int dx = 0; dx < largura_barra - 1; dx++)
            {
                ssd1306_set_pixel(buffer, x + dx, base_y - y, true);
            }
        }
    }
}

void mostrar_contador_bolas(uint8_t *buffer, int total)
{
    char texto[20];
    snprintf(texto, sizeof(texto), "Bolas: %d", total);
    ssd1306_draw_string(buffer, 0, 0, texto);
}
