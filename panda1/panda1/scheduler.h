#ifndef SCHEDULER_H
#define SCHEDUELR_H

#include "pcb.h"
#include "ash.h"

extern pcb_t *current_proc;
extern struct list_head process_ready_list;
extern int process_count;
extern int blocked_count; 

void scheduler();

#endif