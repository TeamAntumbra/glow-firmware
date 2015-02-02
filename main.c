#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "led.h"
#include "rawusb.h"

#include "api.h"
#include "api-core.h"
#include "api-bootcontrol.h"
#include "api-eeprom.h"
#include "api-light.h"
#include "api-temp.h"

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
    &api_temp,
};

static uint16_t curr, curg = 1, curb;
static bool overheat;

void api_core_fill_diagnostic(uint8_t *diagbuf)
{
    if (overheat)
        memcpy(diagbuf, "OVHT", 4);
}

int main(void)
{
    cli();
    api_core_recover_reset();
    led_init();
    led_set_rgb(0, 1, 0);
    api_temp_init();
    sei();

    rawusb_init();

    uint8_t inrgb[6];
    uint16_t tempcount = 0;
    while (1) {
        rawusb_tick();
        if (rawusb_recv_bulk(0x03, inrgb, sizeof inrgb)) {
            curr = (uint16_t)inrgb[0] << 8 | (uint16_t)inrgb[1];
            curg = (uint16_t)inrgb[2] << 8 | (uint16_t)inrgb[3];
            curb = (uint16_t)inrgb[4] << 8 | (uint16_t)inrgb[5];
            if (!overheat)
                led_set_rgb(curr, curg, curb);
        }
        api_dispatch_packet(use_apis, sizeof use_apis / sizeof *use_apis);

        if (++tempcount == 0) {
            if (!overheat && api_temp_read() > 363150) { // 90 C
                overheat = true;
                led_set_rgb(1, 0, 0);
            }
            else if (overheat && api_temp_read() < 353150) { // 80 C
                overheat = false;
                led_set_rgb(curr, curg, curb);
            }
        }
    }
}
