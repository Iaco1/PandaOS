#ifndef UTIL_H
#define UTIL_H
#include "pandos_const.h"

void interrupts_on(unsigned int *status);

void plt_on(unsigned int *status);

void plt_off(unsigned int *status);

void kernel_mode(unsigned int *status);

#endif