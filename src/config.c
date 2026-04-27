#include <swilib.h>
#include "tuner.h"
#include "config.h"
#include "bookmarks.h"
#include "ui/ui_theme_type.h"

CONFIG CFG = {
    {CFG_CBOX, "Theme", 0, 2},
    UI_THEME_TYPE_BLACK,
    {{"White"}, {"Black"}},
    {CFG_LEVEL, "Bookmarks", 1, 0},
    {CFG_UINT, "Bookmark 1 (kHz)", 0, TUNER_MAX_FREQ},
    0,
    {CFG_UINT, "Bookmark 2 (kHz)", 0, TUNER_MAX_FREQ},
    0,
    {CFG_UINT, "Bookmark 3 (kHz)", 0, TUNER_MAX_FREQ},
    0,
    {CFG_UINT, "Bookmark 4 (kHz)", 0, TUNER_MAX_FREQ},
    0,
    {CFG_UINT, "Bookmark 5 (kHz)", 0, TUNER_MAX_FREQ},
    0,
    {CFG_UINT, "Bookmark 6 (kHz)", 0, TUNER_MAX_FREQ},
    0,
    {CFG_UINT, "Bookmark 7 (kHz)", 0, TUNER_MAX_FREQ},
    0,
    {CFG_UINT, "Bookmark 8 (kHz)", 0, TUNER_MAX_FREQ},
    0,
    {CFG_UINT, "Bookmark 9 (kHz)", 0, TUNER_MAX_FREQ},
    0,
    {CFG_LEVEL, "", 0, 0},
};

char CFG_PATH[] = "?:\\zbin\\etc\\NativeTuner.bcfg";

void Config_Init() {
    CFG_PATH[0] = BCFG_GetDefaultDisk();
    if (BCFG_LoadConfig(CFG_PATH, &CFG, sizeof(CFG)) == -1) {
        BCFG_SaveConfig(CFG_PATH, &CFG, sizeof(CFG));
    }
    Bookmarks_Serialize();
}

int Config_Save(MAIN_CSM *csm) {
    CFG_PATH[0] = BCFG_GetDefaultDisk();
    Bookmarks_Deserialize();
    CFG.theme_type = csm->theme_type;
    return (BCFG_SaveConfig(CFG_PATH, &CFG, sizeof(CFG)) == 0);
}
