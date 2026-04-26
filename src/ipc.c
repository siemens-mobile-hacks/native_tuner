#include <swilib.h>
#include <stdlib.h>

const char *IPC_NAME = "NativeTuner";

void IPC_SendMessage(int submess, void *data) {
    IPC_REQ *ipc = malloc(sizeof(IPC_REQ));
    ipc->name_to = IPC_NAME;
    ipc->name_from = IPC_NAME;
    ipc->data = data;
    GBS_SendMessage(MMI_CEPID, MSG_IPC, submess, ipc);
}

void IPC_DestroyMessage(IPC_REQ *ipc) {
    mfree(ipc);
}
