#include <swilib.h>
#include "ipc.h"

int KeyHook(int submess, int msg) {
    if (msg == KEY_DOWN || msg == LONG_PRESS) {
        if (submess == VOL_UP_BUTTON) {
            static IPC_REQ ipc;
            IPC_Send(&ipc, IPC_VOL_UP);
            return KEYHOOK_BREAK;
        } else if (submess == VOL_DOWN_BUTTON) {
            static IPC_REQ ipc;
            IPC_Send(&ipc, IPC_VOL_DOWN);
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