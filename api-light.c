#include "api-light.h"

static void cmd_getendpoint(const void *cmdbuf)
{
    proto_send(0, &api_light_endpoint, 1);
}

static const api_cmd cmds[] = {
    {4, 0, cmd_getendpoint},
};

const api_cmd_list api_core = {
    .ncmds = sizeof cmds / sizeof *cmds,
    .cmds = cmds,
};