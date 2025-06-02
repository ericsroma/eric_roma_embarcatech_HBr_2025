#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "include/led.h"
#include "include/button.h"
#include "include/buzzer.h"
#include "include/filters.h"
#include "include/display.h"
#include "hardware/clocks.h"
#include "inc/ssd1306.h"

#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)
#define BUZZER_A 21
#define BUZZER_B 10
#define N 5

#define SAMPLE_RATE 48000
#define DURATION_S 2
#define BUFFER_SIZE (SAMPLE_RATE * DURATION_S)

uint16_t adc_buffer[BUFFER_SIZE];
uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];

// DMA globals
uint dma_channel;
dma_channel_config dma_cfg;

// Define qual parte do display será usada (tela toda)
struct render_area frame_area = {
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1
};

// Função para capturar amostras do microfone via DMA
void sample_mic_dma(uint16_t *buffer, size_t n_samples) {
    adc_fifo_drain();
    adc_run(false);

    dma_channel_configure(
        dma_channel, &dma_cfg,
        buffer,              // Destino
        &(adc_hw->fifo),     // Fonte
        n_samples,           // Número de amostras
        true                 // Iniciar imediatamente
    );

    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_channel);
    adc_run(false);
}

size_t stream_mic_to_buffer(uint32_t sample_rate_hz, uint32_t duration_s)
{
    size_t total_samples = sample_rate_hz * duration_s;
    if (total_samples > BUFFER_SIZE) total_samples = BUFFER_SIZE;

    float clkdiv = (float)clock_get_hz(clk_adc) / (sample_rate_hz * 1.0f);
    adc_set_clkdiv(clkdiv);

    // Captura via DMA
    sample_mic_dma(adc_buffer, total_samples);

    // Passa-baixa suave (ajuste alpha conforme necessário)
    float alpha = 0.2f;
    uint16_t y = adc_buffer[0];
    for (size_t i = 1; i < total_samples; ++i) {
        y = filtro_passabaixa(adc_buffer[i], y, alpha);
        adc_buffer[i] = y;
    }


    return total_samples;
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    adc_gpio_init(MIC_PIN);
    adc_init();
    adc_select_input(MIC_CHANNEL);

    // Configura FIFO do ADC para DMA
    adc_fifo_setup(
        true,   // Habilita FIFO
        true,   // Habilita request de dados do DMA
        1,      // Threshold para ativar request DMA é 1 leitura do ADC
        false,  // Não usar bit de erro
        false   // Não fazer downscale das amostras para 8-bits, manter 12-bits
    );

    // Configura DMA
    dma_channel = dma_claim_unused_channel(true);
    dma_cfg = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // 16 bits
    channel_config_set_read_increment(&dma_cfg, false); // Lê de um único registrador
    channel_config_set_write_increment(&dma_cfg, true); // Escreve em array
    channel_config_set_dreq(&dma_cfg, DREQ_ADC); // Usa requisição do ADC

    pwm_init_buzzer(BUZZER_A, SAMPLE_RATE);
    pwm_init_buzzer(BUZZER_B, SAMPLE_RATE);

    setup_buttons();
    setup_led_rgb();
    set_led_color(0, 0, 0);

    setup_i2c();
    ssd1306_init();

    limpar_display(display_buffer);

    enum {ESPERA_GRAVAR, GRAVANDO, ESPERA_REPRODUZIR, REPRODUZINDO} estado = ESPERA_GRAVAR;
    size_t n = 0;

    while (true)
    {
        switch (estado)
        {
            case ESPERA_GRAVAR:
                printf("Aperte o Botão A para gravar áudio...\n");
                if (button_a_pressed)
                {
                    button_a_pressed = false;
                    set_led_color(1, 0, 0);
                    printf("Gravando áudio do microfone...\n");
                    n = stream_mic_to_buffer(SAMPLE_RATE, DURATION_S);
                    set_led_color(0, 0, 0);

                    printf("Exibindo forma de onda no display OLED...\n");
                    desenhar_onda_audio(display_buffer, adc_buffer, n);

                    estado = ESPERA_REPRODUZIR;
                }
                break;

            case ESPERA_REPRODUZIR:
                printf("Gravação concluída! Aperte o Botão B para reproduzir...\n");
                if (button_b_pressed)
                {
                    button_b_pressed = false;
                    set_led_color(0, 1, 0);
                    printf("Reproduzindo buffer no buzzer...\n");
                    play_buffer_on_two_buzzers(BUZZER_A, BUZZER_B, adc_buffer, n, SAMPLE_RATE);
                    set_led_color(0, 0, 0);

                    printf("Reiniciando ciclo...\n\n");
                    estado = ESPERA_GRAVAR;
                }
                break;

            default:
                estado = ESPERA_GRAVAR;
                break;
        }
        tight_loop_contents(); // Mantém o processador ativo para interrupções
    }
}