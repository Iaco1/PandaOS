#include "pcb.h"
#include "list.h"
#include "assert.h"
#include "pandos_types.h"
#include "pandos_const.h"
#include "container_of.h"
#include <stdio.h>

static struct list_head pcbFree_h;
static pcb_t pcbFree_table[MAXPROC];

/*
 Funzione che inizializza la lista pcbFree in modo da contenere tutti gli elementi della pcbFree_table.
*/

void initPcbs(){
    INIT_LIST_HEAD(&pcbFree_h); //inizializzo la nuova lista aggiungendo solo l'elemento di testa
    for(int i = 0; i < MAXPROC; i++){
        list_add_tail(&pcbFree_table[i].p_list, &pcbFree_h); //inserisco tutti gli elementi nella coda della 
        //nuova lista
    }
}
/*
  Inserisce il PCB puntato da p nella lista dei PCB liberi pcbFree.
*/

void freePcb(pcb_t *p){
 list_add_tail(&p->p_list, &pcbFree_h);
}

/*
  Restituisce NULL se la pcbFree_h è vuota, sennò rimuove un elemento dalla pcbFree_h, inizializza tutti i campi a
  NULL e restituisce l'elemento rimosso.
*/

pcb_t *allocPcb(){
    if(list_empty(&pcbFree_h)){
        return NULL;
    } else {
        /* Rimuovere un elemento da pcbfree_h, salvarlo e poi resistuirlo */
        //si prende pcbFree_h.next dato che pcbFree_h rappresenta la testa di tutta la lista, noi vogliamo il primo elemento utile
        pcb_t *tmp = container_of(&pcbFree_h.next, pcb_t, p_list); 
        list_del(pcbFree_h.next); //stacca l'elemento dalla lista, restano da settare a NULL i puntatori di:
        //parent, child e sib.
        INIT_LIST_HEAD(&tmp->p_child); //dato che si tratta di due struct list_head inizializzo due liste vuote
        INIT_LIST_HEAD(&tmp->p_sib); //ovvero setto i puntatori next e prev a null
        tmp->p_parent = NULL;
        return tmp;
    }
}

/*
  Crea una lista di PCB, inizializzandola come lista vuota.
*/

void  mkEmptyProcQ(struct list_head *head){
    INIT_LIST_HEAD(head); //inizializza la sentinella della lista, il resto è vuoto
}

/*
  Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti.
*/
int emptyProcQ(struct list_head *head){
if(list_empty(head)){
    return 1;
 } else {
    return 0;
 }
}

/*
  Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
*/

void insertProcQ(struct list_head* head, pcb_t *p){
   list_add_tail(&p->p_list, head);
}

/*
  Restituisce l’elemento di testa della coda dei processi da head, SENZA RIMUOVERLO. Ritorna NULL se 
  la coda non ha elementi.
*/

pcb_t* headProcQ(struct list_head* head){
    if(list_empty(head)){ //possibile miglioria: usare emptyProcQ
      return NULL; 
    } else {
        pcb_t *tmp = container_of(head->next, pcb_t, p_list); //a differenza di prima in questo caso basta passare
        //head->next dato che head è già un puntatore alla struttura
        return tmp;
    }

}

/*
 Rimuove il primo elemento dalla coda dei processi puntata da head. Ritorna NULL se la coda è vuota.
  Altrimenti ritorna il puntatore all’elemento rimosso dalla lista.
*/

pcb_t* removeProcQ(struct list_head* head){
  if(emptyProcQ(head)){
    return NULL;
  } else {
    pcb_t* tmp = container_of(head->next, pcb_t, p_list); //si potrebbe usare headProcQ
    list_del(tmp);
    return tmp;
  }
}


/*
 Rimuove il PCB puntato da p dalla coda dei processi puntata da head. Se p non è presente nella coda, 
 restituisce NULL. (NOTA: p può trovarsi in una posizione arbitraria della coda).
*/
pcb_t* outProcQ(struct list_head* head, pcb_t *p){
struct list_head* tmp; //uso tmp come puntatore ausiliario per scorrere sulla mia lista
list_for_each(tmp, head){
  if(tmp == &p->p_list){ //se tmp e p puntanto alla stessa cosa allora p è nella cosa di processi puntata da head
    list_del(tmp); //rimuovo quell'elemento
    return p; 
  }
}
return NULL;
}

int emptyChild( pcb_t *p){
  if(list_empty(&p->p_child)){
    return 1;
  } else {
    return 0;
  }
}

/*
 Inserisce il PCB puntato da p come figlio del PCB puntato da prnt.
*/


void insertChild(pcb_t *prnt,pcb_t *p){
 p->p_parent = prnt; //setto prnt come padre di p
 /*
   Inserisco p_sib in prnt->p_child, mi basta fare questo dato che la funzione add_tail aggiornerà automanticamente i puntatori.
 */
 list_add_tail(&p->p_sib, &prnt->p_child); 
}

