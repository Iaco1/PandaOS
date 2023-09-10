#include "scheduler.h"

void scheduler() {
    cur_proc = removeProcQ(&ready_queue);

    if (cur_proc == NULL) {
        if (proc_count == 0)
            HALT();

        else if (proc_count > 0 && softblock_count > 0) {
            unsigned int status = getSTATUS();
            interrupts_on(&status);
            plt_off(&status);
            setSTATUS(status);

            WAIT();
            scheduler();
        }

        else if (proc_count > 0 && softblock_count == 0)
            PANIC();
    }

    setTIMER(TIMESLICE);
    LDST(&cur_proc->p_s);
}