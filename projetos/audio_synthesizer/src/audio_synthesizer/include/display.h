#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stddef.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define I2C_SDA 14
#define I2C_SCL 15

void setup_i2c(void);
void desenhar_onda_audio(uint8_t *buffer, uint16_t *adc_buffer, size_t n_samples);
void limpar_display(uint8_t *buffer);

#endif