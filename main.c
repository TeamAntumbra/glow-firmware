#include <avr/io.h>
#include <avr/interrupt.h>

#include "led.h"
#include "rawusb.h"

int main(void)
{
    cli();
    rawusb_init();
    led_init();
    led_set_rgb(0, 1, 0);
    sei();

    uint8_t incmd[64];

    while (1) {
        rawusb_tick();

        if (rawusb_recv_bulk(0x01, incmd, sizeof incmd)) {
            rawusb_send_bulk(0x82, incmd, sizeof incmd);
        }
    }
}
