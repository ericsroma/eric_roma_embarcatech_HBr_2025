#include "headers/bola.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA_DISPLAY 128
#define ALTURA_DISPLAY 64
#define VELOCIDADE_MAX 1.0f
#define ATRITO 0.1f

const int linhas_pinos[] = {12, 18, 24, 30, 36, 42, 48};
const int total_linhas = sizeof(linhas_pinos) / sizeof(linhas_pinos[0]);

int eh_linha_pinos(int y) {
    for (int i = 0; i < total_linhas; i++) {
        if (y == linhas_pinos[i]) return 1;
    }
    return 0;
}

int escolha_aleatoria(void) {
    return (rand() % 2 == 0) ? -1 : 1;
}

void inicializar_bola(Bola *bola) {
    bola->x = LARGURA_DISPLAY / 2.0f;
    bola->y = 0;
    bola->vx = 0.0f;
    bola->estado = 0;
}

void atualizar_bola(Bola *bola) {
    bola->y += 1;
    bola->x += bola->vx;

    if (eh_linha_pinos((int)bola->y)) {
        int direcao = escolha_aleatoria();
        bola->vx = direcao * VELOCIDADE_MAX;
    } else {
        if (bola->vx > 0) {
            bola->vx -= ATRITO;
            if (bola->vx < 0) bola->vx = 0;
        } else if (bola->vx < 0) {
            bola->vx += ATRITO;
            if (bola->vx > 0) bola->vx = 0;
        }
    }

    if (bola->x < 0) bola->x = 0;
    if (bola->x > (LARGURA_DISPLAY - 1)) bola->x = LARGURA_DISPLAY - 1;
}

int verificar_chegada_fundo(Bola *bola) {
    return ((int)bola->y >= ALTURA_DISPLAY);
}
