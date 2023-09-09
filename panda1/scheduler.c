#include "scheduler.h"
#include <umps3/umps/libumps.h>

void scheduler(){
    pcb_t* tmp = removeProcQ(&ready_list);
    if (tmp==NULL)
    {
        if(process_count==0)
        {
            HALT();
        } 
        else if(blocked_count>0 && process_count > 0)
        {
            /*disable plt*/
            current_proc = NULL;
            setSTATUS(IECON | IMON ); 
            WAIT();
        }
        else if(process_count > 0 && blocked_count == 0)
        {//deadlock detected
        //deadlock detected action: passup or die? kill a pcb?
        PANIC();
        }
    }
    current_proc=tmp;

    setTIMER(TIMESLICE); //load 5 milliseconds
    LDST(&current_proc->p_s);

}