#ifndef RAWUSB_H
#define RAWUSB_H

#include <stdbool.h>
#include <stdint.h>

void rawusb_init(void);
void rawusb_tick(void);
bool rawusb_recv_bulk(uint8_t ep, void *out, uint16_t sz);
bool rawusb_send_bulk(uint8_t ep, const void *in, uint16_t sz);

#endif
