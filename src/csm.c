#include <swilib.h>
#include <string.h>
#include "ipc.h"
#include "config.h"
#include "keyhook.h"
#include "bookmarks.h"
#include "ui/ui.h"
#include "csm.h"

#define CSM_STATE_DEFAULT 0
#define CSM_STATE_UPDATE_INFO 0xA

const int minus11 = -11;
unsigned short maincsm_name_body[140];

static void InitBackground() {
    KeyHook_Init();
    UI_LoadImages(UI_THEME_WHITE);
}

static void DestroyBackground() {
    KeyHook_Destroy();
    UI_DestroyImages(&IMAGES);
    kill_elf();
}

static void OnCreate(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if (!Tuner_Init(&csm->tuner)) {
        csm->csm.state = CSM_STATE_CLOSED;
        return;
    }
    csm->gui_id = UI_Create(csm);
    SUBPROC(InitBackground);
}

static void OnClose(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if (csm->please_wait_gui_id) {
        GeneralFunc_flag1(csm->please_wait_gui_id, 1);
    }
    GBS_DelTimer(&csm->tmr_set_stereo_status);
    Tuner_Destroy(&csm->tuner);
    Config_Save();
    SUBPROC(DestroyBackground);
}

static UI_DATA *GetUIData(const MAIN_CSM *csm) {
    void *gui = FindGUIbyId(csm->gui_id, (CSM_RAM**)&csm);
    return (gui) ? TViewGetUserPointer(gui) : NULL;
}

static int OnMessage(CSM_RAM *data, GBS_MSG *msg) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if ((msg->msg == MSG_GUI_DESTROYED) && ((int)msg->data0 == csm->gui_id)) {
        csm->csm.state = CSM_STATE_CLOSED;
    } else if (msg->msg == TUNER_MSG_OBS) {
        Obs_Sound_SetPurpose(csm->tuner.hobj, 0x21);
        Obs_Mam_SetPurpose(csm->tuner.hobj, 0x21);
        Obs_Start(csm->tuner.hobj);
        Tuner_SetFreq(104200);
    } else if (msg->msg == MSG_IPC) {
        IPC_REQ *ipc = msg->data0;
        if (strcmpi(ipc->name_to, IPC_NAME) == 0) {
            if (msg->submess == IPC_LOAD_IMAGES_START) {
                csm->theme.temp = (int)ipc->data;
                if (!csm->please_wait_gui_id) {
                    csm->please_wait_gui_id = ShowPleaseWaitBox(1);
                }
            } else if (msg->submess == IPC_LOAD_IMAGES_END) {
                if (csm->please_wait_gui_id) {
                    GeneralFunc_flag1(csm->please_wait_gui_id, 1);
                    csm->please_wait_gui_id = 0;
                }
                IMGHDR **images = ipc->data;
                if (images) {
                    IMAGES = images;
                    csm->theme.current = csm->theme.temp;
                } else {
                    MsgBoxError(0x11, (int)"Failed to load images");
                    if (!IMAGES) {
                        csm->csm.state = CSM_STATE_CLOSED;
                    }
                }
            } else if (msg->submess == IPC_TUNER_SET_FREQ_TEMP) {
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
                csm->tuner.level = 0;
                // csm->tuner.stereo_status = 0;
                csm->csm.state = CSM_STATE_UPDATE_INFO;
            } else if (msg->submess == IPC_TUNER_SEEK_START) {
                UI_DATA *ui_data = GetUIData(csm);
                if (ui_data) {
                    ui_data->seek_on = 1;
                    if (IsGuiOnTop(csm->gui_id)) {
                        UI_DrawMainInfo(ui_data);
                    }
                    ui_data->bm = -1;
                }
            } else if (msg->submess == IPC_TUNER_SEEK_FINISHED) {
                UI_DATA *ui_data = GetUIData(csm);
                if (ui_data) {
                    ui_data->seek_on = 0;
                    DirectRedrawGUI_ID(csm->gui_id);
                }
            } else if (msg->submess == IPC_TUNER_SET_CURRENT_LEVEL) {
                const UI_DATA *ui_data = GetUIData(csm);
                csm->tuner.level = (int)ipc->data;
                if (ui_data) {
                    if (IsGuiOnTop(csm->gui_id)) {
                        UI_DrawLevel(ui_data);
                    }
                }
            } else if (msg->submess == IPC_TUNER_SET_STEREO_STATUS) {
                const UI_DATA *ui_data = GetUIData(csm);
                csm->tuner.stereo_status = (int)ipc->data;
                if (ui_data) {
                    if (IsGuiOnTop(csm->gui_id)) {
                        UI_DrawStereoStatus(ui_data);
                    }
                }
            } else if (msg->submess == IPC_TUNER_UPDATE_INFO) {
                csm->csm.state = CSM_STATE_UPDATE_INFO;
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
            UI_DATA *ui_data = GetUIData(csm);
            if (ui_data) {
                ui_data->bm = Bookmarks_Find(csm->tuner.freq);
            }
            ShowMSG(1, (int)"NativeTuner config updated!");
        }
    }
    if (csm->csm.state == CSM_STATE_UPDATE_INFO) {
        Tuner_SetCurrentLevel();
        GBS_StartTimerProc(&csm->tmr_set_stereo_status, MsToTicks(100), (void*)Tuner_SetStereoStatus);
        csm->csm.state = CSM_STATE_DEFAULT;
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

void CSM_UpdateName() {
    wsprintf((WSHDR *)&MAINCSM.maincsm_name, "%s", "Radio");
}

int CSM_Create() {
    MAIN_CSM main_csm = { 0 };
    return CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
}
