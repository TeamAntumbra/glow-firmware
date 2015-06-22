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

#ifndef API_H
#define API_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void (*api_cmd_fn)(const void *cmdbuf);

typedef struct {
    uint32_t api;
    uint16_t cmd;
    api_cmd_fn fn;
} api_cmd;

typedef struct {
    size_t ncmds;
    const api_cmd *cmds;
} api_cmd_list;

/* Not reentrant */
bool api_dispatch_packet(const api_cmd_list **lists, size_t nlists);

/* Not to be called outside api_dispatch_packet */
bool api_get_supported(uint32_t api);

#endif
