#pragma once

#include <swilib.h>
#include "tuner.h"
#include "ui/ui_theme.h"


typedef struct {
    UI_THEME current;
    UI_THEME temp;
} THEME;

typedef struct {
    CSM_RAM csm;
    TUNER tuner;
    THEME theme;
    int gui_id;
    int please_wait_gui_id;
    GBSTMR tmr_set_stereo_status;
    uint32_t freq_tmp;
} MAIN_CSM;

int CSM_Create();
void CSM_UpdateName();
