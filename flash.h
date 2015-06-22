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

#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>
#include <avr/pgmspace.h>

#define FLASH_NUM_PAGES (LOADER_OFFSET / SPM_PAGESIZE)

// Buffers are SPM_PAGESIZE bytes long
void flash_read(uint16_t index, void *dst);
void flash_erase(uint16_t index);
void flash_write(uint16_t index, const void *src);

#endif