/*
  Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL.
*/

pcb_t* removeChild(pcb_t *p){
 if(emptyChild(&p->p_child)){
  return NULL;
 } else {
  pcb_t *child_1 = list_first_entry(&p->p_child, pcb_t, p_sib);
  list_del(&child_1->p_sib);
  child_1->p_parent = NULL;
  return child_1;
 }
}

pcb_t *outChild(pcb_t* p){
  if(p->p_parent == NULL){
    return NULL;  //se p non ha un padre
  }
  list_del(&p->p_sib);
  p->p_parent = NULL;  //senò come removechild solo che non devo trovare il primo elementto della lista
  return p;
}

int main(void){
   
   initPcbs();

    pcb_t *first_pcb = allocPcb();
    if(first_pcb == NULL){
        printf("Failed to allocate first PCB\n");
    } else {
        printf("Allocated first PCB: %p\n", first_pcb);
    }

    pcb_t *second_pcb = allocPcb();
    if(second_pcb == NULL){
        printf("Failed to allocate second PCB\n");
    } else {
        printf("Allocated second PCB: %p\n", second_pcb);
    }

    freePcb(first_pcb);
    printf("Freed first PCB: %p\n", first_pcb);

    pcb_t *third_pcb = allocPcb();
    if(third_pcb == NULL){
        printf("Failed to allocate third PCB\n");
    } else {
        printf("Allocated third PCB: %p\n", third_pcb);
    }

     struct list_head test_queue;
     mkEmptyProcQ(&test_queue);
     assert(emptyProcQ(&test_queue) == 1); // la coda deve essere vuota all'inizio

     pcb_t p1, p2, p3;
     insertProcQ(&test_queue, &p1);
     insertProcQ(&test_queue, &p2);
     insertProcQ(&test_queue, &p3);

     assert(emptyProcQ(&test_queue) == 0); // la coda non deve essere più vuota
     assert(headProcQ(&test_queue) == &p1); // la testa della coda deve essere &p1

     pcb_t *removed = removeProcQ(&test_queue);
     assert(removed == &p1); // deve essere rimosso &p1 dalla coda
     assert(headProcQ(&test_queue) == &p2); // la testa della coda ora deve essere &p2

     outProcQ(&test_queue, &p3);
     assert(headProcQ(&test_queue) == &p2); // &p3 deve essere rimosso dalla coda, la testa deve ancora essere &p2

     printf("Tutti i test sono passati con successo\n"); 

     pcb_t parent, child1, child2, child3;
     INIT_LIST_HEAD(&parent.p_child);
     INIT_LIST_HEAD(&child1.p_child);
     INIT_LIST_HEAD(&child2.p_child);
     INIT_LIST_HEAD(&child3.p_child);

    // check that parent has no children initially
    assert(emptyChild(&parent) == 1);

    // insert child1 and check that parent now has a child
    insertChild(&parent, &child1);
    assert(emptyChild(&parent) == 0);
  
    // insert child2 and check that both children are siblings
    insertChild(&parent, &child2);
    assert(list_is_last(&child1.p_sib, &parent.p_child) == 0);
    assert(list_is_last(&child2.p_sib, &parent.p_child) == 1); //check if parent's child list is succesfully updated
    assert(child1.p_sib.next == &child2.p_sib);
    assert(child2.p_sib.prev == &child1.p_sib); //check if the two childs are set as siblings
    
    insertChild(&parent, &child3);
    // remove the first child process from the parent
    pcb_t *removed_child = removeChild(&parent);
    // check that the removed child is child1
    assert(removed_child == &child1);

    // check that child1 is no longer a child of the parent
    assert(emptyChild(&parent.p_child) == 0);
    assert(list_first_entry(&parent.p_child, pcb_t, p_sib) == &child2); //assumiamo che quella dei siblings sia 
    //una lista monolinkata (come mostrato sul libro)
    assert(list_empty(&child1.p_sib) == 1);   
    assert(list_is_last(&child2.p_sib, &parent.p_child) == 0); 
    
    pcb_t child4;
    INIT_LIST_HEAD(&child4.p_child);
    insertChild(&parent, &child4);

    pcb_t *removeChild_2 = outChild(&child3);

    // Check if child3 is removed from the parent's child list
    assert(list_empty(&child3.p_sib) == 1); //check if child3 still has siblings
    assert(list_is_head(&child2.p_sib, &parent.p_child)==1);
    assert(list_is_last(&child4.p_sib, &parent.p_child)==1);
    
    
    // Check if removed_child is child3
    assert(removed_child == &child3);

    printf("Ok, test passati");

    return 0;
}
    


