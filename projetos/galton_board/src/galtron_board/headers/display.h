#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "inc/ssd1306.h"
#include "headers/bola.h"

void setup_i2c(void);
void tabuleiro(uint8_t *buffer);
void desenhar_bola(uint8_t *buffer, Bola *bola);
void desenhar_histograma(uint8_t *buffer, int *histograma, int num_canaletas);
void mostrar_contador_bolas(uint8_t *buffer, int total);

#endif
