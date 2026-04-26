#include <swilib.h>
#include <stdio.h>
#include <stdlib.h>
#include "ui.h"
#include "../ipc.h"

IMGHDR **IMAGES = NULL;

#define GetImgPath(filename) "2:\\japp\\fmradio\\fmradio\\images\\" filename

const char *FILE_NAMES[IMG_COUNT] = {
    "_FMR_background.png",
    "_Arrow_left.png",
    "_Arrow_right.png",
    "_Volume.png",
    "_Mute_line.png",
    "_Coverage.png",
    "_Arrow_up.png",
    "_Arrow_down.png",
    "_Mono.png",
    "_Stereo.png"
};

void GetFileName(char *path, UI_IMG img, UI_THEME theme) {
    const char *dir = "2:\\japp\\fmradio\\fmradio\\images";
    sprintf(path, "%s\\%s%s", dir, (theme == UI_THEME_WHITE) ? "W" : "B", FILE_NAMES[img]);
}

void UI_LoadImages(UI_THEME theme) {
    IPC_SendMessage(IPC_LOAD_IMAGES_START, (void*)theme);
    char path[256];
    const size_t size = sizeof(IMGHDR*) * IMG_COUNT;
    IMGHDR **imgs = malloc(size);
    zeromem(imgs, (int)size);
    for (int i = 0; i <IMG_COUNT; i++) {
        GetFileName(path, i, theme);
        IMGHDR *img = CreateIMGHDRFromPngFile(path, 0);
        if (img) {
            imgs[i] = img;
        } else {
            UI_DestroyImages(&imgs);
            break;
        }
    }
    if (imgs) {
        UI_DestroyImages(&IMAGES);
        IMAGES = imgs;
    }
    IPC_SendMessage(IPC_LOAD_IMAGES_END, imgs);
}

void DestroyIMGHDR(IMGHDR **img) {
    if (*img) {
        mfree((*img)->bitmap);
        mfree((*img));
        *img = NULL;
    }
}

void UI_DestroyImages(IMGHDR ***images) {
    if (*images) {
        for (int i = 0; i < IMG_COUNT; i++) {
            DestroyIMGHDR(&(*images)[i]);
        }
        mfree(*images);
        *images = NULL;
    }
}
