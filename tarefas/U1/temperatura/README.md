# Monitor da temperatura interna da MCU
Esta tarefa utiliza a placa BitDogLab (RP2040) para para ler a temperatura interna do RP2040. O valor da leitura do ADC é convertido em temperatura em ºC.

## Objetivo
Implementar a leitura a temperatura interna do RP20 da BitDogLab usando o canal ADC do RP2040, convertendo os valores para tempertatura em graus Celcius e exibindo-os no terminal serial.


## Lista de Materiais
| Componente            | Conexão na BitDogLab                |
|-----------------------|------------------------------------ |
| BitDogLab (RP2040)    | -                                   |

## Execução
1. Abra o projeto no VS Code com ambiente configurado para o SDK do Raspberry Pi Pico (CMake + compilador ARM);

2. Compile o projeto usando Ctrl+Shift+B ou via terminal com cmake e make;

3. Coloque a Pico no modo de boot (pressione o botão BOOTSEL e conecte o cabo USB);

4. Copie o arquivo .uf2 gerado para a unidade RPI-RP2;

5. A placa será reiniciada automaticamente e iniciará o programa;

6. Abra o terminal serial;

7. O valor da temperatura interna do RP2040 é exibido continuamente no terminal.

## Fluxo de Funcionamento
### 1.  Inicialização do Sistema
O sistema inicializa o ADC e a comunicação serial;

O sensor de temperatura interno do Raspberry Pi Pico é ativado no canal ADC4.

### 2. Leitura da Temperatura
O valor analógico do sensor de temperatura é lido através do ADC;

Este valor é convertido para temperatura em graus Celsius utilizando a fórmula específica.

### 3. Exibição da Temperatura
A temperatura convertida é exibida na comunicação serial a cada segundo.

### 4. Loop Contínuo
A leitura e exibição da temperatura continuam indefinidamente, com intervalos de 1 segundo entre as leituras.

## Arquivos
- `temperatura.c`: Arquivo principal do firmware;

- `CMakeLists.txt`: Arquivo de configuração para compilação com o SDK do Pico.
