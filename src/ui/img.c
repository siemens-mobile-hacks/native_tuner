#include <stdio.h>
#include <stdlib.h>
#include <swilib.h>
#include "ui.h"

void DestroyImages(IMGHDR ***images);

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

void GetFileName(char *path, UI_IMG img, UI_STYLE style) {
    const char *dir = "2:\\japp\\fmradio\\fmradio\\images";
    sprintf(path, "%s\\%s%s", dir, (style == UI_STYLE_WHITE) ? "W" : "B", FILE_NAMES[img]);
}

IMGHDR **LoadImages(UI_STYLE style) {
    char path[256];
    const size_t size = sizeof(IMGHDR*) * IMG_COUNT;
    IMGHDR **images = malloc(size);
    zeromem(images, (int)size);

    IMGHDR *img = NULL;
    for (int i = 0; i <IMG_COUNT; i++) {
        GetFileName(path, i, style);
        img = CreateIMGHDRFromPngFile(path, 0);
        if (img) {
            images[i] = img;
        } else {
            DestroyImages(&images);
            return NULL;
        }
    }
    return images;
}

void DestroyIMGHDR(IMGHDR **img) {
    if (*img) {
        mfree((*img)->bitmap);
        mfree((*img));
        *img = NULL;
    }
}

void DestroyImages(IMGHDR ***images) {
    for (int i = 0; i < IMG_COUNT; i++) {
        DestroyIMGHDR(&(*images)[i]);
    }
    *images = NULL;
}
