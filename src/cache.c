#include <swilib.h>
#include <stdio.h>
#include "cache.h"

#define PATH "1:\\NativeTuner.cache"

int Cache_Read(CACHE *cache) {
    FILE *file = fopen(PATH, "r");
    if (file) {
        fseek(file, 0, SEEK_END);
        const int size = ftell(file);
        if (size == sizeof(CACHE)) {
            fseek(file, 0, SEEK_SET);
            if (fread(cache, size, 1, file) == 1) {
                fclose(file);
                return 1;
            }
            fclose(file);
        } else {
            fclose(file);
            uint32_t err;
            sys_unlink(PATH, &err);
        }
    }
    return 0;
}

int Cache_Write(const CACHE *cache) {
    FILE *file = fopen(PATH, "w");
    if (file) {
        const int size = sizeof(CACHE);
        if (fwrite(cache, size, 1, file) == 1) {
            fclose(file);
            return 1;
        }
        fclose(file);
    }
    return 0;
}
