#include "led.h"
#include "option.h"

#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>

#define MULTIPLIER_OPTION_ID 0x4d554c54

static uint8_t numer, denom;

void led_init(void)
{
    DDRB &= ~(_BV(DDB5) | _BV(DDB6) | _BV(DDB7));
    PORTB &= ~(_BV(PORTB5) | _BV(PORTB6) | _BV(PORTB7));

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1C1) | _BV(WGM11);
        TCCR1B = _BV(WGM13) | _BV(WGM12);
        ICR1 = 0x3ff; // 10 bit
        OCR1A = 0;
        OCR1B = 0;
        OCR1C = 0;
        TIMSK1 = 0;
        TCNT1 = 0;
        TCCR1B |= _BV(CS10);
    }

    uint8_t multdef[2] = {4, 5};
    option_get(MULTIPLIER_OPTION_ID, multdef, sizeof multdef);
    numer = multdef[0];
    denom = multdef[1];

    led_set_rgb(0, 0, 0);
}

void led_set_rgb(uint16_t r, uint16_t g, uint16_t b)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        OCR1A = ((uint32_t)r * numer / denom) >> 6;
        OCR1B = ((uint32_t)g * numer / denom) >> 6;
        OCR1C = ((uint32_t)b * numer / denom) >> 6;

        /* 0% duty cycle is not typically possible with AVR fast PWM. Special-case 0 to tri-state the sink pins. */
        DDRB = r == 0 ? DDRB & ~_BV(DDB5) : DDRB | _BV(DDB5);
        DDRB = g == 0 ? DDRB & ~_BV(DDB6) : DDRB | _BV(DDB6);
        DDRB = b == 0 ? DDRB & ~_BV(DDB7) : DDRB | _BV(DDB7);
    }
}
