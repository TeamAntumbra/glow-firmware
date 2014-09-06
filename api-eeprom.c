#include "api-eeprom.h"
#include "option.h"
#include "proto.h"

#include <avr/eeprom.h>

static void cmd_info(const void *cmdbuf)
{
    proto_send_start(0);
    proto_send_u16(OPTION_EEPROM_SIZE);
    proto_send_end();
}

static void cmd_read(const void *cmdbuf)
{
    uint16_t off = proto_get_u16(&cmdbuf);
    uint8_t len = proto_get_u8(&cmdbuf);

    proto_send_start(0);
    if ((uint32_t)off + len > OPTION_EEPROM_SIZE)
        proto_send_u8(1);
    else if (len > 48)
        proto_send_u8(2);
    else {
        proto_send_u8(0);
        proto_send_pad(7);
        for (uint8_t i = 0; i < len; ++i)
            proto_send_u8(eeprom_read_byte((uint8_t *)off + i));
    }
    proto_send_end();
}

static void cmd_write(const void *cmdbuf)
{
    uint16_t off = proto_get_u16(&cmdbuf);
    uint8_t len = proto_get_u8(&cmdbuf);
    proto_skip_pad(&cmdbuf, 5);

    proto_send_start(0);
    if ((uint32_t)off + len > OPTION_EEPROM_SIZE)
        proto_send_u8(1);
    else if (len > 48)
        proto_send_u8(2);
    else {
        for (uint8_t i = 0; i < len; ++i)
            eeprom_update_byte((uint8_t *)off + i,
                               proto_get_u8(&cmdbuf));
        proto_send_u8(0);
    }
    proto_send_end();
}

static api_cmd cmds[] = {
    {2, 0, cmd_info},
    {2, 1, cmd_read},
    {2, 2, cmd_write},
};

api_cmd_list api_eeprom = {
    .ncmds = sizeof cmds / sizeof *cmds,
    .cmds = cmds,
};
