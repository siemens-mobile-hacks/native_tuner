#pragma once

#include <swilib.h>

#define TUNER_MSG_OBS 0x9990
#define TUNER_MIN_FREQ 87500
#define TUNER_MAX_FREQ 108000

typedef enum {
    TUNER_SEEK_DOWN,
    TUNER_SEEK_UP,
} TunerSeekDirection;

typedef struct {
    MUTEX mtx;
    int volume;
    int obs_volume;
    int is_mute;
} TUNER_VOLUME;

typedef struct {
    HObj hobj;
    uint32_t freq;
    TUNER_VOLUME volume;
    int level;
} TUNER;

int Tuner_Init(TUNER *tuner);
void Tuner_Destroy(TUNER *tuner);

int Tuner_SetPower(int power);
int Tuner_GetPowerState();
int Tuner_SetFreq(uint32_t freq);
int Tuner_DecFreq(const TUNER *tuner);
int Tuner_IncFreq(const TUNER *tuner);
int Tuner_Seek(uint32_t start_freq, TunerSeekDirection direction);
int Tuner_SetVolume(TUNER *tuner, int volume);
int Tuner_DecVolume(TUNER *tuner);
int Tuner_IncVolume(TUNER *tuner);
int Tuner_ToggleMute(TUNER *tuner);;
void Tuner_SetCurrentLevel();
