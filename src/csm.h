#pragma once

#include <swilib.h>
#include "tuner.h"
#include "ui/ui_theme_type.h"

typedef struct {
    UI_THEME_TYPE theme_type;
    uint32_t freq;
} TEMP;

typedef struct {
    CSM_RAM csm;
    TUNER tuner;
    UI_THEME_TYPE theme_type;
    int gui_id;
    int please_wait_gui_id;
    GBSTMR tmr_set_stereo_status;
    TEMP temp;
} MAIN_CSM;

int CSM_Create();
void CSM_UpdateName();
