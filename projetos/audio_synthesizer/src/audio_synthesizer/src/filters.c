#include "include/filters.h"
#include "pico/stdlib.h"

uint16_t filtro_passabaixa(uint16_t x, uint16_t y_ant, float alpha) 
{
    return (uint16_t)(alpha * x + (1.0f - alpha) * y_ant);
}
