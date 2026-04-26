#pragma once

#include <swilib.h>

enum {
    IPC_LOAD_THEME_START,
    IPC_LOAD_THEME_END,
    //
    IPC_TUNER_SET_FREQ,
    IPC_TUNER_SET_FREQ_TEMP,
    IPC_TUNER_SEEK_START,
    IPC_TUNER_SEEK_FINISHED,
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
