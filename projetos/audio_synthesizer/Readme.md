# Sintetizador de Áudio Digital para BitDogLab

Este projeto implementa um Sintetizador de Áudio utilizando a placa BitDogLab com o microcontrolador Raspberry Pi Pico. O sistema permite capturar áudio de um microfone, processar e armazenar as amostras em RAM, aplicar filtro digital, e reproduzir o áudio em dois buzzers via PWM. O controle das funções é feito por dois botões, com retorno visual por LED RGB e visualização da forma de onda em tempo real em um display OLED.

## Objetivo

O projeto implementa um sintetizador de áudio funcional em C estruturado, capaz de capturar, processar, armazenar e reproduzir áudio. O sistema exibe a forma de onda capturada no display OLED e utiliza botões para controlar gravação e reprodução, além de indicar o estado do sistema com o LED RGB.

## Lista de Materiais

| Componente            | Conexão na BitDogLab                |
|-----------------------|-------------------------------------|
| BitDogLab (RP2040)    | -                                   |
| Botão A               | GPIO5                               |
| Botão B               | GPIO6                               |
| Display OLED I2C      | SDA: GPIO14 / SCL: GPIO15           |
| Buzzer A              | GPIO21                              |
| Buzzer B              | GPIO10                              |
| LED RGB               | R: GPIO13 / G: GPIO11 / B: GPIO12   |
| Microfone             | ADC2 (GPIO28)                       |

## Execução

1. Abra o projeto no VS Code, usando o ambiente com suporte ao SDK do Raspberry Pi Pico (CMake + compilador ARM);
2. Compile o projeto normalmente (Ctrl+Shift+B no VS Code ou via terminal com cmake e make);
3. Conecte sua BitDogLab via cabo USB e coloque a Pico no modo de boot (pressione o botão BOOTSEL e conecte o cabo);
4. Copie o arquivo `.uf2` gerado para a unidade de armazenamento que aparece (RPI-RP2);
5. A Pico reiniciará automaticamente e o sintetizador estará pronto para uso.
6. Use os botões:
   - Botão A para iniciar a gravação do áudio.
   - Botão B para iniciar a reprodução do áudio gravado.

## Funcionalidade do Sistema

### 1. Inicialização do Sistema
- O display OLED exibe o título e, após a gravação, a forma de onda do áudio capturado.
- O LED RGB indica o estado do sistema: vermelho para gravação, verde para reprodução.

### 2. Gravação de Áudio
- Ao pressionar o Botão A, o sistema inicia a captura do áudio do microfone via ADC, utilizando DMA para eficiência.
- As amostras são armazenadas em um buffer na RAM.
- Um filtro digital passa-baixa é aplicado para suavizar o áudio.

### 3. Visualização da Forma de Onda
- Após a gravação, a forma de onda do áudio é exibida em tempo real no display OLED, com cada coluna representando uma amostra.

### 4. Reprodução do Áudio
- Ao pressionar o Botão B, o áudio armazenado é reproduzido nos dois buzzers via PWM, simulando diferentes níveis de volume conforme a amplitude das amostras.

### 5. Controle por Botões
- Botão A: Inicia a gravação do áudio.
- Botão B: Inicia a reprodução do áudio gravado.
- Ambos os botões utilizam debounce e interrupção para evitar múltiplos cliques.

### 6. Retorno Visual com LED RGB
- LED vermelho: gravação em andamento.
- LED verde: reprodução em andamento.
- LED apagado: sistema em espera.

## Fluxo de Funcionamento

1. **Espera para Gravar:** O sistema aguarda o usuário pressionar o Botão A.
2. **Gravação:** O áudio é capturado, processado e armazenado. O LED fica vermelho.
3. **Exibição:** A forma de onda do áudio gravado é mostrada no display OLED.
4. **Espera para Reproduzir:** O sistema aguarda o usuário pressionar o Botão B.
5. **Reprodução:** O áudio é reproduzido nos buzzers. O LED fica verde.
6. **Reinício:** O sistema retorna ao estado inicial, pronto para nova gravação.

## Arquivos

- `src/audio_synthesizer/src/main.c`: Arquivo principal que configura e gerencia o funcionamento do sintetizador.
- `src/audio_synthesizer/src/display.c/.h`: Funções para desenhar no display OLED e exibir a forma de onda.
- `src/audio_synthesizer/src/buzzer.c/.h`: Funções para controle e reprodução de áudio nos buzzers via PWM.
- `src/audio_synthesizer/src/led.c/.h`: Controle do LED RGB para indicação de estado.
- `src/audio_synthesizer/src/button.c/.h`: Controle dos botões A e B, incluindo debounce e interrupção.
- `src/audio_synthesizer/src/filters.c/.h`: Implementação do filtro digital passa-baixa.
- `src/audio_synthesizer/inc/ssd1306_i2c.c/.h`: Driver do display OLED SSD1306 via I2C.
- `assets`: Vídeo de funcionamento do projeto.

