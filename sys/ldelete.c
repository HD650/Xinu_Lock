#include <lock.h>
#include <kernel.h>
#include <proc.h>

int ldelete(int ldes)
{
  STATWORD ps;
  disable(ps);
  //if the argument is invaild or the lock is not created yet, it's error
  int index=ldes%NLOCKS;
  if(lock_check(index)||g_locks[index].lstate==LFREE)
  {
    restore(ps);
    return SYSERR;
  }
  
  //clear the state of this lock
  g_locks[index].lstate=LFREE;
  //clear all the processes waitting on this lock
  int proc_id;
  while((proc_id=getfirst(g_locks[index].lqueuehead))!=EMPTY)
  {
    //let these processes return DELERED in wait()
    proctab[proc_id].pwaitret=DELETED;
    //set these processes to ready or they will never wake up
    ready(proc_id,RESCHNO);
  }
  resched();
  restore(ps);
  return OK;
}
