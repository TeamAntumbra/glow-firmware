#ifndef API_H
#define API_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void (*api_cmd_fn)(const void *cmdbuf);

typedef struct {
    uint32_t api;
    uint16_t cmd;
    api_cmd_fn fn;
} api_cmd;

typedef struct {
    size_t ncmds;
    const api_cmd *cmds;
} api_cmd_list;

/* Not reentrant */
bool api_dispatch_packet(const api_cmd_list *lists, size_t nlists);

/* Not to be called outside api_dispatch_packet */
bool api_get_supported(uint32_t api);

#endif
