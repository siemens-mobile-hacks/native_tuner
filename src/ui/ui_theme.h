#pragma once

#include "ui_theme_type.h"

typedef struct {
    IMGHDR **images;
} UI_THEME;

void UI_LoadTheme(UI_THEME_TYPE theme_type);
void UI_DestroyTheme();

extern UI_THEME THEME;
