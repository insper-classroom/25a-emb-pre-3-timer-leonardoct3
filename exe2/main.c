#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile int flag_timer_g = 0;
volatile int flag_timer_r = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

bool timer_g_callback(repeating_timer_t *rt) {
    flag_timer_g = 1;
    return true; // keep repeating    
}

bool timer_r_callback(repeating_timer_t *rt) {
    flag_timer_r = 1;
    return true; // keep repeating    
}

int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    repeating_timer_t timer_r;
    repeating_timer_t timer_g;
    
    int play_r = 0;
    int play_g = 0;

    while (true) {

        if (flag_r) {
            flag_r = 0;
            if (!play_r) {
                if (!add_repeating_timer_us(500000, 
                                    timer_r_callback,
                                    NULL, 
                                    &timer_r)) {
                    printf("Failed to add RED timer\n");
                }
                play_r = 1;
            } else {
                cancel_repeating_timer(&timer_r);
                gpio_put(LED_PIN_R, 0);
                play_r = 0;
            }
        }

        if (flag_g) {
            flag_g = 0;
            if (!play_g) {
                if (!add_repeating_timer_us(250000,
                                    timer_g_callback,
                                    NULL,
                                    &timer_g)) {
                    printf("Failed to add GREEN timer\n");
                }
                play_g = 1;
            } else {
                cancel_repeating_timer(&timer_g);
                gpio_put(LED_PIN_G, 0);
                play_g = 0;
            }
        }

        if (flag_timer_g) {
            gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
            flag_timer_g = 0;
        }

        if (flag_timer_r) {
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
            flag_timer_r = 0;
        }
    }
}
