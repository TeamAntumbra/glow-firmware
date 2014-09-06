#include "api-bootcontrol.h"
#include "option.h"
#include "proto.h"

static void cmd_setboot(const void *cmdbuf)
{
    uint8_t ldrflag = proto_get_u8(&cmdbuf) ? 1 : 0;
    option_set(0xb002104d, &ldrflag, 1);
    proto_send_start(0);
    proto_send_end();
}

static api_cmd cmds[] = {
    {1, 0, cmd_setboot},
};

api_cmd_list api_bootcontrol = {
    .ncmds = sizeof cmds / sizeof *cmds,
    .cmds = cmds,
};
