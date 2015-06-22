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

#ifndef API_TEMP_H
#define API_TEMP_H

#include "api.h"

// Initialize temperature sensor.
void api_temp_init(void);
uint32_t api_temp_read(void);

// Must implement. The simplest implementation would return api_temp_read().
uint32_t api_temp_reported_temp(void);

extern const api_cmd_list api_temp;

#endif
