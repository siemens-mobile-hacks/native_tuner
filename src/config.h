#pragma once

#include <cfg_items.h>
#include "csm.h"

#pragma pack(push, 1)
typedef struct {
    const CFG_HDR cfghdr_0;
    unsigned int has_red_theme;
    const CFG_HDR cfghdr_1;
    unsigned int theme_type;
    const CFG_CBOX_ITEM cfgcbox_1[3];
    const CFG_HDR cfghdr_b_b;
    const CFG_HDR cfghdr_b_0;
    unsigned int bm0;
    const CFG_HDR cfghdr_b_1;
    unsigned int bm1;
    const CFG_HDR cfghdr_b_2;
    unsigned int bm2;
    const CFG_HDR cfghdr_b_3;
    unsigned int bm3;
    const CFG_HDR cfghdr_b_4;
    unsigned int bm4;
    const CFG_HDR cfghdr_b_5;
    unsigned int bm5;
    const CFG_HDR cfghdr_b_6;
    unsigned int bm6;
    const CFG_HDR cfghdr_b_7;
    unsigned int bm7;
    const CFG_HDR cfghdr_b_8;
    unsigned int bm8;
    const CFG_HDR cfghdr_b_e;
} CONFIG;
#pragma pack(pop)

extern CONFIG CFG;
extern char CFG_PATH[];

void Config_Init();
int Config_Save(MAIN_CSM *csm);
