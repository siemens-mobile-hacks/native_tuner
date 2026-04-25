#pragma once

#include <swilib.h>

enum {
    IPC_TUNER_SET_FREQ,
    IPC_TUNER_SET_FREQ_TMP,
    IPC_TUNER_START_SEEK,
    IPC_TUNER_STOP_SEEK,
    IPC_TUNER_SET_LEVEL,
    //
    IPC_VOL_UP,
    IPC_VOL_DOWN,
};

void IPC_Send(IPC_REQ *ipc, int submess);

extern const char *IPC_NAME;
