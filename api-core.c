#include "api-core.h"
#include "option.h"
#include "proto.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/atomic.h>

#define DEVID_OPTION_ID 0x4f564944 // OVID

static void cmd_echo(const void *cmdbuf)
{
    proto_send(0, cmdbuf, 56);
}

static void cmd_ask(const void *cmdbuf)
{
    uint8_t sup = api_get_supported(proto_get_u32(&cmdbuf)) ? 1 : 0;
    proto_send(0, &sup, 1);
}

static void cmd_diagnostic(const void *cmdbuf)
{
    proto_send_start(0);
    proto_send_pad(56);
    proto_send_end();
}

static void cmd_implid(const void *cmdbuf)
{
    proto_send_start(0);
    for (int i = 0; i < 56; ++i) {
        char c = pgm_read_byte(api_core_implementation_id + i);
        proto_send_u8(c);
        if (!c)
            break;
    }
    proto_send_end();
}

static const uint8_t *get_override_devid(uint8_t *lenout)
{
    uint8_t len;
    uint8_t *start;
    if (!option_find(DEVID_OPTION_ID, &len, &start) || len == 0)
        return NULL;

    uint8_t overridep = eeprom_read_byte(start);
    if (!overridep)
        return NULL;

    *lenout = len - 1;
    return start + 1;
}

static void cmd_devid(const void *cmdbuf)
{
    proto_send_start(0);

    uint8_t devidoptlen;
    const uint8_t *devidopt = get_override_devid(&devidoptlen);

    if (devidopt) {
        for (uint8_t i = 0; i < devidoptlen; ++i)
            proto_send_u8(eeprom_read_byte(devidopt + i));
    }

    else {
        for (uint8_t i = 0; i < api_core_device_id_len; ++i)
            proto_send_u8(pgm_read_byte(api_core_device_id + i));
        proto_send_u8(SIGNATURE_0);
        proto_send_u8(SIGNATURE_1);
        proto_send_u8(SIGNATURE_2);
        proto_send_u8(OSCCAL);
    }

    proto_send_end();
}

static void cmd_reset(const void *cmdbuf)
{
    proto_send_start(0);
    proto_send_end();

    wdt_enable(WDTO_120MS);
    while (1);
}

void api_core_recover_reset(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        MCUSR = 0;
        wdt_reset();
        wdt_disable();
    }
}

static void cmd_hwid(const void *cmdbuf)
{
    proto_send_start(0);
    for (int i = 0; i < 56; ++i) {
        char c = pgm_read_byte(api_core_hardware_id + i);
        proto_send_u8(c);
        if (!c)
            break;
    }
    proto_send_end();
}

static const api_cmd cmds[] = {
    {0, 0, cmd_echo},
    {0, 1, cmd_ask},
    {0, 2, cmd_diagnostic},
    {0, 3, cmd_implid},
    {0, 4, cmd_devid},
    {0, 5, cmd_reset},
    {0, 6, cmd_hwid},
};

const api_cmd_list api_core = {
    .ncmds = sizeof cmds / sizeof *cmds,
    .cmds = cmds,
};
