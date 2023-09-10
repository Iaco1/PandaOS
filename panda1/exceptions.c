#include "exceptions.h"

void exception_handler() {
    /* Update time-of-day */
    STCK(cur_tod);
    cur_proc->p_time += cur_tod - start_tod;

    cur_proc->p_s = *(state_t*) BIOSDATAPAGE;
    switch ((getCAUSE() & GETEXECCODE) >> 2) {

        /* Interrupt */
        case 0:
            interrupt_handler(getCAUSE());
            break;

        /* TLB exception */
        case 1: case 2: case 3:
            pass_up_or_die(PGFAULTEXCEPT);
            break;

        /* Program trap */
        case 4: case 5: case 6: case 7:
        case 9: case 10: case 11: case 12:
            pass_up_or_die(GENERALEXCEPT);

        /* SYSCALL */
        case 8:
            STCK(start_tod);
            syscall_handler();
            STCK(cur_tod);
            cur_proc->p_time += cur_tod - start_tod;
            cur_proc->p_s.pc_epc += WORDLEN;
            cur_proc->p_s.reg_t9 += WORDLEN;
            break;

        default:
            break;
    }
}

HIDDEN void syscall_handler() {
    /* Check if process is in user mode */
    if ((cur_proc->p_s.status & USERPON) == USERPON) {
        /* Write RI (10) in Cause.ExcCode */
        cur_proc->p_s.cause &= CLEAREXECCODE;
        cur_proc->p_s.cause |= 1<<2; cur_proc->p_s.cause |= 1<<4;
        cur_proc->p_s.cause &= ~(1<<1); cur_proc->p_s.cause &= ~(1<<3);
        cur_proc->p_s.cause &= ~(1<<5);

        pass_up_or_die(GENERALEXCEPT);
    }

    memaddr a0 = cur_proc->p_s.reg_a0, a1 = cur_proc->p_s.reg_a1,
            a2 = cur_proc->p_s.reg_a2, a3 = cur_proc->p_s.reg_a3;

    switch (a0) {
        case CREATEPROCESS: {
            pcb_t *proc = allocPcb();
            if (proc == NULL) cur_proc->p_s.reg_v0 = -1;
            else {
                cur_proc->p_s.reg_v0 = 0;
                proc_count += 1;

                /* Process initialization */
                proc->p_s = * (state_t*)a1;
                proc->p_supportStruct = (support_t*)a2;
                insertProcQ(&ready_queue, proc);
                insertChild(cur_proc, proc);
                proc->p_time = 0;
                proc->p_semAdd = NULL;
                proc->p_pid = (memaddr)proc; /* Address of the pcb is used as PID */
                nsd_t *ns = (nsd_t*)a3;
                proc->namespaces[NS_PID] = ns==NULL? cur_proc->namespaces[NS_PID] : ns;
            }
        } break;

        case TERMPROCESS:
            a1==0 ? rec_terminate(cur_proc) : rec_terminate((pcb_t*)a1);
            /* The process with specified PID is found by addressing at the PID itself. */
            break;

        case PASSEREN:
            P(cur_proc, (int*)a1);
            break;

        case VERHOGEN:
            V((int*)a1);
            break;

        case DOIO: {
            int *cmdAddr = (int*)a1; int *cmdValues = (int*)a2;

            int interrupt_line = get_int_line(cmdAddr);
            int *sem = get_dev_sem(cmdAddr);

            P(cur_proc,sem);
            *cmdAddr = *cmdValues;

            /* Extract status code from the device's device register and return accordingly */
            memaddr status_reg = (memaddr)(cmdAddr-WORDLEN);
            int status = * (int*)status_reg;

            if (interrupt_line==TERMINT)
                cur_proc->p_s.reg_v0 = status==5 ? 0 : -1;
            else
                cur_proc->p_s.reg_v0 = status==1 ? 0 : -1;
        } break;

        case GETTIME:
            cur_proc->p_s.reg_v0 = cur_proc->p_time;
            break;

        case CLOCKWAIT:
            P(cur_proc, &PSEUDOCLOCKSEM);
            break;

        case GETSUPPORTPTR:
            cur_proc->p_s.reg_v0 = (memaddr)cur_proc->p_supportStruct;
            break;

        case GETPROCESSID:
            if (a1 == TRUE)
                cur_proc->p_s.reg_v0 = (memaddr)(cur_proc->p_parent==NULL ? 0 : cur_proc->p_parent->p_pid);
            else
                cur_proc->p_s.reg_v0 = (memaddr)cur_proc->p_pid;
            break;

        case GETCHILDREN: {
            int size = (int)a2;
            int *children_pids = (int*)cur_proc->p_s.reg_a1;

            /* Iterate through children of current_proc */
            struct list_head *cur; int i=0;
            list_for_each(cur, &cur_proc->p_child) {
                if (i > size)
                    break;
                int pid = container_of(cur, pcb_t, p_child)->p_pid;
                if (pid == cur_proc->p_pid) {
                    children_pids[i] = pid;
                    i = i + 1;
                }
            } cur_proc->p_s.reg_v0 = i;
        } break;

        default: /* SYSCALL 11 or above */
            pass_up_or_die(GENERALEXCEPT);
    }
}

