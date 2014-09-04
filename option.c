#include "option.h"

#include <avr/eeprom.h>
#include <util/atomic.h>

bool option_find(uint32_t opt, uint8_t *len, uint8_t **start)
{
    for (uint16_t i = 0; i < OPTION_EEPROM_SIZE;) {
        uint8_t optb[4];
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            eeprom_read_block(optb, (uint8_t *)i, 4);
        }

        if (optb[0] == 0xff && optb[1] == 0xff &&
            optb[2] == 0xff && optb[3] == 0xff)
            break;

        uint8_t optl;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            optl = eeprom_read_byte((uint8_t *)i + 4);
        }

        if (optb[0] == (opt >> 24 & 0xff) && optb[1] == (opt >> 16 & 0xff) &&
            optb[2] == (opt >> 8 & 0xff) && optb[3] == (opt & 0xff)) {
            *len = optl;
            *start = (uint8_t *)i + 5;
            return true;
        }

        i += 5 + optl;
    }
    return false;
}

bool option_get(uint32_t opt, void *out, uint8_t bufsz)
{
    uint8_t realsz;
    uint8_t *start;
    if (!option_find(opt, &realsz, &start))
        return false;

    uint8_t copysz = bufsz < realsz ? bufsz : realsz;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        eeprom_read_block(out, start, copysz);
    }
    return true;
}

bool option_set(uint32_t opt, const void *in, uint8_t bufsz)
{
    uint8_t realsz;
    uint8_t *start;
    if (!option_find(opt, &realsz, &start))
        return false;

    uint8_t copysz = bufsz < realsz ? bufsz : realsz;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        eeprom_update_block(in, start, copysz);
    }
    return true;
}
