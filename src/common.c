#include <stdio.h>

void FreqToStr(char *str, uint32_t freq, int append_units) {
    sprintf(str, "%.1f %s", freq / 1000.0, (append_units) ? " MHz" : "");
}
