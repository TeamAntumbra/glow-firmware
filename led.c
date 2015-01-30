#include "led.h"

#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>

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

    led_set_rgb(0, 0, 0);
}

void led_set_rgb(uint16_t r, uint16_t g, uint16_t b)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        OCR1A = ((uint32_t)r * 3 / 4) >> 6;
        OCR1B = ((uint32_t)g * 3 / 4) >> 6;
        OCR1C = ((uint32_t)b * 3 / 4) >> 6;

        /* 0% duty cycle is not typically possible with AVR fast PWM. Special-case 0 to tri-state the sink pins. */
        DDRB = r == 0 ? DDRB & ~_BV(DDB5) : DDRB | _BV(DDB5);
        DDRB = g == 0 ? DDRB & ~_BV(DDB6) : DDRB | _BV(DDB6);
        DDRB = b == 0 ? DDRB & ~_BV(DDB7) : DDRB | _BV(DDB7);
    }
}
