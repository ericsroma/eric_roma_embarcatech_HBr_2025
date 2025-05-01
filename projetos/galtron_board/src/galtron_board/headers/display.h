#ifndef DISPLAY_H
#define DISPLAY_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

void tabuleiro(struct render_area *frame_area);
void setup_i2c();

#endif // DISPLAY_H

