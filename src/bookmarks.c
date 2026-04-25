#include <swilib.h>
#include "tuner.h"
#include "config.h"

#define MAX_BOOKMARKS 9

uint32_t BOOKMARKS[MAX_BOOKMARKS];

inline uint32_t ConvertBookmark(uint32_t freq) {
    return (freq < TUNER_MIN_FREQ || freq > TUNER_MAX_FREQ) ? 0 : freq;
}

void Bookmarks_Serialize() {
    BOOKMARKS[0] = ConvertBookmark(CFG.bm0);
    BOOKMARKS[1] = ConvertBookmark(CFG.bm1);
    BOOKMARKS[2] = ConvertBookmark(CFG.bm2);
    BOOKMARKS[3] = ConvertBookmark(CFG.bm3);
    BOOKMARKS[4] = ConvertBookmark(CFG.bm4);
    BOOKMARKS[5] = ConvertBookmark(CFG.bm5);
    BOOKMARKS[6] = ConvertBookmark(CFG.bm6);
    BOOKMARKS[7] = ConvertBookmark(CFG.bm7);
    BOOKMARKS[8] = ConvertBookmark(CFG.bm8);
}

void Bookmarks_Deserialize() {
    CFG.bm0 = BOOKMARKS[0];
    CFG.bm1 = BOOKMARKS[1];
    CFG.bm2 = BOOKMARKS[2];
    CFG.bm3 = BOOKMARKS[3];
    CFG.bm4 = BOOKMARKS[4];
    CFG.bm5 = BOOKMARKS[5];
    CFG.bm6 = BOOKMARKS[6];
    CFG.bm7 = BOOKMARKS[7];
    CFG.bm8 = BOOKMARKS[8];
}

int Bookmarks_Set(int id, uint32_t freq) {
    if (id >= 0 && id < MAX_BOOKMARKS) {
        for (int i = 0; i < MAX_BOOKMARKS; i++) {
            if (BOOKMARKS[i] == freq) {
                BOOKMARKS[i] = 0;
            }
        }
        BOOKMARKS[id] = freq;
        return id;
    }
    return -1;
}

int Bookmarks_Find(uint32_t freq) {
    for (int i = 0; i < MAX_BOOKMARKS; i++) {
        if (BOOKMARKS[i] == freq) {
            return i;
        }
    }
    return -1;
}

int Bookmarks_GetNext(int current) {
    if (current < MAX_BOOKMARKS) {
        for (int i = current + 1; i < MAX_BOOKMARKS; i++) {
            if (BOOKMARKS[i]) {
                return i;
            }
        }
    }
    return -1;
}

int Bookmarks_GetPrev(int current) {
    if (current <= -1 || current >= MAX_BOOKMARKS) {
        current = MAX_BOOKMARKS;
    }
    for (int i = current - 1; i >= 0; i--) {
        if (BOOKMARKS[i]) {
            return i;
        }
    }
    return -1;
}

int Bookmarks_GetFirst() {
    for (int i = 0; i < MAX_BOOKMARKS; i++) {
        if (BOOKMARKS[i]) {
            return i;
        }
    }
    return -1;
}

int Bookmarks_GetLast() {
    for (int i = MAX_BOOKMARKS - 1; i >= 0; i--) {
        if (BOOKMARKS[i]) {
            return i;
        }
    }
    return -1;
}
