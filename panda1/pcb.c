#include "pcb.h"
#include "list.h"
#include "pandos_types.h"
#include "pandos_const.h"
#include "container_of.h"

static struct list_head pcbFree_h;
static pcb_t pcbFree_table[MAXPROC];

/*
 Funzione che inizializza la lista pcbFree in modo da contenere tutti gli elementi della pcbFree_table.
*/

void initPcbs()
{
    INIT_LIST_HEAD(&pcbFree_h); //inizializzo la nuova lista aggiungendo solo l'elemento di testa
    for(int i = 0; i < MAXPROC; i++){
        list_add_tail(&pcbFree_table[i].p_list, &pcbFree_h); //inserisco tutti gli elementi nella coda della 
        //nuova lista
    }
}
/*
  Inserisce il PCB puntato da p nella lista dei PCB liberi pcbFree.
*/

void freePcb(pcb_t *p)
{
 list_add_tail(&p->p_list, &pcbFree_h);
}

/*
  Restituisce NULL se la pcbFree_h è vuota, sennò rimuove un elemento dalla pcbFree_h, inizializza tutti i campi a
  NULL e restituisce l'elemento rimosso.
*/

pcb_t *allocPcb()
{
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

void  mkEmptyProcQ(struct list_head *head)
{
    INIT_LIST_HEAD(head); //inizializza la sentinella della lista, il resto è vuoto
}

/*
  Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti.
*/
int emptyProcQ(struct list_head *head)
{
if(list_empty(head)){
    return 1;
 } else {
    return 0;
 }
}

/*
  Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
*/

void insertProcQ(struct list_head* head, pcb_t *p)
{
   list_add_tail(&p->p_list, head);
}

/*
  Restituisce l’elemento di testa della coda dei processi da head, SENZA RIMUOVERLO. Ritorna NULL se 
  la coda non ha elementi.
*/

pcb_t* headProcQ(struct list_head* head)
{
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

pcb_t* removeProcQ(struct list_head* head)
{
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
pcb_t* outProcQ(struct list_head* head, pcb_t *p)
{
struct list_head* tmp; //uso tmp come puntatore ausiliario per scorrere sulla mia lista
list_for_each(tmp, head){
  if(tmp == &p->p_list){ //se tmp e p puntanto alla stessa cosa allora p è nella cosa di processi puntata da head
    list_del(tmp); //rimuovo quell'elemento
    return p; 
  }
}
return NULL;
}

/*
  Restituisce TRUE se il PCB puntato da p non ha figli, FALSE altrimenti
*/

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


void insertChild(pcb_t *prnt,pcb_t *p)
{
 p->p_parent = prnt; //setto prnt come padre di p
 if(!emptyChild(&prnt->p_child)){ //se prnt ha almeno un figlio
  pcb_t *child_1 = list_first_entry(&prnt->p_child, pcb_t, p_sib);
  list_add_tail(&p->p_sib, &child_1->p_sib); //lo aggiungo ai fratelli (suppongo che se ci sono altri figli questi siano già considerati fratelli di child_1)
 }
 /*
   Inserisco p_sib in prnt->p_child dato che voglio che p non perda i suoi fratelli.
 */
 list_add_tail(&p->p_sib, &prnt->p_child); 
}