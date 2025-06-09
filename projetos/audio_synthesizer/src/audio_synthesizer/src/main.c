/*
 * Autor: Eric Senne Roma
 * Data de Criação: 09/06/2025
 * Descrição: Este arquivo contém o código principal do projeto dirigido 1 do curso de Residência Tecnológica EmbarcaTech.
 *            O objetivo é implementar um Sintetizador de Áudio utilizando a placa BitDogLab com o microcontrolador Raspberry Pi Pico.
 *            O sistema permite capturar áudio de um microfone via ADC, armazenar as amostras em RAM via DMA, processar o sinal com filtro digital,
 *            e reproduzir o áudio em dois buzzers utilizando PWM. O controle das funções é feito por dois botões: um para iniciar a gravação
 *            e outro para iniciar a reprodução. O LED RGB fornece retorno visual dos estados (gravação e reprodução).
 *            Durante a gravação, a forma de onda do áudio é exibida em tempo real em um display OLED via I2C, ilustrando graficamente as amostras capturadas.
 */


// Inclusão das bibliotecas padrão e periféricos do Raspberry Pi Pico
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"         // Biblioteca para uso do ADC (conversor analógico-digital)
#include "hardware/dma.h"         // Biblioteca para uso do DMA (acesso direto à memória)
#include "include/led.h"          // Controle do LED RGB
#include "include/button.h"       // Controle dos botões A e B
#include "include/buzzer.h"       // Controle dos buzzers via PWM
#include "include/filters.h"      // Filtros digitais (passa-baixa)
#include "include/display.h"      // Controle do display OLED
#include "hardware/clocks.h"      // Controle dos clocks do sistema
#include "inc/ssd1306.h"          // Driver do display OLED SSD1306

// Definições dos pinos e parâmetros do projeto
#define MIC_CHANNEL 2                     // Canal do ADC usado pelo microfone
#define MIC_PIN (26 + MIC_CHANNEL)        // Pino GPIO correspondente ao canal do microfone
#define BUZZER_A 21                       // Pino do primeiro buzzer
#define BUZZER_B 10                       // Pino do segundo buzzer

#define SAMPLE_RATE 48000                 // Taxa de amostragem do áudio (amostras por segundo)
#define DURATION_S 2                      // Duração máxima da gravação (em segundos)
#define BUFFER_SIZE (SAMPLE_RATE * DURATION_S) // Tamanho do buffer de áudio

uint16_t adc_buffer[BUFFER_SIZE];         // Buffer para armazenar as amostras do ADC (áudio)
uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8]; // Buffer para o display OLED

// Variáveis globais para configuração do DMA
uint dma_channel;
dma_channel_config dma_cfg;

// Estrutura que define a área de renderização do display OLED
struct render_area frame_area = 
{
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1
};

/**
 * Função para capturar amostras do microfone via DMA
 * 
 * @param buffer     Ponteiro para o buffer onde as amostras serão armazenadas
 * @param n_samples  Número de amostras a serem capturadas
 * 
 * Configura o ADC e o DMA para transferir os dados diretamente do ADC para o buffer.
 */
void sample_mic_dma(uint16_t *buffer, size_t n_samples) 
{
    adc_fifo_drain();    // Limpa o FIFO do ADC
    adc_run(false);      // Garante que o ADC está parado

    // Configura o canal DMA para transferir dados do ADC para o buffer
    dma_channel_configure(
        dma_channel, &dma_cfg,
        buffer,              // Destino: buffer de áudio
        &(adc_hw->fifo),     // Fonte: FIFO do ADC
        n_samples,           // Número de amostras
        true                 // Iniciar imediatamente
    );

    adc_run(true);       // Inicia o ADC
    dma_channel_wait_for_finish_blocking(dma_channel); // Aguarda o DMA terminar
    adc_run(false);      // Para o ADC
}

/**
 * Função para capturar e filtrar o áudio do microfone
 * 
 * @param sample_rate_hz  Taxa de amostragem desejada (Hz)
 * @param duration_s      Duração da gravação (segundos)
 * @return                Número de amostras capturadas
 * 
 * Ajusta o clock do ADC, captura via DMA e aplica filtro passa-baixa.
 */
