#include <lock.h>
#include <kernel.h>

struct lock_t g_locks[NLOCKS];
int g_lock_id;

void linit()
{
  int i=0;
  g_lock_id=0;
  //init all the slots with init state, init state locks can be use
  for(;i<NLOCKS;i++)
  {
    g_locks[i].lstate=LFREE;
    g_locks[i].lqueuehead=newqueue();
    g_locks[i].lqueuetail=g_locks[i].lqueuehead+1;
    g_locks[i].lholder=-1;
    g_locks[i].lid=-1;
    g_locks[i].lmaxprio=MININT;
    g_locks[i].ltype=DELETED;
    g_locks[i].lnumw=0;
    g_locks[i].lnumr=0;
    g_locks[i].lnumh=0;
    g_locks[i].lmaxw=MININT;
  }
}
