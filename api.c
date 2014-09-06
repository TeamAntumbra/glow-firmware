#include "api.h"
#include "proto.h"

static bool dispatch_list(const api_cmd_list *list, uint32_t api, uint32_t cmd,
                          const void *cmdbuf)
{
    for (size_t i = 0; i < list->ncmds; ++i) {
        if (list->cmds[i].api == api && list->cmds[i].cmd == cmd) {
            (*list->cmds[i].fn)(cmdbuf);
            return true;
        }
    }
    return false;
}

static const api_cmd_list *curlists;
static size_t n_curlists = 0;

bool api_get_supported(uint32_t api)
{
    for (size_t i = 0; i < n_curlists; ++i) {
        for (size_t j = 0; j < curlists[i].ncmds; ++j) {
            if (curlists[i].cmds[j].api == api)
                return true;
        }
    }
    return false;
}

bool api_dispatch_packet(const api_cmd_list *lists, size_t nlists)
{
    uint32_t api;
    uint16_t cmd;
    const void *cmdbuf = proto_recv(&api, &cmd);
    if (!cmdbuf)
        return false;

    curlists = lists;
    n_curlists = nlists;
    for (size_t i = 0; i < nlists; ++i) {
        if (dispatch_list(&lists[i], api, cmd, cmdbuf))
            goto out;
    }

    proto_send_start(1);
    proto_send_end();

 out:
    curlists = NULL;
    n_curlists = 0;
    return true;
}
