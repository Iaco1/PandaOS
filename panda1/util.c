#include "util.h"

void interrupts_on(unsigned int *status) {
    *status |= IEPON;
    *status |= IMON;
}

void plt_on(unsigned int *status) {
    *status |= TEBITON;
}

void plt_off(unsigned int *status) {
    *status |= TEBITON;
    *status ^= TEBITON;
}

void kernel_mode(unsigned int *status) {
    *status |= USERPON;
    *status ^= USERPON;
}