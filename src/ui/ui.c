#include <swilib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ui.h"
#include "img.h"
#include "layout.h"
#include "../bookmarks.h"
#include "../functions.h"

#define LGP_PAUSE_PIC (LGP_STOP_PIC - 1)

HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"", LGP_NULL};

const int SOFTKEYS[] = {SET_LEFT_SOFTKEY, SET_MIDDLE_SOFTKEY, SET_RIGHT_SOFTKEY};

static SOFTKEY_DESC SOFTKEY_D[] = {
    {0x0018, 0x0000, (int)""},
    {0x003D, 0x0000, (int)LGP_PAUSE_PIC},
    {0x0001, 0x0000, (int)"Exit"},
};

static SOFTKEYSTAB SOFTKEYS_TAB = {
    SOFTKEY_D, 3
};

void UI_DrawMainInfo(const UI_DATA *data) {
    WSHDR ws;
    char str[32];
    uint16_t wsbody[32];
    CreateLocalWS(&ws, wsbody, 31);

    if (!data->seek_on) {
        sprintf(str, "%.1f", data->tuner->freq / 1000.0);
        str_2ws(&ws, str, 127);
    } else {
        wsprintf(&ws, "...");
    }
    int font = FONT_MEDIUM;
    int x, y, x2, y2, w, h;
    // freq
    x = UI_FREQ_X;
    y = UI_FREQ_Y;
    w = UI_FREQ_W;
    h = GetFontYSIZE(font);
    x2 = x + w;
    y2 = y + h;
    DrawCroppedIMGHDR(x, y, x, y + GLOBAL_OFFSET_Y, w, h, 0, data->images[IMG_BG]);
    DrawString(&ws, x, y, x2, y2, font, TEXT_ALIGNMIDDLE,
        GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(23));
    //bm
    x = UI_BM_X;
    y = UI_BM_Y;
    w = UI_BM_W;
    h = GetFontYSIZE(font);
    x2 = x + w;
    y2 = y + h;
    DrawCroppedIMGHDR(x, y, x, y + GLOBAL_OFFSET_Y, w, h, 0, data->images[IMG_BG]);
    if (data->bm >= 0) {
        wsprintf(&ws, "%d", data->bm + 1);
        DrawString(&ws, x, y, x2, y2, font, TEXT_ALIGNMIDDLE,
            GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(23));
    }
}

void UI_DrawArrowLeft(const UI_DATA *data) {
    DrawIMGHDR(UI_ARROW_LEFT_X, UI_ARROW_LEFT_Y, data->images[IMG_ARROW_LEFT]);
}

void UI_DrawArrowRight(const UI_DATA *data) {
    DrawIMGHDR(UI_ARROW_RIGHT_X, UI_ARROW_RIGHT_Y, data->images[IMG_ARROW_RIGHT]);
}

void UI_DrawArrowUp(const UI_DATA *data) {
    DrawIMGHDR(UI_ARROW_UP_X, UI_ARROW_UP_Y, data->images[IMG_ARROW_UP]);
}

void UI_DrawArrowDown(const UI_DATA *data) {
    DrawIMGHDR(UI_ARROW_DOWN_X, UI_ARROW_DOWN_Y, data->images[IMG_ARROW_DOWN]);
}

void UI_ClearArrowUp(const UI_DATA *data) {
    const int w = data->images[IMG_ARROW_UP]->w;
    const int h = data->images[IMG_ARROW_UP]->h;
    DrawCroppedIMGHDR(UI_ARROW_UP_X, UI_ARROW_UP_Y, UI_ARROW_UP_X, UI_ARROW_UP_Y + GLOBAL_OFFSET_Y,
        w, h, 0, data->images[IMG_BG]);
}

void UI_ClearArrowDown(const UI_DATA *data) {
    const int w = data->images[IMG_ARROW_DOWN]->w;
    const int h = data->images[IMG_ARROW_DOWN]->h;
    DrawCroppedIMGHDR(UI_ARROW_DOWN_X, UI_ARROW_DOWN_Y, UI_ARROW_DOWN_X, UI_ARROW_DOWN_Y + GLOBAL_OFFSET_Y,
        w, h, 0, data->images[IMG_BG]);
}

void UI_DrawVolume(const UI_DATA *data) {
    const int x = UI_VOLUME_X;
    const int y = UI_VOLUME_Y;
    const int w = data->images[IMG_VOLUME]->w;
    const int h = UI_VOLUME_H;
    if (data->tuner->volume.volume) {
        const int offset_y = h * (data->tuner->volume.volume - 1);
        DrawCroppedIMGHDR(x, y, 0, offset_y, w, h, 0, data->images[IMG_VOLUME]);
    } else {
        DrawCroppedIMGHDR(x, y, x, y + GLOBAL_OFFSET_Y, w, h, 0, data->images[IMG_BG]);
    }
    if (data->tuner->volume.is_mute) {
        DrawIMGHDR(UI_VOLUME_MUTE_X, UI_VOLUME_MUTE_Y, data->images[IMG_MUTE]);
    }
}

