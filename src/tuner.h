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
    int stereo_status;
} TUNER;

int Tuner_Init(TUNER *tuner, void *user_pointer);
void Tuner_Destroy(TUNER *tuner);

int Tuner_SetPower(int power);
int Tuner_GetPowerState();
void Tuner_TogglePowerAsync();
int Tuner_SetFreqAsync(uint32_t freq);
int Tuner_DecFreqAsync(const TUNER *tuner);
int Tuner_IncFreqAsync(const TUNER *tuner);
int Tuner_IsValidFreq(uint32_t freq);
void Tuner_FreqToStr(char *str, uint32_t freq, int append_units);
int Tuner_SeekAsync(uint32_t start_freq, TunerSeekDirection direction);
int Tuner_MapVolume(int volume);
int Tuner_SetVolume(TUNER *tuner, int volume);
int Tuner_DecVolume(TUNER *tuner);
int Tuner_IncVolume(TUNER *tuner);
int Tuner_IsValidVolume(int volume);
int Tuner_SetMute(TUNER *tuner, int mute);
int Tuner_ToggleMute(TUNER *tuner);
void Tuner_UpdateCurrentLevelAsync();
void Tuner_UpdateStereoStatusAsync();
