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

void tabuleiro(uint8_t *buffer) //Desenha o tabuleiro do Galton Board
{
    const int niveis = 6; // Número de níveis do tabuleiro (0 a 6)
    const int inicio_y = 24; //o tabuleiro começa 24 px pra baixo em relação ao topo do display
    const int espaco_y = 6; // Espaçamento vertical entre os níveis é de 6px
    const int espaco_x = 8; // Espaçamento horizontal entre os pinos é de 8px
    

    for (int linha = 0; linha <= niveis; linha++)
    {
        int num_pinos = linha + 1; //pinos formando triangulo (linha 0 tem 1 pino, linha 1 tem 2 pinos, etc) 
        int total_largura = (num_pinos - 1) * espaco_x; // (num_pinos - 1) = número de espaços entre os pinos, espaco_x = tamanho de cada espaço
        int offset_x =  (60 - total_largura) / 2; // O tabuleiro será centralizado na área disponível de 60 pixels

        int y = inicio_y + linha * espaco_y; //calcula a posição vertical de cada linha de pinos

        for (int p = 0; p < num_pinos; p++) //itera por cada pino na linha atual
        {
            int x = offset_x + p * espaco_x; //calcula a posição horizontal (x) de cada pino dentro da linha
            ssd1306_set_pixel(buffer, x, y, true); //desenha o pino na posição (x, y)
        }

    }
}

void desenhar_bola(uint8_t *buffer, Bola *bola)
{
    int x = (int)bola->x;
    int y = (int)bola->y;

    if (x >= 0 && x < 64 && y >= 0 && y < ALTURA_DISPLAY) // Limitado à metade esquerda 
    {
        ssd1306_set_pixel(buffer, x, y, true);
    }
}

void desenhar_histograma(uint8_t *buffer, int *histograma)
{
    const int base_y = 63; // Linha de base para o histograma a partir do topo do display
    const int altura_max = ALTURA_DISPLAY; // Altura máxima das barras
    const int inicio_x = 75; // Posição inicial no eixo X
    const int largura_barra_total = LARGURA_DISPLAY - inicio_x;
    const int espaco_barra = largura_barra_total / 8;

    // Encontrar o valor máximo atual
    int max = 1;
    for (int i = 0; i < 8; i++)
    {
        if (histograma[i] > max)
            max = histograma[i];
    }

    // Verifica se é necessário normalizar
    bool precisa_normalizar = (max > altura_max);

    for (int i = 0; i < 8; i++)
    {
        // Altura real ou normalizada
        int altura;
        int x = inicio_x + i * espaco_barra;
        
        if (precisa_normalizar)
        {
            altura = (histograma[i] * altura_max) / max;
        }
        else
        {
            altura = histograma[i];
        }

        // Desenhar barra
        for (int y = 0; y < altura; y++)
        {
            for (int dx = 0; dx < espaco_barra - 1; dx++)
            {
                ssd1306_set_pixel(buffer, x + dx, base_y - y, true);
            }
        }

        // Se for zero, marcar um ponto
        if (histograma[i] == 0)
        {
            ssd1306_set_pixel(buffer, x + espaco_barra / 2, base_y, true);
        }
    }
}


void mostrar_contador_bolas(uint8_t *buffer, int total)
{
    char texto[20];
    snprintf(texto, sizeof(texto), "Bolas: %d", total);
    ssd1306_draw_string(buffer, 0, 0, texto);
}
