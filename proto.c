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

#include "proto.h"
#include "rawusb.h"

#include <stdlib.h>
#include <string.h>

static uint8_t globuf[64];
static uint8_t buildptr;

const void *proto_recv(uint32_t *api, uint16_t *cmd)
{
    if (!rawusb_recv_bulk(0x01, globuf, sizeof globuf))
        return NULL;

    const void *buf = globuf;
    *api = proto_get_u32(&buf);
    *cmd = proto_get_u16(&buf);
    proto_skip_pad(&buf, 2);
    return buf;
}

uint32_t proto_get_u32(const void **buf)
{
    const uint8_t *bs = *buf;
    *buf = bs + 4;
    return ((uint32_t)bs[0] << 24 | (uint32_t)bs[1] << 16 |
            (uint32_t)bs[2] << 8 | (uint32_t)bs[3]);
}

uint16_t proto_get_u16(const void **buf)
{
    const uint8_t *bs = *buf;
    *buf = bs + 2;
    return (uint16_t)bs[0] << 8 | (uint16_t)bs[1];
}

uint8_t proto_get_u8(const void **buf)
{
    const uint8_t *bs = *buf;
    *buf = bs + 1;
    return bs[0];
}

void proto_skip_pad(const void **buf, uint8_t sz)
{
    *buf = (uint8_t *)*buf + sz;
}

bool proto_send(uint8_t status, const void *out, uint8_t sz)
{
    proto_send_start(status);
    proto_send_add(out, sz);
    return proto_send_end();
}

void proto_send_start(uint8_t status)
{
    buildptr = 0;
    proto_send_u8(status);
    proto_send_pad(7);
}

void proto_send_add(const void *out, uint8_t sz)
{
    if (sz > sizeof globuf - buildptr)
        sz = sizeof globuf - buildptr;
    memcpy(globuf + buildptr, out, sz);
    buildptr += sz;
}

void proto_send_pad(uint8_t sz)
{
    if (sz > sizeof globuf - buildptr)
        sz = sizeof globuf - buildptr;
    memset(globuf + buildptr, 0, sz);
    buildptr += sz;
}

void proto_send_u32(uint32_t n)
{
    if (buildptr < sizeof globuf) globuf[buildptr++] = n >> 24 & 0xff;
    if (buildptr < sizeof globuf) globuf[buildptr++] = n >> 16 & 0xff;
    if (buildptr < sizeof globuf) globuf[buildptr++] = n >> 8 & 0xff;
    if (buildptr < sizeof globuf) globuf[buildptr++] = n & 0xff;
}

void proto_send_u16(uint16_t n)
{
    if (buildptr < sizeof globuf) globuf[buildptr++] = n >> 8 & 0xff;
    if (buildptr < sizeof globuf) globuf[buildptr++] = n & 0xff;
}

void proto_send_u8(uint8_t n)
{
    if (buildptr < sizeof globuf) globuf[buildptr++] = n;
}

bool proto_send_end(void)
{
    proto_send_pad(56);
    return rawusb_send_bulk(0x82, globuf, sizeof globuf);
}
