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

extern volatile int modo_desbalanceado;


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

// Desenha a bola na posição atual no buffer do display, se ela estiver dentro dos limites visíveis
void desenhar_bola(uint8_t *buffer, Bola *bola)
{
    // Converte as coordenadas de ponto flutuante da bola para inteiros (posição no display)
    int x = (int)bola->x;
    int y = (int)bola->y;

    // Verifica se a posição da bola está dentro dos limites válidos do display
    // Neste caso, o eixo X está limitado à metade esquerda do display (0 a 63)
    // E o eixo Y está limitado à altura total do display
    if (x >= 0 && x < 64 && y >= 0 && y < ALTURA_DISPLAY)
    {
        // Ativa o pixel correspondente no buffer do display OLED
        ssd1306_set_pixel(buffer, x, y, true);
    }
}

// Função para desenhar um histograma no buffer do display
// buffer: ponteiro para o buffer de pixels do display OLED (SSD1306)
// histograma: vetor de 8 inteiros representando os valores das colunas do histograma
void desenhar_histograma(uint8_t *buffer, int *histograma)
{
    const int base_y = 63; // Define a linha de base do histograma (parte inferior do display OLED 128x64)
    const int altura_max = ALTURA_DISPLAY; // Altura máxima das barras (valor fixo correspondente à altura do display)
    const int inicio_x = 75; // Posição inicial no eixo X onde o histograma começa
    const int largura_barra_total = LARGURA_DISPLAY - inicio_x; // Espaço horizontal disponível para o histograma
    const int espaco_barra = largura_barra_total / 8; // Espaço horizontal reservado para cada barra (8 barras no total)

    // Encontrar o maior valor atual no histograma
    int max = 1; // Começa com 1 para evitar divisão por zero
    for (int i = 0; i < 8; i++)
    {
        if (histograma[i] > max)
            max = histograma[i]; // Atualiza o valor máximo, se encontrar um maior
    }

    // Verifica se os valores precisam ser normalizados (i.e., redimensionados proporcionalmente)
    bool precisa_normalizar = (max > altura_max);

    // Loop para desenhar cada uma das 8 barras do histograma
    for (int i = 0; i < 8; i++)
    {
        int altura;
        int x = inicio_x + i * espaco_barra; // Calcula a posição X da barra atual

        // Normaliza a altura se o valor máximo for maior que a altura do display
        if (precisa_normalizar)
        {
            altura = (histograma[i] * altura_max) / max; // Ajusta proporcionalmente
        }
        else
        {
            altura = histograma[i]; // Usa o valor diretamente se não for necessário normalizar
        }

        // Desenha a barra preenchendo pixels de baixo para cima
        for (int y = 0; y < altura; y++)
        {
            for (int dx = 0; dx < espaco_barra - 1; dx++) // Desenha a largura da barra (menos 1 pixel para espaçamento)
            {
                ssd1306_set_pixel(buffer, x + dx, base_y - y, true); // Define o pixel no buffer do display
            }
        }

        // Se o valor do histograma for zero, desenha um ponto na base para marcar presença
        if (histograma[i] == 0)
        {
            ssd1306_set_pixel(buffer, x + espaco_barra / 2, base_y, true); // Ponto centralizado na base da barra
        }
    }
}



// Exibe no display o número total de bolas e o modo atual (Balanceado ou Desbalanceado)
void mostrar_contador_bolas(uint8_t *buffer, int total)
{
    char texto[20]; // Buffer de texto para formatar a string

    // Escreve no buffer de texto a quantidade total de bolas
    snprintf(texto, sizeof(texto), "Bolas: %d", total);

    // Exibe a string no canto superior esquerdo do display (posição x = 0, y = 0)
    ssd1306_draw_string(buffer, 0, 0, texto);

    // Verifica o modo atual e exibe a letra correspondente no canto superior direito
    if (modo_desbalanceado) 
    {
        ssd1306_draw_char(buffer, 120, 0, 'D'); // D para "Desbalanceado"
    }
    else 
    {
        ssd1306_draw_char(buffer, 120, 0, 'B'); // B para "Balanceado"
    }
}

// Exibe os valores de cada barra do histograma no display (modo debug/diagnóstico)
void mostrar_analise_histograma(uint8_t *buffer, int *histograma)
{
    char texto[20]; // Buffer de texto para cada linha de saída
    int y = 0;       // Posição vertical inicial (topo do display)

    // Loop para exibir o valor de cada uma das 8 barras do histograma
    for (int i = 0; i < 8; i++) 
    {
        // Formata o texto com o índice da barra e seu valor
        snprintf(texto, sizeof(texto), "Barra %d: %d", i, histograma[i]);

        // Escreve a string na posição atual do display
        ssd1306_draw_string(buffer, 0, y, texto);

        // Move para a próxima linha (supondo altura de 8 pixels por linha)
        y += 8;
    }
}
