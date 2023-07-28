#include "ash.h"
#include "ns.h"
#include "exceptions.h"

extern void test();

int process_count, blocked_count;

struct list_head *ready_list;

pcb_t *current_proc;

int device_sem[DEVICECNT + 1]; /* Semaphore array (additional semaphore for pseudo-clock) */

passupvector_t *pv = (passupvector_t*)PASSUPVECTOR;

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

    mkEmptyProcQ(ready_list);

    current_proc = NULL;

    for (int i = 0; i < DEVICECNT + 1; i = i + 1)
        device_sem[i] = 0;

    /* Load interval timer with 100ms */
    LDIT(100); // TODO: fix timer value

    /* Starting process instantiation */
    pcb_t *proc = allocPcb();
    process_count += 1;
    insertProcQ(ready_list,proc);
    proc->p_s.status ^= (IEPON + IMON + TEBITON); /* Toggle interrupts and local timer */
    RAMTOP(proc->p_s.reg_sp);
    proc->p_s.pc_epc = (memaddr)test;
    proc->p_s.reg_t9 = (memaddr)test;

    proc->p_parent = NULL;
    proc->p_supportStruct = NULL;

    /* TODO: call scheduler */

    return 0;
}
