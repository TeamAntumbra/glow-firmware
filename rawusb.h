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

#ifndef RAWUSB_H
#define RAWUSB_H

#include <stdbool.h>
#include <stdint.h>

void rawusb_init(void);
void rawusb_tick(void);
bool rawusb_recv_bulk(uint8_t ep, void *out, uint16_t sz);
bool rawusb_send_bulk(uint8_t ep, const void *in, uint16_t sz);

#endif
