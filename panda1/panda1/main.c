#include "ash.h"
#include "ns.h"
#include "pcb.h"
#include "exceptions.h"

#define SEMCOUNT (DEVICECNT + 8 + 1)
extern void test();
extern void uTLB_RefillHandler();

int process_count, blocked_count;

struct list_head process_ready_list;

pcb_t *current_proc;

int device_sem[SEMCOUNT]; /* Semaphore array (additional semaphore for pseudo-clock) */

passupvector_t *pv = (passupvector_t*)PASSUPVECTOR;

void bp12(void){};
void bp13(void){};
void bp14(void){};

int main() {
    /* Populate pass-up vector */
    pv->tlb_refill_handler = (memaddr)uTLB_RefillHandler;
    pv->tlb_refill_stackPtr = KERNELSTACK;
    pv->exception_handler = (memaddr) exception_handler;
    pv->exception_stackPtr = KERNELSTACK;

    /* Level 2 data structures initialization */
    initPcbs();
    initASH();
    initNamespaces();
    
    /* Global variables initialization */
    process_count = 0;
    blocked_count = 0;
    current_proc = NULL;

    mkEmptyProcQ(&process_ready_list);


    for (int i = 0; i < SEMCOUNT; i = i + 1)
        device_sem[i] = 0;

    /* Load interval timer with 100ms */
    LDIT(PSECOND); // TODO: fix timer value
    /* Starting process instantiation */	

    current_proc = allocPcb();
    if(current_proc == NULL){
        //bp13();
    }
    insertProcQ(&process_ready_list,current_proc);
    bp14();
    if(emptyProcQ(&process_ready_list)){
        bp12();
    }
    process_count += 1;
    current_proc->p_s.status ^= (IEPON + IMON + TEBITON); /* Toggle interrupts and local timer */
    RAMTOP(current_proc->p_s.reg_sp);
    current_proc->p_s.pc_epc = (memaddr)test;
    current_proc->p_s.reg_t9 = (memaddr)test;
    current_proc->p_time = 0;
    current_proc->p_parent = NULL;
    current_proc->p_supportStruct = NULL;

    
    /* TODO: call scheduler */
    scheduler();
    return 0;
}