HIDDEN int get_int_line(int *cmdAddr) {
    /* Get starting address of the given device's device register */
    memaddr base = (memaddr)(cmdAddr - WORDLEN);

    /* Get position of the device in the device registers memory area
     * (integer between 0 and 39) */
    int dev_i = (base - DEVREGBASE) / DEVREGSIZE;

    /* Divide by the number of devices per interrupt line (considering also
     * that the first device interrupt line is 3) */
    return (dev_i / DEVPERINT) + DISKINT;
}

HIDDEN int *get_dev_sem(int *cmdAddr) {
    /* Determine device position as in get_int_line and index
     * the device_sem array accordingly. */
    memaddr base = (memaddr)(cmdAddr - WORDLEN);
    int dev_i = (base - DEVREGBASE) / DEVREGSIZE;
    return &device_sem[dev_i];
}

HIDDEN void pass_up_or_die(int excpt) {
    if (cur_proc->p_supportStruct == NULL) { // Die
        rec_terminate(cur_proc);
    } else { // Pass up
        /* Copy the saved exception state in a location accessible to Support Level */
        cur_proc->p_supportStruct->sup_exceptState[excpt] = * (state_t*)BIOSDATAPAGE;
        /* Pass control */
        context_t tmp = cur_proc->p_supportStruct->sup_exceptContext[excpt];
        LDCXT(tmp.stackPtr,tmp.status,tmp.pc);
    }
}

HIDDEN void rec_terminate(pcb_t *pcb) {
    if (emptyChild(pcb) == TRUE)
        terminate(pcb);
    else {
        while (emptyChild(pcb) == FALSE)
            rec_terminate(removeChild(pcb));
        terminate(pcb);
    }
}

HIDDEN void terminate(pcb_t *pcb) {
    if (pcb!=NULL) {
        outChild(pcb);
        proc_count = proc_count - 1;
        if (pcb->p_semAdd != NULL && !is_device_sem(pcb->p_semAdd)) {
            *pcb->p_semAdd += 1;
            softblock_count -= 1;
        }
    }
}

HIDDEN bool is_device_sem(int* semAdd) {
    for(int i=0; i < DEVICECNT + 1; i = i + 1)
        if (&device_sem[i] == semAdd) return true;
    return false;
}

void P(pcb_t* pcb, int *semAddr) {
    *semAddr -= 1;
    if (*semAddr < 0) {
        cur_proc->p_s.pc_epc += WORDLEN;
        /* Copy the saved processor state into the pcb */
        pcb->p_s = *(state_t*)BIOSDATAPAGE;
        STCK(cur_tod);
        cur_proc->p_time += cur_tod-start_tod;

        insertBlocked(semAddr,pcb);
        *semAddr += 1;

        softblock_count++;
        scheduler();
    }
}

void V(int *semAddr) {
    *semAddr += 1;
    if (headBlocked(semAddr) != NULL) {
        insertProcQ(&ready_queue, removeBlocked(semAddr));
        *semAddr -= 1;
        softblock_count--;
    }
}