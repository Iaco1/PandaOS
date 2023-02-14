#ifndef ns_h
#define ns_h
#include "pandos_types.h"
#include "pandos_const.h"
#include "list.h"

static nsd_t ns_Table[NS_TYPE_MAX][MAXPROC];
//alternativamente, static nsd_t* ns_Table[NS_TYPE_MAX]={ PID_nsd[MAX_PROC] } ?

/*array of lists; each index re*/
static struct list_head ns_Free_h[NS_TYPE_MAX];
static struct list_head ns_Active_h[NS_TYPE_MAX];

/*each new namespace should be added to the ns_Table array in their own position*/
//static nsd_t PID_nsd[MAXPROC];

static void initNamespaces();
nsd_t* getNamespace(pcb_t *p, int type);
nsd_t* allocNamespace(int type);
void freeNamespace(nsd_t *ns );
bool check_ns_Free(int type, int n);
int addNamespace(pcb_t *p, nsd_t *ns);





#endif