#include "include/button.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/types.h"

volatile bool button_a_pressed = false;
volatile bool button_b_pressed = false;

#define DEBOUNCE_MS 200

static uint32_t last_a_time = 0;
static uint32_t last_b_time = 0;

void button_callback(uint gpio, uint32_t events) 
{
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (gpio == BUTTON_A) {
        if (now - last_a_time > DEBOUNCE_MS) {
            button_a_pressed = true;
            last_a_time = now;
        }
    }
    if (gpio == BUTTON_B) {
        if (now - last_b_time > DEBOUNCE_MS) {
            button_b_pressed = true;
            last_b_time = now;
        }
    }
}

void setup_buttons() 
{
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_callback);
}