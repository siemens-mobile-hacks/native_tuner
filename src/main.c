#include <swilib.h>
#include <stdio.h>
#include <string.h>

#include "bookmarks.h"
#include "ipc.h"
#include "tuner.h"
#include "config.h"
#include "keyhook.h"
#include "functions.h"
#include "ui/ui.h"

typedef struct {
    CSM_RAM csm;
    TUNER tuner;
    int gui_id;

    uint32_t freq_tmp;
} MAIN_CSM;

const int minus11 = -11;
unsigned short maincsm_name_body[140];

static void OnCreate(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;

    if (!Tuner_Init(&csm->tuner)) {
        csm->csm.state = CSM_STATE_CLOSED;
        return;
    } else {
        csm->gui_id = UI_Create(&csm->tuner);
        if (csm->gui_id == -1) {
            csm->csm.state = CSM_STATE_CLOSED;
            return;
        }
    }
    KeyHook_Init();
}

static void OnClose(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    Tuner_Destroy(&csm->tuner);
    KeyHook_Destroy();
    SUBPROC(kill_elf);
}

static UI_DATA *GetUIData(const MAIN_CSM *csm) {
    void *gui = FindGUIbyId(csm->gui_id, (CSM_RAM**)&csm);
    return (gui) ? TViewGetUserPointer(gui) : NULL;
}

static int OnMessage(CSM_RAM *data, GBS_MSG *msg) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if ((msg->msg == MSG_GUI_DESTROYED) && ((int)msg->data0 == csm->gui_id)) {
        csm->csm.state = CSM_STATE_CLOSED;
    } else if (msg->msg == 0x7000) {
        Obs_Sound_SetPurpose(csm->tuner.hobj, 0x21);
        Obs_Mam_SetPurpose(csm->tuner.hobj, 0x21);
        Obs_Start(csm->tuner.hobj);
    } else if (msg->msg == MSG_IPC) {
        IPC_REQ *ipc = msg->data0;
        if (strcmpi(ipc->name_to, IPC_NAME) == 0) {
            if (msg->submess == IPC_TUNER_SET_FREQ_TMP) {
                csm->freq_tmp = (uint32_t)ipc->data;
            } else if (msg->submess == IPC_TUNER_SET_FREQ) {
                UI_DATA *ui_data = GetUIData(csm);
                csm->tuner.freq = csm->freq_tmp;
                if (ui_data) {
                    ui_data->bm = Bookmarks_Find(csm->tuner.freq);
                    if (IsGuiOnTop(csm->gui_id)) {
                        UI_DrawMainInfo(ui_data);
                    }
                }
            } else if (msg->submess == IPC_TUNER_START_SEEK) {
                UI_DATA *ui_data = GetUIData(csm);
                if (ui_data) {
                    ui_data->seek_on = 1;
                    if (IsGuiOnTop(csm->gui_id)) {
                        UI_DrawMainInfo(ui_data);
                    }
                    ui_data->bm = -1;
                }
            } else if (msg->submess == IPC_TUNER_STOP_SEEK) {
                UI_DATA *ui_data = GetUIData(csm);
                if (ui_data) {
                    ui_data->seek_on = 0;
                    DirectRedrawGUI_ID(csm->gui_id);
                }
            } else if (msg->submess == IPC_TUNER_SET_LEVEL) {
                const UI_DATA *ui_data = GetUIData(csm);
                csm->tuner.level = (int)ipc->data;
                if (ui_data) {
                    if (IsGuiOnTop(csm->gui_id)) {
                        UI_DrawLevel(ui_data);
                    }
                }
            } else if (msg->submess == IPC_VOL_UP || msg->submess == IPC_VOL_DOWN) {
                const UI_DATA *ui_data = GetUIData(csm);
                const int success = (msg->submess == IPC_VOL_UP) ? Tuner_IncVolume(&csm->tuner) : Tuner_DecVolume(&csm->tuner);
                if (ui_data) {
                    if (success) {
                        if (IsGuiOnTop(csm->gui_id)) {
                            UI_DrawVolume(ui_data);
                        }
                    }
                }
            }
            IPC_DestroyMessage(ipc);
        }
    } else if (msg->msg == MSG_RECONFIGURE_REQ) {
        if (strcmp(CFG_PATH, msg->data0) == 0) {
            Config_Init();
            ShowMSG(1, (int)"NativeTuner config updated!");
        }
    }
    return 1;
}

static const struct {
    CSM_DESC maincsm;
    WSHDR maincsm_name;
} MAINCSM = {
    {
        OnMessage,
        OnCreate,
#ifdef NEWSGOLD
        0,
        0,
        0,
        0,
#endif
        OnClose,
        sizeof(MAIN_CSM),
        1,
        &minus11
    },
    {
    maincsm_name_body,
    NAMECSM_MAGIC1,
    NAMECSM_MAGIC2,
    0x0,
    139,
    0
    }
};

void UpdateCSMname(void) {
    wsprintf((WSHDR *)&MAINCSM.maincsm_name, "%s", "Radio");
}

int main() {
    MAIN_CSM main_csm = { 0 };
    Config_Init();
    LockSched();
    UpdateCSMname();
    CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
    UnlockSched();
    return 0;
}
