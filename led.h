#ifndef LED_H
#define LED_H

#include <stdint.h>

void led_init(void);
void led_set_rgb(uint16_t r, uint16_t g, uint16_t b);

#endif
