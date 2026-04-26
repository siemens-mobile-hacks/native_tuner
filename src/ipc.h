#pragma once

#include <swilib.h>

enum {
    IPC_LOAD_IMAGES_START,
    IPC_LOAD_IMAGES_END,
    //
    IPC_TUNER_SET_FREQ,
    IPC_TUNER_SET_FREQ_TMP,
    IPC_TUNER_START_SEEK,
    IPC_TUNER_STOP_SEEK,
    IPC_TUNER_SET_CURRENT_LEVEL,
    IPC_TUNER_SET_STEREO_STATUS,
    IPC_TUNER_UPDATE_INFO,
    //
    IPC_VOL_UP,
    IPC_VOL_DOWN,
};

void IPC_SendMessage(int submess, void *data);
void IPC_DestroyMessage(IPC_REQ *ipc);

extern const char *IPC_NAME;
