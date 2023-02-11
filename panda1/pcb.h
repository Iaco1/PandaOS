
#ifndef pcb_h
#define pcb_h
#include "pandos_types.h"
#include "pandos_const.h"

void initPcbs();
void freePcbs(pcb_t *p);
pcb_t *allocPcb();
void  mkEmptyProcQ(pcb_t * p);
int emptyProcQ(struct list_head *head);
void insertProcQ(struct list_head* head, pcb_t *p);
pcb_t headProcQ(struct list_head* head);
pcb_t* removeProcQ(struct list_head* head);
pcb_t* outProcQ(struct list_head* head, pcb_t *p);
#endif /* pcb_h */
