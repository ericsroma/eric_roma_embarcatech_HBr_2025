#include "headers/bola.h" //Cabeçalho da biblioteca bola.h
#include <stdlib.h> // Para rand() e srand()
#include <time.h> // Para time()

//Tamanho do display OLED (128x64 pixels)
#define LARGURA_DISPLAY 128
#define ALTURA_DISPLAY 64

#define VELOCIDADE_MAX 1.0f //velocidade máxima lateral da bola
#define ATRITO 0.1f //desaceleração gradual da bola após um desvio

extern volatile int modo_desbalanceado;

int eh_linha_pinos(int y) 
{
    const int niveis = 6; // Número de níveis do tabuleiro (0 a 6)
    const int inicio_y = 24; // O tabuleiro começa 24 px para baixo em relação ao topo do display
    const int espaco_y = 6; // Espaçamento vertical entre os níveis é de 6px

    for (int linha = 0; linha <= niveis; linha++) 
    {
        // Calcula a posição vertical (y) de cada linha de pinos
        int y_linha = inicio_y + linha * espaco_y;

        // Verifica se o y da bola corresponde à linha de pinos calculada
        if (y == y_linha) 
        {
            return 1; // Se sim, é uma linha de pinos
        }
    }

    return 0; // Caso contrário, não é linha de pinos
}

int escolha_direcionada(void)
{
    if (modo_desbalanceado)
        return (rand() % 10 < 8) ? 1 : -1; // 80% direita, 20% esquerda
    else
        return (rand() % 2 == 0) ? -1 : 1; // 50/50
}


void inicializar_bola(Bola *bola)
{
    bola->x = 30.0f;  // Posição central fixa no topo (já que offset_x = (60 - 0) / 2)
    bola->y = 0.0f; //Posição inicial da bola (topo do display)
    bola->vx = 0.0f; //Velocidade inicial da bola (parada)
    bola->estado = 0; //Estado inicial da bola (em queda)
}

void atualizar_bola(Bola *bola) 
{
    //Aplica gravidade: a bola sempre desce 1 pixel por atualização
    bola->y += 1;

    //Move horizontalmente conforme a velocidade vx
    bola->x += bola->vx;

    //Verifica se a bola está na linha de pinos e vai bater em um pino
    if (eh_linha_pinos((int)bola->y)) 
    {
        //Se estiver, a bola cai em uma direção aleatória (esquerda ou direita)
        int direcao = escolha_direcionada();
        
        //Atribui uma nova velocidade horizontal com base na direção sorteada
        bola->vx = direcao * VELOCIDADE_MAX;
    } 
    else //Se a bola já desviou de um pino
    {
        //Aplica atrito horizontal para desacelerar a bola
        if (bola->vx > 0) // Se a velocidade horizontal for positiva (bola indo para a direita)
        {
            bola->vx -= ATRITO; // Reduz a velocidade, simulando o atrito "freando" o movimento
            if (bola->vx < 0) bola->vx = 0; // Garante que a velocidade não fique negativa ao passar de zero
        } 
        else if (bola->vx < 0) // Se a velocidade horizontal for negativa (bola indo para a esquerda)
        {
            bola->vx += ATRITO; // Aumenta (em direção ao zero), simulando o atrito "freando" também nesse sentido
            if (bola->vx > 0) bola->vx = 0; // Garante que a velocidade não ultrapasse o zero (invertendo a direção sem querer)
        }
    }

    // Impede que a bola ultrapasse os limites horizontais do display
    if (bola->x < 0) bola->x = 0;
    if (bola->x > (LARGURA_DISPLAY - 1)) bola->x = LARGURA_DISPLAY - 1;
}

int verificar_chegada_fundo(Bola *bola) 
{
    return ((int)bola->y >= ALTURA_DISPLAY); // Verifica se a bola chegou ao fundo do display (64 pixels de altura)
}
