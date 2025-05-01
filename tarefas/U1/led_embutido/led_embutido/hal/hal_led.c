#include "include/hal_led.h"
#include "include/led_embutido.h"
#include "pico/stdlib.h"

void hal_led_init(void) 
{
    led_embutido_init();
}

void hal_led_toggle(void) 
{
    static bool led_on = false;

    if (led_on) 
    {
        led_embutido_set(0);
    } 
    else 
    {
        led_embutido_set(1);
    }

    led_on = !led_on;
    sleep_ms(500);
}