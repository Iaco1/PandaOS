#include "ash.h"
#include "pcb.h"
#include <stdio.h> /* Test */

HIDDEN semd_t *get(int *semAdd) {
    /* Find semaphore bucket in the ASH */
    const u32 key = hash_ptr(semAdd, HASH_BITS(semd_h));
    struct hlist_node *bkt = semd_h[hash_min(key, HASH_BITS(semd_h))].first;
        /* semd_h is hashed the same way the hash_add macro does it */
    return bkt==NULL ? NULL : container_of(bkt, semd_t, s_link);
}

int insertBlocked(int *semAdd, pcb_t *p) {
    semd_t *semd = get(semAdd);
    if (semd==NULL) {
        if (list_empty(&semdFree_h))
            return TRUE;
        /* Extract and remove a semaphore from the semdFree list */
        semd = container_of(semdFree_h.next, semd_t, s_freelink);
        list_del(&semd->s_freelink);
        /* Initialize as active the extracted semaphore */
        semd->s_key = semAdd;
        mkEmptyProcQ(&semd->s_procq);
        hash_add(semd_h, &semd->s_link, hash_ptr(semd->s_key, HASH_BITS(semd_h)));
            /* s_freelink is deliberately left uninitialized.
             * s_link is initialized inside hash_add. */
    }

    p->p_semAdd = semd->s_key;
    insertProcQ(&semd->s_procq, p);
    return FALSE;
}

pcb_t *removeBlocked(int *semAdd) {
    semd_t *semd = get(semAdd);
    if (semd==NULL) return NULL;

    pcb_t *p = removeProcQ(&semd->s_procq);
    p->p_semAdd = NULL;
    if (emptyProcQ(&semd->s_procq)) {
        /* Remove semaphore from the ASH and add it to the semdFree list */
        hash_del(&semd->s_link);
        list_add(&semd->s_freelink, &semdFree_h);
    } return p;
}

pcb_t *outBlocked(pcb_t *p) {
    semd_t *semd = get(p->p_semAdd);
    if (semd==NULL) return NULL;

    outProcQ(&semd->s_procq, p);
    p->p_semAdd = NULL;
    if (emptyProcQ(&semd->s_procq)) {
        hash_del(&semd->s_link);
        list_add(&semd->s_freelink, &semdFree_h);
    } return p;
}

pcb_t *headBlocked(int *semAdd) {
    semd_t *semd = get(semAdd);
    return semd==NULL || emptyProcQ(&semd->s_procq) ?
        NULL : headProcQ(&semd->s_procq);
    /* Return the head of the process queue if
     * the semaphore is found and the queue is not empty */
}

void initASH() {
    INIT_LIST_HEAD(&semdFree_h);
    for (int i = 0; i < MAXPROC; i = i + 1)
        list_add(&semd_table[i].s_freelink, &semdFree_h);
}