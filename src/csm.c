#include <swilib.h>
#include <stdio.h>
#include <string.h>
#include "ipc.h"
#include "cache.h"
#include "config.h"
#include "keyhook.h"
#include "bookmarks.h"
#include "functions.h"
#include "ui/ui.h"
#include "csm.h"

#define DEFAULT_FREQ 104200
#define DEFAULT_VOLUME 5

enum {
    CSM_STATE_DEFAULT = 0x0,
    CSM_STATE_UPDATE_INFO = 0xA
};

const int minus11 = -11;
unsigned short maincsm_name_body[140];

static int SaveTunerSettings(const MAIN_CSM *csm) {
    CACHE cache;
    cache.volume = csm->tuner.volume.volume;
    cache.is_mute = csm->tuner.volume.is_mute;
    cache.freq = csm->tuner.freq;
    return Cache_Write(&cache);
}

static void LoadTunerSettings(MAIN_CSM *csm) {
    CACHE cache;
    if (Cache_Read(&cache)) {
        const int volume = Tuner_IsValidVolume(cache.volume) ? cache.volume : DEFAULT_VOLUME;
        const uint32_t freq = Tuner_IsValidFreq(cache.freq) ? cache.freq : DEFAULT_FREQ;
        csm->tuner.freq = freq;
        csm->tuner.volume.volume = volume;
        csm->tuner.volume.is_mute = cache.is_mute;
    } else {
        csm->tuner.freq = DEFAULT_FREQ;
        csm->tuner.volume.volume = DEFAULT_VOLUME;
    }
    csm->tuner.volume.obs_volume = Tuner_MapVolume(csm->tuner.volume.volume);
}

static void InitBackground() {
    KeyHook_Init();
    UI_LoadTheme(CFG.theme_type);
}

static void DestroyBackground() {
    KeyHook_Destroy();
    UI_DestroyTheme();
    kill_elf();
}

static void OnCreate(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if (!Tuner_Init(&csm->tuner, csm)) {
        csm->csm.state = CSM_STATE_CLOSED;
        return;
    }
    LoadTunerSettings(csm);
    csm->gui_id = UI_Create(csm);
    SUBPROC(InitBackground);
}

static void OnClose(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if (csm->please_wait_gui_id) {
        GeneralFunc_flag1(csm->please_wait_gui_id, 1);
    }
    GBS_DelTimer(&csm->tmr_set_stereo_status);
    if (!SaveTunerSettings(csm)) {
        MsgBoxError(1, (int)"Failed to save settings");
    }
    Tuner_Destroy(&csm->tuner);
    Config_Save(csm);
    SUBPROC(DestroyBackground);
}

static UI_DATA *GetUIData(const MAIN_CSM *csm) {
    void *gui = FindGUIbyId(csm->gui_id, (CSM_RAM**)&csm);
    return (gui) ? TViewGetUserPointer(gui) : NULL;
}

static void Obs_Prepare_Handler(const HObj hobj) {
    MAIN_CSM *csm = NULL;
    Obs_GetUserPointer(hobj, &csm);
    Obs_Sound_SetPurpose(hobj, 0x21);
    Obs_Mam_SetPurpose(hobj, 0x21);
    if (csm) {
        if (csm->tuner.volume.is_mute) {
            Tuner_SetMute(&csm->tuner, 1);
        } else {
            Tuner_SetVolume(&csm->tuner, csm->tuner.volume.volume);
        }
    }
    Obs_Start(hobj);
}

static void Obs_2_Handler(const HObj hobj) {
    MAIN_CSM *csm = NULL;
    Obs_GetUserPointer(hobj, &csm);
    if (csm) {
        Tuner_SetFreq(csm->tuner.freq);
    }
}

static OBSevent OBSeventHandlers[] = {
    {0x2, Obs_2_Handler},
    {OBS_EV_PrepareCon, Obs_Prepare_Handler},
    {OBS_EV_EndList, NULL},
};

static int OnMessage(CSM_RAM *data, GBS_MSG *msg) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if ((msg->msg == MSG_GUI_DESTROYED) && ((int)msg->data0 == csm->gui_id)) {
        csm->csm.state = CSM_STATE_CLOSED;
    } else if (msg->msg == TUNER_MSG_OBS) {
        if (csm->tuner.hobj && csm->tuner.hobj == (HObj)msg->data0) {
            Obs_TranslateMessageGBS(msg, OBSeventHandlers);
        }
    } else if (msg->msg == MSG_IPC) {
        IPC_REQ *ipc = msg->data0;
        if (strcmpi(ipc->name_to, IPC_NAME) == 0) {
            if (msg->submess == IPC_LOAD_THEME_START) {
                csm->temp.theme_type = (int)ipc->data;
                if (!csm->please_wait_gui_id) {
                    csm->please_wait_gui_id = ShowPleaseWaitBox(1);
                }
            } else if (msg->submess == IPC_LOAD_THEME_END) {
                if (csm->please_wait_gui_id) {
                    GeneralFunc_flag1(csm->please_wait_gui_id, 1);
                    csm->please_wait_gui_id = 0;
                }
                IMGHDR **images = ipc->data;
                if (images) {
                    THEME.images = images;
                    csm->theme_type = csm->temp.theme_type;
                } else {
                    MsgBoxError(0x11, (int)"Failed to load images");
                    if (!THEME.images) {
                        csm->csm.state = CSM_STATE_CLOSED;
                    }
                }
            } else if (msg->submess == IPC_TUNER_SET_POWER) {
                const int power = (int)ipc->data;
                Tuner_SetPower(power);
                CSM_UpdateName(csm);
                DirectRedrawGUI_ID(csm->gui_id);
            } else if (msg->submess == IPC_TUNER_SET_FREQ_TEMP) {
                csm->temp.freq = (uint32_t)ipc->data;
            } else if (msg->submess == IPC_TUNER_SET_FREQ) {
                UI_DATA *ui_data = GetUIData(csm);
                csm->tuner.freq = csm->temp.freq;
                if (ui_data) {
                    ui_data->bm = Bookmarks_Find(csm->tuner.freq);
                    if (IsGuiOnTop(csm->gui_id)) {
                        UI_DrawMainInfo(ui_data);
                    }
                }
                csm->tuner.level = 0;
                // csm->tuner.stereo_status = 0;
                csm->csm.state = CSM_STATE_UPDATE_INFO;
                CSM_UpdateName(csm);
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
            if (csm->theme_type != CFG.theme_type) {
                SUBPROC(UI_LoadTheme, CFG.theme_type);
            }
            ShowMSG(1, (int)"NativeTuner config updated!");
        }
    }
    if (csm->csm.state == CSM_STATE_UPDATE_INFO) {
        Tuner_UpdateCurrentLevel();
        GBS_StartTimerProc(&csm->tmr_set_stereo_status, MsToTicks(100), (void*)Tuner_UpdateStereoStatus);
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

void CSM_UpdateName(MAIN_CSM *csm) {
    WSHDR *ws = (WSHDR *)&MAINCSM.maincsm_name;
    wsprintf(ws, "%c %s", (Tuner_GetPowerState() == 1) ? FONT_ICON_PLAY : FONT_ICON_PAUSE, "FM");
    if (csm && csm->tuner.freq) {
        char s_freq[32];
        Tuner_FreqToStr(s_freq, csm->tuner.freq, 1);
        wstrcatprintf(ws, " %s", s_freq);
    }
}

int CSM_Create() {
    MAIN_CSM main_csm = { 0 };
    return CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
}
