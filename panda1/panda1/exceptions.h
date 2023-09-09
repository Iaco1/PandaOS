#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include "ns.h"
#include "ash.h"
#include "interrupts.h"
#include "scheduler.h"
#include <umps3/umps/libumps.h>
#define PSEUDOCLOCKSEM device_sem[DEVICECNT]
#define DEVREGBASE 0x10000054

extern int process_count;
extern int blocked_count;
extern struct list_head process_ready_list;
extern pcb_t *current_proc;
extern int device_sem[DEVICECNT + 1];

void *memcpy(void *dest, const void *src, unsigned int n);

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

//HIDDEN void interruptHandler(pcb_t* pcb);

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

HIDDEN void P(pcb_t *pcb, int *semAddr);

void V(int *semAddr);

#endif
