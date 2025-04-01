# Sistema de Captura e Análise do Som do Coração

Este projeto implementa um sistema embarcado para a captura e análise da frequência cardíaca, bem como da condição do coração, visando o diagnóstico de doenças cardíacas valvulares por meio da análise de sons cardíacos com técnicas de aprendizado de máquina. A solução utiliza a placa BitDogLab para a captura dos sinais e a visualização das informações processadas em tempo real.

## Objetivo

Cria um menu com dois modos (Captura do Som cardíaco e Visualização das Informações do som) usando:
- Display OLED
- Matriz de LEDS
- LED RGB
- Microfone Digital INMP441
- Botões para acionamento das funções


##  Lista de materiais: 

| Componente            | Conexão na BitDogLab                |
|-----------------------|------------------------------------ |
| BitDogLab (RP2040)    | -                                   |
| Matriz WS2812B 5x5    | GPIO7                               |
| Botão A               | GPIO5                               |
| Botão B               | GPIO6                               |
| LED RGB               | R: GPIO13 / G: GPIO11 / B: GPIO12   |
| Matriz WS2812B 5x5    | GPIO7                               |
| Display OLED I2C      | SDA: GPIO14 / SCL: GPIO15           |
| Microfone INMP441     | SCK: GPIO3 / SD: GPIO2 / WS: GPIO4  |

## Execução

1. Abra o projeto no VS Code, usando o ambiente com suporte ao SDK do Raspberry Pi Pico (CMake + compilador ARM);
2. Compile o projeto normalmente (Ctrl+Shift+B no VS Code ou via terminal com cmake e make);
3. Conecte sua BitDogLab via cabo USB e coloque a Pico no modo de boot (pressione o botão BOOTSEL e conecte o cabo);
4. Copie o arquivo .uf2 gerado para a unidade de armazenamento que aparece (RPI-RP2);
5. A Pico reiniciará automaticamente e começará a executar o código;
6. Coloque o acuscultador na saída do microfone e no peito do paciente
7. Pressione o botão A para iniciar o modo Captura
8. Pressione o botão B para inciiar o modo Visualização

## Fluxo de Funcionamento

### 1. Inicialização do Sistema
- O visor **OLED** exibe um menu inicial.
- O usuário pode pressionar **botão A** para iniciar a captura ou **botão B** para visualizar os resultados.

### 2. Captura de Áudio
- Ao pressionar **botão A**, o **LED verde** acende (modo de espera).
- Pressionando novamente **botão A**, o sistema inicia a gravação e o **LED muda para vermelho**.
- O áudio é capturado pelo **microfone digital** e salvo no **formato .wav**.
- A gravação é iniciada/pausada pelo usuário através das teclas:
  - **G** → Iniciar gravação.
  - **S** → Parar gravação.

### 3. Processamento do Sinal
- Segmentação e amplificação no **Audacity**.
- Filtragem e redução de ruído via **Python**.
- O áudio processado é salvo para análise pelo modelo de machine learning.

### 4. Classificação do Som Cardíaco
O modelo de **machine learning** classifica o som em uma das categorias:
- **Normal (N)**
- **Estenose Aórtica (AS)**
- **Regurgitação Mitral (MR)**
- **Estenose Mitral (MS)**
- **Prolapso da Válvula Mitral (MVP)**

### 5. Estimativa da Frequência Cardíaca
- Aplicação de um **algoritmo de detecção de picos** para calcular a frequência cardíaca.

### 6. Exibição dos Resultados
- Pressionando **botão B**, os dados são transmitidos para a **BitDogLab via Wi-Fi** (**formato JSON**).
- O visor **OLED** exibe:
  - **Frequência cardíaca detectada**.
  - **Classificação da condição cardíaca**.
- A **matriz de LEDs** exibe um **símbolo de coração**:
  - **Vermelho** → Condição normal.
  - **Azul** → Condição anormal.
- Um **LED RGB** simula os batimentos cardíacos, piscando na frequência correspondente.

##  Arquivos
- `src/codigos_python`: Pasta com os códigos em Python no Computador para o Projeto;
- `src/firmware`: Pasta com os códigos o firmwate para rodar na placao;
- `assets`: Pasta com exemplos de áudios do sons do coração;