void UI_DrawLevel(const UI_DATA *data) {
    const int x = UI_LEVEL_X;
    const int y = UI_LEVEL_Y;
    const int w = data->images[IMG_COVERAGE]->w;
    const int h = UI_LEVEL_H;
    if (data->tuner->level) {
        const int offset_y = h * (data->tuner->level - 1);
        DrawCroppedIMGHDR(x, y, 0, offset_y, w, h, 0, data->images[IMG_COVERAGE]);
    } else {
        DrawCroppedIMGHDR(x, y, x, y + GLOBAL_OFFSET_Y, w, h, 0, data->images[IMG_BG]);
    }
}

static void SetCurrentLevel_Proc(void *gui) {
    const UI_DATA *data = TViewGetUserPointer(gui);
    Tuner_SetCurrentLevel();
    GUI_StartTimerProc(gui, data->tmr_set_level, 500, SetCurrentLevel_Proc);
}

static void OnRedraw(GUI *gui) {
    const UI_DATA *data = TViewGetUserPointer(gui);

    DrawCroppedIMGHDR(0, YDISP, 0, YDISP + GLOBAL_OFFSET_Y, ScreenW(), ScreenH() - SoftkeyH(),
        0, data->images[IMG_BG]);
    UI_DrawMainInfo(data);
    UI_DrawVolume(data);
    UI_DrawLevel(data);

    const TVIEW_DESC *tview = gui->definition;
    if (tview->global_hook_proc) {
        tview->global_hook_proc(gui, UI_CMD_REDRAW);
    }

    WIDGET *softkeys = GetDataOfItemByID(gui, 0);
    if (softkeys) {
        softkeys->methods->onRedraw(softkeys);
    }
}

static int OnKey(GUI *gui, GUI_MSG *msg) {
    UI_DATA *data = TViewGetUserPointer(gui);

    const int submess = msg->gbsmsg->submess;
    if (msg->keys == 0x3D) {
        fmdl_set_power(!fmdl_get_power_state());
        DirectRedrawGUI();
        return -1;
    } else if (msg->keys == 0x01) {
        return 1;
    }

    static int lock_seek = 0, lock_bm = 0;
    if (msg->gbsmsg->msg == KEY_DOWN || msg->gbsmsg->msg == LONG_PRESS) {
        if (!data->seek_on) {
            if (submess == '*') {
                if (Tuner_ToggleMute(data->tuner)) {
                    UI_DrawVolume(data);
                }
            } else if (submess == UP_BUTTON) {
                int bm = Bookmarks_GetNext(data->bm);
                if (bm < 0) {
                    bm = Bookmarks_GetFirst();
                }
                if (bm >= 0 && bm != data->bm) {
                    const uint32_t freq = BOOKMARKS[bm];
                    if (Tuner_SetFreq(freq)) {
                        UI_DrawArrowUp(data);
                    }
                } else {
                    UI_ClearArrowUp(data);
                }
            } else if (submess == DOWN_BUTTON) {
                int bm = Bookmarks_GetPrev(data->bm);
                if (bm < 0) {
                    bm = Bookmarks_GetLast();
                }
                if (bm >= 0 && bm != data->bm) {
                    const uint32_t freq = BOOKMARKS[bm];
                    if (Tuner_SetFreq(freq)) {
                        UI_DrawArrowDown(data);
                    }
                } else {
                    UI_ClearArrowDown(data);
                }
            }
        }
        if (msg->gbsmsg->msg == KEY_DOWN) {
            if (submess >= '1' && submess <= '9') {
                lock_bm = 0;
            } else if (submess == LEFT_BUTTON || submess == RIGHT_BUTTON) {
                lock_seek = 0;
            }
        } else { // LONG_PRESS
            if (submess == LEFT_BUTTON) {
                if (!lock_seek) {
                    if (Tuner_Seek(data->tuner->freq, TUNER_SEEK_DOWN)) {
                        UI_DrawArrowLeft(data);
                        lock_seek = 1;
                    }
                }
            } else if (submess == RIGHT_BUTTON) {
                if (!lock_seek) {
                    if (Tuner_Seek(data->tuner->freq, TUNER_SEEK_UP)) {
                        UI_DrawArrowRight(data);
                        lock_seek = 1;
                    }
                }
            } else if (!data->seek_on) {
                if (submess >= '1' && submess <= '9') {
                    if (!lock_bm) {
                        char message[128];
                        const int id = submess - '0' - 1;
                        data->bm = Bookmarks_Set(id, data->tuner->freq);
                        if (data->bm >= 0) {
                            sprintf(message, "Bookmark %d set to %d kHz", data->bm + 1, data->tuner->freq);
                            ShowMSG(1, (int)message);
                        }
                        lock_bm = 1;
                    }
                }
            }
        }
    } else if (msg->gbsmsg->msg == KEY_UP) {
        if (submess == LEFT_BUTTON) {
            if (!lock_seek) {
                if (Tuner_DecFreq(data->tuner)) {
                    UI_DrawMainInfo(data);
                    UI_DrawArrowLeft(data);
                    GUI_StartTimerProc(gui, data->tmr_redraw, 100, (void(*)(void*))DirectRedrawGUI);
                }
            }
        } else if (submess == RIGHT_BUTTON) {
            if (!lock_seek) {
                if (Tuner_IncFreq(data->tuner)) {
                    UI_DrawMainInfo(data);
                    UI_DrawArrowRight(data);
                    GUI_StartTimerProc(gui, data->tmr_redraw, 100, (void(*)(void*))DirectRedrawGUI);
                }
            }
        } else if (submess == UP_BUTTON) {
            UI_ClearArrowUp(data);
        } else if (submess == DOWN_BUTTON) {
            UI_ClearArrowDown(data);
        } else if (!data->seek_on) {
            if (submess >= '1' && submess <= '9') {
                if (!lock_bm) {
                    const int id = submess - '0' - 1;
                    const uint32_t freq = BOOKMARKS[id];
                    if (freq >= TUNER_MIN_FREQ && freq <= TUNER_MAX_FREQ) {
                        Tuner_SetFreq(freq);
                    }
                }
            }
        }
    }
    return -1;
}

