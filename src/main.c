#include <swilib.h>
#include "csm.h"
#include "tuner.h"
#include "config.h"

int main() {
    if (!Tuner_GetPowerState()) {
        Config_Init();
        LockSched();
        CSM_UpdateName();
        CSM_Create();
        UnlockSched();
    } else {
        MsgBoxError(1, (int)"Radio already in use");
        SUBPROC(kill_elf);
    }
    return 0;
}
