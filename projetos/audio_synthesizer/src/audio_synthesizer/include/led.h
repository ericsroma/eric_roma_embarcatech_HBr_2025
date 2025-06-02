#ifndef LED_H
#define LED_H

#include <stdbool.h>

#define LED_R 13
#define LED_G 11
#define LED_B 12

void setup_led_rgb(void);
void set_led_color(bool r, bool g, bool b);

#endif