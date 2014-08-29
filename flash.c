#include "flash.h"

#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <util/atomic.h>

void flash_read(uint16_t index, void *dst)
{
    uint16_t i;
    for (i = 0; i < SPM_PAGESIZE; ++i)
        ((uint8_t *)dst)[i] = pgm_read_byte(index * SPM_PAGESIZE + i);
}

void flash_erase(uint16_t index) __attribute__((section (".flash_parts")));
void flash_erase(uint16_t index)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        boot_page_erase_safe(index * SPM_PAGESIZE);
        boot_rww_enable_safe();
        boot_spm_busy_wait();
    }
}

void flash_write(uint16_t index, const void *src)
    __attribute__((section (".flash_parts")));
void flash_write(uint16_t index, const void *src)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        boot_page_erase_safe(index * SPM_PAGESIZE);

        uint16_t i;
        for (i = 0; i < SPM_PAGESIZE; i += 2) {
            boot_page_fill_safe(index * SPM_PAGESIZE + i,
                                ((const uint16_t *)src)[i/2]);
        }

        boot_page_write_safe(index * SPM_PAGESIZE);
        boot_rww_enable_safe();
        boot_spm_busy_wait();
    }
}
