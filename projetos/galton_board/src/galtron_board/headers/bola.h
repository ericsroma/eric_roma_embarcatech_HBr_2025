#ifndef BOLA_H
#define BOLA_H

typedef struct {
    float x;
    float y;
    float vx;
    int estado;  // 0 = em queda, 1 = chegou no fundo
} Bola;

int escolha_aleatoria(void);
int eh_linha_pinos(int y);
void inicializar_bola(Bola *bola);
void atualizar_bola(Bola *bola);
int verificar_chegada_fundo(Bola *bola);

#endif
