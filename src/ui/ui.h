#pragma once

#include "img.h"
#include "../csm.h"

typedef enum {
    IMG_BG,
    IMG_ARROW_LEFT,
    IMG_ARROW_RIGHT,
    IMG_VOLUME,
    IMG_MUTE,
    IMG_COVERAGE,
    IMG_ARROW_UP,
    IMG_ARROW_DOWN,
    IMG_MONO,
    IMG_STEREO,
    IMG_COUNT,
} UI_IMG;

typedef struct {
    MAIN_CSM *csm;
    int bm;
    int seek_on;
    int tmr_redraw;
    int tmr_update_info;
} UI_DATA;

int UI_Create(MAIN_CSM *csm);

void UI_DrawMainInfo(const UI_DATA *data);
void UI_DrawVolume(const UI_DATA *data);
void UI_DrawLevel(const UI_DATA *data);
void UI_DrawStereoStatus(const UI_DATA *data);
