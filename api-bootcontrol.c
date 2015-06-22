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

#include "api-bootcontrol.h"
#include "option.h"
#include "proto.h"

#define FORCELOADER_OPTION_ID 0x424f4f54 // BOOT

static void cmd_setboot(const void *cmdbuf)
{
    uint8_t ldrflag = proto_get_u8(&cmdbuf) ? 1 : 0;
    option_set(FORCELOADER_OPTION_ID, &ldrflag, 1);
    proto_send_start(0);
    proto_send_end();
}

bool api_bootcontrol_loaderforced(void)
{
    uint8_t forceldr = 1;
    option_get(FORCELOADER_OPTION_ID, &forceldr, 1);
    return forceldr != 0;
}

static const api_cmd cmds[] = {
    {1, 0, cmd_setboot},
};

const api_cmd_list api_bootcontrol = {
    .ncmds = sizeof cmds / sizeof *cmds,
    .cmds = cmds,
};
