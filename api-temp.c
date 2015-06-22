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

#include "api-temp.h"
#include "option.h"
#include "proto.h"
#include <string.h>
#include <avr/io.h>

#define TEMPCAL_OPTION_ID 0x5443414c // TCAL

static int32_t a_sensor = 0, a_temp = 0, b_sensor = 0, b_temp = 0;

static uint32_t convu32(uint8_t *bufle)
{
    return (uint32_t)bufle[0] << 24 | (uint32_t)bufle[1] << 16 | (uint32_t)bufle[2] << 8 | bufle[3];
}

static void loadcal(void)
{
    uint8_t calbuf[16];
    memset(calbuf, 0, sizeof calbuf);
    option_get(TEMPCAL_OPTION_ID, calbuf, sizeof calbuf);
    a_sensor = convu32(calbuf);
    a_temp = convu32(calbuf + 4);
    b_sensor = convu32(calbuf + 8);
    b_temp = convu32(calbuf + 12);
}

void api_temp_init(void)
{
    ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0);
    ADCSRB = _BV(MUX5);
    ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    loadcal();
}

static uint16_t readsensor(void)
{
    uint16_t val = ADCL;
    val |= (uint16_t)ADCH << 8;
    return val;
}

uint32_t api_temp_read(void)
{
    return ((int32_t)readsensor() - a_sensor) * (b_temp - a_temp) / (b_sensor - a_sensor) + a_temp;
}

static void cmd_readsensor(const void *cmdbuf)
{
    proto_send_start(0);
    proto_send_u32(readsensor());
    proto_send_end();
}

static void cmd_readtemp(const void *cmdbuf)
{
    proto_send_start(0);
    proto_send_u32(api_temp_reported_temp());
    proto_send_end();
}

static void cmd_readcal(const void *cmdbuf)
{
    uint8_t outbuf[16];
    memset(outbuf, 0, sizeof outbuf);
    option_get(TEMPCAL_OPTION_ID, outbuf, sizeof outbuf);
    proto_send(0, outbuf, sizeof outbuf);
}

static void cmd_writecal(const void *cmdbuf)
{
    option_set(TEMPCAL_OPTION_ID, cmdbuf, 16);
    loadcal();
    proto_send_start(0);
    proto_send_end();
}

static const api_cmd cmds[] = {
    {5, 0, cmd_readsensor},
    {5, 1, cmd_readtemp},
    {5, 2, cmd_readcal},
    {5, 3, cmd_writecal},
};

const api_cmd_list api_temp = {
    .ncmds = sizeof cmds / sizeof *cmds,
    .cmds = cmds,
};