static void GHook(GUI *gui, int cmd) {
    UI_DATA *data = TViewGetUserPointer(gui);
    if (cmd == UI_CMD_REDRAW) {
        SOFTKEY_D[1].lgp_id = (fmdl_get_power_state() == 0) ? LGP_PLAY_PIC : LGP_PAUSE_PIC;
        SetMenuSoftKey(gui, &SOFTKEY_D[1], SET_MIDDLE_SOFTKEY);
    } else if (cmd == UI_CMD_CREATE) {
        static GUI_METHODS methods;
        LockSched();
        memcpy(&methods, gui->methods, sizeof(GUI_METHODS));
        methods.onRedraw = OnRedraw;
        gui->methods = &methods;
        UnlockSched();
        data->tmr_redraw = GUI_NewTimer(gui);
        data->tmr_set_level = GUI_NewTimer(gui);
    } else if (cmd == UI_CMD_FOCUS) {
        DisableIDLETMR();
        SetCurrentLevel_Proc(gui);
    } else if (cmd == UI_CMD_UNFOCUS) {
        GUI_DeleteTimer(gui, data->tmr_redraw);
        GUI_DeleteTimer(gui, data->tmr_set_level);
    } else if (cmd == UI_CMD_DESTROY) {
        GUI_DeleteTimer(gui, data->tmr_redraw);
        GUI_DeleteTimer(gui, data->tmr_set_level);
        DestroyImages(&(data->images));
        mfree(data);
    }
}

static TVIEW_DESC TVIEW_D = {
    8,
    OnKey,
    GHook,
    NULL,
    SOFTKEYS,
    &SOFTKEYS_TAB,
    {0, 0, 0, 0},
    FONT_SMALL,
    0x64,
    0x65,
    0,
    0,
};

int UI_Create(TUNER *tuner) {
    IMGHDR **images = LoadImages(UI_STYLE_WHITE);
    if (images) {
        memcpy(&(HEADER_D.rc), GetHeaderRECT(), sizeof(RECT));
        memcpy(&(TVIEW_D.rc), GetMainAreaRECT(), sizeof(RECT));

        UI_DATA *data = malloc(sizeof(UI_DATA));
        zeromem(data, sizeof(UI_DATA));
        data->tuner = tuner;
        data->images = images;

        WSHDR *ws = AllocWS(1);
        void *gui = TViewGetGUI(malloc_adr(), mfree_adr());
        TViewSetDefinition(gui, &TVIEW_D);
        TViewSetText(gui, ws, malloc_adr(), mfree_adr());
        TViewSetUserPointer(gui, data);
        SetHeader(gui, &HEADER_D, malloc_adr());

        return CreateGUI(gui);
    } else {
        MsgBoxError(1, (int)"Failed to load images");
    }
    return -1;
}
