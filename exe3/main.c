#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int pressed = 0;
volatile int released = 0;
volatile uint64_t start_ms = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        if (gpio == BTN_PIN_R) {
            pressed = 1;
            start_ms = to_ms_since_boot(get_absolute_time());
        }

    } else if (events == 0x8) { // rise edge
        if (gpio == BTN_PIN_R)
            released = 1;
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {

        if (released) {
            released = 0;
            uint64_t now = to_ms_since_boot(get_absolute_time());
            if ((now - start_ms) >= 500) {
                gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
            }
            pressed = 0;
        }
    }
}
