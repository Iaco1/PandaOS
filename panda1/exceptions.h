#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include "pandos_const.h"
#include "pandos_types.h"
#include "pcb.h"
#include "ash.h"
#include "scheduler.h"
#include "interrupts.h"
#include <umps3/umps/libumps.h>

/* Plus 8 subdevices for terminals and 1 for clock */
#define SEMCOUNT (DEVICECNT + 8 + 1)
#define PSEUDOCLOCKSEM device_sem[SEMCOUNT]
#define DEVREGBASE 0x10000054

extern pcb_t *cur_proc;
extern struct list_head ready_queue;
extern int proc_count, softblock_count;
extern cpu_t start_tod;

cpu_t cur_tod;

void exception_handler();

HIDDEN void syscall_handler();

/**
 * Determine interrupt line from given command address.
 * @param cmdAddr command address of the device
 * @return interrupt line of the device
 */
HIDDEN int get_int_line(int *cmdAddr);

/**
 * Determine device semaphore from given command address.
 * @param cmdAddr command address of the device
 * @return device semaphore address
 */
HIDDEN int *get_dev_sem(int *cmdAddr);

HIDDEN void pass_up_or_die(int excpt);

/**
 * Terminates a process and all its progeny.
 * @param pcb root of the process tree to kill
 */
HIDDEN void rec_terminate(pcb_t *pcb);

/**
 * Subroutine for rec_terminate. Kills a single process.
 * @param pcb process to kill
 */
HIDDEN void terminate(pcb_t *pcb);

/**
 * Check if a given semaphore is a device semaphore.
 * @param semAdd semaphore address
 * @return true if one of the semaphores in device_sem has address semAdd, else false
 */
HIDDEN bool is_device_sem(int* semAdd);

void P(pcb_t *pcb, int *semAddr);

void V(int *semAddr);

#endif