#ifndef PCB_H
#define PCB_H
#include <list.h>
#define MAXPROC 20

/*
 Funzione che inizializza la lista pcbFree in modo da contenere tutti gli elementi della pcbFree_table.
*/
void initPcbs(){
    INIT_LIST_HEAD(&pcbFree_h); 
    for(int i = 0; i < MAXPROC; i++){
        list_add_tail(&procp[i].p_next, &pcbFree_h);
    }
}

/*
  Inserisce il PCB puntato da p nella lista dei PCB liberi pcbFree.
*/
void freePcbs(pcb_t *p){
 list_add_tail(&p->p_next, &pcbFree_h);
}

#endif
