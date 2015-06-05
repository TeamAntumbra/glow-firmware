#ifndef API_TEMP_H
#define API_TEMP_H

#include "api.h"

// Initialize temperature sensor.
void api_temp_init(void);
uint32_t api_temp_read(void);

// Must implement. The simplest implementation would return api_temp_read().
uint32_t api_temp_reported_temp(void);

extern const api_cmd_list api_temp;

#endif
