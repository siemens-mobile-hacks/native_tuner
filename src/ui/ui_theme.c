#include <swilib.h>
#include <stdio.h>
#include <stdlib.h>
#include "ui.h"
#include "../ipc.h"

UI_THEME THEME = { 0 };

#define GetImgPath(filename) "2:\\japp\\fmradio\\fmradio\\images\\" filename

const char *FILE_NAMES[IMG_COUNT] = {
    "FMR_background",
    "Arrow_left",
    "Arrow_right",
    "Volume",
    "Mute_line",
    "Coverage",
    "Arrow_up",
    "Arrow_down",
    "Mono",
    "Stereo"
};

void GetFileName(char *path, UI_IMG img, UI_THEME_TYPE theme_type) {
    const char *dir = "2:\\japp\\fmradio\\fmradio\\images";
    sprintf(path, "%s\\%s_%s.png", dir, (theme_type == UI_THEME_TYPE_WHITE) ? "W" : "B", FILE_NAMES[img]);
}

void DestroyIMGHDR(IMGHDR **img) {
    if (*img) {
        mfree((*img)->bitmap);
        mfree((*img));
        *img = NULL;
    }
}

void DestroyThemeImages(IMGHDR ***images) {
    if (*images) {
        for (int i = 0; i < IMG_COUNT; i++) {
            DestroyIMGHDR(&(*images)[i]);
        }
        mfree(*images);
        *images = NULL;
    }
}

void UI_LoadTheme(UI_THEME_TYPE theme_type) {
    IPC_SendMessage(IPC_LOAD_THEME_START, (void*)theme_type);
    char path[256];
    const size_t size = sizeof(IMGHDR*) * IMG_COUNT;
    IMGHDR **imgs = malloc(size);
    zeromem(imgs, (int)size);
    for (int i = 0; i <IMG_COUNT; i++) {
        GetFileName(path, i, theme_type);
        IMGHDR *img = CreateIMGHDRFromPngFile(path, 0);
        if (img) {
            imgs[i] = img;
        } else {
            DestroyThemeImages(&imgs);
            break;
        }
    }
    if (imgs) {
        DestroyThemeImages(&THEME.images);
        THEME.images = imgs;
    }
    IPC_SendMessage(IPC_LOAD_THEME_END, imgs);
}

void UI_DestroyTheme() {
    DestroyThemeImages(&THEME.images);
}
