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

#include "api-light.h"
#include "proto.h"

static void cmd_getendpoint(const void *cmdbuf)
{
    proto_send(0, &api_light_endpoint, 1);
}

static const api_cmd cmds[] = {
    {4, 0, cmd_getendpoint},
};

const api_cmd_list api_light = {
    .ncmds = sizeof cmds / sizeof *cmds,
    .cmds = cmds,
};
