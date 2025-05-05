#ifndef BOLA_H
#define BOLA_H

typedef struct {
    float x; //posição horizontal da bolinha
    float y; //posição vertical da bolinha
    float vx; //velocidade horizontal da bolinha
    int estado;  // 0 = em queda, 1 = chegou na canaleta
} Bola;

int escolha_aleatoria(void);
int eh_linha_pinos(int y);
void inicializar_bola(Bola *bola);
void atualizar_bola(Bola *bola);
int verificar_chegada_fundo(Bola *bola);

#endif
