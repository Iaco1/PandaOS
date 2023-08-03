/*
    library file to handle interrupts
*/
#include "list.h"
#include <umps3/umps/libumps.h>
#include "pcb.h"
#include "pandos_const.h"
#include "pandos_types.h"
#include <umps3/umps/arch.h>
#include "types.h"
#include "exceptions.h"

#ifndef INTERRUPTS_H
#define INTERRUPTS_H


#define LINE_0_MASK CAUSE_IP(IL_IPI)
#define LINE_1_MASK CAUSE_IP(IL_CPUTIMER)
#define LINE_2_MASK CAUSE_IP(IL_TIMER)
#define LINE_3_MASK CAUSE_IP(IL_DISK)
#define LINE_4_MASK CAUSE_IP(IL_FLASH)
#define LINE_5_MASK CAUSE_IP(IL_ETHERNET)
#define LINE_6_MASK CAUSE_IP(IL_PRINTER)
#define LINE_7_MASK CAUSE_IP(IL_TERMINAL)


extern pcb_t *current_proc;
extern struct list_head *ready_list;
extern int device_sem[DEVICECNT + 1];


void interrupt_handler(unsigned int cause);
void plt(state_t *processor_state);
void non_timer_interrupts(int line);
void timer_interrupt();
void terminal_handling();


#endif
