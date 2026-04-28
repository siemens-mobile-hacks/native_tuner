#pragma once

#include <swilib.h>

#pragma pack(push, 1)
typedef struct {
    int volume;
    int is_mute;
    uint32_t freq;
} CACHE;
#pragma pack(pop)

int Cache_Read(CACHE *cache);
int Cache_Write(const CACHE *cache);
