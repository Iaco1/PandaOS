#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "pandos_types.h"
#include "pcb.h"
#include "util.h"
#include <umps3/umps/libumps.h>

extern pcb_t *cur_proc;
extern struct list_head ready_queue;
extern int proc_count, softblock_count;

void scheduler();

#endif