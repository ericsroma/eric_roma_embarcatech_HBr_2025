# Leitura Analógica do Joystick com Exibição no Display OLED
Esta tarefa utiliza a placa BitDogLab (RP2040) para capturar os valores analógicos dos eixos X e Y de um joystick e exibi-los em tempo real em um display OLED. Os valores lidos são convertidos para tensões e atualizados continuamente na tela.

## Objetivo
Implementar a leitura contínua dos eixos analógicos do joystick da BitDogLab usando os canais ADC do RP2040, convertendo os valores para tensões (0 a 3.3V) e exibindo-os no display OLED.

## Lista de Materiais
| Componente            | Conexão na BitDogLab                |
|-----------------------|------------------------------------ |
| BitDogLab (RP2040)    | -                                   |
|Joystick               |	VRX: GPIO26 / VRY: GPIO27 / SW: GPIO22 |
| Display OLED I2C      | SDA: GPIO14 / SCL: GPIO15           |

## Execução
1. Abra o projeto no VS Code com ambiente configurado para o SDK do Raspberry Pi Pico (CMake + compilador ARM);

2. Compile o projeto usando Ctrl+Shift+B ou via terminal com cmake e make;

3. Coloque a Pico no modo de boot (pressione o botão BOOTSEL e conecte o cabo USB);

4. Copie o arquivo .uf2 gerado para a unidade RPI-RP2;

5. A placa será reiniciada automaticamente e iniciará o programa;

6. Mova o joystick para alterar os valores dos eixos X e Y, que serão lidos e convertidos para tensões em tempo real;

7. As tensões correspondentes aos eixos serão exibidas continuamente no display OLED.

## Fluxo de Funcionamento
### 1. Inicialização do Sistema
O sistema inicializa os periféricos: ADC, GPIOs do joystick e o display OLED via I2C;

A área de renderização do display é definida com base nas dimensões da tela.

### 2. Leitura do Joystick
Os canais ADC0 (GPIO26) e ADC1 (GPIO27) são configurados para ler os eixos X e Y, respectivamente;

A cada iteração do loop principal, os valores analógicos são lidos e convertidos para tensões (0 a 3.3V);

O botão do joystick (SW) é configurado como entrada com pull-up interno, mas não é utilizado na lógica atual.

### 3. Atualização do Display
Os valores convertidos dos eixos X e Y são passados para a função `atualizar_display`, que atualiza o conteúdo exibido no display OLED com as tensões correspondentes.

### 4. Loop Contínuo
A leitura e a exibição dos valores continuam indefinidamente, com consumo eficiente de energia através do uso de tight_loop_contents().

## Arquivos
- `joystick_val.c`: Arquivo principal do firmware;

- `headers/display.h`: Header com funções auxiliares para o display OLED;

- `bibliotecas/display.c`: Código que implementa função para atualização do display OLED;

- `CMakeLists.txt`: Arquivo de configuração para compilação com o SDK do Pico.
