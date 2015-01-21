#include "api-bootcontrol.h"
#include "option.h"
#include "proto.h"

#define FORCELOADER_OPTION_ID 0x424f4f54 // BOOT

static void cmd_setboot(const void *cmdbuf)
{
    uint8_t ldrflag = proto_get_u8(&cmdbuf) ? 1 : 0;
    option_set(FORCELOADER_OPTION_ID, &ldrflag, 1);
    proto_send_start(0);
    proto_send_end();
}

bool api_bootcontrol_loaderforced(void)
{
    uint8_t forceldr = 1;
    option_get(FORCELOADER_OPTION_ID, &forceldr, 1);
    return forceldr != 0;
}

static const api_cmd cmds[] = {
    {1, 0, cmd_setboot},
};

const api_cmd_list api_bootcontrol = {
    .ncmds = sizeof cmds / sizeof *cmds,
    .cmds = cmds,
};
