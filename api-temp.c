#include "api-temp.h"
#include "option.h"
#include "proto.h"
#include <string.h>

static void cmd_readsensor(const void *cmdbuf)
{
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
}

static const api_cmd cmds[] = {
    {5, 0, cmd_readsensor},
    {5, 1, cmd_readtemp},
    {5, 2, cmd_readcal},
    {5, 3, cmd_writecal},
};

const api_cmd_list api_core = {
    .ncmds = sizeof cmds / sizeof *cmds,
    .cmds = cmds,
};
