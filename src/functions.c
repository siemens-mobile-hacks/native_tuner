#include <swilib.h>

int (*fmdl_set_power)(int) = (int (*)(int power))(0xA0577988);
int (*fmdl_get_power_state)() = (int (*)())(0xA0577AA4);
int (*fmdl_get_current_level)(void *) = (int (*)(void *cb))(0xA05792FC);
int (*fmdl_get_stereo_status)(void *) = (int (*)(void *cb))(0xA0579120);
int (*fmdl_set_freq)(uint32_t, int *, void *) = (int (*)(uint32_t freq, int *unk, void *cb))(0xA0578A3C);
int (*fmdl_auto_search)(uint32_t, void *, void *) = (int (*)(uint32_t freq, void *unk, void *cb))(0xA0578BD4);

int (*Obs_Sound_SetLogSourceEx)(HObj, uint8_t) = (int (*)(HObj, uint8_t))(0xA0CC3964);
