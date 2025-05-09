#include "headers/display.h"
#include "headers/bola.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_CANALETAS 8
#define TICK_MS 20
#define LARGURA_DISPLAY 128
#define ALTURA_DISPLAY 64

#define MAX_BOLAS 10          // Máximo de bolas simultâneas em queda
#define INTERVALO_NOVA_BOLA 10 // A cada 10 ticks, uma nova bola é adicionada

int main() 
{
    stdio_init_all();
    setup_i2c();
    ssd1306_init();

    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    srand(time(NULL));

    Bola bolas[MAX_BOLAS];            // Vetor de bolas
    bool ativa[MAX_BOLAS] = {false};  // Status de cada bola
    int histograma[NUM_CANALETAS] = {0};
    int total_bolas = 0;
    uint8_t buffer[ssd1306_buffer_length];

    int tick_count = 0;

    while (1) 
    {
        memset(buffer, 0, sizeof(buffer));

        // A cada INTERVALO_NOVA_BOLA ticks, cria uma nova bola se houver espaço
        if (tick_count % INTERVALO_NOVA_BOLA == 0) 
        {
            for (int i = 0; i < MAX_BOLAS; i++) 
            {
                if (!ativa[i]) 
                {
                    inicializar_bola(&bolas[i]);
                    ativa[i] = true;
                    break;
                }
            }
        }

        // Atualiza e desenha bolas ativas
        for (int i = 0; i < MAX_BOLAS; i++) 
        {
            if (ativa[i]) 
            {
                atualizar_bola(&bolas[i]);
                desenhar_bola(buffer, &bolas[i]);

                if (verificar_chegada_fundo(&bolas[i])) 
                {
                    int canaleta = (int)(bolas[i].x / (64.0 / NUM_CANALETAS));
                    if (canaleta >= 0 && canaleta < NUM_CANALETAS)
                        histograma[canaleta]++;

                    total_bolas++;
                    ativa[i] = false;  // Libera slot para nova bola
                }
            }
        }

        tabuleiro(buffer);
        desenhar_histograma(buffer, histograma);
        mostrar_contador_bolas(buffer, total_bolas);

        render_on_display(buffer, &frame_area);

        tick_count++;
        sleep_ms(TICK_MS);
    }

    return 0;
}
