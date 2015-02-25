#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "led.h"
#include "option.h"
#include "rawusb.h"

#include "api.h"
#include "api-core.h"
#include "api-bootcontrol.h"
#include "api-eeprom.h"
#include "api-light.h"
#include "api-temp.h"

static const char impl_id[] PROGMEM = "io.antumbra.glow.v3.app:" ANTUMBRA_COMMIT_ID;
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

enum thermal_state {
    NORMAL,
    RESTRICT,
    SHUTDOWN,
} overheat_state = NORMAL;

#define THERMALDEF_OPTION_ID 0x4f564854
static uint32_t normal_down, restrict_up, restrict_down, shutdown_up;
static uint8_t restrict_numer, restrict_denom;

enum thermal_state get_current_state(void)
{
    uint32_t temp = api_temp_read();
    if (overheat_state == NORMAL)
        return (temp >= shutdown_up ? SHUTDOWN :
                temp >= restrict_up ? RESTRICT :
                NORMAL);
    else if (overheat_state == RESTRICT)
        return (temp <= normal_down ? NORMAL :
                temp >= shutdown_up ? SHUTDOWN :
                RESTRICT);
    else // SHUTDOWN
        return (temp <= normal_down ? NORMAL :
                temp <= restrict_down ? RESTRICT :
                SHUTDOWN);
}

void set_leds(uint16_t r, uint16_t g, uint16_t b)
{
    curr = r;
    curg = g;
    curb = b;

    if (overheat_state == NORMAL)
        led_set_rgb(curr, curg, curb);
    else if (overheat_state == RESTRICT)
        led_set_rgb((uint32_t)curr * restrict_numer / restrict_denom,
                    (uint32_t)curg * restrict_numer / restrict_denom,
                    (uint32_t)curb * restrict_numer / restrict_denom);
    else if (overheat_state == SHUTDOWN)
        led_set_rgb(1, 0, 0);
}

void thermal_check(void)
{
    enum thermal_state newstate = get_current_state();
    if (newstate != overheat_state) {
        overheat_state = newstate;
        set_leds(curr, curg, curb);
    }
}

void api_core_fill_diagnostic(uint8_t *diagbuf)
{
    if (overheat_state == RESTRICT)
        memcpy(diagbuf, "RSTR", 4);
    else if (overheat_state == SHUTDOWN)
        memcpy(diagbuf, "OVHT", 4);
}

static uint32_t convu32(uint8_t *buf)
{
    return (uint32_t)buf[0] << 24 | (uint32_t)buf[1] << 16 | (uint32_t)buf[2] << 8 | buf[3];
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

    uint8_t thermaldef[18];
    memset(thermaldef, 0xff, sizeof thermaldef);
    option_get(THERMALDEF_OPTION_ID, thermaldef, sizeof thermaldef);
    normal_down = convu32(thermaldef);
    restrict_up = convu32(thermaldef + 4);
    restrict_down = convu32(thermaldef + 8);
    shutdown_up = convu32(thermaldef + 12);
    restrict_numer = convu32(thermaldef + 16);
    restrict_denom = convu32(thermaldef + 17);

    uint8_t inrgb[6];
    uint16_t tempcount = 0;
    while (1) {
        rawusb_tick();
        if (rawusb_recv_bulk(0x03, inrgb, sizeof inrgb))
            set_leds((uint16_t)inrgb[0] << 8 | (uint16_t)inrgb[1],
                     (uint16_t)inrgb[2] << 8 | (uint16_t)inrgb[3],
                     (uint16_t)inrgb[4] << 8 | (uint16_t)inrgb[5]);
        api_dispatch_packet(use_apis, sizeof use_apis / sizeof *use_apis);

        if (++tempcount == 0)
            thermal_check();
    }
}
