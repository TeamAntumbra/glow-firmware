#ifndef PROTO_H
#define PROTO_H

#include <stdint.h>
#include <stdbool.h>

/* recv and send functions use same buffer. Do not call recv while assembling
   outbound packet. */

const void *proto_recv(uint32_t *api, uint16_t *cmd);
uint32_t proto_get_u32(const void **buf);
uint16_t proto_get_u16(const void **buf);
uint8_t proto_get_u8(const void **buf);
void proto_skip_pad(const void **buf, uint8_t sz);

bool proto_send(uint8_t status, const void *out, uint8_t sz);
void proto_send_start(uint8_t status);
void proto_send_add(const void *out, uint8_t sz);
void proto_send_pad(uint8_t sz);
void proto_send_u32(uint32_t n);
void proto_send_u16(uint16_t n);
void proto_send_u8(uint8_t n);
bool proto_send_end(void);

#endif
