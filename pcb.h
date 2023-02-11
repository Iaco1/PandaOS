
#ifndef pcb_h
#define pcb_h
#include <list.h>

/*
 Funzione che inizializza la lista pcbFree in modo da contenere tutti gli elementi della pcbFree_table.
*/
void initPcbs();

/*
  Inserisce il PCB puntato da p nella lista dei PCB liberi pcbFree.
*/
void freePcbs(pcb_t *p);
/*
  Restituisce NULL se la pcbFree_h è vuota, sennò rimuove un elemento dalla pcbFree_h, inizializza tutti i campi a
  NULL e restituisce l'elemento rimosso.
*/




#endif /* pcb_h */
