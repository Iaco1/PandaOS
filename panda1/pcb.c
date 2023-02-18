#include "pcb.h"
#include "list.h"
#include "pandos_types.h"
#include "pandos_const.h"
#include "container_of.h"

static struct list_head pcbFree_h;
static pcb_t pcbFree_table[MAXPROC];

/* 
  Initializes the pcbFree_h list and adds all the pcb contained in the pcbFree_table 
  to the list.
*/
void initPcbs()
{
    INIT_LIST_HEAD(&pcbFree_h); 
    for(int i = 0; i < MAXPROC; i++){
        list_add_tail(&pcbFree_table[i].p_list, &pcbFree_h); 
    }
}

/*
  Adds the given pcb to pcbFree_h list's tail.
*/

void freePcb(pcb_t *p)
{
 list_add_tail(&p->p_list, &pcbFree_h);
}

/*
  Allocates a new pcb from the pcbFree_h list. If the list is empty, it returns NULL.
  If it is not, it removes the first pcb from the list and returns it.
*/
 
pcb_t *allocPcb(void) {
  if(list_empty(&pcbFree_h)){
    return NULL;
  } else {
    pcb_t *tmp = container_of(pcbFree_h.next, pcb_t, p_list); 
    list_del(pcbFree_h.next); 
    tmp->p_parent = NULL; 
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
  Initializes the list head of the process queue.
*/
void  mkEmptyProcQ(struct list_head *head)
{
    INIT_LIST_HEAD(head); 
}

/*
  Returns true if the process queue is empty, if it's not it returns false.
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
  Inserts the pcb p in the tail of the process queue.
*/

void insertProcQ(struct list_head* head, pcb_t *p)
{
   list_add_tail(&p->p_list, head);
}

/*
  Returns the first pcb in the process queue. If the queue is empty, it returns NULL.
*/

pcb_t* headProcQ(struct list_head* head)
{
    if(list_empty(head)){ 
      return NULL; 
    } else {
        pcb_t *tmp = container_of(head->next, pcb_t, p_list); 
        return tmp;
    }

}
/*
  If the queue is not empty it removes the first pcb in the queue and returns it.
  Otherwise it returns NULL.
*/

pcb_t* removeProcQ(struct list_head* head)
{
  if(emptyProcQ(head)){
    return NULL;
  } else {
    pcb_t* tmp = container_of(head->next, pcb_t, p_list); 
    list_del(head->next);
    return tmp;
  }
}
/* 
 Checks if the given pcb is in the process queue. 
 If it is, it removes the pcb from the queue and returns it.
 Otherwise it returns NULL.
*/

pcb_t* outProcQ(struct list_head* head, pcb_t *p)
{
struct list_head* tmp; 
list_for_each(tmp, head){
  if(tmp == &p->p_list){
    list_del(tmp); 
    return p; 
  }
}
return NULL;
}
/* 
  If the pcb's child list is empty it returns 1, otherwise it returns 0.
*/
int emptyChild( pcb_t *p){
  if(list_empty(&p->p_child)){
    return 1;
  } else {
    return 0;
  }
} 
/*
  Inserts the given pcb into prnt's child list.
*/

void insertChild(pcb_t *prnt,pcb_t *p)
{
 p->p_parent = prnt; 
  ist_add_tail(&p->p_sib, &prnt->p_child); 
}

/*
  If p's child list is not empty it removes the first pcb in the list and returns it.
  Otherwise it returns NULL.
*/

pcb_t* removeChild(pcb_t *p){
  if(emptyChild(p)){
    return NULL;
  } else {
    pcb_t *tmp = container_of(p->p_child.next, pcb_t, p_sib);
    list_del(p->p_child.next); 
    tmp->p_parent = NULL; 
    return tmp;
  }
}

/*
  Removes the given pcb from its parent's child list and returns a pointer to it. 
  If p has no parent, it returns NULL.
*/

pcb_t *outChild(pcb_t* p){
  if(p->p_parent == NULL){
    return NULL;  
  }
  list_del(&p->p_sib);
  p->p_parent = NULL;  
  return p;
}
/*
  Counts the number of children of the given pcb and returns it.
*/

int count_children(pcb_t* p){
  if(emptyChild(p)){
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
