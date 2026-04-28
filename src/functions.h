#pragma once

extern int (*fmdl_set_power)(int power);
extern int (*fmdl_get_power_state)();
extern int (*fmdl_get_stereo_status)(void *cb);
extern int (*fmdl_get_current_level)(void *cb);
extern int (*fmdl_set_freq)(uint32_t freq, int *unk, void *cb);
extern int (*fmdl_auto_search)(uint32_t freq, void *unk, void *cb);

extern int (*Obs_Sound_SetLogSourceEx)(HObj, uint8_t);
extern int (*Obs_GetUserPointer)(HObj, void *);
