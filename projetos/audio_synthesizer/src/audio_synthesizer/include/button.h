#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>
#include "pico/stdlib.h"

#define BUTTON_A 5
#define BUTTON_B 6

extern volatile bool button_a_pressed;
extern volatile bool button_b_pressed;

void setup_buttons(void);

#endif