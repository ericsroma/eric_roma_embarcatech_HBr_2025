
// Filtros digitais para processamento de áudio
#include "include/filters.h"
#include "pico/stdlib.h"

/**
 * Filtro digital passa-baixa de primeira ordem.
 * 
 * @param x      Amostra atual do sinal de entrada
 * @param y_ant  Saída anterior do filtro (valor filtrado anterior)
 * @param alpha  Coeficiente do filtro (0 < alpha < 1), controla a suavização
 * 
 * @return       Valor filtrado (mais suave, com menos ruído)
 * 
 * O filtro calcula uma média ponderada entre a amostra atual e a saída anterior,
 * atenuando variações rápidas (ruídos) e preservando variações lentas (sinal útil).
 */
uint16_t filtro_passabaixa(uint16_t x, uint16_t y_ant, float alpha) 
{
    return (uint16_t)(alpha * x + (1.0f - alpha) * y_ant);
}