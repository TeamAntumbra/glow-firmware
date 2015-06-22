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

#ifndef OPTION_H
#define OPTION_H

#include <stdint.h>
#include <stdbool.h>

#define OPTION_EEPROM_SIZE 512

bool option_find(uint32_t opt, uint8_t *len, uint8_t **start);
bool option_get(uint32_t opt, void *out, uint8_t bufsz);
bool option_set(uint32_t opt, const void *in, uint8_t bufsz);

#endif
