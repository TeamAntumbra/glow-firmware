#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "led.h"
#include "rawusb.h"

#include "api.h"
#include "api-core.h"
#include "api-bootcontrol.h"
#include "api-eeprom.h"
#include "api-light.h"

static const char impl_id[] PROGMEM = "Glow v3 beta " ANTUMBRA_COMMIT_ID;
static const uint8_t dev_id[] PROGMEM = {ANTUMBRA_COMMIT_ID_HEX};
static const char hw_id[] PROGMEM = "io.antumbra.glow.v3";

const char *api_core_implementation_id = impl_id;
const uint8_t *api_core_device_id = dev_id;
const uint8_t api_core_device_id_len = sizeof dev_id;
const char *api_core_hardware_id = hw_id;

const uint8_t api_light_endpoint = 0x03;

static const api_cmd_list *use_apis[] = {
    &api_core,
    &api_bootcontrol,
    &api_eeprom,
    &api_light,
};

int main(void)
{
    cli();
    api_core_recover_reset();
    led_init();
    led_set_rgb(0, 1, 0);
    sei();

    rawusb_init();

    uint8_t inrgb[6];
    while (1) {
        rawusb_tick();
        if (rawusb_recv_bulk(0x03, inrgb, sizeof inrgb)) {
            uint16_t inr = (uint16_t)inrgb[0] << 8 | (uint16_t)inrgb[1],
                     ing = (uint16_t)inrgb[2] << 8 | (uint16_t)inrgb[3],
                     inb = (uint16_t)inrgb[4] << 8 | (uint16_t)inrgb[5];
            led_set_rgb((uint32_t)inr * 0xff / 0xffff,
                        (uint32_t)ing * 0xff / 0xffff,
                        (uint32_t)inb * 0xff / 0xffff);
        }
        api_dispatch_packet(use_apis, sizeof use_apis / sizeof *use_apis);
    }
}
