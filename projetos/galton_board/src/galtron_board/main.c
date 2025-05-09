/*
 * Autor: Eric Senne Roma
 * Data de Criação: 09/05/2025
 * Descrição: Este arquivo contém o código principal para o projeto dirigido 1 do curso de Residência Tecnológica EmbarcaTech.
 *            Trata-se de um sistema que utiliza a Placa BitDogLab com o microcontrolador Raspberry Pi Pico W para simular uma Galton Board Digital.
 *            A simulação é exibida em um display OLED via comunicação I2C e utiliza dois botões: um para alternar entre modo normal e desbalanceado, e outro para pausar ou retomar a simulação.
 *            O sistema gera bolas que descem de forma pseudoaleatória e atualiza um histograma com a distribuição de chegada nas canaletas, permitindo visualizar o comportamento estatístico do movimento.
 */

// Inclusão dos cabeçalhos com as funções utilizadas
#include "headers/display.h"    // Funções para desenhar no display OLED
#include "headers/bola.h"       // Estrutura e funções da bola (movimento, criação etc.)
#include "hardware/i2c.h"       // Biblioteca da Pico para comunicação I2C
#include "pico/stdlib.h"        // Biblioteca padrão da Raspberry Pi Pico
#include <stdlib.h>             // Para uso de rand(), srand(), etc.
#include <time.h>               // Para inicializar o gerador aleatório com base no tempo
#include <string.h>             // Para funções de manipulação de strings como memset()

// Constantes do sistema
#define NUM_CANALETAS 8             // Número de divisões na base onde as bolas caem
#define TICK_MS 20                  // Tempo entre cada atualização da tela (20ms)
#define LARGURA_DISPLAY 128         // Largura total do display
#define ALTURA_DISPLAY 64           // Altura total do display

#define MAX_BOLAS 10                // Máximo de bolas caindo ao mesmo tempo
#define INTERVALO_NOVA_BOLA 10      // A cada 10 ciclos, uma nova bola é criada (se houver espaço)

#define BUTTON_A 5                  // Botão A está no pino GP5
#define BUTTON_B 6                  // Botão B está no pino GP6
#define DEBOUNCE_MS 200             // Tempo de espera para evitar múltiplos cliques indesejados

// Variáveis globais para controlar o estado da simulação
volatile int modo_desbalanceado = 0;        // Indica se a simulação está em modo desbalanceado
volatile bool simulacao_pausada = false;    // Controla se a simulação está pausada

// Controle de debounce dos botões
volatile uint32_t last_button_a_time = 0;   // Último clique válido do botão A
volatile uint32_t last_button_b_time = 0;   // Último clique válido do botão B

// Função de callback para interrupção dos botões
void button_callback(uint gpio, uint32_t events) 
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (gpio == BUTTON_B)  // Se o botão B foi pressionado
    {
        if (current_time - last_button_b_time > DEBOUNCE_MS) 
        {
            simulacao_pausada = !simulacao_pausada;  // Alterna entre pausado e rodando
            last_button_b_time = current_time;       // Atualiza o tempo do último clique
        }
    } 
    else if (gpio == BUTTON_A)  // Se o botão A foi pressionado
    {
        if (current_time - last_button_a_time > DEBOUNCE_MS) 
        {
            modo_desbalanceado = !modo_desbalanceado; // Alterna entre modo normal e desbalanceado
            last_button_a_time = current_time;        // Atualiza o tempo do último clique
        }
    }
}

// Configuração dos botões como entrada, ativação de pull-up e interrupção
void setup_botao() 
{
    // Configura botão B (pausar simulação)
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    // Configura botão A (modo desbalanceado)
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);
}

// Função principal do programa
int main() 
{
    stdio_init_all();    // Inicializa comunicação serial (útil para debug)
    setup_i2c();         // Inicia comunicação I2C com o display
    ssd1306_init();      // Inicializa o display OLED
    setup_botao();       // Configura os botões com interrupção

    // Define qual parte do display será usada (tela toda)
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);  // Calcula o tamanho do buffer de vídeo

    srand(time(NULL));  // Inicia o gerador aleatório com base no tempo atual

    // Declaração dos vetores usados na simulação
    Bola bolas[MAX_BOLAS];             // Vetor com as bolas
    bool ativa[MAX_BOLAS] = {false};   // Indica quais bolas estão em uso
    int histograma[NUM_CANALETAS] = {0}; // Armazena quantas bolas caíram em cada canaleta
    int total_bolas = 0;               // Contador total de bolas que chegaram no fundo
    uint8_t buffer[ssd1306_buffer_length]; // Buffer da tela
    int tick_count = 0;                // Contador de ciclos (ticks)

    // Loop principal da simulação
    while (1) 
    {
        memset(buffer, 0, sizeof(buffer)); // Limpa a tela antes de desenhar

        // Se a simulação estiver pausada, apenas mostra o histograma detalhado
        if (simulacao_pausada) 
        {
            mostrar_analise_histograma(buffer, histograma); // Mostra os dados
            render_on_display(buffer, &frame_area);         // Exibe na tela
            sleep_ms(TICK_MS);                              // Espera o próximo ciclo
            continue;                                       // Pula o restante do loop
        }

        // Cria uma nova bola se for o momento certo e ainda houver espaço
        if (tick_count % INTERVALO_NOVA_BOLA == 0) 
        {
            for (int i = 0; i < MAX_BOLAS; i++) 
            {
                if (!ativa[i]) 
                {
                    inicializar_bola(&bolas[i]);  // Inicializa a bola (posição, direção etc.)
                    ativa[i] = true;              // Marca como ativa
                    break;                        // Cria apenas uma bola por vez
                }
            }
        }

        // Atualiza e desenha as bolas que estão caindo
        for (int i = 0; i < MAX_BOLAS; i++) 
        {
            if (ativa[i]) 
            {
                atualizar_bola(&bolas[i]);        // Move a bola para a próxima posição
                desenhar_bola(buffer, &bolas[i]); // Desenha a bola no buffer

                // Verifica se a bola chegou ao fundo do display
                if (verificar_chegada_fundo(&bolas[i])) 
                {
                    // Define a canaleta onde a bola caiu (só conta se estiver à esquerda)
                    int canaleta = (int)(bolas[i].x / (64.0 / NUM_CANALETAS));
                    if (canaleta >= 0 && canaleta < NUM_CANALETAS)
                        histograma[canaleta]++;   // Atualiza o histograma

                    total_bolas++;                // Atualiza o contador total
                    ativa[i] = false;             // Libera espaço para nova bola
                }
            }
        }

        // Desenha os elementos fixos da simulação
        tabuleiro(buffer);                          // Desenha pinos ou canaletas
        desenhar_histograma(buffer, histograma);    // Desenha gráfico de barras
        mostrar_contador_bolas(buffer, total_bolas); // Mostra número total de bolas

        render_on_display(buffer, &frame_area);     // Atualiza o display com o buffer

        tick_count++;       // Incrementa o contador de ciclos
        sleep_ms(TICK_MS);  // Aguarda o próximo frame
    }

    return 0; // Nunca será alcançado, pois o loop é infinito
}
