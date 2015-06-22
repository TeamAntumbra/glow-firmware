/* Copyright (c) 2015 Antumbra

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>. */

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

static const api_cmd cmds[] = {
    {2, 0, cmd_info},
    {2, 1, cmd_read},
    {2, 2, cmd_write},
};

const api_cmd_list api_eeprom = {
    .ncmds = sizeof cmds / sizeof *cmds,
    .cmds = cmds,
};
