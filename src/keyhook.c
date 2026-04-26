#include <swilib.h>
#include "ipc.h"

int KeyHook(int submess, int msg) {
    if (msg == KEY_DOWN || msg == LONG_PRESS) {
        if (submess == VOL_UP_BUTTON) {
            IPC_SendMessage(IPC_VOL_UP, NULL);
            return KEYHOOK_BREAK;
        } else if (submess == VOL_DOWN_BUTTON) {
            IPC_SendMessage(IPC_VOL_DOWN, NULL);
            return KEYHOOK_BREAK;
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