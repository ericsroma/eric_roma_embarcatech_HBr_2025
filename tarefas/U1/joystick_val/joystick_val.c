/*
 * Autor: Eric Senne Roma
 * Data de Criação: 13/04/2025
 * Descrição: Código da tarefa para ler os valores convertidos digitalmente do joystick da BitDogLab e mostrar no display OLED.
 */

 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/adc.h" // Biblioteca para manipulação do ADC no RP2040
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
  // Inicializa o ADC e os pinos de entrada analógica
  adc_init();         // Inicializa o módulo ADC
  adc_gpio_init(VRX); // Configura o pino VRX (eixo X) para entrada ADC
  adc_gpio_init(VRY); // Configura o pino VRY (eixo Y) para entrada ADC

  // Inicializa o pino do botão do joystick
  gpio_init(SW);             // Inicializa o pino do botão
  gpio_set_dir(SW, GPIO_IN); // Configura o pino do botão como entrada
  gpio_pull_up(SW);          // Ativa o pull-up no pino do botão para evitar flutuações
}

// Função para ler os valores dos eixos do joystick (X e Y)
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value)
{
  // Leitura do valor do eixo X do joystick
  adc_select_input(ADC_CHANNEL_0); // Seleciona o canal ADC para o eixo X
  sleep_us(2);                     // Pequeno delay para estabilidade
  *vrx_value = adc_read();         // Lê o valor do eixo X (0-4095)

  // Leitura do valor do eixo Y do joystick
  adc_select_input(ADC_CHANNEL_1); // Seleciona o canal ADC para o eixo Y
  sleep_us(2);                     // Pequeno delay para estabilidade
  *vry_value = adc_read();         // Lê o valor do eixo Y (0-4095)
}

// Função principal
int main() 
{
  uint16_t vrx_value, vry_value; // Valores dos eixos X e Y do joystick
  stdio_init_all();              // Inicializa entrada/saída padrão
  setup_i2c();                   // Inicializa a comunicação I2C
  ssd1306_init();                // Inicializa o display OLED SSD1306
  calculate_render_area_buffer_length(&frame_area); // Calcula o buffer da área de renderização

  setup_joystick();             // Configura o joystick (pinos e ADC)

  while (1) 
  {
    joystick_read_axis(&vrx_value, &vry_value); // Lê os valores dos eixos do joystick

    // Converte os valores lidos para tensões (0 a 3.3V)
    float vrx_voltage = (3.3f * vrx_value) / 4095.0f;
    float vry_voltage = (3.3f * vry_value) / 4095.0f;

    // Atualiza o display com os valores convertidos
    atualizar_display(vrx_voltage, vry_voltage, &frame_area);

    tight_loop_contents(); // Mantém o loop eficiente em termos de energia
  }
}