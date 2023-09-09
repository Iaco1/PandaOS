#include "scheduler.h"
#include "pandos_const.h"
#include <umps3/umps/libumps.h>

void bp(void) {}
void bp1(void) {}
void bp2(void) {}
void bp3(void) {}
void bp4(void) {}
void bp5(void) {}
void bp6(void) {}
void bp7(void) {}
void bp8(void) {}
void bp9(void) {}
void bp10(void) {}
void bp11(void) {}


void scheduler()
{
    if(emptyProcQ(&process_ready_list)){
        //bp11();
    }
    current_proc = removeProcQ(&process_ready_list);
    //bp();
    if (current_proc==NULL)
    {
        if(process_count==0)
        {
            //bp1();
            HALT();
        } 
        else if(blocked_count>0 && process_count > 0)
        {
            /*disable plt*/
            //bp5();
            unsigned int status = getSTATUS();
            status ^= IECON | IMON;
            setSTATUS(status); 
            //bp6();
            setTIMER(100000000);
            //bp7();
            WAIT();
            //bp2();
        }
        else if(process_count > 0 && blocked_count == 0)
        {//deadlock detected
        //deadlock detected action: passup or die? kill a pcb?
        //bp3();
        PANIC();
        //bp10();
        }
    }
    
    //bp4();
    setTIMER(TIMESLICE);
    //bp8();
    /*Load the state of the current process*/
    LDST(&(current_proc->p_s));
    //bp9();
} 