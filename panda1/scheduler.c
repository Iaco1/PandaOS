#include "scheduler.h"
#include <umps3/umps/libumps.h>

void scheduler(){
    pcb_t tmp = removeProcQ(ready_list);
    if (tmp==NULL){
        if(process_count==0) HALT();
        if(blocked_count>0){
            //set Status register to enable interrupts?
            current_proc->p_s.status.TE=0;
            WAIT();
        }
        else //deadlock detected
        //deadlock detected action: passup or die? kill a pcb?
        PANIC();
    }
    current_proc=tmp;

    plt(current_proc->p_s);  //plt(5)?
    LDST(current_proc->p_s);

}