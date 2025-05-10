# Galton Board Digital para BitDogLab

Este projeto implementa uma simulação digital de uma Galton Board utilizando a placa BitDogLab com o microcontrolador Raspberry Pi Pico W. A simulação é visualizada em um display OLED e controlada por dois botões, permitindo alternar entre modos normal e desbalanceado, além de pausar ou retomar a simulação.

A Galton Board é uma máquina que simula a distribuição normal, com bolas caindo aleatoriamente e se distribuindo em diferentes canaletas. O sistema atualizado exibe o movimento das bolas e o histograma com a distribuição de chegada nas canaletas.

## Objetivo

 O projeto implementa uma simulação funcional de uma Galton Board Digital em C estruturado, com múltiplas bolas caindo simultaneamente. Ele exibe um histograma em tempo real no display OLED, permitindo a análise da distribuição dos resultados. A simulação também oferece a possibilidade de experimentar com desbalanceamento e exibe um contador total de bolas no display.
 
 ##  Lista de materiais: 
 
| Componente            | Conexão na BitDogLab                |
|-----------------------|------------------------------------ |
| BitDogLab (RP2040)    | -                                   |
| Botão A               | GPIO5                               |
| Botão B               | GPIO6                               |
| Display OLED I2C      | SDA: GPIO14 / SCL: GPIO15           |

## Execução

1. Abra o projeto no VS Code, usando o ambiente com suporte ao SDK do Raspberry Pi Pico (CMake + compilador ARM);
2. Compile o projeto normalmente (Ctrl+Shift+B no VS Code ou via terminal com cmake e make);
3. Conecte sua BitDogLab via cabo USB e coloque a Pico no modo de boot (pressione o botão BOOTSEL e conecte o cabo);
4. Copie o arquivo .uf2 gerado para a unidade de armazenamento que aparece (RPI-RP2);
5. A Pico reiniciará automaticamente e começará a executar o código e a simulação do Galton Board Digital.
6. Use os botões:
- Botão A para alternar entre o modo normal e o desbalanceado (80% para direita).
- Botão B para pausar ou retomar a simulação.

Funcionalidade do Sistema
### 1. Inicialização do Sistema
- O display OLED exibe a simulação da Galton Board com o histograma das canaletas e o contador total de bolas.

### 2. Simulação
- As bolas descem aleatoriamente e se distribuem entre as canaletas, com base na simulação de um processo de distribuição normal.

- O movimento das bolas é visualizado no display OLED em tempo real.

### 3. Atualização do Histograma
- A cada ciclo, a posição das bolas é atualizada, e um histograma é gerado com a quantidade de bolas que caíram em cada canaleta.

- A distribuição das bolas é exibida no display OLED, e o histograma é atualizado a cada novo ciclo.

### 4. Controle de Botões
- O Botão A alterna entre o modo normal e o modo desbalanceado da simulação.

- O Botão B pausa (mostrando os resultados quantitativos do histograma) ou retoma a simulação.

- Ambos os botões utilizam um sistema de debounce para evitar múltiplos cliques.

## Fluxo de Funcionamento
### 1. Modo Normal
- As bolas são lançadas de forma aleatória, com uma distribuição normal.

### 2. Modo Desbalanceado
- A simulação é modificada para um comportamento desbalanceado (80% de chance de ir pra direita e 20% de ir pra esquerda), afetando a distribuição das bolas nas canaletas.

### 3. Pausa da Simulação
- Ao pressionar o botão B, a simulação é pausada, e o histograma das canaletas pode ser analisado.

### 4. Exibição no Display
- O display OLED exibe:

	- O movimento das bolas na simulação.

	- O histograma das canaletas.

	- O número total de bolas que chegaram no fundo da Galton Board.

##  Arquivos
- `src/display.c/.h`: Funções para desenhar no display OLED;
- `src/bola.c/.h`: Estrutura e funções relacionadas às bolas (movimento, criação, etc.);
- `src/main.c`: Arquivo principal que configura e gerencia o comportamento da simulação.
- `assets`: Contém o vídeo com o funcionamento
