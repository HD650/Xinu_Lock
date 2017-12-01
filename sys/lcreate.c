#include <lock.h>
#include <kernel.h>


extern int g_lock_id;

int lcreate()
{
  STATWORD ps;
  disable(ps);
  
  // go through all the lock slots
  int i=0;
  int index=0;
  int res=-1;
  for(;i<NLOCKS;i++)
  {
    index=g_lock_id%NLOCKS;
    if(g_locks[index].lstate==LFREE)
    {
      g_locks[index].lstate=LUSED;
      g_locks[index].lid=index;
      
      break;
    }
    //update the global lock id
    g_lock_id+=1;
    //if the global lock id is going to overflow, reset it
    if(g_lock_id==MAXINT)
      g_lock_id=0;
  }
  if(res!=-1)
  {
    restore(ps);
    return res;
  }
  restore(ps);
  return SYSERR;
}
