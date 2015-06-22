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

#include "api-flash.h"
#include "flash.h"
#include "proto.h"

#include <string.h>
#include <avr/pgmspace.h>

static uint8_t flashpagebuf[SPM_PAGESIZE];

static void cmd_info(const void *cmdbuf)
{
    proto_send_start(0);
    proto_send_u16(SPM_PAGESIZE);
    proto_send_u32(FLASH_NUM_PAGES);
    proto_send_end();
}

static void cmd_buf_read(const void *cmdbuf)
{
    uint16_t off = proto_get_u16(&cmdbuf);
    uint8_t len = proto_get_u8(&cmdbuf);

    proto_send_start(0);
    if ((uint32_t)off + len > sizeof flashpagebuf)
        proto_send_u8(1);
    else if (len > 48)
        proto_send_u8(2);
    else {
        proto_send_u8(0);
        proto_send_pad(7);
        proto_send_add(flashpagebuf + off, len);
    }
    proto_send_end();
}

static void cmd_buf_write(const void *cmdbuf)
{
    uint16_t off = proto_get_u16(&cmdbuf);
    uint8_t len = proto_get_u8(&cmdbuf);
    proto_skip_pad(&cmdbuf, 5);

    proto_send_start(0);
    if ((uint32_t)off + len > sizeof flashpagebuf)
        proto_send_u8(1);
    else if (len > 48)
        proto_send_u8(2);
    else {
        memcpy(flashpagebuf + off, cmdbuf, len);
        proto_send_u8(0);
    }
    proto_send_end();
}

static void cmd_page_read(const void *cmdbuf)
{
    uint32_t pageidx = proto_get_u32(&cmdbuf);
    proto_send_start(0);
    if (pageidx >= FLASH_NUM_PAGES)
        proto_send_u8(1);
    else {
        proto_send_u8(0);
        flash_read(pageidx, flashpagebuf);
    }
    proto_send_end();
}

static void cmd_page_write(const void *cmdbuf)
{
    uint32_t pageidx = proto_get_u32(&cmdbuf);
    proto_send_start(0);
    if (pageidx >= FLASH_NUM_PAGES)
        proto_send_u8(1);
    else {
        proto_send_u8(0);
        flash_write(pageidx, flashpagebuf);
    }
    proto_send_end();
}

static const api_cmd cmds[] = {
    {3, 0, cmd_info},
    {3, 1, cmd_buf_read},
    {3, 2, cmd_buf_write},
    {3, 3, cmd_page_read},
    {3, 4, cmd_page_write},
};

const api_cmd_list api_flash = {
    .ncmds = sizeof cmds / sizeof *cmds,
    .cmds = cmds,
};
