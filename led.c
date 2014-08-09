#include "led.h"

#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>

static uint8_t mult = 15;
static uint8_t curr, curg, curb;

// stupid logarithmic humans
#define EXP_SCALE(x) (exptable[(sizeof exptable - 1) * mult / 15])
static const uint8_t exptable[] = {
    0, 1, 2, 3, 5, 7, 10, 15, 22, 31, 44, 63, 90, 127, 180, 255,
};

void led_init(void)
{
    DDRB &= ~(_BV(DDB5) | _BV(DDB6) | _BV(DDB7));

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1C1) | _BV(WGM10);
        TCCR1B = _BV(WGM12);
        OCR1A = 0;
        OCR1B = 0;
        OCR1C = 0;
        TIMSK1 = 0;
        TCNT1 = 0;
        TCCR1B |= _BV(CS10);
    }

    led_set_rgb(0, 0, 0);
}

void led_set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        uint8_t rs, gs, bs;
        rs = r * EXP_SCALE(mult) / 0xff;
        gs = g * EXP_SCALE(mult) / 0xff;
        bs = b * EXP_SCALE(mult) / 0xff;

        OCR1A = rs;
        OCR1B = gs;
        OCR1C = bs;
        TCNT1 = 0;

        DDRB = rs == 0 ? DDRB & ~_BV(DDB5) : DDRB | _BV(DDB5);
        DDRB = gs == 0 ? DDRB & ~_BV(DDB6) : DDRB | _BV(DDB6);
        DDRB = bs == 0 ? DDRB & ~_BV(DDB7) : DDRB | _BV(DDB7);

        curr = r;
        curg = g;
        curb = b;
    }
}

uint8_t led_get_mult(void)
{
    return mult;
}

void led_set_mult(uint8_t m)
{
    mult = m;
    led_set_rgb(curr, curg, curb);
}
