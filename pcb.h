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

