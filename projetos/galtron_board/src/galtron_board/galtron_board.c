#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/rand.h"  // Para funções de aleatoriedade do Pico

int escolher_direcao() 
{
    return get_rand_32() % 2; // Usa a função de aleatoriedade do Pico
}

void testar_escolha_direcao(int tentativas) 
{
    int esquerda = 0;
    int direita = 0;

    for (int i = 0; i < tentativas; i++) 
    {
        int resultado = escolher_direcao();

        if (resultado == 0) 
        {
            esquerda++;
        } 
        else 
        {
            direita++;
        }
    }

    printf("Total de escolhas:\n");
    printf("Esquerda: %d\n", esquerda);
    printf("Direita : %d\n", direita);

    float porcentagem_e = 100.0f * esquerda / tentativas;
    float porcentagem_d = 100.0f * direita / tentativas;

    printf("Porcentagem:\n");
    printf("Esquerda: %.2f%%\n", porcentagem_e);
    printf("Direita : %.2f%%\n", porcentagem_d);
}

int main() 
{
    stdio_init_all();

    testar_escolha_direcao(10000); // Testa com 10 mil escolhas

    return 0;
}