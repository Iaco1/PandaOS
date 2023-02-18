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
pcb_t *allocPcb(void) {
  if(list_empty(&pcbFree_h)){
    return NULL;
  } else {
    pcb_t *tmp = container_of(pcbFree_h.next, pcb_t, p_list); //tmp punta al primo elemento della lista
    list_del(pcbFree_h.next); //rimuovo l'elemento dalla lista
    tmp->p_parent = NULL; //inizializzo tutti i campi a NULL
    INIT_LIST_HEAD(&tmp->p_child);
    INIT_LIST_HEAD(&tmp->p_sib);
    INIT_LIST_HEAD(&tmp->p_list);
    tmp->p_s.cause = 0;
    tmp->p_s.entry_hi= 0;
    tmp->p_s.hi = 0;
    tmp->p_s.lo = 0;
    tmp->p_s.pc_epc = 0;
    tmp->p_s.status = 0;
    for(int i = 0; i<29; i++){
      tmp->p_s.gpr[i] = 0;
    }
    tmp->p_time = 0;
    tmp->p_semAdd = NULL;
    for(int i = 0; i < NS_TYPE_MAX; i++){
        tmp->namespaces[i] = NULL;
    }
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

/*
  Rimuove il PCB puntato da p dalla lista dei figli del padre. Se il PCB puntato da p non ha un padre, 
  restituisce NULL, altrimenti restituisce l’elemento rimosso (cioè p). A differenza della removeChild, 
  p può trovarsi in una posizione arbitraria (ossia non è necessariamente il primo figlio del padre).
*/

pcb_t *outChild(pcb_t* p){
  if(p->p_parent == NULL){
    return NULL;  //se p non ha un padre
  }
  list_del(&p->p_sib);
  p->p_parent = NULL;  //senò come removechild solo che non devo trovare il primo elementto della lista
  return p;
}

/*
  Funzione per contare il numero di figli di un processo.
*/

int count_children(pcb_t* p){
  if(emptyChild(&p->p_child)){
    return 0;
  } else {
     int count = 0;
    struct list_head *child;
    list_for_each(child, &p->p_child) {
        count++;
    }
    return count;
  }

}
