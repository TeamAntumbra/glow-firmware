#include "api-temp.h"
#include "option.h"
#include "proto.h"
#include <string.h>
#include <avr/io.h>

void api_temp_init(void)
{
    ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0);
    ADCSRB = _BV(MUX5);
    ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
}

static void cmd_readsensor(const void *cmdbuf)
{
    uint16_t val = ADCL;
    val |= (uint16_t)ADCH << 8;
    proto_send_start(0);
    proto_send_pad(2);
    proto_send_u16(val);
    proto_send_end();
}

static void cmd_readtemp(const void *cmdbuf)
{
}

static void cmd_readcal(const void *cmdbuf)
{
    uint8_t outbuf[16];
    memset(outbuf, 0, sizeof outbuf);
    option_get(0x54454d50, outbuf, sizeof outbuf);
    proto_send(0, outbuf, sizeof outbuf);
}

static void cmd_writecal(const void *cmdbuf)
{
    option_set(0x54454d50, cmdbuf, 16);
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
