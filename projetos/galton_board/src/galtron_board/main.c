#include "headers/display.h"
#include "headers/bola.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_CANALETAS 16
#define TICK_MS 50
#define LARGURA_DISPLAY 128
#define ALTURA_DISPLAY 64

int main() 
{
    stdio_init_all();
    setup_i2c();
    ssd1306_init();

    // Áreas do display
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);

    // Inicialização da semente randômica
    srand(time(NULL));

    // Inicializa estruturas
    Bola bola;
    int histograma[NUM_CANALETAS] = {0};
    int total_bolas = 0;
    uint8_t buffer[ssd1306_buffer_length];  // Buffer único

    inicializar_bola(&bola);

    while (1) {
        memset(buffer, 0, sizeof(buffer));  // Limpa o buffer

        // Atualiza posição da bola
        atualizar_bola(&bola);

        // Desenha tudo no mesmo buffer
        tabuleiro(buffer);
        desenhar_bola(buffer, &bola);
        desenhar_histograma(buffer, histograma, NUM_CANALETAS);
        mostrar_contador_bolas(buffer, total_bolas);

        // Renderiza tudo de uma vez
        render_on_display(buffer, &frame_area);

        // Chegou ao fundo?
        if (verificar_chegada_fundo(&bola)) {
            int canaleta = (int)(bola.x / (LARGURA_DISPLAY / NUM_CANALETAS));
            if (canaleta >= 0 && canaleta < NUM_CANALETAS)
                histograma[canaleta]++;

            total_bolas++;
            inicializar_bola(&bola);
        }

        sleep_ms(TICK_MS);
    }

    return 0;
}
