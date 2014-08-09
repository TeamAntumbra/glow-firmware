#ifndef LED_H
#define LED_H

#include <stdint.h>

void led_init(void);
void led_set_rgb(uint8_t r, uint8_t g, uint8_t b);

uint8_t led_get_mult(void);
void led_set_mult(uint8_t m);

#endif
