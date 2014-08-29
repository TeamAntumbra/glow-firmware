#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

// Buffers are SPM_PAGESIZE bytes long
void flash_read(uint16_t index, void *dst);
void flash_erase(uint16_t index);
void flash_write(uint16_t index, const void *src);

#endif
