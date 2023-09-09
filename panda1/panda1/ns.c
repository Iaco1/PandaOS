#include "ns.h"
#include "pcb.h"


/*Inizializes all the aviable Namespace lists. This method is called only once,
when the data structure is first inizialized.*/

void initNamespaces()
{
    for(int type=0; type<NS_TYPE_MAX; type++){ 
    /*cycle repeated for each type of namespace:
    both Free and Active ns_lists are initialized*/

        INIT_LIST_HEAD(&ns_Free_h[type]);
        INIT_LIST_HEAD(&ns_Active_h[type]);

        for(int j = 0; j < MAXPROC; j++){
            ns_Table[type][j].n_type=type;
            list_add_tail(&ns_Table[type][j].n_link, &ns_Free_h[type]);
        }
    }
}

/*Returns the type Namespace associated to the process p (NULL if unable).*/

nsd_t* getNamespace(pcb_t *p, int type){
    if(type>NS_TYPE_MAX) return NULL;
    else return p->namespaces[type];
}

/*Allocates a Namespace of a given type from the correct list.*/

nsd_t* allocNamespace(int type){
    if(list_empty(&ns_Free_h[type])) return NULL;
    /*if there are any free namespaces of the needed type, the first one
    will be allocated and added to the active list.*/
    else{ 
        nsd_t *ns = container_of(ns_Free_h[type].next, nsd_t, n_link); 
        list_del(ns_Free_h[type].next); 
        list_add_tail(&ns->n_link, &ns_Active_h[type]); 
        return ns;
    }
}

/*Frees a Namespace from its current list, and adds it to its corresponding ns_Free_h.*/   

void freeNamespace(nsd_t *ns ){
    int type=ns->n_type;
    list_del(&ns->n_link);
    list_add_tail(&ns->n_link, &ns_Free_h[type]);
}

/*Links the process p and all its children to the namespace ns.
Returns true if it is possible, false otherwise.*/

int addNamespace(pcb_t *p, nsd_t *ns){
    /*if ns's corresponding ns_Free_h list is empty, then ns 
    must be inside it's list of Active Namespaces*/
    if(list_empty(&ns_Free_h[ns->n_type])) return false;
    p->namespaces[ns->n_type]=ns;
    struct list_head *child;
    list_for_each(child,  &p->p_child) {
        pcb_t *pc = container_of(child, pcb_t, p_sib);
        pc->namespaces[ns->n_type]=ns;
     }
    return true;
}
