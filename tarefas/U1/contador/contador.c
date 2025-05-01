/*
 * Autor: Eric Senne Roma
 * Data de Criação: 13/04/2025
 * Descrição: Código da tarefa de Contador decrescente com registro de eventos por interrupção.
 */

 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/gpio.h"
 #include "hardware/timer.h"
 #include "headers/display.h"
 
 // GPIO dos botões
 #define BUTTON_A 5
 #define BUTTON_B 6
 
 // Variáveis globais
 volatile bool counting_active = false;
 volatile bool button_a_pressed = false;
 volatile int countdown = 0;
 volatile int b_press_count = 0;
 
 // Estrutura do Temporizador da contagem regressiva
 struct repeating_timer countdown_timer;
 
 // Área de renderização do display
 struct render_area frame_area = 
 {
     .start_column = 0,
     .end_column = ssd1306_width - 1,
     .start_page = 0,
     .end_page = ssd1306_n_pages - 1
 };

// Função de debounce
bool debounce(uint gpio, absolute_time_t *last_press_time, uint32_t debounce_time_us) 
{
    absolute_time_t now = get_absolute_time();
    
    if (absolute_time_diff_us(*last_press_time, now) > debounce_time_us) 
    {
        *last_press_time = now;
        return true;
    }
    
    return false;
}
 
 // ISR para ambos os botões:
void button_callback(uint gpio, uint32_t events) 
{
    static absolute_time_t last_press_time_a = {0};
    static absolute_time_t last_press_time_b = {0};
    
    if (gpio == BUTTON_A) 
    {
        if (debounce(gpio, &last_press_time_a, 200000)) // Debounce de 200ms
        { 
            button_a_pressed = true;
        }
    }
    
    if (gpio == BUTTON_B) 
    {
        if (debounce(gpio, &last_press_time_b, 200000) && counting_active) // Debounce de 200ms
        { 
            b_press_count++;
            atualizar_display(countdown, b_press_count, &frame_area);
        }
    }
}

 // Callback do temporizador da contagem regressiva
 bool countdown_timer_callback(struct repeating_timer *t) 
 {
     if (countdown > 0) 
     {
         countdown--;
         atualizar_display(countdown, b_press_count, &frame_area);
     }
 
     if (countdown == 0) 
     {
         cancel_repeating_timer(&countdown_timer);
         counting_active = false;
         atualizar_display(0, b_press_count, &frame_area);
     }
 
     return countdown > 0;
 }
 
 // Setup dos botões
 void setup_gpio() 
 {
     // Configura os dois botões
     gpio_init(BUTTON_A);
     gpio_set_dir(BUTTON_A, GPIO_IN);
     gpio_pull_up(BUTTON_A);
 
     gpio_init(BUTTON_B);
     gpio_set_dir(BUTTON_B, GPIO_IN);
     gpio_pull_up(BUTTON_B);
 
     // Ativa interrupções com callback único
     gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);
     gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true); // mesmo callback será usado
 }
 
 int main() 
 {
     stdio_init_all();
     setup_i2c();
     ssd1306_init();
     calculate_render_area_buffer_length(&frame_area);
 
     setup_gpio();
     atualizar_display(0, 0, &frame_area);
 
     while (true) 
     {
         if (button_a_pressed) 
         {
             button_a_pressed = false;
 
             countdown = 9;
             b_press_count = 0;
             counting_active = true;
 
             atualizar_display(countdown, b_press_count, &frame_area);
 
             add_repeating_timer_ms(1000, countdown_timer_callback, NULL, &countdown_timer);
         }
 
         tight_loop_contents(); // manter para economia de energia
     }
 
     return 0;
 }
 