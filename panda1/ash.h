#include "pandos_types.h"
#include "hashtable.h"

HIDDEN semd_t semd_table[MAXPROC];

HIDDEN DEFINE_HASHTABLE(semd_h, 5);

HIDDEN struct list_head semdFree_h;

/**
 * Find semaphore with key semAdd in the ASH.
 * @param semAdd semaphore key
 * @return pointer to the semd_t; NULL if absent
 */
HIDDEN semd_t *get(int *semAdd);

int insertBlocked(int *semAdd, pcb_t *p);

pcb_t *removeBlocked(int *semAdd);

pcb_t *outBlocked(pcb_t *p);

pcb_t *headBlocked(int *semAdd);

void initASH();