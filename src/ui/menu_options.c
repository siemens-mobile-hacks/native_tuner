#include <stdio.h>
#include <swilib.h>
#include <string.h>
#include "ui.h"
#include "set_freq.h"
#include "../csm.h"
#include "../config.h"

enum {
    ITEM_SET_FREQ,
    ITEM_THEME,
    ITEM_SETTINGS,
    ITEM_EXIT,
    ITEMS_N,
};

static HEADER_DESC HEADER_D = {{0, 0, 0, 0},NULL, (int)"Options", LGP_NULL};

static const int SOFTKEYS[] = {SET_LEFT_SOFTKEY, SET_RIGHT_SOFTKEY, SET_MIDDLE_SOFTKEY};

static const SOFTKEY_DESC SOFTKEYS_D[]= {
    {0x0018,0x0000,(int)"Select"},
    {0x0001,0x0000,(int)"Back"},
    {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
    SOFTKEYS_D,0
};

static void SetFreq_Proc(GUI *gui) {
    SetFreq_Create();
    GeneralFuncF1(1);
}

static void Theme_Proc(GUI *gui) {
    void *main_gui = MenuGetUserPointer(gui);
    const UI_DATA *ui_data = TViewGetUserPointer(main_gui);
    SUBPROC(UI_LoadTheme, (ui_data->csm->theme_type == UI_THEME_TYPE_WHITE) ? UI_THEME_TYPE_BLACK : UI_THEME_TYPE_WHITE);
    GeneralFuncF1(1);
}

static void Settings_Proc(GUI *gui) {
    WSHDR ws;
    uint16_t wsbody[128];
    CreateLocalWS(&ws, wsbody, 127);
    str_2ws(&ws, CFG_PATH, 127);
    ExecuteFile(&ws, NULL, NULL);
    GeneralFuncF1(1);
}

static void Exit_Proc(GUI *gui) {
    void *main_gui = MenuGetUserPointer(gui);
    const UI_DATA *ui_data = TViewGetUserPointer(main_gui);

    GeneralFuncF1(1);
    GeneralFunc_flag1(ui_data->csm->gui_id, 1);
}

static int ICON[] = {ICON_EMPTY};

static MENUITEM_DESC ITEMS[ITEMS_N] = {
    {ICON, (int)"Set frequency", LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
    {ICON, (int)"Theme", LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
    {ICON, (int)"Settings", LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
    {ICON, (int)"Exit", LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}
};

static const MENUPROCS_DESC PROCS[ITEMS_N] =  {
    SetFreq_Proc,
    Theme_Proc,
    Settings_Proc,
    Exit_Proc
};

static void ItemProc(void *gui, int item_n, void *user_pointer) {
    const UI_DATA *ui_data = TViewGetUserPointer(user_pointer);
    const MENUITEM_DESC *desc = &ITEMS[item_n];
    void *item = AllocMenuItem(gui);
    WSHDR *ws = AllocMenuWS(gui, 128);
    if (item_n == ITEM_THEME) {
        wsprintf(ws, "%t [%s]", desc->lgp_id_small, (ui_data->csm->theme_type == UI_THEME_TYPE_WHITE) ? "White" : "Black");
    } else {
        wsprintf(ws, "%t", desc->lgp_id_small);
    }
    SetMenuItemIconArray(gui, item, desc->icon);
    SetMenuItemIcon(gui, item_n, 0);
    SetMenuItemText(gui, item, ws, item_n);
}

static int OnKey(GUI *gui, GUI_MSG *msg) {
    if (msg->keys == 0x3D) {
        const int item_n = GetCurMenuItem(gui);
        PROCS[item_n](gui);
    }
    return 0;
}

static const MENU_DESC MENU_D = {
    8,
    OnKey,
    NULL,
    NULL,
    SOFTKEYS,
    &SOFTKEYS_TAB,
    MENU_FLAGS_ENABLE_TEXT_SCROLLING | MENU_FLAGS_ENABLE_ICONS,
    ItemProc,
    NULL,
    NULL,
    ITEMS_N
};

int MenuOptions_Create(GUI *gui) {
    memcpy(&(HEADER_D.rc), GetOptionsHeaderRECT(), sizeof(RECT));
    return CreateMenu(1, 0, &MENU_D, &HEADER_D, 0,ITEMS_N, gui, 0);
}
