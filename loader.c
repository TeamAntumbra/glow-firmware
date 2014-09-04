#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <string.h>

#include "led.h"
#include "rawusb.h"
#include "flash.h"
#include "proto.h"
#include "option.h"

static const char impl_id[] PROGMEM = "Glow V3 ldr " ANTUMBRA_COMMIT_ID;
static const uint8_t dev_ver[] PROGMEM = {ANTUMBRA_COMMIT_ID_HEX};

static uint8_t flashpagebuf[SPM_PAGESIZE];
#define NUM_FLASH_PAGES (LOADER_OFFSET / SPM_PAGESIZE)

static const uint8_t *get_override_devid(uint8_t *lenout)
{
    uint8_t len;
    uint8_t *start;
    if (!option_find(0x5370001d, &len, &start) || len == 0)
        return NULL;

    uint8_t overridep = eeprom_read_byte(start);
    if (!overridep)
        return NULL;

    *lenout = len - 1;
    return start + 1;
}

int main(void)
{
    cli();
    led_init();
    MCUCR = _BV(IVCE);
    MCUCR = _BV(IVSEL);
    sei();

    DDRB &= ~_BV(DDB2);
    PORTB |= _BV(PORTB2);

    uint8_t forceldr = 0;
    option_get(0xb002104d, &forceldr, 1);

    if (~PINB & _BV(PINB2) || forceldr)
        led_set_rgb(0, 0, 1);
    else {
        cli();
        MCUCR = _BV(IVCE);
        MCUCR = 0;
        __asm__("jmp 0");
    }

    rawusb_init();

    while (1) {
        rawusb_tick();

        uint32_t api;
        uint16_t cmd;
        const void *cmdbuf = proto_recv(&api, &cmd);
        if (cmdbuf) {
            // Echo
            if (api == 0 && cmd == 0)
                proto_send(0, cmdbuf, 56);

            // Query
            else if (api == 0 && cmd == 1) {
                uint32_t qapi = proto_get_u32(&cmdbuf);
                uint8_t sup = (qapi == 0 ||
                               qapi == 1 ||
                               qapi == 2 ||
                               qapi == 3) ? 1 : 0;
                proto_send(0, &sup, 1);
            }

            // Diagnostic
            else if (api == 0 && cmd == 2) {
                proto_send_start(0);
                proto_send_pad(56);
                proto_send_end();
            }

            // Implementation ID
            else if (api == 0 && cmd == 3) {
                proto_send_start(0);
                for (int i = 0; i < sizeof impl_id; ++i)
                    proto_send_u8(pgm_read_byte(impl_id + i));
                proto_send_end();
            }

            // Device ID
            else if (api == 0 && cmd == 4) {
                proto_send_start(0);

                uint8_t devidoptlen;
                const uint8_t *devidopt = get_override_devid(&devidoptlen);

                if (devidopt) {
                    for (int i = 0; i < devidoptlen; ++i)
                        proto_send_u8(eeprom_read_byte(devidopt + i));
                }

                else {
                    for (int i = 0; i < sizeof dev_ver; ++i)
                        proto_send_u8(pgm_read_byte(dev_ver + i));
                    proto_send_u8(SIGNATURE_0);
                    proto_send_u8(SIGNATURE_1);
                    proto_send_u8(SIGNATURE_2);
                    proto_send_u8(OSCCAL);
                }

                proto_send_end();
            }

            // Set Boot
            else if (api == 1 && cmd == 0) {
                uint8_t ldrflag = proto_get_u8(&cmdbuf) ? 1 : 0;
                option_set(0xb002104d, &ldrflag, 1);
                proto_send_start(0);
                proto_send_end();
            }

            // EEPROM Info
            else if (api == 2 && cmd == 0) {
                proto_send_start(0);
                proto_send_u16(OPTION_EEPROM_SIZE);
                proto_send_end();
            }

            // EEPROM Read
            else if (api == 2 && cmd == 1) {
                uint16_t off = proto_get_u16(&cmdbuf);
                uint8_t len = proto_get_u8(&cmdbuf);

                proto_send_start(0);
                if ((uint32_t)off + len > OPTION_EEPROM_SIZE)
                    proto_send_u8(1);
                else if (len > 48)
                    proto_send_u8(2);
                else {
                    proto_send_u8(0);
                    proto_send_pad(7);
                    for (uint8_t i = 0; i < len; ++i)
                        proto_send_u8(eeprom_read_byte((uint8_t *)off + i));
                }
                proto_send_end();
            }

            // EEPROM Write
            else if (api == 2 && cmd == 2) {
                uint16_t off = proto_get_u16(&cmdbuf);
                uint8_t len = proto_get_u8(&cmdbuf);
                proto_skip_pad(&cmdbuf, 5);

                proto_send_start(0);
                if ((uint32_t)off + len > OPTION_EEPROM_SIZE)
                    proto_send_u8(1);
                else if (len > 48)
                    proto_send_u8(2);
                else {
                    for (uint8_t i = 0; i < len; ++i)
                        eeprom_update_byte((uint8_t *)off + i,
                                           proto_get_u8(&cmdbuf));
                    proto_send_u8(0);
                }
                proto_send_end();
            }

            // Flash Info
            else if (api == 3 && cmd == 0) {
                proto_send_start(0);
                proto_send_u16(SPM_PAGESIZE);
                proto_send_u32(NUM_FLASH_PAGES);
                proto_send_end();
            }

            // Flash Buffer Read
            else if (api == 3 && cmd == 1) {
                uint16_t off = proto_get_u16(&cmdbuf);
                uint8_t len = proto_get_u8(&cmdbuf);

                proto_send_start(0);
                if ((uint32_t)off + len > sizeof flashpagebuf)
                    proto_send_u8(1);
                else if (len > 48)
                    proto_send_u8(2);
                else {
                    proto_send_u8(0);
                    proto_send_pad(7);
                    proto_send_add(flashpagebuf + off, len);
                }
                proto_send_end();
            }

            // Flash Buffer Write
            else if (api == 3 && cmd == 2) {
                uint16_t off = proto_get_u16(&cmdbuf);
                uint8_t len = proto_get_u8(&cmdbuf);
                proto_skip_pad(&cmdbuf, 5);

                proto_send_start(0);
                if ((uint32_t)off + len > sizeof flashpagebuf)
                    proto_send_u8(1);
                else if (len > 48)
                    proto_send_u8(2);
                else {
                    memcpy(flashpagebuf + off, cmdbuf, len);
                    proto_send_u8(0);
                }
                proto_send_end();
            }

            // Flash Page Read
            else if (api == 3 && cmd == 3) {
                uint32_t pageidx = proto_get_u32(&cmdbuf);
                proto_send_start(0);
                if (pageidx >= NUM_FLASH_PAGES)
                    proto_send_u8(1);
                else {
                    proto_send_u8(0);
                    flash_read(pageidx, flashpagebuf);
                }
                proto_send_end();
            }

            // Flash Page Write
            else if (api == 3 && cmd == 4) {
                uint32_t pageidx = proto_get_u32(&cmdbuf);
                proto_send_start(0);
                if (pageidx >= NUM_FLASH_PAGES)
                    proto_send_u8(1);
                else {
                    proto_send_u8(0);
                    flash_write(pageidx, flashpagebuf);
                }
                proto_send_end();
            }

            else {
                proto_send_start(1);
                proto_send_end();
            }
        }
    }
}
