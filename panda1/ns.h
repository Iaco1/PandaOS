#ifndef ns_h
#define ns_h
#include "pandos_types.h"
#include "pandos_const.h"
#include "list.h"
#include "pcb.h"
/*Each Namespace is represented inside the ns_Table by a different index, which 
corresponds to its type. Each time a new Namespace is added, it should be defined in
the file pandos_types.h and the value of NS_TYPE_LAST should be updated accordingly.
Here are listed the current Namespaces and their declared type:
NS_PID 0
*/
static nsd_t ns_Table[NS_TYPE_MAX][MAXPROC];

static struct list_head ns_Free_h[NS_TYPE_MAX];
static struct list_head ns_Active_h[NS_TYPE_MAX];

void initNamespaces();
nsd_t* getNamespace(pcb_t *p, int type);
nsd_t* allocNamespace(int type);
void freeNamespace(nsd_t *ns );
bool check_ns_Free(int type, int n);
int addNamespace(pcb_t *p, nsd_t *ns);

 



#endif