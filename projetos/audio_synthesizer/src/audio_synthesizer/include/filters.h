#ifndef FILTERS_H
#define FILTERS_H

#include <stdint.h>

/**
 * Filtro digital passa-baixa de primeira ordem.
 * 
 * @param x      Amostra atual do sinal de entrada
 * @param y_ant  Saída anterior do filtro (valor filtrado anterior)
 * @param alpha  Coeficiente do filtro (0 < alpha < 1), controla a suavização
 * 
 * @return       Valor filtrado (mais suave, com menos ruído)
 */
uint16_t filtro_passabaixa(uint16_t x, uint16_t y_ant, float alpha);

#endif