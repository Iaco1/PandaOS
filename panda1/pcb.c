#include <pcb.h>
#include <list.h>

void initPcbs(){
    INIT_LIST_HEAD(&pcbFree_h);
    for(int i = 0; i < MAXPROC; i++){
        list_add_tail(&procp[i].p_next, pcbFree_h);
    }
}

void freePcbs(pcb_t *p){
 list_add_tail(&p->p_next, pcbFree_h);
}

/*
  Restituisce NULL se la pcbFree_h è vuota, sennò rimuove un elemento dalla pcbFree_h, inizializza tutti i campi a
  NULL e restituisce l'elemento rimosso.
*/

pcb_t *allocPcb(){
    if(list_empty(&pcbfree_h)){
        return NULL;
    } else {
        /* Rimuovere un elemento da pcbfree_h, salvarlo e poi resistuirlo */
        //list_del rimuove l'elemento dalla lista
        //container_of ci permette di ottenere il puntatore, quindi va fatta prima la chiamata 'container_of'
        //container_of prende: 1) l'elemento di cui vogliamo avere il puntatore
        //2) il tipo 3)p_next è il nome della variabile del prossimo elemento della lista
        // e poi va rimosso l'elemento dalla lista
        pcb_t *tmp = container_of(&pcbFree_h.next, pcb_t, p_next );
        list_del(&pcbFree_h.next); //stacca l'elemento dalla lista, restano da settare a NULL i puntatori di:
        //parent, child e sib
        tmp->p_prnt = NULL;
        tmp->p_child = NULL;
        tmp->p_sib = NULL;
        tmp->p_next = NULL;
        tmp->p_prev = NULL;  
        return tmp;
    }
}

/*
  Crea una lista di PCB, inizializzandola come lista vuota.
*/

void  mkEmptyProcQ(pcb_t * p){
    INIT_LIST_HEAD(&p); //inizializza la sentinella della lista, il resto è vuoto
}

/*
  Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti.
*/
int emptyProcQ(struct list_head *head){
if(head->next == NULL && head->prev == NULL){
    return 1;
 } else {
    return 0;
 }
}

/*
  Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
*/

void insertProcQ(struct list_head* head, pcb_t *p){
   list_add_tail(&p->p_next, head);
}



