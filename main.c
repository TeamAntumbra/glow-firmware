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

static uint16_t curr, curg, curb;

static enum thermal_state {
    NORMAL,
    RESTRICT,
    SHUTDOWN,
} overheat_state = NORMAL;
static uint32_t current_temp;

// OVHT
#define THERMALDEF_OPTION_ID 0x4f564854
static uint32_t restrict_start, shutdown_start;

// Only valid for RESTRICT
static uint16_t scale_component(uint16_t c)
{
    return c - (uint32_t)c * (current_temp - restrict_start) / (shutdown_start - restrict_start);
}

static void set_leds_thermal_scale(uint16_t r, uint16_t g, uint16_t b)
{
    curr = r;
    curg = g;
    curb = b;

    if (overheat_state == NORMAL) {
        led_set_rgb(curr, curg, curb);
    }
    else if (overheat_state == RESTRICT) {
        led_set_rgb(scale_component(curr), scale_component(curg), scale_component(curb));
    }
    else if (overheat_state == SHUTDOWN) {
        led_set_rgb(1, 0, 0);
    }
}

static bool did_start_temp_buf;
static uint32_t temp_buf[32];
static uint8_t temp_idx;

static void thermal_check(void)
{
    temp_buf[temp_idx] = api_temp_read();
    temp_idx = (temp_idx + 1) % (sizeof temp_buf / sizeof *temp_buf);
    if (!did_start_temp_buf) {
        for (uint8_t i = 1; i < sizeof temp_buf / sizeof *temp_buf; ++i)
            temp_buf[i] = temp_buf[0];
        did_start_temp_buf = true;
    }

    uint32_t tempsum = 0;
    for (uint8_t i = 0; i < sizeof temp_buf / sizeof *temp_buf; ++i)
        tempsum += temp_buf[i];
    current_temp = tempsum / (sizeof temp_buf / sizeof *temp_buf);

    overheat_state = (current_temp < restrict_start ? NORMAL :
                      restrict_start <= current_temp && current_temp < shutdown_start ? RESTRICT :
                      SHUTDOWN);
    set_leds_thermal_scale(curr, curg, curb);
}

void api_core_fill_diagnostic(uint8_t *diagbuf)
{
    if (overheat_state == RESTRICT)
        memcpy(diagbuf, "RSTR", 4);
    else if (overheat_state == SHUTDOWN)
        memcpy(diagbuf, "OVHT", 4);
}

uint32_t api_temp_reported_temp(void)
{
    return current_temp;
}

static uint32_t convu32(const uint8_t *buf)
{
    return (uint32_t)buf[0] << 24 | (uint32_t)buf[1] << 16 | (uint32_t)buf[2] << 8 | buf[3];
}

static uint16_t convu16(const uint8_t *buf)
{
    return (uint16_t)buf[0] << 8 | buf[1];
}

// ICLR
#define STARTUPLED_OPTION_ID 0x49434c52
static void led_set_startup_value(void)
{
    uint8_t startupdef[6] = {0, 0, 0, 1, 0, 0}; // low green if undef
    option_get(STARTUPLED_OPTION_ID, startupdef, sizeof startupdef);
    curr = convu16(startupdef);
    curg = convu16(startupdef + 2);
    curb = convu16(startupdef + 4);
    led_set_rgb(curr, curg, curb);
}

int main(void)
{
    cli();
    api_core_recover_reset();
    led_init();
    led_set_startup_value();
    api_temp_init();
    sei();

    rawusb_init();

    uint8_t thermaldef[8] = {0};
    option_get(THERMALDEF_OPTION_ID, thermaldef, sizeof thermaldef);
    restrict_start = convu32(thermaldef);
    shutdown_start = convu32(thermaldef + 4);

    uint8_t inrgb[6];
    uint16_t tempcount = 0;
    while (1) {
        rawusb_tick();
        if (rawusb_recv_bulk(0x03, inrgb, sizeof inrgb)) {
            set_leds_thermal_scale(convu16(inrgb), convu16(inrgb + 2), convu16(inrgb + 4));
        }
        api_dispatch_packet(use_apis, sizeof use_apis / sizeof *use_apis);

        if (++tempcount == 0)
            thermal_check();
    }
}
