#ifndef FILTERS_H
#define FILTERS_H

#include <stdint.h>

uint16_t filtro_passabaixa(uint16_t x, uint16_t y_ant, float alpha);

#endif