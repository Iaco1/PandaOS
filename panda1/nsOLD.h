#include "pandos_types.h"

static nsd_t ns_Table[NS_TYPE_MAX][MAXPROC];
//alternativamente, static nsd_t* ns_Table[NS_TYPE_MAX]={ PID_nsd[MAX_PROC] } ?

/*array of lists; each index refers to a different type of Namespace*/
static nsd_t* ns_Free_h[NS_TYPE_MAX];
static nsd_t* ns_Active_h[NS_TYPE_MAX];

/*each new namespace should be added to the ns_Table array in their own position*/
//static nsd_t PID_nsd[MAXPROC];

//ns_Table[NS_PID]=PID_nsd;


static void initNamespaces(){
    for(int type=0; type<NS_TYPE_MAX; type++){ 
    /*cycle repeated for each type of namespace:
    both Free and active ns_lists are initialized*/

        INIT_HLIST_HEAD(&ns_Free_h[type]->n_link);
        INIT_HLIST_HEAD(&ns_Active_h[type]->n_link);

        for(int j=0; j<MAXPROC-1; j++){
            ns_Table[type][j].n_type=type;

            //necessario?
            INIT_HLIST_HEAD(ns_Table[type][j].n_link);

            list_add(ns_Table[type][j].n_link.next, ns_Free_h[type]->n_link.next);
            }
        }
}

nsd_t* getNamespace(pcb_t *p, int type){
    if(type>NS_TYPE_MAX) return NULL;
    else return p->namespaces[type];
}

nsd_t* allocNamespace(int type){
    if(list_empty(ns_Free_h[type]->n_link.next)) return NULL;
    /*if there are any free namespaces of the needed type, the first one
    will be allocated and added to the active list*/
    else{ 
        nsd_t* ns = container_of(ns_Free_h[type]->n_link.next, nsd_t, n_link);
        ns_Free_h[type]->n_link.next=ns->n_link.next;
        ns->n_link.next->prev=ns_Free_h[type]->n_link.next;
        list_add(ns->n_link.next, ns_Active_h[type]->n_link.next);
        return ns;
    }
}

void freeNamespace(nsd_t *ns ){
    int type=ns->n_type;
     __list_del(ns->n_link.prev, ns->n_link.next);
    list_add(ns->n_link.next, ns_Free_h[type]->n_link.next);
}

bool check_ns_Free(int type, int n){
    /*returns true if there are at least n free namespaces of a given type*/
    struct list_head list;
    list.next=ns_Free_h[type]->n_link.next;
    int i=0;
    while( i<n || list.next!=&ns_Free_h[type]->n_link){
        i=i+1;
        list.next=list.next->next;
    }
    return i>=n;
}

int addNamespace(pcb_t *p, nsd_t *ns){
    /*links the process p and all its children to the namespace ns.
    Returns true if it is possible, false otherwise*/
    struct list_head list=p->p_child;
    int p_children=0;
    if (list.next!=&p->p_child){

    //funzione che conta i figli

    }
    int type=ns->n_type;
    if(!check_ns_Free(type, p_children+1)) return false;
    else{
        p->namespaces[type]=allocNamespace(type);

        //allocazione per ciascun figlio

    }
}