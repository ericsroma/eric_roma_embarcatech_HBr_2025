# Contador Decrescente com Registro de Eventos por Interrupção
Esta tarefa utiliza a placa BitDogLab (RP2040) para realizar uma contagem regressiva e registrar eventos por meio de interrupções geradas por botões físicos. O sistema exibe informações em tempo real em um display OLED, permitindo iniciar uma contagem com o botão A e registrar eventos com o botão B durante a contagem.

## Objetivo
Implementar uma contagem decrescente de 9 até 0 segundos, com possibilidade de registrar o número de eventos (ex: interrupções) ocorridos durante a contagem via botão B. O botão A inicia uma nova contagem.

## Lista de Materiais
| Componente            | Conexão na BitDogLab                |
|-----------------------|------------------------------------ |
| BitDogLab (RP2040)    | -                                   |
| Botão A               | GPIO5                               |
| Botão B               | GPIO6                               |
| Display OLED I2C      | SDA: GPIO14 / SCL: GPIO15           |

## Execução
1. Abra o projeto no VS Code com ambiente configurado para o SDK do Raspberry Pi Pico (CMake + compilador ARM);

2. Compile o projeto usando Ctrl+Shift+B ou via terminal com cmake e make;

3. Coloque a Pico no modo de boot (pressione o botão BOOTSEL e conecte o cabo USB);

4. Copie o arquivo .uf2 gerado para a unidade RPI-RP2;

5. A placa será reiniciada automaticamente e iniciará o programa;

6. Pressione o botão A para iniciar uma contagem decrescente de 9 segundos;

7. Durante a contagem, pressione o botão B para registrar eventos, que serão contabilizados e exibidos no display OLED;

8. Quando a contagem atingir zero, o sistema será reinicializado, pronto para uma nova medição.

## Fluxo de Funcionamento
### 1. Inicialização do Sistema
O visor OLED exibe o valor da contagem atual (inicialmente 0) e a quantidade de eventos registrados (também 0).

### 2. Início da Contagem
Ao pressionar o botão A, inicia-se uma nova contagem decrescente de 9 segundos.

A variável countdown é carregada com o valor 9 e o b_press_count é zerado.

O display é atualizado com esses valores.

### 3. Registro de Eventos (Botão B)
Durante a contagem, o botão B pode ser pressionado a qualquer momento.

Cada pressão válida (com debounce de 200ms) aumenta o contador de eventos e atualiza o display.

Pressões fora do intervalo de contagem são ignoradas.

### 4. Fim da Contagem
Quando a contagem chega a zero, o temporizador é cancelado.

A contagem é finalizada e o display é atualizado para indicar o fim da contagem, mantendo o número de eventos registrados.

## Arquivos
- `contador.c`: Arquivo principal do firmware;

- `headers/display.h`: Header com funções auxiliares para o display OLED;

- `bibliotecas/display.c`: Código que implementa função para atualização do display OLED;

- `CMakeLists.txt`: Arquivo de configuração para compilação com o SDK do Pico.
