#include <lock.h>

struct lock_t g_locks[NLOCKS];
int g_lock_id;

void linit()
{
  int i=0;
  g_lock_id=0;
  for(;i<NLOCKS;i++)
  {
    g_locks[i].lstate=LFREE;
    g_locks[i].lqueuehead=newqueue();
    g_locks[i].lqueuetail=g_locks[i].lqueuehead+1;
    g_locks[i].lholder=-1;
    g_locks[i].lmaxprio=MININT;
    g_locks[i].ltype=DELETED;
  }
}
