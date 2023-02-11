#include <pcb.h>
#include <list.h>

void initPcbs(){
    INIT_LIST_HEAD(&pcbFree_h);
    for(int i = 0; i < MAXPROC; i++){
        list_add_tail(&procp[i].p_next, &pcbFree_h);
    }
}

void freePcbs(pcb_t *p){
 list_add_tail(&p->p_next, &pcbFree_h);
}

