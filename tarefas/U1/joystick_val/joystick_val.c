/*
 * Autor: Eric Senne Roma
 * Data de Criação: 13/04/2025
 * Descrição: Código da tarefa para ler os valores convertidos digitalmente do joystick da BitDogLab
 * e mostrar no display OLED com suavização por média exponencial.
 */

 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/adc.h"
 #include "headers/display.h"
 
 // Definição dos pinos usados para o joystick e LEDs
 const int VRX = 26;          // Pino de leitura do eixo X do joystick (conectado ao ADC)
 const int VRY = 27;          // Pino de leitura do eixo Y do joystick (conectado ao ADC)
 const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X do joystick
 const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y do joystick
 const int SW = 22;           // Pino de leitura do botão do joystick
 
 // Área de renderização do display
 struct render_area frame_area = 
 {
     .start_column = 0,
     .end_column = ssd1306_width - 1,
     .start_page = 0,
     .end_page = ssd1306_n_pages - 1
 };
 
 // Função para configurar o joystick (pinos de leitura e ADC)
 void setup_joystick()
 {
     adc_init();         
     adc_gpio_init(VRX);
     adc_gpio_init(VRY);
     gpio_init(SW);
     gpio_set_dir(SW, GPIO_IN);
     gpio_pull_up(SW);
 }
 
 // Função de suavização (média exponencial)
 float suavizar(float entrada, float anterior, float alpha) 
 {
     return alpha * entrada + (1.0f - alpha) * anterior;
 }
/*
    Média Exponencial:

    output(t) = alpha * input(t) + (1 - alpha) * output(t-1)

    input(t) = valor atual do sensor
    output(t) = valor suavizado atual
    output(t-1) = valor suavizado anterior
    alpha = fator de suavização (0 < alpha < 1)
*/
 
 // Função para ler os valores dos eixos do joystick (X e Y)
 void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value)
 {
     adc_select_input(ADC_CHANNEL_0);
     sleep_us(2);
     *vrx_value = adc_read();
 
     adc_select_input(ADC_CHANNEL_1);
     sleep_us(2);
     *vry_value = adc_read();
 }
 
 // Função principal
 int main() 
 {
     uint16_t vrx_raw, vry_raw;
     float vrx_suavizado = 0.0f, vry_suavizado = 0.0f;
     const float alpha = 0.1f; // Fator de suavização (quanto menor, mais suave e lento)
 
     stdio_init_all();
     setup_i2c();
     ssd1306_init();
     calculate_render_area_buffer_length(&frame_area);
     setup_joystick();
 
     while (1) 
     {
         joystick_read_axis(&vrx_raw, &vry_raw);
 
         // Converte para tensão
         float vrx_novo = (3.3f * vrx_raw) / 4095.0f;
         float vry_novo = (3.3f * vry_raw) / 4095.0f;
 
         // Suaviza os valores
         vrx_suavizado = suavizar(vrx_novo, vrx_suavizado, alpha);
         vry_suavizado = suavizar(vry_novo, vry_suavizado, alpha);
 
         // Atualiza o display com os valores suavizados
         atualizar_display(vrx_suavizado, vry_suavizado, &frame_area);
 
         tight_loop_contents();
     }
 }
 