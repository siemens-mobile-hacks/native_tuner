#include <swilib.h>
#include <stdio.h>
#include "ipc.h"
#include "tuner.h"
#include "functions.h"

static void CB_SetFreq() {
    IPC_SendMessage(IPC_TUNER_SET_FREQ, NULL);
}

static void CB_AutoSearch(int state, uint32_t freq) {
    Tuner_SetFreq(freq);
    IPC_SendMessage(IPC_TUNER_SEEK_FINISHED, NULL);
}

static void CB_UpdateCurrentLevel(int state, const uint8_t *level) {
    if ((state == 1 || state == 2) && level) {
        IPC_SendMessage(IPC_TUNER_SET_CURRENT_LEVEL, (void*)(int)*level);
    }
}

static void CB_UpdateStereoStatus(int state, const uint8_t *stereo) {
    if ((state == 1 || state == 2) && stereo) {
        IPC_SendMessage(IPC_TUNER_SET_STEREO_STATUS, (void*)(int)*stereo);
    }
}

int Tuner_SetPower(int power) {
    return fmdl_set_power(power);
}

int Tuner_GetPowerState() {
    return fmdl_get_power_state();
}

void Tuner_TogglePower() {
    IPC_SendMessage(IPC_TUNER_SET_POWER, (void*)!Tuner_GetPowerState());
}

int Tuner_Init(TUNER *tuner, void *user_pointer) {
    zeromem(&tuner->volume, sizeof(TUNER_VOLUME));
    MutexCreate(&tuner->volume.mtx);
    if (Tuner_SetPower(1) == 0) {
        uint32_t err;
        tuner->hobj = Obs_CreateObject(0x4B,0x34,1,TUNER_MSG_OBS,1,0, &err);
        if (tuner->hobj) {
            Obs_SetUserPointer(tuner->hobj, user_pointer);
            Obs_Sound_SetLogSourceEx(tuner->hobj, 3);
            const int status = Obs_Prepare(tuner->hobj);
            if (status == 0 || status == 0x8006) {
                return 1;
            }
        }
    }
    Tuner_SetPower(0);
    Obs_DestroyObject(tuner->hobj);
    tuner->hobj = 0;
    return 0;
}

void Tuner_Destroy(TUNER *tuner) {
    Tuner_SetPower(0);
    Obs_DestroyObject(tuner->hobj);
    MutexDestroy(&tuner->volume.mtx);
}

int Tuner_SetFreq(uint32_t freq) {
    int success = 0;
    freq = ((freq + 50) / 100) * 100; // round
    if (freq < TUNER_MIN_FREQ) {
        freq = TUNER_MIN_FREQ;
    } else if (freq > TUNER_MAX_FREQ) {
        freq = TUNER_MAX_FREQ;
    }
    int unk = 0;
    if (fmdl_set_freq(freq, &unk, CB_SetFreq) == 1) {
        IPC_SendMessage(IPC_TUNER_SET_FREQ_TEMP, (void*)freq);
        success = 1;
    }
    return success;
}

int Tuner_DecFreq(const TUNER *tuner) {
    if (tuner->freq < TUNER_MIN_FREQ + 100) {
        return 0;
    }
    return Tuner_SetFreq(tuner->freq - 100);
}

int Tuner_IncFreq(const TUNER *tuner) {
    if (tuner->freq > TUNER_MAX_FREQ - 100) {
        return 0;
    }
    return Tuner_SetFreq(tuner->freq + 100);
}

int Tuner_IsValidFreq(uint32_t freq) {
    return (freq >= TUNER_MIN_FREQ && freq <= TUNER_MAX_FREQ);
}

void Tuner_FreqToStr(char *str, uint32_t freq, int append_units) {
    sprintf(str, "%.1f %s", freq / 1000.0, (append_units) ? " MHz" : "");
}

int Tuner_Seek(uint32_t start_freq, TunerSeekDirection direction) {
    int success = 0;
    if (direction == TUNER_SEEK_DOWN) {
        start_freq -= 100;
        if (start_freq <= TUNER_MIN_FREQ) {
            start_freq = TUNER_MAX_FREQ - 100;
        }
    } else {
        start_freq += 100;
        if (start_freq >= TUNER_MAX_FREQ) {
            start_freq = TUNER_MIN_FREQ + 100;
        }
    }
    int dir = direction;
    if (fmdl_auto_search(start_freq, &dir, CB_AutoSearch) == 1) {
        IPC_SendMessage(IPC_TUNER_SEEK_START, NULL);
        success = 1;
    }
    return success;
}

int Tuner_MapVolume(int volume) {
    switch (volume) {
        case 0:  return 0;
        case 1:  return 1;
        case 2:  return 2;
        case 3:  return 4;
        case 4:  return 5;
        case 5:  return 7;
        case 6:  return 8;
        case 7:  return 10;
        case 8:  return 11;
        case 9:  return 13;
        case 10: return 15;
        default: return 0;
    }
}

int Tuner_SetVolume(TUNER *tuner, int volume) {
    MutexLock(&tuner->volume.mtx);
    int success = 0;
    const int obs_volume = Tuner_MapVolume(volume);
    const int status = Obs_Sound_SetVolumeEx(tuner->hobj, obs_volume, 1);
    if (status == 0 || status == 0x8006) {
        tuner->volume.volume = volume;
        tuner->volume.obs_volume = obs_volume;
        tuner->volume.is_mute = 0;
        success = 1;
    }
    MutexUnlock(&tuner->volume.mtx);
    return success;
}

int Tuner_DecVolume(TUNER *tuner) {
    int volume = tuner->volume.volume - 1;
    if (volume < 0) {
        volume = 0;
    }
    return Tuner_SetVolume(tuner, volume);
}

int Tuner_IncVolume(TUNER *tuner) {
    int volume = tuner->volume.volume + 1;
    if (volume > 10) {
        volume = 10;
    }
    return Tuner_SetVolume(tuner, volume);
}

int Tuner_IsValidVolume(int volume) {
    return (volume >= 0 && volume <= 10);
}

int Tuner_SetMute(TUNER *tuner, int mute) {
    int success = 0;
    if (Tuner_GetPowerState() == 1) {
        const int volume = (mute) ? 0 : tuner->volume.obs_volume;
        const int status = Obs_Sound_SetVolumeEx(tuner->hobj, volume, 1);
        if (status == 0 || status == 0x8006) {
            tuner->volume.is_mute = mute;
            success = 1;
        }
    }
    return success;
}

int Tuner_ToggleMute(TUNER *tuner) {
    return Tuner_SetMute(tuner, !tuner->volume.is_mute);
}

void Tuner_UpdateCurrentLevel() {
    fmdl_get_current_level(CB_UpdateCurrentLevel);
}

void Tuner_UpdateStereoStatus() {
    fmdl_get_stereo_status(CB_UpdateStereoStatus);
}
