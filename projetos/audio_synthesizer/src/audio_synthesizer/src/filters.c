#include "include/filters.h"
#include "pico/stdlib.h"

uint16_t filtro_passabaixa(uint16_t x, uint16_t y_ant, float alpha) 
{
    return (uint16_t)(alpha * x + (1.0f - alpha) * y_ant);
}

uint16_t filtro_passaalta(uint16_t x, uint16_t x_ant, uint16_t y_ant, float alpha) 
{
    return (uint16_t)(alpha * (y_ant + x - x_ant));
}

uint16_t filtro_media_movel(uint16_t *janela, int N, int *pos, uint32_t *soma, uint16_t novo) 
{
    *soma -= janela[*pos];
    janela[*pos] = novo;
    *soma += novo;
    *pos = (*pos + 1) % N;
    return (uint16_t)(*soma / N);
}