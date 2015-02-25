#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "led.h"
#include "rawusb.h"
#include "option.h"

#include "api.h"
#include "api-core.h"
#include "api-bootcontrol.h"
#include "api-eeprom.h"
#include "api-flash.h"

static const char impl_id[] PROGMEM = "io.antumbra.glow.v3.ldr:" ANTUMBRA_COMMIT_ID;
static const uint8_t dev_id[] PROGMEM = {ANTUMBRA_COMMIT_ID_HEX};
static const char hw_id[] PROGMEM = "io.antumbra.glow.v3";

const char *api_core_implementation_id = impl_id;
const uint8_t *api_core_device_id = dev_id;
const uint8_t api_core_device_id_len = sizeof dev_id;
const char *api_core_hardware_id = hw_id;

static const api_cmd_list *use_apis[] = {
    &api_core,
    &api_bootcontrol,
    &api_eeprom,
    &api_flash,
};

void api_core_fill_diagnostic(uint8_t *diagbuf) {}

int main(void)
{
    cli();
    api_core_recover_reset();
    led_init();
    MCUCR = _BV(IVCE);
    MCUCR = _BV(IVSEL);
    sei();

    DDRB &= ~_BV(DDB2);
    PORTB |= _BV(PORTB2);

    if (~PINB & _BV(PINB2) || api_bootcontrol_loaderforced())
        led_set_rgb(0, 0, 1);
    else {
        cli();
        MCUCR = _BV(IVCE);
        MCUCR = 0;
        __asm__("jmp 0");
    }

    rawusb_init();

    while (1) {
        rawusb_tick();
        api_dispatch_packet(use_apis, sizeof use_apis / sizeof *use_apis);
    }

    return 0;
}
