#include "ns.h"

/*
 Inizializza tutte le liste dei namespace liberi. Questo metodo viene invocato
una volta sola durante l’inizializzazione della struttura dati.
*/

static void initNamespaces(){
    for(int type=0; type<NS_TYPE_MAX; type++){ 
    /*cycle repeated for each type of namespace:
    both Free and active ns_lists are initialized*/

        INIT_LIST_HEAD(&ns_Free_h[type].n_link);
        INIT_LIST_HEAD(&ns_Active_h[type].n_link);

        for(int j = 0; j < MAXPROC; j++){
            ns_Table[type][j].n_type=type;
            list_add_tail(&ns_Table[type][j].n_link, &ns_Free_h[type].n_link);
        }
    }
}

/*
  Ritorna il namespace di tipo type associato al processo p (o NULL).
*/
nsd_t* getNamespace(pcb_t *p, int type){
    if(type>NS_TYPE_MAX) return NULL;
    else return p->namespaces[type];
}

/* 
  Alloca un namespace di tipo type dalla lista corretta.
*/

nsd_t* allocNamespace(int type){
    if(list_empty(&ns_Free_h[type].n_link)) return NULL;
    /*if there are any free namespaces of the needed type, the first one
    will be allocated and added to the active list*/
    else{ 
        nsd_t *ns = container_of(ns_Free_h[type].n_link.next, nsd_t, n_link); //prendo il primo ns libero del dato tip
        list_del(ns_Free_h[type].n_link.next); //lo rimuovo dalla lista dei ns liberi per quel tipo
        list_add_tail(ns, &ns_Active_h[type].n_link); //lo aggiugo alla lista di ns attivi
        return ns;
    }
}
/*
  Libera il namespace ns ri-inserendolo nella lista di namespace corretta.
*/
    
void freeNamespace(nsd_t *ns ){
    int type=ns->n_type;
    list_del(&ns->n_link);
    list_add_tail(ns, &ns_Free_h[type].n_link);
}

/*returns true if there are at least n free namespaces of a given type*/

bool check_ns_Free(int type, int n){
    int count = 0;
    if(!list_empty(&ns_Free_h[type].n_link)){
        struct list_head *list=ns_Free_h[type].n_link.next;
        while(count<n || list == &ns_Free_h[type].n_link){
            count++;
            list=list->next;
            }
    }
    return count>=n;
}


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


/*links the process p and all its children to the namespace ns.
    Returns true if it is possible, false otherwise*/

int addNamespace(pcb_t *p, nsd_t *ns){
    int p_children=count_children()
    int type=ns->n_type;
    if(!check_ns_Free(type, p_children+1)) return false;
    /*if there are not enough free type_nsd, the program stops here*/
    else{
        p->namespaces[type]=allocNamespace(type);
        struct list_head *child;
        list_for_each(child,  &p->p_child) {
            pcb_t *pc = container_of( &p->p_child, pcb_t, p_child);
            //dubbio se vada usato p_child o p_sib
            pc->namespaces[type]=allocNamespace(type);
            }
    }
    return true;
}

