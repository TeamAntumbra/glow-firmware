#ifndef OPTION_H
#define OPTION_H

#include <stdint.h>
#include <stdbool.h>

bool option_find(uint32_t opt, uint8_t *len, uint8_t **start);
bool option_get(uint32_t opt, void *out, uint8_t bufsz);
bool option_set(uint32_t opt, const void *in, uint8_t bufsz);

#endif
