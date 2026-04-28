#include <stdlib.h>
#include <swilib.h>
#include <string.h>
#include "../tuner.h"

static HEADER_DESC HEADER_D = {{0, 0, 0, 0}, NULL, (int)"Set frequency", LGP_NULL};

static const SOFTKEY_DESC SOFTKEY_D[] = {
    {0x0018, 0x0000, (int)"Set"},
    {0x0000, 0x0000, (int)""},
};

static const SOFTKEYSTAB SOFTKEYS_TAB = {
    SOFTKEY_D, 0,
};

int OnKey(GUI *gui, GUI_MSG *msg) {
    if (msg->keys == 0x18) {
        EDITCONTROL ec;
        char number[8];

        ExtractEditControl(gui, 2, &ec);
        ws_2str(ec.pWS, number, 6);

        uint32_t freq = strtoull(number, NULL, 10);
        if (freq < TUNER_MIN_FREQ || freq > TUNER_MAX_FREQ) {
            MsgBoxError(1, (int)"Frequency is out of range");
        } else {
            Tuner_SetFreq(freq);
            return 1;
        }
    }
    return 0;
}

void GHook(GUI *gui, int cmd) {
    if (cmd == TI_CMD_REDRAW) {
        SetSoftKey(gui, &SOFTKEY_D[0], SET_LEFT_SOFTKEY);
    }
}

static INPUTDIA_DESC INPUTDIA_D = {
    8,
    OnKey,
    GHook,
    NULL,
    0,
    &SOFTKEYS_TAB,
    {0, 0, 0, 0},
    FONT_MEDIUM,
    0x64,
    0x65,
    0,
    0,
    INPUTDIA_FLAGS_SWAP_SOFTKEYS,
};

int SetFreq_Create() {
    memcpy(&(HEADER_D.rc), GetHeaderRECT(), sizeof(RECT));
    memcpy(&(INPUTDIA_D.rc), GetMainAreaRECT(), sizeof(RECT));

    const void *ma = malloc_adr();
    void *eq = AllocEQueue(ma, mfree_adr());

    WSHDR ws;
    EDITCONTROL ec;
    uint16_t wsbody[128];
    CreateLocalWS(&ws, wsbody, 127);
    PrepareEditControl(&ec);

    wsprintf(&ws, "%s", "Frequency (kHz):")
    ConstructEditControl(&ec, ECT_HEADER, ECF_APPEND_EOL, &ws, 127);
    AddEditControlToEditQend(eq, &ec, ma);

    ConstructEditControl(&ec, ECT_NORMAL_NUM, ECF_APPEND_EOL, &ws, 6);
    AddEditControlToEditQend(eq, &ec, ma);

    return CreateInputTextDialog(&INPUTDIA_D, &HEADER_D, eq, 1, NULL);
}