size_t stream_mic_to_buffer(uint32_t sample_rate_hz, uint32_t duration_s)
{
    size_t total_samples = sample_rate_hz * duration_s;
    if (total_samples > BUFFER_SIZE) total_samples = BUFFER_SIZE;

    // Ajusta o clock do ADC para a taxa de amostragem desejada
    float clkdiv = (float)clock_get_hz(clk_adc) / (sample_rate_hz * 1.0f);
    adc_set_clkdiv(clkdiv);

    // Captura as amostras do microfone via DMA
    sample_mic_dma(adc_buffer, total_samples);

    // Aplica filtro passa-baixa para suavizar o áudio e reduzir ruídos
    float alpha = 0.2f; // Coeficiente do filtro (quanto menor, mais suave)
    uint16_t y = adc_buffer[0];
    for (size_t i = 1; i < total_samples; ++i) {
        y = filtro_passabaixa(adc_buffer[i], y, alpha);
        adc_buffer[i] = y;
    }

    return total_samples;
}

/**
 * Função principal do programa
 * 
 * Inicializa todos os periféricos, gerencia o estado do sistema e executa o loop principal.
 */
int main()
{
    stdio_init_all();    // Inicializa entrada/saída padrão (UART/USB)
    sleep_ms(2000);      // Aguarda para garantir que o terminal esteja pronto

    // Inicializa o ADC para o microfone
    adc_gpio_init(MIC_PIN);
    adc_init();
    adc_select_input(MIC_CHANNEL);

    // Configura o FIFO do ADC para uso com DMA
    adc_fifo_setup(
        true,   // Habilita FIFO
        true,   // Habilita request de dados do DMA
        1,      // Threshold para ativar request DMA é 1 leitura do ADC
        false,  // Não usar bit de erro
        false   // Não fazer downscale das amostras para 8-bits, manter 12-bits
    );

    // Configura o canal DMA para transferir dados do ADC
    dma_channel = dma_claim_unused_channel(true);
    dma_cfg = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // 16 bits por amostra
    channel_config_set_read_increment(&dma_cfg, false); // Lê sempre do mesmo registrador
    channel_config_set_write_increment(&dma_cfg, true); // Escreve sequencialmente no buffer
    channel_config_set_dreq(&dma_cfg, DREQ_ADC); // Usa requisição do ADC

    // Inicializa os buzzers (PWM) para reprodução do áudio
    pwm_init_buzzer(BUZZER_A, SAMPLE_RATE);
    pwm_init_buzzer(BUZZER_B, SAMPLE_RATE);

    // Inicializa botões, LED RGB e display OLED
    setup_buttons();
    setup_led_rgb();
    set_led_color(0, 0, 0); // LED apagado inicialmente

    setup_i2c();
    ssd1306_init();

    limpar_display(display_buffer); // Limpa o display OLED

    // Estados do sistema: espera para gravar, gravando, espera para reproduzir, reproduzindo
    enum {ESPERA_GRAVAR, GRAVANDO, ESPERA_REPRODUZIR, REPRODUZINDO} estado = ESPERA_GRAVAR;
    size_t n = 0; // Número de amostras gravadas

    // Loop principal do programa
    while (true)
    {
        switch (estado)
        {
            case ESPERA_GRAVAR:
                // Aguarda o usuário pressionar o botão A para iniciar a gravação
                printf("Aperte o Botão A para gravar áudio...\n");
                if (button_a_pressed)
                {
                    button_a_pressed = false;
                    set_led_color(1, 0, 0); // LED vermelho indica gravação
                    printf("Gravando áudio do microfone...\n");
                    n = stream_mic_to_buffer(SAMPLE_RATE, DURATION_S);
                    set_led_color(0, 0, 0); // LED apagado após gravação

                    printf("Exibindo forma de onda no display OLED...\n");
                    desenhar_onda_audio(display_buffer, adc_buffer, n);

                    estado = ESPERA_REPRODUZIR; // Avança para o próximo estado
                }
                break;

            case ESPERA_REPRODUZIR:
                // Aguarda o usuário pressionar o botão B para iniciar a reprodução
                printf("Gravação concluída! Aperte o Botão B para reproduzir...\n");
                if (button_b_pressed)
                {
                    button_b_pressed = false;
                    set_led_color(0, 1, 0); // LED verde indica reprodução
                    printf("Reproduzindo buffer no buzzer...\n");
                    play_buffer_on_two_buzzers(BUZZER_A, BUZZER_B, adc_buffer, n, SAMPLE_RATE);
                    set_led_color(0, 0, 0); // LED apagado após reprodução

                    printf("Reiniciando ciclo...\n\n");
                    estado = ESPERA_GRAVAR; // Volta para o início
                }
                break;

            default:
                estado = ESPERA_GRAVAR;
                break;
        }
        tight_loop_contents(); // Mantém o processador ativo para interrupções e polling
    }
}