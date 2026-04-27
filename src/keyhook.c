#include <swilib.h>
#include "ipc.h"
#include "tuner.h"

int KeyHook(int submess, int msg) {
    if (msg == KEY_DOWN || msg == LONG_PRESS) {
        if (submess == VOL_UP_BUTTON) {
            if (Tuner_GetPowerState() == 1) {
                IPC_SendMessage(IPC_VOL_UP, NULL);
                return KEYHOOK_BREAK;
            }
        } else if (submess == VOL_DOWN_BUTTON) {
            if (Tuner_GetPowerState() == 1) {
                IPC_SendMessage(IPC_VOL_DOWN, NULL);
                return KEYHOOK_BREAK;
            }
        }
    }
    return KEYHOOK_NEXT;
}

void KeyHook_Init() {
    AddKeybMsgHook(KeyHook);
}

void KeyHook_Destroy() {
    RemoveKeybMsgHook(KeyHook);
}