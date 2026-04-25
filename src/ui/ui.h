#pragma once

#include "../tuner.h"

typedef enum {
    UI_STYLE_WHITE,
    UI_STYLE_BLACK,
} UI_STYLE;

typedef enum {
    IMG_BG,
    IMG_ARROW_LEFT,
    IMG_ARROW_RIGHT,
    IMG_VOLUME,
    IMG_MUTE,
    IMG_COVERAGE,
    IMG_ARROW_UP,
    IMG_ARROW_DOWN,
    IMG_COUNT,
} UI_IMG;

typedef enum {
    UI_STATUS_PAUSE,
    UI_STATUS_SEEK,
} UI_STATUS;

typedef struct {
    TUNER *tuner;
    IMGHDR **images;
    int bm;
    int pause;
    int seek_on;
    int tmr_redraw;
    int tmr_set_level;
} UI_DATA;

int UI_Create(TUNER *tuner);

void UI_DrawMainInfo(const UI_DATA *data);
void UI_DrawVolume(const UI_DATA *data);
void UI_DrawLevel(const UI_DATA *data);
